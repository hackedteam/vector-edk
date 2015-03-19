#include <Uefi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Include/CompressionTypes.h"
#include "Include/EfiStruct.h"

#include "Tiano/Decompress.h"
#include "Tiano/Compress.h"
#include "LZMA/LzmaDecompress.h"
#include "LZMA/LzmaCompress.h"
#include "Include/CommonLib.h"

#define Py_ssize_t INTN

#define PyLong_FromUnsignedLong(x) (PyObject *)(x)

/*++
  
Routine Description::

  This function calculates the value needed for a valid UINT16 checksum

Arguments:

  Buffer      Pointer to buffer containing byte data of component.
  Size        Size of the buffer

Returns:

  The 16 bit checksum value needed.

--*/
UINT16 CalculateChecksum16( UINT16 *Buffer, UINTN Size)
{
  return (UINT16) (0x10000 - CalculateSum16 (Buffer, Size));
}

UINT8 CalculateChecksum8(UINT8 *Buffer, UINTN Size)
{
	return (UINT8) (0x100 - CalculateSum8(Buffer, Size));
}


/*++
  
Routine Description:

  This function calculates the UINT16 sum for the requested region.

Arguments:

  Buffer      Pointer to buffer containing byte data of component.
  Size        Size of the buffer

Returns:

  The 16 bit checksum

--*/
UINT16 CalculateSum16 (UINT16       *Buffer, UINTN        Size)
{
  UINTN   Index;
  UINT16  Sum;

  Sum = 0;

  //
  // Perform the word sum for buffer
  //
  for (Index = 0; Index < Size; Index++) {
    Sum = (UINT16) (Sum + Buffer[Index]);
  }

  return (UINT16) Sum;
}


UINT8	CalculateSum8(UINT8 *Buffer, UINTN Size)
{
	UINTN Index;
	UINT8 Sum;

	Sum = 0;

	//
	// Perform the byte sum for buffer
	//
	for (Index = 0; Index < Size; Index++) {
		Sum = (UINT8) (Sum + Buffer[Index]);
	}

	return Sum;
}

UINT32 Expand24bit(UINT8 *ptr)
{
	return ((ptr[0]) | (ptr[1] << 8) | (ptr[2] << 16));
}

void Pack24bit(UINT32 value, UINT8 *ffsSize)
{
	if (value >= 0xffffff)
	{
		ffsSize[0] = 0xff;
		ffsSize[1] = 0xff;
		ffsSize[2] = 0xff;
	}
	else
	{
		ffsSize[2] = (UINT8) ((value) >> 16);
		ffsSize[1] = (UINT8) ((value) >> 8);
		ffsSize[0] = (UINT8) ((value));
	}
}


UINT32 efi_ffs_file_size(PEFI_FFS_FILE_HEADER pFile)
{
	return Expand24bit(pFile->Size);
}

//////////////////////////////////////////////////////////////////////////////////////////
// cod
//////////////////////////////////////////////////////////////////////////////////////////

#define EFI_COMPRESSION   1 //defined as PI_STD, section type= 0x01
#define TIANO_COMPRESSION 2 //not defined, section type= 0x01
#define LZMA_COMPRESSION  3 //not defined, section type= 0x02

#define MAX_FFS_SIZE        0x1000000

EFI_STATUS
Extract (
  IN      VOID    *Source,
  IN      UINT32  SrcSize,
     OUT  VOID    **Destination,
     OUT  UINT32  *DstSize,
  IN      UINTN   Algorithm
  )
{
  VOID          *Scratch;
  UINT32        ScratchSize;
  EFI_STATUS    Status;

  GETINFO_FUNCTION    GetInfoFunction;
  DECOMPRESS_FUNCTION DecompressFunction;

  GetInfoFunction = NULL;
  DecompressFunction = NULL;
  Scratch = NULL;
  ScratchSize = 0;
  Status = EFI_SUCCESS;

  switch (Algorithm) {
  case 0:
    *Destination = (VOID *)malloc(SrcSize);
    if (*Destination != NULL) {
      memcpy(*Destination, Source, SrcSize);
    } else {
      Status = EFI_OUT_OF_RESOURCES;
    }
    break;
  case 1:
    GetInfoFunction = EfiGetInfo;
    DecompressFunction = EfiDecompress;
    break;
  case 2:
    GetInfoFunction = EfiGetInfo;
    DecompressFunction = TianoDecompress;
    break;
  case 3:
    GetInfoFunction = LzmaGetInfo;
    DecompressFunction = LzmaDecompress;
    break;
  default:
    Status = EFI_INVALID_PARAMETER;
  }
  if (GetInfoFunction != NULL) {
    Status = GetInfoFunction(Source, SrcSize, DstSize, &ScratchSize);
    if (Status == EFI_SUCCESS) {
      if (ScratchSize > 0) {
        Scratch = (VOID *)malloc(ScratchSize);
      }
      *Destination = (VOID *)malloc(*DstSize);
      if (((ScratchSize > 0 && Scratch != NULL) || ScratchSize == 0) && *Destination != NULL) {
        Status = DecompressFunction(Source, SrcSize, *Destination, *DstSize, Scratch, ScratchSize);
      } else {
        Status = EFI_OUT_OF_RESOURCES;
      }
    }
  }

  return Status;
}

EFI_STATUS
CopyObject(
  UINT8  *DstData,
  UINT8     *SrcBuf,
  UINT32    MaxSize
  )
{
  /*UINT32        ObjLen;
  UINT8         *TmpBuf;
  Py_ssize_t    SegNum;
  Py_ssize_t    Index;

  ObjLen = 0;
  SegNum = DstData->ob_type->tp_as_buffer->bf_getsegcount((PyObject *)DstData, NULL);
  TmpBuf = SrcBuf;
  for (Index = 0; Index < SegNum; ++Index) {
    VOID *BufSeg;
    Py_ssize_t Len;

    Len = DstData->ob_type->tp_as_buffer->bf_getreadbuffer((PyObject *)DstData, Index, &BufSeg);
    if (Len < 0) {
      return EFI_INVALID_PARAMETER;
    }

    if (ObjLen + Len > MaxSize) {
      return EFI_BUFFER_TOO_SMALL;
    }

    memcpy(BufSeg, &TmpBuf[ObjLen], Len);
    TmpBuf += Len;
    ObjLen += Len;
  }
*/
	memcpy(DstData, SrcBuf, MaxSize);
  return EFI_SUCCESS;
}

EFI_STATUS
SetObject(
  UINT8  *SrcData,
  UINT32    MaxSize,
  UINT8     Pattern
  )
{
  /*UINT32        ObjLen;
  Py_ssize_t    SegNum;
  Py_ssize_t    Index;

  ObjLen = 0;
  SegNum = SrcData->ob_type->tp_as_buffer->bf_getsegcount((PyObject *)SrcData, NULL);
  for (Index = 0; Index < SegNum; ++Index) {
    VOID *BufSeg;
    Py_ssize_t Len;

    Len = SrcData->ob_type->tp_as_buffer->bf_getreadbuffer((PyObject *)SrcData, Index, &BufSeg);
    if (Len < 0) {
      return EFI_INVALID_PARAMETER;
    }

    if (ObjLen + Len > MaxSize) {
      return EFI_BUFFER_TOO_SMALL;
    }

	memset(BufSeg, Pattern, Len);
    ObjLen += Len;
  }*/
  memset(SrcData, Pattern, MaxSize);

  return EFI_SUCCESS;
}
/*
 UefiDecompress(data_buffer, size, huffman_type)
*/
STATIC
UINT8*
UefiDecompress(
  UINT8       *SrcBuf,
  UINT32      SrcDataSize,
  UINT8       type,
  UINT8     **OutBuffer,
  UINT32     *OutSize
  )
{
  UINT32        DstDataSize;
  EFI_STATUS    Status;
  
  UINT8         *DstBuf;

  DstDataSize = 0;
  DstBuf = NULL;

  
  Status = Extract((VOID *)SrcBuf, SrcDataSize, (VOID **)&DstBuf, &DstDataSize, type);
  if (Status != EFI_SUCCESS) {
    //PyErr_SetString(PyExc_Exception, "Failed to decompress\n");
    goto ERROR;
  }

  *OutBuffer = DstBuf;
  *OutSize = DstDataSize;
  
  return *OutBuffer;

ERROR:
  return NULL;
}

/*
 UefiCompress(data_buffer, size, huffman_type)
*/
STATIC
UINT8*
UefiCompress(
  UINT8         *SrcBuf,
  UINT32        SrcDataSize,
  UINT8       type,
  UINT8     **OutBuffer,
  UINT32     *OutSize

  )
{
 
  UINT32        DstDataSize;
  EFI_STATUS    Status;
  
  UINT8         *DstBuf;

  // Pick the compress function based on compression type
  COMPRESS_FUNCTION CompressFunction;

  DstDataSize = 0;
  DstBuf = NULL;
  CompressFunction = NULL;

  //printf("Buffer %x %s", SrcDataSize, SrcBuf);
  
  if (type == 3) {
    CompressFunction = (COMPRESS_FUNCTION) LzmaCompress;
  } else {
    CompressFunction = (COMPRESS_FUNCTION) ((type == EFI_COMPRESSION) ? EfiCompress : TianoCompress);
  }
  //printf("[FIRST CALL] Requested %x\n", DstDataSize);
  Status = CompressFunction(SrcBuf, SrcDataSize, DstBuf, &DstDataSize);
  
  if (Status == EFI_BUFFER_TOO_SMALL) {
    // The first call to compress fills in the expected destination size.
	//printf("[SECOND CALL] Requested %x\n", DstDataSize);
    DstBuf = malloc (DstDataSize);
    if (!DstBuf) {
      goto ERROR;
    }
    // The second call to compress compresses.
    Status = CompressFunction(SrcBuf, SrcDataSize, DstBuf, &DstDataSize);
  }

  if (Status != EFI_SUCCESS) {
    //printf("Failed to compress %x\n", Status);
    //PyErr_SetString(PyExc_Exception, "Failed to compress\n");
    goto ERROR;
  }

  *OutBuffer = DstBuf;
  *OutSize = DstDataSize;

  return *OutBuffer;

ERROR:
  *OutBuffer = NULL;
  *OutSize = 0;
  return NULL;
}

/**

The following functions are semi-cyclic, they call a Python-abstraction that calls
replica version of the following two entry points. Each uses a cased short to determine
the huffman-decode implementation.

**/


//////////////////////////////////////////////////////////////////////////////////////////
// cod
//////////////////////////////////////////////////////////////////////////////////////////

#define EFI_COMPRESSION   1 //defined as PI_STD, section type= 0x01
#define TIANO_COMPRESSION 2 //not defined, section type= 0x01
#define LZMA_COMPRESSION  3 //not defined, section type= 0x02

UINT8 VolumeImageCompressed(UINT8 *buffer, UINT32 size);
PEFI_FFS_FILE_HEADER FindFileInVolume(PEFI_FIRMWARE_VOLUME_HEADER pFVH, PEFI_FFS_FILE_HEADER pFirstFile);
UINT8 *PtrToFreeSpace(PEFI_FIRMWARE_VOLUME_HEADER pFVH);
UINT32 FreeSpaceInVolume(PEFI_FIRMWARE_VOLUME_HEADER pFVH);
STATIC int find_volume(UINT8 *buffer, UINT32 size);

UINT16 calculateChecksum16(UINT16* buffer, UINT32 bufferSize)
{
    UINT16 counter = 0;
    UINT32 index = 0;

    if(!buffer)
        return 0;

    bufferSize /= sizeof(UINT16);

    for (; index < bufferSize; index++) {
        counter = (UINT16) (counter + buffer[index]);
    }

    return (UINT16) 0x10000 - counter;
}


UINT8 calculateChecksum8(UINT8* buffer, UINT32 bufferSize)
{
	UINT8 counter = 0;
    if(!buffer)
        return 0;
    
    while(bufferSize--)
        counter += buffer[bufferSize];

    return (UINT8) 0x100 - counter;
}

///////////////////////////////////////////////////////////////////////////////
EFI_STATUS
GetErasePolarity (
  IN EFI_FIRMWARE_VOLUME_HEADER *mFvHeader,
  OUT BOOLEAN   *ErasePolarity
  )
/*++

Routine Description:

  This function returns with the FV erase polarity.  If the erase polarity
  for a bit is 1, the function return TRUE.

Arguments:

  ErasePolarity   A pointer to the erase polarity.

Returns:

  EFI_SUCCESS              The function completed successfully.
  EFI_INVALID_PARAMETER    One of the input parameters was invalid.
  EFI_ABORTED              Operation aborted.
  
--*/
{
  EFI_STATUS  Status;

  //
  // Verify library has been initialized.
  //
  if (mFvHeader == NULL) {
    return EFI_ABORTED;
  }

  //
  // Verify input parameters.
  //
  if (ErasePolarity == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mFvHeader->Attributes & EFI_FVB2_ERASE_POLARITY) {
    *ErasePolarity = TRUE;
  } else {
    *ErasePolarity = FALSE;
  }

  return EFI_SUCCESS;
}


STATIC 
PEFI_COMMON_SECTION_HEADER GetFirstFileSection(PEFI_FFS_FILE_HEADER pFile)
{
	UINT32 length;
	UINT8 *pSection;

	length = efi_ffs_file_size(pFile);
	pSection = (UINT8 *) pFile;

	if (length == 0x00ffffff)
	{
		pSection += sizeof(EFI_FFS_FILE_HEADER2);
	}
	else
	{
		pSection += sizeof(EFI_FFS_FILE_HEADER);
	}

	return (PEFI_COMMON_SECTION_HEADER) pSection;
}


STATIC
int
find_volume(UINT8 *buffer, UINT32 size)
{
  UINT32 pos, count;
  PEFI_FIRMWARE_VOLUME_HEADER pFVH;
  
  pos = 0;
  count = 0;
  
  if (size <= sizeof(EFI_FIRMWARE_VOLUME_HEADER) || buffer == NULL)
	goto DONE;
  
  while(pos < size)
  {
    pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) &buffer[pos];
    
    if (pFVH->Signature == FVH_SIGNATURE)
      count++;
    pos += 16;
  }
 
DONE:
  printf("\n[find_volume] memory of 0x%x bytes contain %d volumes.", size, count);
  return count;  
}

UINT8
	IsFileVolume(UINT8 *buffer, UINT32 size)
{
	PEFI_FIRMWARE_VOLUME_HEADER pFVH;
	EFI_GUID FileVolumeGUID = { 0x7a9354d9, 0x0468, 0x444a, { 0x81, 0xce, 0x0b, 0xf6, 0x17, 0xd8, 0x90, 0xdf }};
	EFI_GUID UefiVolumeGUID = { 0x8c8ce578, 0x8a3d, 0x4f1c, { 0x99, 0x35, 0x89, 0x61, 0x85, 0xc3, 0x2d, 0xd3 }};
	pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) buffer;


	if (memcmp(&pFVH->GUID, &FileVolumeGUID, sizeof(EFI_GUID)) == 0)
		return 1;

	if (memcmp(&pFVH->GUID, &UefiVolumeGUID, sizeof(EFI_GUID)) == 0)
		return 1;

	return 0;
}



UINT8 *
ptr_to_volume(UINT8 *buffer, UINT32 size, UINT8 index)
{
  UINT32 pos, count;
  PEFI_FIRMWARE_VOLUME_HEADER pFVH;
  
  pos = 0;
  count = 0;
  
  if (size <= sizeof(EFI_FIRMWARE_VOLUME_HEADER))
    return NULL;

  if (buffer == NULL)
    return NULL;
  
  while(pos < size)
  {
    pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) &buffer[pos];
    
    if (pFVH->Signature == FVH_SIGNATURE)
	{
	  if (count == index)
		return (UINT8 *) pFVH;

      count++;
	}

    pos += 16;
  }
  
  return NULL;  
}

UINT32
volume_size(UINT8 *buffer, UINT32 size, UINT8 index)
{
  UINT32 pos, count;
  PEFI_FIRMWARE_VOLUME_HEADER pFVH;
  
  pos = 0;
  count = 0;
  
  if (size <= sizeof(EFI_FIRMWARE_VOLUME_HEADER))
    return count;

  if (buffer == NULL)
    return count;
  
  while(pos < size)
  {
    pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) &buffer[pos];
    
    if (pFVH->Signature == FVH_SIGNATURE)
	{
	  if (count == index)
		  return (UINT32) pFVH->FvLength;

      count++;
	}

    pos += 16;
  }
  
  return 0;  
}

void guid2str(char *szOut, EFI_GUID *guid)
{
	sprintf(szOut, "%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x",
		guid->Data1,
		guid->Data2,
		guid->Data3,
		guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3], guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);

}

STATIC UINT8 *unpack_lzma(UINT8 *SrcData, UINT32 SrcSize, UINT32 *DstSize)
{
	VOID          *Scratch;
	UINT32        ScratchSize, DestinationSize;
	UINT8         *DstData;

	ScratchSize = 0;
	DestinationSize = 0;
	Scratch = NULL;
	DstData = NULL;

	//printf("[in] LzmaGetInfo(%x, %x, DestinationSize, ScratchSize)\n", SrcData, SrcSize);
	//printf("[header] %02x%02x%02x%02x%02x\n", SrcData[0], SrcData[1], SrcData[2], SrcData[3], SrcData[4]);
	LzmaGetInfo(SrcData, SrcSize, &DestinationSize, &ScratchSize);
	//printf("[out] LzmaGetInfo(%x, %x, %x, %x)\n", SrcData, SrcSize, DestinationSize, ScratchSize);	
	if (DestinationSize == 0)
	{	// unsupported algo!
		//printf("LzmaGetInfo failed \n");
		return NULL;
	}


	*DstSize = DestinationSize;
	//Scratch = malloc(ScratchSize);
	DstData = malloc(DestinationSize);
	
	//printf("Decompress data!\n");
	LzmaDecompress(SrcData, SrcSize, DstData, DestinationSize, Scratch, ScratchSize);

	
	//free(Scratch);

	return DstData;
}

UINT8 *pack_lzma(UINT8 *SrcData, UINT32 SrcSize, UINT32 *DstSize)
{
	UINT8 *buffer;
	*DstSize = 0;

	if (LzmaCompress(SrcData, SrcSize, NULL, DstSize) != RETURN_BUFFER_TOO_SMALL)
	{
		*DstSize = 0;
		return NULL;
	}

	buffer = malloc(*DstSize);

	if (LzmaCompress(SrcData, SrcSize, buffer, DstSize) != RETURN_SUCCESS)
	{
		free(buffer);
		buffer = NULL;
	}

	return buffer;
}

UINT8 *pack_EfiCompress(UINT8 *SrcData, UINT32 SrcSize, UINT32 *DstSize)
{
	UINT8         *DstData;
	//EFI_TIANO_HEADER *header = SrcData;

	DstData = NULL;
	*DstSize = 0;

	//EfiGetInfo(SrcData, SrcSize, DstSize, &ScratchSize);
	
	if (EfiCompress(SrcData, SrcSize, NULL, DstSize) != RETURN_BUFFER_TOO_SMALL)
	{
		return NULL;
	}

	DstData = malloc(*DstSize);


	if (EfiCompress(SrcData, SrcSize, DstData, DstSize) != RETURN_SUCCESS)
	{
		free(DstData);
		return NULL;
	}

	return DstData;
}

UINT8 *unpack_EfiCompress(UINT8 *SrcData, UINT32 SrcSize, UINT32 *DstSize)
{
	VOID          *Scratch;
	UINT32        ScratchSize;
	UINT8         *DstData;
	//EFI_TIANO_HEADER *header = SrcData;

	ScratchSize = 0;
	Scratch = NULL;
	DstData = NULL;
	*DstSize = 0;

	EfiGetInfo(SrcData, SrcSize, DstSize, &ScratchSize);
	
	if (ScratchSize == 0)
	{	// unsupported algo!
		return NULL;
	}

	Scratch = malloc(ScratchSize);
	DstData = malloc(*DstSize);

	if (EfiDecompress((VOID *) SrcData, SrcSize, (VOID *) DstData, *DstSize, Scratch, ScratchSize) != RETURN_SUCCESS)
	{
		TianoDecompress((VOID *) SrcData, SrcSize, (VOID *) DstData, *DstSize, Scratch, ScratchSize);
	}

	if (Scratch != NULL)
		free(Scratch);

	return DstData;
}

void volume_scan(UINT8 *buffer, UINT32 size);


PEFI_FFS_FILE_HEADER LookupFileInVolume(PEFI_FIRMWARE_VOLUME_HEADER pFVH, UINT32 Index)
{
	PEFI_FFS_FILE_HEADER pPtr;
	UINT8 *RawPtr;

	RawPtr = (UINT8 *) pFVH;
	pPtr = NULL;
	pPtr = FindFileInVolume(pFVH, pPtr);
	while(Index > 0)
	{
		pPtr = FindFileInVolume(pFVH, pPtr);
		Index--;
	}
	
	return pPtr;
}

UINT8 *OpenVolume(UINT8 *buffer, UINT32 size, UINT8 VolumeIndex, UINT16 FileIndex, UINT32 *DstSize)
{
	UINT32 fsize;
	UINT32 buffer_size;
	UINT8 *pData, *compressedSection;
	PEFI_COMMON_SECTION_HEADER pCommonSectionHeader;
	PEFI_FIRMWARE_VOLUME_HEADER pVol = NULL;
	int decompressedsize, tmpDestSize;
	UINT8 *decompressedData, *tmp;
	EFI_COMPRESSION_SECTION* compressedSectionHeader;
	PEFI_FFS_FILE_HEADER pFirstFile;
	
	pVol = (PEFI_FIRMWARE_VOLUME_HEADER) ptr_to_volume(buffer, size, VolumeIndex);

	if (pVol == NULL)
	{	// cannot find volume!
		goto ERROR;
	}

	if (FileIndex == 0xffff)
	{	// FileIndex -1
		decompressedData = malloc(pVol->FvLength);
		memset(decompressedData, 0, pVol->FvLength);

		memcpy(decompressedData, pVol, pVol->FvLength);

		*DstSize = pVol->FvLength;
		return decompressedData;
	}

	pFirstFile = LookupFileInVolume(pVol, FileIndex);
	
	pData = (UINT8 *) pFirstFile;
	fsize = Expand24bit(pFirstFile->Size);

	if (fsize == 0x00ffffff)
	{
		pData += sizeof(EFI_FFS_FILE_HEADER2);
	}
	else
	{
		//fwrite(pData + sizeof(EFI_FFS_FILE_HEADER), fsize - sizeof(EFI_FFS_FILE_HEADER), 1, f);
		pData += sizeof(EFI_FFS_FILE_HEADER);
	}

	pCommonSectionHeader = (PEFI_COMMON_SECTION_HEADER) pData;
	if (pCommonSectionHeader->Type == EFI_SECTION_COMPRESSION)
	{	// Compressed!
		pCommonSectionHeader = (PEFI_COMMON_SECTION_HEADER) pData;

		if (pCommonSectionHeader->Type == EFI_SECTION_COMPRESSION)
		{	// Compressed!
			compressedSectionHeader = (EFI_COMPRESSION_SECTION *) pCommonSectionHeader;

			if (compressedSectionHeader->CompressionType == EFI_STANDARD_COMPRESSION)
			{
			}

			pData = pData + sizeof(EFI_COMPRESSION_SECTION);
			decompressedData = unpack_lzma(pData, (int) Expand24bit(compressedSectionHeader->Size), &tmpDestSize);

			if (decompressedData == NULL)
			{
				decompressedData = unpack_EfiCompress(pData, (int) Expand24bit(compressedSectionHeader->Size), &tmpDestSize);
				
				if (decompressedData == NULL)
				{	// unknown algo!
					goto ERROR;
				}
			}

			if (decompressedData != NULL)
			{

			}
		}
	}

	*DstSize = tmpDestSize;
	return decompressedData;

ERROR:
	*DstSize = 0;
	return NULL;
}

UINT8 *CloseVolume(UINT8 *fvVolume, UINT32 fvVolumeSize, UINT8 *buffer, UINT32 size, UINT8 VolumeIndex, UINT16 FileIndex, UINT32 *DstVolumeSize)
{
	UINT32 fsize;
	UINT32 buffer_size;
	UINT8 *pData, *compressedSection;
	PEFI_COMMON_SECTION_HEADER pCommonSectionHeader;
	PEFI_FIRMWARE_VOLUME_HEADER pVol = NULL;
	int decompressedsize, tmpDestSize;
	UINT8 *newVolume, *tmp;
	EFI_COMPRESSION_SECTION* compressedSectionHeader, *newCompressedSectionHeader;
	PEFI_FFS_FILE_HEADER pFirstFile, pUpdateFile;
	UINT32 copy_size, header_file_length;
    BOOLEAN ErasePolarity;
	
	pVol = (PEFI_FIRMWARE_VOLUME_HEADER) ptr_to_volume(fvVolume, fvVolumeSize, VolumeIndex);
	newVolume = NULL;
	
	if (pVol == NULL)
	{	// cannot find volume!
		goto ERROR;
	}

	if (FileIndex == 0xffff)
	{	// not compressed! return a copy of volume
		newVolume = malloc(fvVolumeSize);
		memcpy(newVolume, buffer, size);
		*DstVolumeSize = size;
	}
	else
	{
		pFirstFile = LookupFileInVolume(pVol, FileIndex);
		pData = (UINT8 *) pFirstFile;

		fsize = Expand24bit(pFirstFile->Size);

		if (fsize == 0x00ffffff)
		{
			pData += sizeof(EFI_FFS_FILE_HEADER2);
		}
		else
		{
			//fwrite(pData + sizeof(EFI_FFS_FILE_HEADER), fsize - sizeof(EFI_FFS_FILE_HEADER), 1, f);
			pData += sizeof(EFI_FFS_FILE_HEADER);
		}

		pCommonSectionHeader = (PEFI_COMMON_SECTION_HEADER) pData;
		if (pCommonSectionHeader->Type == EFI_SECTION_COMPRESSION)
		{	// Compressed!
			pCommonSectionHeader = (PEFI_COMMON_SECTION_HEADER) pData;

			if (pCommonSectionHeader->Type == EFI_SECTION_COMPRESSION)
			{	// Compressed!
				compressedSectionHeader = (EFI_COMPRESSION_SECTION *) pCommonSectionHeader;

				if (compressedSectionHeader->CompressionType == EFI_CUSTOMIZED_COMPRESSION)
				{
					//pData = pData + sizeof(EFI_COMPRESSION_SECTION);
					tmp = pack_lzma(buffer, size, &tmpDestSize);
				}
				else if (compressedSectionHeader->CompressionType == EFI_STANDARD_COMPRESSION)
				{
					tmp = pack_EfiCompress(buffer, size, &tmpDestSize);
				}
				else if (compressedSectionHeader->CompressionType == EFI_NOT_COMPRESSED)
				{
					// ?d?d?d
				}
			}
		}

		GetErasePolarity(pVol, &ErasePolarity);
		
		newVolume = malloc(fvVolumeSize);
		memset(newVolume, (ErasePolarity) ? 0xff : 0x00, pVol->FvLength);

		copy_size = (UINT8 *) pCommonSectionHeader - fvVolume;
		memcpy(newVolume, fvVolume, copy_size);

		newCompressedSectionHeader = (EFI_COMPRESSION_SECTION *)(newVolume + copy_size);
		pUpdateFile = LookupFileInVolume((PEFI_FIRMWARE_VOLUME_HEADER) newVolume, FileIndex);
		memcpy(newVolume + copy_size + sizeof(EFI_COMPRESSION_SECTION), tmp, tmpDestSize);

		memcpy(newCompressedSectionHeader, compressedSectionHeader, sizeof(EFI_COMPRESSION_SECTION));
		//newCompressedSectionHeader->CompressionType = compressedSectionHeader->CompressionType;

		// length of file header
		header_file_length = (UINT8*) compressedSectionHeader - (UINT8 *)pFirstFile;

		Pack24bit(tmpDestSize + sizeof(EFI_COMPRESSION_SECTION), newCompressedSectionHeader->Size);
		
		Pack24bit(tmpDestSize + sizeof(EFI_COMPRESSION_SECTION) +  header_file_length, pUpdateFile->Size);

        pUpdateFile->IntegrityCheck.Checksum.Header = 0;
        pUpdateFile->IntegrityCheck.Checksum.File = 0;
        pUpdateFile->IntegrityCheck.Checksum.Header = calculateChecksum8((UINT8*)pUpdateFile, sizeof(EFI_FFS_FILE_HEADER)-1);
		if (pUpdateFile->Attributes & FFS_ATTRIB_CHECKSUM)
			pUpdateFile->IntegrityCheck.Checksum.File = calculateChecksum8((UINT8*) pUpdateFile + sizeof(EFI_FFS_FILE_HEADER), 
			tmpDestSize + sizeof(EFI_COMPRESSION_SECTION));

		*DstVolumeSize = size;
	}

	*DstVolumeSize = fvVolumeSize;
	return newVolume;

ERROR:
	*DstVolumeSize = 0;
	if (newVolume != NULL)
		free(newVolume);

	return NULL;
}

UINT8* VolumeHeader(UINT8 *buffer, UINT32 size)
{
  PEFI_FIRMWARE_VOLUME_HEADER pFVH;
  UINT32 pos, count;  
 
  pos = 0;
  count = 0;
  
  if (size <= sizeof(EFI_FIRMWARE_VOLUME_HEADER) || buffer == NULL)
    return NULL;
  
  while(pos < size)
  {
    pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) &buffer[pos];
    
    if (pFVH->Signature == FVH_SIGNATURE)
      return (UINT8*) pFVH;
    pos += 4;
  }
  
  return NULL;
}


int PushFileInVolume(UINT8 *buffer, UINT32 size, UINT8 *NewFile, UINT32 fileSize)
{
	PEFI_FIRMWARE_VOLUME_HEADER pFVH;
	UINT32 FreeSpace;
	UINT8 *Ptr;
    BOOLEAN ErasePolarity;
	EFI_FFS_FILE_HEADER *FfsFileHeader;
	UINT32 HeaderSize;
	UINT8* FileBuffer;
    EFI_FFS_FILE_STATE OldState;

    pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) VolumeHeader(buffer, size);	// move to header! (skip section header)
    FreeSpace = FreeSpaceInVolume(pFVH);

	GetErasePolarity(pFVH, &ErasePolarity);	// get polarity of volume!


	if (FreeSpace < fileSize)
	{
		printf("\nPushFileInVolume - Required space %x available %x", fileSize, FreeSpace);
		return -1;
	}

	Ptr = PtrToFreeSpace(pFVH);

	if (Ptr == NULL)
	{
		printf("\nPushFileInVolume - Lookup of free space failed!");
		return -1;
	}

	memcpy(Ptr, NewFile, fileSize);

	FfsFileHeader = (EFI_FFS_FILE_HEADER *) (Ptr);

	FfsFileHeader->Attributes |= FFS_ATTRIB_CHECKSUM;

	if (efi_ffs_file_size(FfsFileHeader) >= MAX_FFS_SIZE)
	{
		FileBuffer = (UINT8 *) (NewFile + sizeof(EFI_FFS_FILE_HEADER2));
		HeaderSize = sizeof(EFI_FFS_FILE_HEADER2);
	}
	else
	{
		FileBuffer = (UINT8 *) (NewFile + sizeof(EFI_FFS_FILE_HEADER));
		HeaderSize = sizeof(EFI_FFS_FILE_HEADER);
	}

	// save old state...
    OldState = FfsFileHeader->State;
    
  //
  // Fill in checksums and state, these must be zero for checksumming
  //
  // FileHeader.IntegrityCheck.Checksum.Header = 0;
  // FileHeader.IntegrityCheck.Checksum.File = 0;
  // FileHeader.State = 0;

	FfsFileHeader->State = 0;
	FfsFileHeader->IntegrityCheck.Checksum16 = 0;

	FfsFileHeader->IntegrityCheck.Checksum.Header = CalculateChecksum8((UINT8 *) FfsFileHeader, HeaderSize);

	if (FfsFileHeader->Attributes & FFS_ATTRIB_CHECKSUM)
	{
		FfsFileHeader->IntegrityCheck.Checksum.File = CalculateChecksum8(FileBuffer, efi_ffs_file_size(FfsFileHeader) - HeaderSize);
	}

	if (ErasePolarity != 0)
	{
	  FfsFileHeader->State = ~OldState;
	}
	else
	{  // Set hightest bit?
      FfsFileHeader->State = OldState;
	}
	return 0;
}

int PopFileInVolume(UINT8 *buffer, UINT32 size, UINT8 *NewFile, UINT32 fileSize)
{
	PEFI_FIRMWARE_VOLUME_HEADER pFVH;
	UINT32 FreeSpace;
	UINT8 *Ptr;
    BOOLEAN ErasePolarity;
	EFI_FFS_FILE_HEADER *FfsFileHeader;
	EFI_FFS_FILE_HEADER *pFfsFileCursor;
	UINT32 HeaderSize;
	UINT8* FileBuffer;
    EFI_FFS_FILE_STATE OldState;
	int file_found;

    pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) VolumeHeader(buffer, size);	// move to header! (skip section header)

	GetErasePolarity(pFVH, &ErasePolarity);	// get polarity of volume!

	FfsFileHeader = (EFI_FFS_FILE_HEADER *) (NewFile);

	pFfsFileCursor = FindFileInVolume(pFVH, NULL);

	file_found = 0;

	while(pFfsFileCursor != NULL && file_found == 0)
	{
		if (memcmp(&pFfsFileCursor->Name, &FfsFileHeader->Name, sizeof(EFI_GUID)) == 0)
		{	// file found!
			EFI_FFS_FILE_HEADER *pFirstFile;
			UINT32 filesize;

			pFirstFile = FindFileInVolume(pFVH, pFfsFileCursor);
			Ptr = PtrToFreeSpace(pFVH);
			filesize = efi_ffs_file_size(pFfsFileCursor);

			// alignment
			if ((filesize % 8) != 0)
				filesize = (filesize & 0xfffffff8) + 0x08;

			if (pFirstFile == NULL)
			{	// last file in volume to delete
				if (ErasePolarity)
					memset(pFfsFileCursor, 0xff, efi_ffs_file_size(pFfsFileCursor));
				else
					memset(pFfsFileCursor, 0x00, efi_ffs_file_size(pFfsFileCursor));
			}
			else
			{
				memcpy(pFfsFileCursor, pFirstFile, Ptr - (UINT8 *) pFirstFile);
				Ptr -= filesize;	// 

				if (ErasePolarity)
					memset(Ptr, 0xff, filesize);
				else
					memset(Ptr, 0x00, filesize);
			}

			file_found = 1;
		}
		pFfsFileCursor = FindFileInVolume(pFVH, pFfsFileCursor);
	}

	return file_found;
}

/*void volume_scan(UINT8 *buffer, UINT32 size)
{
	char szFileNameGUID[40];

	PEFI_FIRMWARE_VOLUME_HEADER pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) buffer;
	UINT32 FvLength = pFVH->FvLength;

	UINT32 NoFiles = 0, FreeSpace = 0;
	/*switch(VerifyFv(pFVH))
	{
		case -1: printf("invalid FV header signature\n"); break;
		case -2: printf("invalid FV header checksum\n"); break;
		default: break;
	}*/

	/*guid2str(szFileNameGUID, &pFVH->GUID);

	NoFiles  = CountFileInVolume(pFVH);
	FreeSpace = FreeSpaceInVolume(pFVH);

	printf("\nEFI FIRMWARE VOLUME %s\n\t Length: %08x", szFileNameGUID, (int) pFVH->FvLength);
	printf("\nFile Count: %x\tFree Space %x", NoFiles, FreeSpace);
	
	file_scan(pFVH, (char *) pFVH + pFVH->HeaderLength, (int) pFVH->FvLength - pFVH->HeaderLength);	// recursive scan!
	//volume_dump(pFVH);
}*/

PEFI_FFS_FILE_HEADER FindFileInVolume(PEFI_FIRMWARE_VOLUME_HEADER pFVH, PEFI_FFS_FILE_HEADER pFirstFile)
{
	PEFI_FFS_FILE_HEADER pPtr;
	UINT8 *RawPtr;
	UINT32 fsize;
	UINT32 next_pos;

	RawPtr = (UINT8 *) pFVH;

	if (pFirstFile == NULL)
		pPtr = (PEFI_FFS_FILE_HEADER) &RawPtr[pFVH->HeaderLength];
	else
	{
		pPtr = pFirstFile;  
		fsize = efi_ffs_file_size(pFirstFile);
		next_pos = (UINT32) ((UINT8*)pPtr - (UINT8*) pFVH);

		// move to next zone
		if ((fsize % 8) == 0)
			next_pos += fsize;	
		else
			next_pos += (fsize & 0xfffffff8) + 0x08;

		if (next_pos >= pFVH->FvLength)
			pPtr = NULL;
		else
			pPtr = (PEFI_FFS_FILE_HEADER) &RawPtr[next_pos];
	}

	return pPtr;
}

UINT32 CountFileInVolume(PEFI_FIRMWARE_VOLUME_HEADER pFVH)
{
	PEFI_FFS_FILE_HEADER pPtr;
	UINT8 *RawPtr, EndPtr;
	UINT32 count = 0;
	EFI_GUID endFF, end00;

	memset(&endFF, 0xff, sizeof(EFI_GUID));
	memset(&end00, 0x0, sizeof(EFI_GUID));

	pPtr = NULL;
	do
	{
		pPtr = FindFileInVolume(pFVH, pPtr);

		if (pPtr != NULL)
		{
			if (memcmp(&pPtr->Name, &endFF, sizeof(EFI_GUID)) == 0 || memcmp(&pPtr->Name, &end00, sizeof(EFI_GUID)) == 0)
			{
				pPtr = NULL;
			}
			else
				count++;
		}
	} while(pPtr != NULL);

	return count;
}

UINT32 FreeSpaceInVolume(PEFI_FIRMWARE_VOLUME_HEADER pFVH)
{
	PEFI_FFS_FILE_HEADER pPtr;
	UINT32 VolumeSpace, VolumeFree;
	UINT32 FileSize;

	UINT8 *RawPtr, EndPtr;
	EFI_GUID endFF, end00;

	memset(&endFF, 0xff, sizeof(EFI_GUID));
	memset(&end00, 0x0, sizeof(EFI_GUID));

	VolumeSpace = pFVH->FvLength;
	VolumeFree = pFVH->FvLength;

	VolumeFree -= pFVH->HeaderLength;	// subtracts header length

	pPtr = NULL;
	do
	{
		pPtr = FindFileInVolume(pFVH, pPtr);

		if (pPtr != NULL)
		{
			if (memcmp(&pPtr->Name, &endFF, sizeof(EFI_GUID)) == 0 || memcmp(&pPtr->Name, &end00, sizeof(EFI_GUID)) == 0)
			{
				pPtr = NULL;
			}
			else
			{
				FileSize = efi_ffs_file_size(pPtr);
				
				if ((FileSize % 8) != 0)
					FileSize = (FileSize & 0xfffffff8) + 0x08;
				
				VolumeFree -= FileSize;
			}
		}
	} while(pPtr != NULL);

	return VolumeFree;
}

UINT8 *PtrToFreeSpace(PEFI_FIRMWARE_VOLUME_HEADER pFVH)
{
	PEFI_FFS_FILE_HEADER pPtr;
	UINT32 VolumeSpace, VolumeFree;
	UINT32 FileSize;

	UINT8 *RawPtr, EndPtr;
	EFI_GUID endFF, end00;

	memset(&endFF, 0xff, sizeof(EFI_GUID));
	memset(&end00, 0x0, sizeof(EFI_GUID));

	pPtr = NULL;
	do
	{
		pPtr = FindFileInVolume(pFVH, pPtr);

		if (pPtr != NULL)
		{
			if (memcmp(&pPtr->Name, &endFF, sizeof(EFI_GUID)) == 0 || memcmp(&pPtr->Name, &end00, sizeof(EFI_GUID)) == 0)
			{
				return (UINT8*)pPtr;
			}
		}
	} while(pPtr != NULL);

	return NULL;
}

UINT8 VolumeImageCompressed(UINT8 *buffer, UINT32 size)
{
	PEFI_FIRMWARE_VOLUME_HEADER pFVH = (PEFI_FIRMWARE_VOLUME_HEADER) buffer;
	//UINT32 FvLength = (UINT32) pFVH->FvLength;
	PEFI_FFS_FILE_HEADER pFirstFile;
	int pos=0;
	UINT32 fsize;
	UINT32 buffer_size;
	UINT8 *pData, *compressedSection;
	PEFI_COMMON_SECTION_HEADER pCommonSectionHeader;
	//PEFI_FIRMWARE_VOLUME_HEADER pVol = NULL;
	int newsize, section_compressed;
	UINT8 *newdata;
	EFI_COMPRESSION_SECTION* compressedSectionHeader;

	pos = pFVH->HeaderLength;	// first file after 

	pFirstFile = FindFileInVolume(pFVH, NULL);
	pData = (UINT8 *) pFirstFile;

	fsize = Expand24bit(pFirstFile->Size);

	if (fsize == 0x00ffffff)
	{
		pData += sizeof(EFI_FFS_FILE_HEADER2);
	}
	else
	{
		//fwrite(pData + sizeof(EFI_FFS_FILE_HEADER), fsize - sizeof(EFI_FFS_FILE_HEADER), 1, f);
		pData += sizeof(EFI_FFS_FILE_HEADER);
	}

	pCommonSectionHeader = (PEFI_COMMON_SECTION_HEADER) pData;
	if (pCommonSectionHeader->Type == EFI_SECTION_COMPRESSION)
	{	// Compressed!
			return 1;
	}

	return 0;
}

UINT8 LookupDxeImage(UINT8 *buffer, UINT32 size, UINT8 *IndexOut, UINT16 *FileIndex)
{
	UINT8 MaxVolume = (UINT8) find_volume(buffer, size);
	UINT8 i;
	UINT8 *VolumePtr;
	UINT32 VolumeSize, c;
			
	if (MaxVolume == 0)
	{
		goto ERROR;
	}

	for(i = 0; i < MaxVolume; i++)
	{
		UINT32 FileInVolume;
		PEFI_FIRMWARE_VOLUME_HEADER pVol;
		PEFI_FFS_FILE_HEADER pFirstFile;

		VolumePtr = ptr_to_volume(buffer, size, i);
		VolumeSize = volume_size(buffer, size, i);

		if (VolumePtr == NULL || VolumeSize == 0)
			continue;

		if (IsFileVolume(VolumePtr, VolumeSize) == 0)
			continue;
		
		// Volume may be candidate
		FileInVolume = CountFileInVolume((PEFI_FIRMWARE_VOLUME_HEADER) VolumePtr);

		pVol = (PEFI_FIRMWARE_VOLUME_HEADER) VolumePtr;
		
		pFirstFile = NULL;

		for(c = 0; c < FileInVolume; c++)
		{
			UINT8 *pData;
			PEFI_COMMON_SECTION_HEADER pSection;
			EFI_COMPRESSION_SECTION *compressedSectionHeader;

			UINT32 fsize;

			pFirstFile = FindFileInVolume(pVol, pFirstFile);
			pData = (UINT8 *) pFirstFile;

			fsize = Expand24bit(pFirstFile->Size);

			pSection = GetFirstFileSection(pFirstFile);

			if (pSection->Type == EFI_SECTION_COMPRESSED)
			{
				*IndexOut = i;
				*FileIndex = c;
				return 1;
			}

			/*if (pFirstFile->Type == EFI_FV_FILETYPE_DXE_CORE)
			{	// candidate .. may be uncompressed!
				
				*IndexOut = i;
				*FileIndex = -1;
				return TRUE;
			}
			else*/ if (pFirstFile->Type == EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE)
			{	// EFI_FV_FILETYPE_DXE_CORE
				// section
				*IndexOut = i;
				*FileIndex = c;
				return 1;
			}
			else if (pFirstFile->Type == EFI_FV_FILETYPE_DRIVER)
			{
				*IndexOut = i;
				*FileIndex = (UINT16) 0xffff;
				return 1;
			}
		}
	}

ERROR:	// no dxe found
	*IndexOut = 0;
	*FileIndex = 0;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Python stubs

UINT32
Py_FvVolumeCount(
  UINT8         *SrcBuf,
  UINT32        SrcDataSize
  )
{
    
  UINT32        DstDataSize;
  EFI_STATUS    Status;


  // Pick the compress function based on compression type
  COMPRESS_FUNCTION CompressFunction;

  DstDataSize = 0;

  //printf("Scanning buffer...");
  DstDataSize = find_volume(SrcBuf, SrcDataSize);
  
  return DstDataSize;
}

///////////////////////////////////////////////////////////////////////////
//
STATIC
UINT32
Py_OpenVolume(
  UINT8         *SrcBuf,
  UINT32        SrcDataSize,
  UINT32		VolumeIndex
  )
{
  UINT32        DstDataSize;
  EFI_STATUS    Status;
  
  UINT8			*VolumePtr;
  UINT32		VolumeSize;


  DstDataSize = 0;

  //printf("Buffer %x %s", SrcDataSize, SrcBuf);
  
  VolumePtr =  ptr_to_volume(SrcBuf, SrcDataSize, VolumeIndex);
  VolumeSize = volume_size(SrcBuf, SrcDataSize, VolumeIndex);

  if (VolumePtr == NULL || VolumeSize == 0)
  {
	  //printf("Volume not found!");
	  goto ERROR;
  }

  if (IsFileVolume(VolumePtr, SrcDataSize) == 0)
  {		// unknown volume!
	 // printf("Unknown Volume File System");
	  goto ERROR;
  }

  DstDataSize = find_volume(SrcBuf, SrcDataSize);

  return DstDataSize;

ERROR:

  return 0;

}

/////////////////////////////////////////////////////////////////////
//
UINT8
Py_FvVolumeImageCompressed(
  UINT8         *SrcBuf,
  UINT32        SrcDataSize,
  UINT32        VolumeIndex
  )
{
	return VolumeImageCompressed(SrcBuf, SrcDataSize);
}

UINT32
Py_FvVolumeSize(
  UINT8         *SrcBuf,
  UINT32        SrcDataSize,
  UINT32        VolumeIndex
  )
{
  UINT32        Result;
 
  Result = volume_size(SrcBuf, SrcDataSize, VolumeIndex);
	return Result;
}


UINT32
Py_FvVolumeAddress(
  UINT8         *SpiBuffer,
  UINT32        SpiDataSize,
  UINT32        VolumeIndex
  )
{
  EFI_STATUS    Status;
  UINT8 *VolumePtr;
  UINT32        Result;
  
 
  //printf("Scanning buffer...");
  VolumePtr = ptr_to_volume(SpiBuffer, SpiDataSize, VolumeIndex);
  
  if (VolumePtr != NULL)
  {
    Result = (UINT32) (VolumePtr - SpiBuffer);
  }
  else
  {
    Result = 0xffffffff;
  }
  
  return Result;

}

EFI_STATUS 
Py_LookupDxeImage(
  UINT8         *SpiBuffer,
  UINT32        SpiDataSize,
  UINT32 *VolHandle
  )
{
  EFI_STATUS    Status;
  UINT8 *VolumePtr;
  UINT16		FileIndex;
  UINT8			VolumeIndex;
  UINT32		Mask24;
  UINT16		Mask16;

  //printf("Buffer %x %s", SrcDataSize, SrcBuf);
  
  //printf("Scanning buffer...");
  if (LookupDxeImage(SpiBuffer, SpiDataSize, &VolumeIndex, &FileIndex))
  {
    //printf("[FV.Dxe] return %x %x\n", VolumeIndex, FileIndex);
    Mask24 = (UINT32) VolumeIndex << 16;
	Mask16 = (UINT32) FileIndex;
    *VolHandle = Mask24 | Mask16;
	//printf("[FV.Dxe] return %x\n", *VolHandle);
	Status = EFI_SUCCESS;
  }
  else
  {
    printf("LookupDxeImage fail\n");
    *VolHandle = 0xff000000;
	Status = EFI_INVALID_PARAMETER;
  }
  
  return Status;
}


UINT8
Py_FvIsFileVolume(
  UINT8         *SpiBuffer,
  UINT32        SpiDataSize,
  UINT32        VolumeIndex
  )
{
  EFI_STATUS    Status;
  UINT8         *VolumePtr;
  UINT32        Result;
  
  //printf("Scanning buffer...");
  VolumePtr = ptr_to_volume(SpiBuffer, SpiDataSize, VolumeIndex);
  
  if (VolumePtr != NULL)
  {
    Result = 1;
  }
  else
  {
    Result = 0;
  }
  
  
  return Result;
}

EFI_STATUS 
 Py_VolumeFromHandle(
   UINT32        VolumeHandle,
   UINT32       *VolIndex
 )
{
  UINT8			VolumeIndex;
  UINT16		FileIndex;
  
  // Pick the compress function based on compression type
 
  if ((VolumeHandle & 0xff000000) != 0)
  {
    //printf("Error to access to volume handle");
//	PyErr_SetString(PyExc_Exception, "handle not valid!\n");
    goto ERROR;
  }
  
  VolumeIndex = (UINT8) (VolumeHandle >> 16);
  FileIndex = (UINT16) (VolumeHandle & 0xffff);
  
  *VolIndex = (UINT32) VolumeIndex;
  return EFI_SUCCESS;

ERROR:
  return EFI_INVALID_PARAMETER;

}

EFI_STATUS 
Py_FvOpenVolume(
  UINT8      *SpiBuffer,
  UINT32      SpiLength,
  UINT32      VolumeHandle,
  UINT8     **OutBuffer,
  UINT32     *OutSize
  )
{
  UINT32        NewBufferLength;
  EFI_STATUS    Status;
  UINT8         *NewBuffer;
  UINT32        Result;
  UINT8			VolumeIndex;
  UINT16		FileIndex;
  
  // Pick the compress function based on compression type
  Status = EFI_INVALID_PARAMETER;
  
  if ((VolumeHandle & 0xff000000) != 0)
  {
    printf("VolumeHandle invalid!\n");
//	PyErr_SetString(PyExc_Exception, "OpenVolume handle not valid!\n");
    goto ERROR;
  }
  
  VolumeIndex = (UINT8) (VolumeHandle >> 16);
  FileIndex = (UINT16) (VolumeHandle & 0xffff);
  
  NewBuffer = OpenVolume(SpiBuffer, SpiLength, VolumeIndex, FileIndex, &NewBufferLength);

  *OutBuffer = NewBuffer;
  *OutSize = NewBufferLength;

  Status = EFI_SUCCESS;

  return Status;

ERROR:
  *OutBuffer = NULL;
  *OutSize = 0;

  return Status;
}

///////////////////////////////////////////////////////////////////////////////
// FvVolumeAddFile(volume, volumelength, newfile, newfilelength)
// 	volume : PyObject -> string buffer
//	volume length : length
//	newfile : PyObject -> string buffer
//	newfilelength: length
EFI_STATUS
Py_FvVolumeAddFile(
  UINT8         *VolumeBuffer,
  UINT32        VolumeLength,
  UINT8         *NewFileBuffer,
  UINT32        NewFileLength
  )
{
  EFI_STATUS    Status;
  UINT32        Result;
  
  // Pick the compress function based on compression type
  Result = 0xffffffff;
  
  //printf("Scanning buffer...");
  if (!PushFileInVolume(VolumeBuffer, VolumeLength, NewFileBuffer, NewFileLength))
  {
	  Status = EFI_SUCCESS;
  }
  else
  {
	  Status = EFI_INVALID_PARAMETER;
  }
  
  return Status;

}

///////////////////////////////////////////////////////////////////////////////
// FvVolumeAddFile(volume, volumelength, newfile, newfilelength)
// 	volume : PyObject -> string buffer
//	volume length : length
//	newfile : PyObject -> string buffer
//	newfilelength: length
EFI_STATUS
Py_FvDeleteFileFromVolume(
  UINT8         *VolumeBuffer,
  UINT32        VolumeLength,
  UINT8         *NewFileBuffer,
  UINT32        NewFileLength
  )
{
  EFI_STATUS    Status;
  UINT32        Result;
  
  // Pick the compress function based on compression type
  Result = 0xffffffff;
  
  //printf("Scanning buffer...");
  if (PopFileInVolume(VolumeBuffer, VolumeLength, NewFileBuffer, NewFileLength))
  {
	  Status = EFI_SUCCESS;
  }
  else
  {
	  Status = EFI_INVALID_PARAMETER;
  }
  
  return Status;

}
EFI_STATUS
Py_FvCloseVolume(
  UINT8         *VolumeBuffer,
  UINT32        VolumeLength,
  UINT8         *DxeFileBuffer,
  UINT32        DxeLength,
  UINT32        VolumeHandle,
  UINT8     **OutBuffer,
  UINT32     *OutSize

  )
{
  UINT32        NewBufferLength;
  EFI_STATUS    Status;
  UINT8        *NewBuffer;
  UINT32        Result;
  UINT8			VolumeIndex;
  UINT16		FileIndex;
  
  // Pick the compress function based on compression type
  Result = 0xffffffff;

  if ((VolumeHandle & 0xff000000) != 0)
  {
    //printf("Error to access to volume handle");
//	PyErr_SetString(PyExc_Exception, "OpenVolume handle not valid!\n");
	Status = EFI_INVALID_PARAMETER;
    goto ERROR;
  }
  
  VolumeIndex = (UINT8) (VolumeHandle >> 16);
  FileIndex = (UINT16) (VolumeHandle & 0xffff);
  
   printf("[FvCloseVolume](%x, %x, %x, %x, %x, NewBufferLength)\n", VolumeBuffer, VolumeLength, DxeFileBuffer, DxeLength, VolumeIndex);
	
  //printf("Buffer %x %s", SrcDataSize, SrcBuf);
  
  printf("Scanning buffer...");
  // CloseVolume -> VolumeIndex must be 0
  NewBuffer = CloseVolume(VolumeBuffer, VolumeLength, DxeFileBuffer, DxeLength, 0, FileIndex, &NewBufferLength);
  printf("[FvCloseVolume]CloseVolume %x %x\n", NewBuffer, NewBufferLength);

  *OutBuffer = NewBuffer;
  *OutSize = NewBufferLength;

  Status = EFI_SUCCESS;
  goto DONE;

ERROR:
  *OutBuffer = NULL;
  *OutSize = 0;
  printf("[FvCloseVolume] error!\n");
DONE:
  return Status;
}

EFI_STATUS 
Py_FvVolumeGet(
  UINT8         *SpiBuffer,
  UINT32        SpiDataSize,
  UINT32        VolumeIndex,
  UINT8     **OutBuffer,
  UINT32     *OutSize

  )
{
  UINT32        VolumeSize;
  EFI_STATUS    Status;
  UINT8 *VolumePtr, *DstPtr;
  EFI_STATUS    Result;
  
  
  Status = EFI_INVALID_PARAMETER;

  //printf("Scanning buffer...");
  VolumePtr = ptr_to_volume(SpiBuffer, SpiDataSize, VolumeIndex);
  VolumeSize = volume_size(SpiBuffer, SpiDataSize, VolumeIndex);
  
  if (VolumePtr != NULL)
  {
 //   printf("allocation of volume!\n");
    DstPtr = malloc(VolumeSize);
	memcpy(DstPtr, VolumePtr, VolumeSize);

	Result = EFI_SUCCESS;
  }
  else
  {
//    printf("ptr_to_volume failed!\n");
    DstPtr = NULL;
  }
  
  *OutBuffer = DstPtr;
  *OutSize = VolumeSize;
  return Result;
}
