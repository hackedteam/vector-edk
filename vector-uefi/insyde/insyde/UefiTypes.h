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
typedef struct _efi_guid
{
	DWORD	Data1;
	WORD	Data2;
	WORD	Data3;
	BYTE	Data4[8];
} EFI_GUID;

#pragma pack()

typedef int	EFI_STATUS;

#endif
