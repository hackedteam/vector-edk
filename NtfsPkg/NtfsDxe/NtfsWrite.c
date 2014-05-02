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
NtfsWrite (
  IN     EFI_FILE_PROTOCOL  *FHand,
  IN OUT UINTN              *BufferSize,
  IN     VOID               *Buffer
  )
/*++

Routine Description:

  Write the content of buffer into files.

Arguments:

  FHand                 - The handle of the file.
  BufferSize            - Size of Buffer.
  Buffer                - Buffer containing write data.

Returns:

  EFI_SUCCESS           - Set the file info successfully.
  EFI_WRITE_PROTECTED   - The disk is write protect.
  EFI_ACCESS_DENIED     - The file is read-only.
  EFI_DEVICE_ERROR      - The OFile is not valid.
  EFI_UNSUPPORTED       - The open file is not a file.
                        - The writing file size is larger than 4GB.
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
		rbytes = ntfs_write_r(&r, IFile->fileState, Buffer, *BufferSize);

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
		Status = EFI_ACCESS_DENIED;
		return Status;
	}
	else
	{	// what's?
	}
	return EFI_DEVICE_ERROR;

}

