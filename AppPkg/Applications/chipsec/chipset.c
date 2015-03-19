#ifndef UEFI_C_SOURCE

	#include "Include/BaseTypes.h"
	#include "Include/UefiTypes.h"
#else
	#include <Uefi.h>

#endif

#include <stdio.h>

#include "config.h"
#include "driver.h"
#include "settings.h"
#include "pci.h"

//#define VID_INTEL	0x1002
#define VID_INTEL	0x8086

#define CHIPSET_ID_COMMON  -1
#define CHIPSET_ID_UNKNOWN 0
#define CHIPSET_ID_SNB     1
#define CHIPSET_ID_JKT     2
#define CHIPSET_ID_IVB     3
#define CHIPSET_ID_IVT     4
#define CHIPSET_ID_HSW     5
#define CHIPSET_ID_BYT     6
#define CHIPSET_ID_BDW     7
#define CHIPSET_ID_QRK     8
#define CHIPSET_ID_AVN     9

#define CHIPSET_CODE_COMMON  "COMMON"
#define CHIPSET_CODE_UNKNOWN ""
#define CHIPSET_CODE_SNB     "SNB"
#define CHIPSET_CODE_JKT     "JKT"
#define CHIPSET_CODE_IVB     "IVB"
#define CHIPSET_CODE_IVT     "IVT"
#define CHIPSET_CODE_HSW     "HSW"
#define CHIPSET_CODE_BYT     "BYT"
#define CHIPSET_CODE_BDW     "BDW"
#define CHIPSET_CODE_QRK     "QRK"
#define CHIPSET_CODE_AVN     "AVN"

//CHAR8 *CHIPSET_FAMILY_XEON[] = { CHIPSET_ID_JKT, CHIPSET_ID_IVT, "" };
//CHAR8 *CHIPSET_FAMILY_CORE[]  = { CHIPSET_ID_SNB,CHIPSET_ID_IVB,CHIPSET_ID_HSW,CHIPSET_ID_BDW, "" };
//CHAR8 *CHIPSET_FAMILY_ATOM[]  = { CHIPSET_ID_BYT,CHIPSET_ID_AVN, "" };
//CHAR8 *CHIPSET_FAMILY_QUARK[] = { CHIPSET_ID_QRK, "" };

typedef struct _Chipset_Dictionary
{
	UINT16	VendorId;
	UINT16 key;
	CHAR8* name;
	UINT16 id;
	CHAR8* code;
	CHAR8* longname;
} CHIPSET_DICTIONARY;

CHIPSET_DICTIONARY Chipset_Dictionary[] =
{
	// 2nd Generation Core Processor Family (Sandy Bridge)
	{	VID_INTEL, 0x0100,	"Sandy Bridge", CHIPSET_ID_SNB, CHIPSET_CODE_SNB, "Desktop 2nd Generation Core Processor (Sandy Bridge CPU / Cougar Point PCH)" },
	{	VID_INTEL, 0x0104,	"Sandy Bridge", CHIPSET_ID_SNB, CHIPSET_CODE_SNB, "Mobile 2nd Generation Core Processor (Sandy Bridge CPU / Cougar Point PCH)" },
	{	VID_INTEL, 0x0108,	"Sandy Bridge", CHIPSET_ID_SNB, CHIPSET_CODE_SNB, "Intel Xeon Processor E3-1200 (Sandy Bridge CPU, C200 Series PCH)" },
	
	{	VID_INTEL, 0x3c00,	"Jaketown", CHIPSET_ID_JKT, CHIPSET_CODE_JKT, "Server 2nd Generation Core Processor (Jaketown CPU / Patsburg PCH)" },

	// 3rd Generation Core Processor Family (Ivy Bridge)
	{	VID_INTEL, 0x0150,	"Ivy Bridge", CHIPSET_ID_IVB, CHIPSET_CODE_IVB, "Desktop 3rd Generation Core Processor (Ivy Bridge CPU / Panther Point PCH)" },
	{	VID_INTEL, 0x0154,	"Ivy Bridge", CHIPSET_ID_IVB, CHIPSET_CODE_IVB, "Mobile 3rd Generation Core Processor (Ivy Bridge CPU / Panther Point PCH)" },
	{	VID_INTEL, 0x0158,	"Ivy Bridge", CHIPSET_ID_IVB, CHIPSET_CODE_IVB, "Intel Xeon Processor E3-1200 v2 (Ivy Bridge CPU, C200/C216 Series PCH)" },
	
	{	VID_INTEL, 0x0E00,	"Ivytown", CHIPSET_ID_IVB, CHIPSET_CODE_IVT, "Server 3rd Generation Core Procesor (Ivytown CPU / Patsburg PCH)" },
	
	
	{	VID_INTEL, 0x0C00,	"Sandy Bridge", CHIPSET_ID_HSW, CHIPSET_CODE_HSW, "Desktop 4th Generation Core Processor (Haswell CPU / Lynx Point PCH)" },
	{	VID_INTEL, 0x0C04,	"Sandy Bridge", CHIPSET_ID_HSW, CHIPSET_CODE_HSW, "Mobile 4th Generation Core Processor (Haswell M/H / Lynx Point PCH)" },
	{	VID_INTEL, 0x0C08,	"Sandy Bridge", CHIPSET_ID_HSW, CHIPSET_CODE_HSW, "Intel Xeon Processor E3-1200 v3 (Haswell CPU, C220 Series PCH)" },
	{	VID_INTEL, 0x0A00,	"Sandy Bridge", CHIPSET_ID_HSW, CHIPSET_CODE_HSW, "4th Generation Core Processor (Haswell U/Y)" },
	{	VID_INTEL, 0x0A04,	"Sandy Bridge", CHIPSET_ID_HSW, CHIPSET_CODE_HSW, "4th Generation Core Processor (Haswell U/Y)" },
	{	VID_INTEL, 0x0A08,	"Sandy Bridge", CHIPSET_ID_HSW, CHIPSET_CODE_HSW, "4th Generation Core Processor (Haswell U/Y)" },

	// Bay Trail SoC
	{	VID_INTEL, 0x0F00,	"Baytrail", CHIPSET_ID_BYT, CHIPSET_CODE_BYT, "Bay Trail" },

	//
	// Atom based SoC platforms
	//
	//
	// Galileo Board
	{	VID_INTEL, 0x0958,	"Galileo", CHIPSET_ID_QRK, CHIPSET_CODE_QRK, "Intel Quark SoC X1000" },


	//
	// QEMU 
	{	VID_INTEL, 0x1237, "i440FX", CHIPSET_ID_UNKNOWN, CHIPSET_CODE_COMMON, "Intel i440FX/LX Platform" },

	// NULL descriptor
	{	0x0000, 0x0000, "Null Chipset", 0, "", "Unknown Vendor/Device Id" }
};

//CHIPSET_DICTIONARY


/***
 *	ChipsetInit : EFI_STATUS
 *	Return RETURN_SUCCESS if initialization is done.. otherwise EFI_UNSUPPORTED
 **/
EFI_STATUS ChipsetInit()
{
	UINT32 DW;
	UINT16 VendorId, DeviceId;
	CHIPSET_DICTIONARY *Ptr = Chipset_Dictionary;

	DW = pci_read_dword(0, 0, 0, 0);

	VendorId = DW & 0xffff;
	DeviceId = (DW >> 16) & 0xffff;

	printf("[Chipset!Init] Vendor %04x Device %04x\n", VendorId, DeviceId);

	do
	{
		if (VendorId == Ptr->VendorId && DeviceId == Ptr->key)
		{
			// Found!
			gConfiguration = SupportedPlatform[0];	// standard platform pointer
			return RETURN_SUCCESS;
		}

		Ptr++;
	} while(Ptr->VendorId != 0);

	return RETURN_UNSUPPORTED;
}
