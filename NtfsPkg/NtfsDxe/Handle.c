/*++

Copyright (c) 2005 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  Handle.c

Abstract:

  Handle functions of Simple File System Protocol

Revision History

--*/

#include "Ntfs.h"
#include "ntfs/ntfsdir.h"


UINTN EFIAPI FillFileName(CHAR8 *Destination, CHAR8 *Path, CHAR8 *FileName)
{
	CHAR8 *Ptr, *ClearPtr;
	BOOL bReset;

	Ptr = Destination;
	ClearPtr = Destination;

	
	while(*Path != 0x00)
	{
		*Destination++ = *Path++;
	}

	Destination--;

	if (AsciiStrCmp(FileName, ".") == 0)
	{
		Destination++;
		goto end;
	}

	if (AsciiStrCmp(FileName, "..") == 0)
	{	
		bReset = TRUE;
		Destination++;
		*Destination = 0x00;

		while(Destination > Ptr && bReset)
		{
			if (*Destination == '\\')
				bReset = FALSE;

			*Destination-- = 0x00;
		}
		Destination++;
		goto end;
	}

	while(*Path == *FileName)
	{	// skip path
		Path++;
		FileName++;
	}

	if (*Destination == '\\' && *FileName == '\\')
	{
		while(*FileName != 0x00)
		{
			*Destination++ = *FileName++;
		}
	}
	else
	{
		Destination++;

		while(*FileName != 0x00)
		{
			*Destination++ = *FileName++;
		}
	}

end:

	bReset = FALSE;
	while(ClearPtr < Destination && *ClearPtr != 0x00)
	{
		if (ClearPtr[0] == '\\' && ClearPtr[1] == '\\')
		{
			ClearPtr++;
			ClearPtr[0] = ClearPtr[1];
			bReset = TRUE;
		}
		else
		{
			if (bReset)
			{
				ClearPtr[0] = ClearPtr[1];
			}
			ClearPtr++;
		}
	}

	/* File path must start with "\\" */
	//ClearPtr = Ptr;

	//if (ClearPtr[0] == '\\')
	//{
	//	while(*ClearPtr != 0)
	//	{
	//		ClearPtr[0] = ClearPtr[1];
	//		ClearPtr++;
	//	}
	//}

	return (UINTN) (Destination - Ptr);
}

EFI_STATUS 
EFIAPI
Ntfs_inode_to_FileHandle(
  IN ntfs_inode *inode,
  OUT EFI_FILE **NewFileHandle)
{
	EFI_STATUS		Status;
	NTFS_IFILE		*NewIFile;

	if (inode == NULL)
	{
		//Print(L"Ntfs_inode_to_FileHandle -> INODE NULL!\n\r");
		return EFI_VOLUME_CORRUPTED;
	}

	NewIFile = AllocateZeroPool(sizeof(NTFS_IFILE));
	NewIFile->Signature = NTFS_IFILE_SIGNATURE;

	NewIFile->Handle.Revision = 0x00010000;
	NewIFile->Handle.Open = NtfsOpen;
	NewIFile->Handle.Close = NtfsClose;
	NewIFile->Handle.Delete = NtfsDelete;
	NewIFile->Handle.Read = NtfsRead;
	NewIFile->Handle.Write = NtfsWrite;
	NewIFile->Handle.GetPosition = NtfsGetPosition;
	NewIFile->Handle.SetPosition = NtfsSetPosition;
	NewIFile->Handle.GetInfo = NtfsGetInfo;
	NewIFile->Handle.SetInfo = NtfsSetInfo;
	NewIFile->Handle.Flush = NtfsFlush;
	NewIFile->inode = inode;	//
	NewIFile->Position = -1;

	memset(NewIFile->FileName, 0, 260);
	memset(NewIFile->FullPath, 0, 260);

	if ((inode->mrec->flags & MFT_RECORD_IS_DIRECTORY) != 0)
	{
		//Print(L"inode %x is directory.\n\r", inode->mft_no);
		NewIFile->Type = FSW_EFI_FILE_TYPE_DIR;
		NewIFile->Position = 0;
	}
	else
	{
		//Print(L"inode %x is file.\n\r", inode->mft_no);
		NewIFile->Type = FSW_EFI_FILE_TYPE_FILE;
		NewIFile->Position = 0;
	}

	*NewFileHandle = &NewIFile->Handle;

	//Print(L"Allocation of NewFileHandle successful at address %x\n\r", NewIFile);
	return EFI_SUCCESS;
}
