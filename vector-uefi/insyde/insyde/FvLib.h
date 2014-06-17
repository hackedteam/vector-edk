#ifndef __FVLIB_H__
	#define __FVLIB_H__

EFI_STATUS VerifyFfsFile ( EFI_FFS_FILE_HEADER  *FfsHeader );
EFI_STATUS VerifyFv ( EFI_FIRMWARE_VOLUME_HEADER   *FvHeader );
EFI_STATUS GetErasePolarity (BOOL   *ErasePolarity);

//////////////////////////////////////////////////////////////////////////
// global object!
extern EFI_FIRMWARE_VOLUME_HEADER *mFvHeader;
extern UINT32	mFvLength;

#endif
