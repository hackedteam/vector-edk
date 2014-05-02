/*++

Copyright (c) 2005 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  flush.c

Abstract:

  Routines that check references and flush OFiles

Revision History

--*/

#include "Ntfs.h"
#include "ntfs/ntfsdir.h"
#include "ntfs/ntfsfile.h"

EFI_STATUS
EFIAPI
NtfsFlush (
  IN EFI_FILE_PROTOCOL  *FHand
  )
/*++

Routine Description:

  Flushes all data associated with the file handle.

Arguments:

  FHand                 - Handle to file to flush.

Returns:

  EFI_SUCCESS           - Flushed the file successfully.
  EFI_WRITE_PROTECTED   - The volume is read only.
  EFI_ACCESS_DENIED     - The file is read only.
  Others                - Flushing of the file failed.

--*/
{
	NTFS_IFILE *IFile;

	IFile = IFILE_FROM_FHAND(FHand);

	if (IFile->Volume->ReadOnly)
		return EFI_WRITE_PROTECTED;
	
	if (IFile->ReadOnly)
		return EFI_ACCESS_DENIED;

	if (IFile->Type == FSW_EFI_FILE_TYPE_FILE)
	{
		ntfsSync(IFile->Volume->vd, IFile->inode);
	}
	else if (IFile->Type == FSW_EFI_FILE_TYPE_DIR)
	{	// how sync a directory?
		ntfsSync(IFile->Volume->vd, IFile->inode);
	}

	return EFI_SUCCESS;

}

