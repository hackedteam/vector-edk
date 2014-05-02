 /*++

Copyright (c) 2005 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  ReadWrite.c

Abstract:

  Functions that perform file read/write

Revision History

--*/

#include "Ntfs.h"
#include "ntfs/ntfsfile.h"
#include "ntfs/ntfsdir.h"
#include "fcntl.h"

EFI_STATUS
EFIAPI
NtfsRead (
  IN     EFI_FILE_PROTOCOL  *FHand,
  IN OUT UINTN              *BufferSize,
     OUT VOID               *Buffer
  )
/*++

Routine Description:

  Get the file info.

Arguments:

  FHand                 - The handle of the file.
  BufferSize            - Size of Buffer.
  Buffer                - Buffer containing read data.

Returns:

  EFI_SUCCESS           - Get the file info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.
  EFI_VOLUME_CORRUPTED  - The file type of open file is error.
  other                 - An error occurred when operation the disk.

--*/
{
	NTFS_IFILE *IFile;
	EFI_STATUS Status;
	struct _reent r;
	int rbytes;

	IFile = IFILE_FROM_FHAND(FHand);	// cast from "EFI" to FHAND

	if (IFile->Type == FSW_EFI_FILE_TYPE_FILE)
	{
		ZeroMem(&r, sizeof(struct _reent));
		rbytes = ntfs_read_r(&r, IFile->fileState, Buffer, *BufferSize);

		if (rbytes >= 0)
		{
			*BufferSize = rbytes;
			return EFI_SUCCESS;
		}
			
		*BufferSize = 0;
		return EFI_DEVICE_ERROR;
	}
	else if (IFile->Type == FSW_EFI_FILE_TYPE_DIR)
	{	// unimplemented!
		//Print(L"NtfsRead: Accessing to directory");
		Status = fsw_efi_dir_read(IFile, BufferSize, Buffer);
		//Print(L"    !exit...\n\r");
		return Status;
	}
	else
	{	// what's?
	}
	return EFI_DEVICE_ERROR;
  
}
