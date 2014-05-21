/*++

Copyright (c) 2005 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  OpenVolume.c

Abstract:

  OpenVolume() function of Simple File System Protocol

Revision History

--*/

#include "Ntfs.h"
//#include "ntfs/dir.h"
#include "ntfs/ntfsdir.h"

EFI_STATUS
EFIAPI
NtfsOpenVolume (
  IN  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *This,
  OUT EFI_FILE_PROTOCOL                **Root
  )
/*++

Routine Description:

  Implements Simple File System Protocol interface function OpenVolume().

Arguments:

  This                  - Calling context.
  File                  - the Root Directory of the volume.

Returns:

  EFI_OUT_OF_RESOURCES  - Can not allocate the memory.
  EFI_VOLUME_CORRUPTED  - The FAT type is error.
  EFI_SUCCESS           - Open the volume successfully.

--*/
{

	EFI_STATUS  Status;
	NTFS_VOLUME  *Volume;
	NTFS_IFILE	*IFile;
	ntfs_inode *inode;

	//AsciiPrint("NtfsOpenVolume: entering...");
	//Print(L"OpenVolume :");

	Volume = VOLUME_FROM_VOL_INTERFACE (This);

	NtfsAcquireLock  ();

	if (Volume->root == NULL)
	{	// open root one time!
		//inode = ntfs_inode_open(Volume->vd->vol, FILE_root);	// access to root!
		//Volume->root = inode;
	}

	inode = ntfs_inode_open(Volume->vd->vol, FILE_root);	// access to root!
	
	if (inode == NULL)
	{
		//Print(L"ntfs_inode_open(vol, FILE_root failed!");
		return EFI_VOLUME_CORRUPTED;
	}

	Ntfs_inode_to_FileHandle(inode, Root);

	if (*Root != NULL)
	{	// circular reference 
		IFile = IFILE_FROM_FHAND(*Root);
		IFile->Volume = Volume;
		IFile->RootDir = TRUE;
		IFile->Type = FSW_EFI_FILE_TYPE_DIR;
		IFile->inode = inode;

		CreateFileName(IFile->FullPath, "\\", "");

		Status = EFI_SUCCESS;
	}
	else
	{
		Status = EFI_INVALID_PARAMETER;
	}

	NtfsReleaseLock();
	return EFI_SUCCESS;
}


