#ifndef __UEFI_TYPES_H_

#define __UEFI_TYPES_H_

#define QWORD	unsigned __int64

#define DWORD unsigned int
#define WORD unsigned short
#define BYTE unsigned char

typedef BYTE	UINT8;
typedef WORD	UINT16;
typedef WORD	CHAR16;
typedef DWORD	UINT32;
typedef QWORD	UINT64;

typedef DWORD	UINT;

#define BOOL	bool


#define UINTN	DWORD
#ifndef TRUE
	#define TRUE	true
	#define FALSE	false
#endif

#pragma pack(1)
//
// Used to verify the integrity of the file.
// 
typedef union _efi_ffs_integrity_check {
	struct _checksum {
			UINT8   Header;
			UINT8   File;
		} Checksum;
	UINT16    Checksum16;
} EFI_FFS_INTEGRITY_CHECK;
	
typedef UINT8 EFI_FV_FILETYPE;
typedef UINT8 EFI_FFS_FILE_ATTRIBUTES;
typedef UINT8 EFI_FFS_FILE_STATE;

//
// File Types Definitions
// 
#define EFI_FV_FILETYPE_ALL                   0x00
#define EFI_FV_FILETYPE_RAW                   0x01
#define EFI_FV_FILETYPE_FREEFORM              0x02
#define EFI_FV_FILETYPE_SECURITY_CORE         0x03
#define EFI_FV_FILETYPE_PEI_CORE              0x04
#define EFI_FV_FILETYPE_DXE_CORE              0x05
#define EFI_FV_FILETYPE_PEIM                  0x06
#define EFI_FV_FILETYPE_DRIVER                0x07
#define EFI_FV_FILETYPE_COMBINED_PEIM_DRIVER  0x08
#define EFI_FV_FILETYPE_APPLICATION           0x09
#define EFI_FV_FILETYPE_SMM                   0x0A
#define EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE 0x0B
#define EFI_FV_FILETYPE_COMBINED_SMM_DXE      0x0C
#define EFI_FV_FILETYPE_SMM_CORE              0x0D
#define EFI_FV_FILETYPE_OEM_MIN               0xc0
#define EFI_FV_FILETYPE_OEM_MAX               0xdf
#define EFI_FV_FILETYPE_DEBUG_MIN             0xe0
#define EFI_FV_FILETYPE_DEBUG_MAX             0xef
#define EFI_FV_FILETYPE_FFS_MIN               0xf0
#define EFI_FV_FILETYPE_FFS_MAX               0xff
#define EFI_FV_FILETYPE_FFS_PAD               0xf0

// 
// FFS File Attributes.
// 
#define FFS_ATTRIB_FIXED              0x04
#define FFS_ATTRIB_DATA_ALIGNMENT     0x38
#define FFS_ATTRIB_CHECKSUM           0x40

//
// FFS_FIXED_CHECKSUM is the checksum value used when the
// FFS_ATTRIB_CHECKSUM attribute bit is clear
//
#define FFS_FIXED_CHECKSUM  0xAA
#define FFS_FIXED_CHECKSUM2  0x5A

// 
// FFS File State Bits.
// 
#define EFI_FILE_HEADER_CONSTRUCTION  0x01
#define EFI_FILE_HEADER_VALID         0x02
#define EFI_FILE_DATA_VALID           0x04
#define EFI_FILE_MARKED_FOR_UPDATE    0x08
#define EFI_FILE_DELETED              0x10
#define EFI_FILE_HEADER_INVALID       0x20

#define EFI_FILE_ALL_STATE_BITS       (EFI_FILE_HEADER_CONSTRUCTION | \
	EFI_FILE_HEADER_VALID | \
	EFI_FILE_DATA_VALID | \
	EFI_FILE_MARKED_FOR_UPDATE | \
	EFI_FILE_DELETED | \
	EFI_FILE_HEADER_INVALID \
)

#ifndef __UEFI_BASETYPE_H__

#pragma pack(1)
typedef struct _efi_guid
{
	DWORD	Data1;
	WORD	Data2;
	WORD	Data3;
	BYTE	Data4[8];
} EFI_GUID;
#endif

#pragma pack()

#endif
