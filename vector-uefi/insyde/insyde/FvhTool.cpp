#define _CRT_SECURE_NO_WARNINGS 1

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include "lzmadec.h"

#include "UefiTypes.h"
#include "PiFirmwareFile.h"
#include "CommonLib.h"
#include "FvLib.h"
#include "macro.h"

#define FVH_SIGNATURE 0x4856465f

extern DWORD efi_ffs_file_size(PEFI_FFS_FILE_HEADER pFile);
extern void* blob_dump(char *outputdir, PEFI_FFS_FILE_HEADER pFile);

PEFI_FFS_FILE_HEADER volume_lookup_file(PEFI_FIRMWARE_VOLUME_HEADER pVolume, EFI_GUID *FileName)
{
	if (pVolume == NULL || FileName == NULL)
		return NULL;

	PEFI_FFS_FILE_HEADER pFile;


	DWORD size = (DWORD) pVolume->FvLength;
	char *VolumeHiLimit = CALC_OFFSET(char *, pVolume, size);
	char *VolumeLoLimit = CALC_OFFSET(char *, pVolume, 0);

	pFile = CALC_OFFSET(PEFI_FFS_FILE_HEADER, VolumeLoLimit, pVolume->HeaderLength);

	while((void *)pFile < (void *)VolumeHiLimit)
	{
		DWORD size = efi_ffs_file_size(pFile);

		if (size == 0xffffff)
		{	// override size
			size = ((PEFI_FFS_FILE_HEADER2) pFile)->ExtendedSize;

			if (size == 0xffffffff)
			{	// out!
				break;
			}
		}

		// so.. we have VOLUME ROOT and file pointer

		if (memcmp(&pFile->Name, FileName, sizeof(EFI_GUID)) == 0)
			return pFile;
		pFile = CALC_OFFSET(PEFI_FFS_FILE_HEADER, pFile, (size + 7 & (-1 << 3)));
	}

	return NULL;
}

void volume_dump(char *outputdir, PEFI_FIRMWARE_VOLUME_HEADER pVolume)
{
	if (pVolume == NULL)
		return;

	if (pVolume->Signature != FVH_SIGNATURE)
		return;

	DWORD size = (DWORD) pVolume->FvLength;
	char *VolumeHiLimit = CALC_OFFSET(char *, pVolume, size);
	char *VolumeLoLimit = CALC_OFFSET(char *, pVolume, 0);

	PEFI_FFS_FILE_HEADER pFile = CALC_OFFSET(PEFI_FFS_FILE_HEADER, VolumeLoLimit, pVolume->HeaderLength);

	while((void *)pFile < (void *)VolumeHiLimit)
	{
		DWORD size = efi_ffs_file_size(pFile);

		if (size == 0xffffff)
		{	// override size
			size = ((PEFI_FFS_FILE_HEADER2) pFile)->ExtendedSize;

			if (size == 0xffffffff)
			{	// out!
				break;
			}
		}

		// so.. we have VOLUME ROOT and file pointer

		blob_dump(outputdir, pFile);

		pFile = CALC_OFFSET(PEFI_FFS_FILE_HEADER, pFile, (size + 7 & (-1 << 3)));
	}
}
