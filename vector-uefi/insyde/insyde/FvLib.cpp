#include <stdio.h>
#include <memory.h>
#include "UefiTypes.h"
#include "PiFirmwareFile.h"
#include "FvLib.h"
#include "CommonLib.h"

EFI_FIRMWARE_VOLUME_HEADER *mFvHeader = NULL;
UINT32	mFvLength = 0;


/***
 *	FvLib
 ***/

EFI_STATUS
VerifyFfsFile (
  EFI_FFS_FILE_HEADER  *FfsHeader
  )
/*++

Routine Description:

  Verify the current pointer points to a FFS file header.

Arguments:

  FfsHeader     Pointer to an alleged FFS file.

Returns:

  EFI_SUCCESS           The Ffs header is valid.
  EFI_NOT_FOUND         This "file" is the beginning of free space.
  EFI_VOLUME_CORRUPTED  The Ffs header is not valid.
  EFI_ABORTED           The erase polarity is not known.

--*/
{
	BOOL ErasePolarity;
	EFI_STATUS          Status;
	EFI_FFS_FILE_HEADER BlankHeader;
	UINT8               Checksum;
	UINT32              FileLength;
	UINT8               SavedChecksum;
	UINT8               SavedState;

	//
	// Get the erase polarity.
	//
	Status = GetErasePolarity (&ErasePolarity);
	if (Status < 0)  
	{
		return -1;
	}

	//
	// Check if we have free space
	//
	if (ErasePolarity) 
	{
		memset (&BlankHeader, -1, sizeof (EFI_FFS_FILE_HEADER));
	}
	else 
	{
		memset (&BlankHeader, 0, sizeof (EFI_FFS_FILE_HEADER));
	}

	if (memcmp (&BlankHeader, FfsHeader, sizeof (EFI_FFS_FILE_HEADER)) == 0) 
	{
		return -1;
	}
	//
	// Convert the GUID to a string so we can at least report which file
	// if we find an error.
	//
	//PrintGuidToBuffer (&FfsHeader->Name, FileGuidString, sizeof (FileGuidString), TRUE);
	//
	// Verify file header checksum
	//
	SavedState = FfsHeader->State;
	FfsHeader->State = 0;
	SavedChecksum = FfsHeader->IntegrityCheck.Checksum.File;
	FfsHeader->IntegrityCheck.Checksum.File = 0;
	Checksum = CalculateSum8 ((UINT8 *) FfsHeader, sizeof (EFI_FFS_FILE_HEADER));
	FfsHeader->State = SavedState;
	FfsHeader->IntegrityCheck.Checksum.File = SavedChecksum;
	if (Checksum != 0) 
	{
		return -2;
	}
	//
	// Verify file checksum
	//
	if (FfsHeader->Attributes & FFS_ATTRIB_CHECKSUM) 
	{
	//
	// Verify file data checksum
	//
		FileLength          = Expand24bit(FfsHeader->Size);
		Checksum            = CalculateSum8 ((UINT8 *) (FfsHeader + 1), FileLength - sizeof (EFI_FFS_FILE_HEADER));
		Checksum            = Checksum + FfsHeader->IntegrityCheck.Checksum.File;
		if (Checksum != 0) 
		{
			
			return -3;
		}
	} 
	else 
	{
		//
		// File does not have a checksum
		// Verify contents are 0xAA as spec'd
		//
		if (FfsHeader->IntegrityCheck.Checksum.File != FFS_FIXED_CHECKSUM) // &&
		//FfsHeader->IntegrityCheck.Checksum.File != FFS_FIXED_CHECKSUM2 )
		{
			return -4;
		}
	}

	return 0;
}


/*++

Routine Description:

  Verify the current pointer points to a valid FV header.

Arguments:

  FvHeader     Pointer to an alleged FV file.

Returns:

  EFI_SUCCESS             The FV header is valid.
  EFI_VOLUME_CORRUPTED    The FV header is not valid.
  EFI_INVALID_PARAMETER   A required parameter was NULL.
  EFI_ABORTED             Operation aborted.

--*/
//
// will not parse compressed sections
//
EFI_STATUS VerifyFv ( EFI_FIRMWARE_VOLUME_HEADER   *FvHeader )
{
	UINT16  Checksum;

	//
	// Verify input parameters
	//
	if (FvHeader == NULL) 
	{
		return -1;
	}

	if (FvHeader->Signature != EFI_FVH_SIGNATURE) 
	{
		return -1;
	}

	//
	// Verify header checksum
	//
	Checksum = CalculateSum16 ((UINT16 *) FvHeader, FvHeader->HeaderLength / sizeof (UINT16));

	if (Checksum != 0) 
	{
		return -2;
	}

	return 0;
}


EFI_STATUS
GetErasePolarity (
  BOOL   *ErasePolarity
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
  if (ErasePolarity == NULL) 
  {
    return -1;
  }

  if (mFvHeader->Attributes & EFI_FVB2_ERASE_POLARITY) {
    *ErasePolarity = TRUE;
  } else {
    *ErasePolarity = FALSE;
  }

  return 0;
}
