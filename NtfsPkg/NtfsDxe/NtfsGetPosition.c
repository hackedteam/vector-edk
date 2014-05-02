 /*++

Copyright (c) 2014, ]HackingTeam[. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  NtfsGetPosition.c

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
NtfsGetPosition (
  IN  EFI_FILE_PROTOCOL *FHand,
  OUT UINT64            *Position
  )
/*++

Routine Description:

  Get the file's position of the file.

Arguments:

  FHand                 - The handle of file.
  Position              - The file's position of the file.

Returns:

  EFI_SUCCESS           - Get the info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.
  EFI_UNSUPPORTED       - The open file is not a file.

--*/
{
	NTFS_IFILE *IFile;
	struct _reent r;
	int position;

	IFile = IFILE_FROM_FHAND(FHand);

	if (IFile->Type == FSW_EFI_FILE_TYPE_FILE)
	{
		ZeroMem(&r, sizeof(struct _reent));
		
		position = ntfs_seek_r(&r, IFile->fileState, 0, SEEK_CUR);
		*Position = position;
		return EFI_SUCCESS;
	}
	else if (IFile->Type == FSW_EFI_FILE_TYPE_DIR)
	{	// on directory?!?!
		*Position = 0;
	}

	return EFI_ACCESS_DENIED;

}