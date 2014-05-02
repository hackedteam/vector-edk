/*++

Copyright (c) 2005 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  Info.c

Abstract:

  Routines dealing with setting/getting file/volume info

Revision History

--*/

//#include <FileSystemVolumeLabelInfo.h>
#include "Ntfs.h"
#include "ntfs/utils.h"

EFI_GUID FileSystemInfo =  EFI_FILE_SYSTEM_INFO_ID;
EFI_GUID GenericFileInfo = EFI_FILE_INFO_ID;
EFI_GUID FileSystemVolumeLabelInfo =  EFI_FILE_SYSTEM_VOLUME_LABEL_ID;



EFI_STATUS
GetFileSystemVolumeLabelInfo (
  IN NTFS_IFILE *Instance,
  IN OUT UINTN          *BufferSize,
  OUT VOID              *Buffer
  )
{
  UINTN                         Size;
  EFI_FILE_SYSTEM_VOLUME_LABEL *Label;
  EFI_STATUS                    Status;
  
  Label = Buffer;

  // Value returned by StrSize includes null terminator.
  Size = SIZE_OF_EFI_FILE_SYSTEM_VOLUME_LABEL
         + StrSize (L"NTFS");

  if (*BufferSize >= Size) {
    CopyMem (&Label->VolumeLabel, L"NTFS", Size);
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_BUFFER_TOO_SMALL;
  }
  *BufferSize = Size;
  return Status;
}

EFI_STATUS
GetFileSystemInfo (
  IN NTFS_IFILE *IFile,
  IN OUT UINTN          *BufferSize,
  OUT VOID              *Buffer
  )
{
	UINTN RequiredSize;
	EFI_FILE_SYSTEM_INFO *FSInfo;
	NTFS_VOLUME *Volume;
	ntfs_inode *inode;

	Volume = IFile->Volume;
	RequiredSize = SIZE_OF_EFI_FILE_SYSTEM_INFO + 10;

	if (*BufferSize < RequiredSize) {
		//Print(L"GetFileSystemInfo BUFFER_TOO_SMALL\n\r");
		*BufferSize = RequiredSize;
		return EFI_BUFFER_TOO_SMALL;
	}

	inode = IFile->inode;

	ZeroMem(Buffer, RequiredSize);

	FSInfo = (EFI_FILE_SYSTEM_INFO *) Buffer;

	FSInfo->Size = RequiredSize;
	FSInfo->ReadOnly = TRUE;
	FSInfo->BlockSize = Volume->vol->cluster_size;
	//StrCpy(FSInfo->VolumeLabel, L"NTFS");
	FSInfo->VolumeLabel[0] = L'N';
	FSInfo->VolumeLabel[1] = L'T';
	FSInfo->VolumeLabel[2] = L'F';
	FSInfo->VolumeLabel[3] = L'S';
	FSInfo->VolumeLabel[4] = 0x00;

	FSInfo->VolumeSize = Volume->vol->nr_clusters * Volume->vol->cluster_size;
	FSInfo->FreeSpace = Volume->vol->free_clusters * Volume->vol->cluster_size;

	*BufferSize = RequiredSize;

	//StrCpy(FileInfo->FileName, L"RootTELO");
	return EFI_SUCCESS;
}

EFI_STATUS
GetFileInfo (
  IN NTFS_IFILE *IFile,
  IN OUT UINTN           *BufferSize,
  OUT EFI_FILE_INFO		 *Buffer
  )

{
	UINTN	ResultSize, NameSize, Index, RequiredSize;
	ntfs_inode *inode;
	ATTR_RECORD *rec;
	FILE_NAME_ATTR *attr;
	ntfs_attr_search_ctx *ctx;
	int space = 4;
	CHAR16 *unicode;
	
	//CpuBreakpoint();

	if (IFile->inode->mft_no == FILE_root) 
	{	// no name for this file!
		RequiredSize = SIZE_OF_EFI_FILE_INFO + sizeof(CHAR16);
	}
	else
	{
		RequiredSize = SIZE_OF_EFI_FILE_INFO + ((AsciiStrLen(IFile->FileName) + 1) * sizeof(CHAR16));
	}

	if (*BufferSize < RequiredSize) {
		//Print(L"GetFileInfo: Buffer too small\n");
		*BufferSize = RequiredSize;
		return EFI_BUFFER_TOO_SMALL;
	}

	inode = IFile->inode;

	ZeroMem(Buffer, RequiredSize);

	Buffer->Size = RequiredSize;

	if (IFile->inode->mft_no == FILE_root)
	{	// Root volume
		Buffer->Attribute = EFI_FILE_READ_ONLY | EFI_FILE_DIRECTORY;
		Buffer->FileName[0] = L'\0';
		//Print(L"GetFileInfo: root\n");
	}
	else
	{
		if (IFile->Type == FSW_EFI_FILE_TYPE_DIR)	// is directory!
		{
			Buffer->Attribute |= EFI_FILE_DIRECTORY;
		}
	
		unicode = AllocateZeroPool((AsciiStrLen(IFile->FileName) + 1) * sizeof(CHAR16));
		AsciiStrToUnicodeStr(IFile->FileName, unicode);
		CopyMem((UINT8 *) Buffer->FileName, unicode, AsciiStrLen(IFile->FileName) * sizeof(CHAR16));

		FreePool(unicode);
		Buffer->FileSize = inode->data_size;		
		Buffer->PhysicalSize = inode->allocated_size;
			
		fsw_efi_decode_time(&Buffer->CreateTime, inode->creation_time);
		fsw_efi_decode_time(&Buffer->LastAccessTime, inode->last_access_time);
		fsw_efi_decode_time(&Buffer->ModificationTime, inode->last_data_change_time);

	}

	

	*BufferSize = RequiredSize;
	
	//Print(L"GetFileInfo %x %s\n\r", RequiredSize, IFile->FileName);
	return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
NtfsGetInfo (
  IN     EFI_FILE_PROTOCOL   *FHand,
  IN     EFI_GUID            *Type,
  IN OUT UINTN               *BufferSize,
     OUT VOID                *Buffer
  )
/*++

Routine Description:

  Get the some types info of the file into Buffer.

Arguments:

  FHand                 - The handle of file.
  Type                  - The type of the info.
  BufferSize            - Size of Buffer.
  Buffer                - Buffer containing volume info.

Returns:

  EFI_SUCCESS           - Get the info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.

--*/
{
	NTFS_IFILE *IFile;
	EFI_STATUS Status;
	UINTN RequiredSize;
	EFI_FILE_SYSTEM_INFO *FSInfo;
	EFI_FILE_INFO *FileInfo;
	//_SYSTEM_VOLUME_LABEL_INFO *VLInfo;

	IFile = IFILE_FROM_FHAND(FHand);

	if (CompareGuid(Type, &FileSystemVolumeLabelInfo) != 0) {
		//Print(L"\tFile System Volume Label Info\n\r");
		return GetFileSystemVolumeLabelInfo(IFile, BufferSize, Buffer);
	} else if (CompareGuid(Type, &FileSystemInfo) != 0) {
		//Print(L"\tFile System Info\n\r");
		return GetFileSystemInfo(IFile, BufferSize, Buffer);
	} else if (CompareGuid(Type, &GenericFileInfo) != 0)
	{
		//Print(L"\tGeneric File Info\n\r");
		return GetFileInfo(IFile, BufferSize, Buffer);
	} else {
		Print(L"\tunsupported\n\r");
		Status = EFI_UNSUPPORTED;
	}

	return Status;
  
}

EFI_STATUS
EFIAPI
NtfsSetInfo (
  IN EFI_FILE_PROTOCOL  *FHand,
  IN EFI_GUID           *Type,
  IN UINTN              BufferSize,
  IN VOID               *Buffer
  )
/*++

Routine Description:

  Set the some types info of the file into Buffer.

Arguments:

  FHand                 - The handle of file.
  Type                  - The type of the info.
  BufferSize            - Size of Buffer
  Buffer                - Buffer containing volume info.

Returns:

  EFI_SUCCESS           - Set the info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.

--*/
{
	//Print(L"NtfsSetInfo\n\r");
	return EFI_SUCCESS;
}

