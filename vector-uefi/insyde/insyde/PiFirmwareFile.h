#ifndef __PI_FIRMWARE_FILE_H__
#define __PI_FIRMWARE_FILE_H__

#pragma pack(1)
//
// Used to verify the integrity of the file.
// 
typedef union {
	struct {
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

//
// Each file begins with the header that describe the 
// contents and state of the files.
// 
typedef struct {
	EFI_GUID                Name;
	EFI_FFS_INTEGRITY_CHECK IntegrityCheck;
	EFI_FV_FILETYPE         Type;
	EFI_FFS_FILE_ATTRIBUTES Attributes;
	UINT8                   Size[3];
	EFI_FFS_FILE_STATE      State;
} EFI_FFS_FILE_HEADER;

// Each file begins with the header that describe the 
// contents and state of the files.
// 
typedef struct {
	EFI_GUID                Name;
	EFI_FFS_INTEGRITY_CHECK IntegrityCheck;
	EFI_FV_FILETYPE         Type;
	EFI_FFS_FILE_ATTRIBUTES Attributes;
	UINT8                   Size[3];
	EFI_FFS_FILE_STATE      State;
	UINT32					ExtendedSize;
} EFI_FFS_FILE_HEADER2;


typedef UINT8 EFI_SECTION_TYPE;

//
// Pseudo type. It is
// used as a wild card when retrieving sections. The section
// type EFI_SECTION_ALL matches all section types.
//
#define EFI_SECTION_ALL                   0x00

//
// Encapsulation section Type values
//
#define EFI_SECTION_COMPRESSION           0x01

#define EFI_SECTION_GUID_DEFINED          0x02

//
// Leaf section Type values
//
#define EFI_SECTION_COMPRESSED			  0x01
#define EFI_SECTION_GUIDDEF				  0x02
#define EFI_SECTION_DISP				  0x03
#define EFI_SECTION_PE		              0x10
#define EFI_SECTION_PIC                   0x11
#define EFI_SECTION_TE                    0x12
#define EFI_SECTION_DXE_DEPEX             0x13
#define EFI_SECTION_VERSION               0x14
#define EFI_SECTION_USER_INTERFACE        0x15
#define EFI_SECTION_COMPATIBILITY16       0x16
#define EFI_SECTION_FIRMWARE_VOLUME_IMAGE 0x17
#define EFI_SECTION_FREEFORM_SUBTYPE_GUID 0x18
#define EFI_SECTION_RAW                   0x19
#define EFI_SECTION_PEI_DEPEX             0x1B
#define EFI_SECTION_SMM_DEPEX             0x1C

//typedef enum _efi_section_type
//{
//	COMPRESSED	= 0x01, /* Compression Section */
//	GUIDDEF		= 0x02, /* GUID-defined section */
//	DISP		= 0x03, /* Disposable section */
//	PE			= 0x10,	/* PE Image */
//	PICPE		= 0x11, /* PE PIC Image */
//	TE			= 0x12, /* TE Image */
//	DXEDEPEX	= 0x13, /* DXE Dependency Expression */
//	VER			= 0x14, /* Version */
//	NAME		= 0x15, /* User Interface name */
//	COMP16		= 0x16, /* Compatibility 16-bit */
//	FVI			= 0x17, /* Firmware Volume Image */
//	GUID		= 0x18, /* Freeform Subtype GUID */
//	RAW			= 0x19, /* raw section */
//	PEIDEPEX	= 0x1b, /* PEI Dependency Expression */
//	SMMDEPEX	= 0x1c	/* SMM Dependency Expression */
//};

typedef struct {
	UINT8				Size[3];
	EFI_SECTION_TYPE	Type;
} EFI_COMMON_SECTION_HEADER;


typedef struct {
	UINT8				Size[3];
	EFI_SECTION_TYPE	Type;
	UINT32				ExtendedSize;
} EFI_COMMON_SECTION_HEADER2;

//
// Leaf section type that contains an 
// IA-32 16-bit executable image.
// 
typedef EFI_COMMON_SECTION_HEADER EFI_COMPATIBILITY16_SECTION;

//
// CompressionType of EFI_COMPRESSION_SECTION.
// 
#define EFI_NOT_COMPRESSED        0x00
#define EFI_STANDARD_COMPRESSION  0x01
//
// An encapsulation section type in which the 
// section data is compressed.
// 
typedef struct {
	EFI_COMMON_SECTION_HEADER   CommonHeader;
	UINT32                      UncompressedLength;
	UINT8                       CompressionType;
} EFI_COMPRESSION_SECTION;

//
// Leaf section which could be used to determine the dispatch order of DXEs.
// 
typedef EFI_COMMON_SECTION_HEADER EFI_DXE_DEPEX_SECTION;

//
// Leaf section witch contains a PI FV.
// 
typedef EFI_COMMON_SECTION_HEADER EFI_FIRMWARE_VOLUME_IMAGE_SECTION;

//
// Leaf section which contains a single GUID.
// 
typedef struct {
	EFI_COMMON_SECTION_HEADER   CommonHeader;
	EFI_GUID                    SubTypeGuid;
} EFI_FREEFORM_SUBTYPE_GUID_SECTION;

//
// Attributes of EFI_GUID_DEFINED_SECTION
// 
#define EFI_GUIDED_SECTION_PROCESSING_REQUIRED  0x01
#define EFI_GUIDED_SECTION_AUTH_STATUS_VALID    0x02
//
// Leaf section which is encapsulation defined by specific GUID
// 
typedef struct {
EFI_COMMON_SECTION_HEADER   CommonHeader;
EFI_GUID                    SectionDefinitionGuid;
UINT16                      DataOffset;
UINT16                      Attributes;
} EFI_GUID_DEFINED_SECTION;

//
// Leaf section which contains PE32+ image.
// 
typedef EFI_COMMON_SECTION_HEADER EFI_PE32_SECTION;

//
// Leaf section which contains PIC image.
// 
typedef EFI_COMMON_SECTION_HEADER EFI_PIC_SECTION;

//
// Leaf section which used to determine the dispatch order of PEIMs.
// 
typedef EFI_COMMON_SECTION_HEADER EFI_PEI_DEPEX_SECTION;

//
// Leaf section which constains the position-independent-code image.
// 
typedef EFI_COMMON_SECTION_HEADER EFI_TE_SECTION;

//
// Leaf section which contains an array of zero or more bytes.
// 
typedef EFI_COMMON_SECTION_HEADER EFI_RAW_SECTION;

//
// Leaf section which contains a unicode string that 
// is human readable file name.
// 
typedef struct {
	EFI_COMMON_SECTION_HEADER   CommonHeader;

//
// Array of unicode string.
// 
	CHAR16                      FileNameString[1];
} EFI_USER_INTERFACE_SECTION;


//
// Leaf section which contains a numeric build number and
// an optional unicode string that represent the file revision. 
// 
typedef struct {
	EFI_COMMON_SECTION_HEADER   CommonHeader;
	UINT16                      BuildNumber;
	CHAR16                      VersionString[1];
} EFI_VERSION_SECTION;


#define SECTION_SIZE(SectionHeaderPtr) \
((UINT32) (*((UINT32 *) ((EFI_COMMON_SECTION_HEADER *) SectionHeaderPtr)->Size) & 0x00ffffff))

#pragma pack()

typedef union {
	EFI_COMMON_SECTION_HEADER         *CommonHeader;
	EFI_COMPRESSION_SECTION           *CompressionSection;
	EFI_GUID_DEFINED_SECTION          *GuidDefinedSection;
	EFI_PE32_SECTION                  *Pe32Section;
	EFI_PIC_SECTION                   *PicSection;
	EFI_TE_SECTION                    *TeSection;
	EFI_PEI_DEPEX_SECTION             *PeimHeaderSection;
	EFI_DXE_DEPEX_SECTION             *DependencySection;
	EFI_VERSION_SECTION               *VersionSection;
	EFI_USER_INTERFACE_SECTION        *UISection;
	EFI_COMPATIBILITY16_SECTION       *Code16Section;
	EFI_FIRMWARE_VOLUME_IMAGE_SECTION *FVImageSection;
	EFI_FREEFORM_SUBTYPE_GUID_SECTION *FreeformSubtypeSection;
	EFI_RAW_SECTION                   *RawSection;
} EFI_FILE_SECTION_POINTER;

#define 	EFI_FVB2_ERASE_POLARITY   0x00000800

#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#define EFI_FVH_SIGNATURE EFI_SIGNATURE_32 ('_', 'F', 'V', 'H')

typedef UINT8	EFI_FV_FILETYPE;

#define EFI_FV_FILETYPE_RAW                   0x01
#define EFI_FV_FILETYPE_FREEFORM              0x02
#define EFI_FV_FILETYPE_SECURITY_CORE         0x03
#define EFI_FV_FILETYPE_PEI_CORE              0x04
#define EFI_FV_FILETYPE_DXE_CORE              0x05
#define EFI_FV_FILETYPE_PEIM                  0x06
#define EFI_FV_FILETYPE_DRIVER                0x07
#define EFI_FV_FILETYPE_COMBINED_PEIM_DRIVER  0x08
#define EFI_FV_FILETYPE_APPLICATION           0x09
#define EFI_FV_FILETYPE_FFS_PAD               0xf0

typedef UINT8 EFI_FFS_FILE_STATE;

#define EFI_FILE_HEADER_CONSTRUCTION 0x01
#define EFI_FILE_HEADER_VALID        0x02
#define EFI_FILE_DATA_VALID          0x04
#define EFI_FILE_MARKED_FOR_UPDATE   0x08
#define EFI_FILE_DELETED             0x10
#define EFI_FILE_HEADER_INVALID      0x20

typedef UINT8 EFI_FFS_FILE_ATTRIBUTES;
typedef UINT8 EFI_SECTION_TYPE;


#define FFS_ATTRIB_TAIL_PRESENT     0x01
#define FFS_ATTRIB_RECOVERY         0x02
#define FFS_ATTRIB_HEADER_EXTENSION 0x04
#define FFS_ATTRIB_DATA_ALIGNMENT   0x38
#define FFS_ATTRIB_CHECKSUM         0x40

#define EFI_NOT_COMPRESSED        0x00
#define EFI_STANDARD_COMPRESSION  0x01

///
/// Pseudo type. It is used as a wild card when retrieving sections.
///  The section type EFI_SECTION_ALL matches all section types.
///
#define EFI_SECTION_ALL                   0x00

///
/// Encapsulation section Type values.
///
#define EFI_SECTION_COMPRESSION           0x01
#define EFI_SECTION_GUID_DEFINED          0x02
#define EFI_SECTION_DISPOSABLE            0x03

///
/// Leaf section Type values.
///
#define EFI_SECTION_PE32                  0x10
#define EFI_SECTION_PIC                   0x11
#define EFI_SECTION_TE                    0x12
#define EFI_SECTION_DXE_DEPEX             0x13
#define EFI_SECTION_VERSION               0x14
#define EFI_SECTION_USER_INTERFACE        0x15
#define EFI_SECTION_COMPATIBILITY16       0x16
#define EFI_SECTION_FIRMWARE_VOLUME_IMAGE 0x17
#define EFI_SECTION_FREEFORM_SUBTYPE_GUID 0x18
#define EFI_SECTION_RAW                   0x19
#define EFI_SECTION_PEI_DEPEX             0x1B
#define EFI_SECTION_SMM_DEPEX             0x1C

//char *outputdirectory = NULL;

#pragma pack(1)
typedef struct _EFI_FV_BLOCK_MAP_ENTRY 
{
	DWORD	NumBlocks;
	DWORD	Length;
} EFI_FV_BLOCK_MAP_ENTRY ;

#pragma pack(1)
typedef struct _efi_firmware_volume_header
{
	char		ZeroVector[0x10];		// RESET VECTOR
	EFI_GUID	GUID;
	__int64		FvLength;				// Firmware Length
	DWORD		Signature;
	DWORD		Attributes;
	WORD		HeaderLength;
	WORD		Checksum;
	BYTE		Reserved[3];
	BYTE		Revision;
	EFI_FV_BLOCK_MAP_ENTRY	BlockMapEntry[1];
} EFI_FIRMWARE_VOLUME_HEADER, *PEFI_FIRMWARE_VOLUME_HEADER;

#pragma pack()

typedef EFI_FFS_FILE_HEADER*	PEFI_FFS_FILE_HEADER;
typedef EFI_FFS_FILE_HEADER2*	PEFI_FFS_FILE_HEADER2;

#endif
