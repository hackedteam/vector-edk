/// static configuration
#ifndef UEFI_C_SOURCE

	#include "Include/BaseTypes.h"
	#include "Include/UefiTypes.h"
#else
	#include <Uefi.h>

#endif

#include <string.h>
#include <stdio.h>

#include "config.h"
#include "driver.h"

REGISTER_FIELD _BIOSWE = { "BIOSWE", 0, 1 };
REGISTER_FIELD _BLE = { "BLE", 1, 1 };
REGISTER_FIELD _SRC = { "SRC", 2, 2 };
REGISTER_FIELD _TSS = { "TSS", 4, 1 };
REGISTER_FIELD _SMM_BWP =	{ "SMM_BWP", 5, 1 };

REGISTER_FIELD *_bc_fields[] =
{
	&_BIOSWE,
	&_BLE,
	&_SRC,
	&_TSS,
	&_SMM_BWP,
	NULL
};

REGISTER_FIELD _PRB = { "PRB", 0, 13 };
REGISTER_FIELD _RPE = { "RPE", 15, 1 };
REGISTER_FIELD _PRL = { "PRL", 16, 1 };
REGISTER_FIELD _WPE = { "WPE", 31, 1 };

REGISTER_FIELD *pr0_fields[] = 
{
	&_BIOSWE,
	&_BLE,
	&_SRC,
	&_TSS,
	&_SMM_BWP,
	NULL
};

REGISTER_FIELD *pr1_fields[] = 
{
	&_BIOSWE,
	&_BLE,
	&_SRC,
	&_TSS,
	&_SMM_BWP,
	NULL

};

REGISTER_FIELD *pr2_fields[] = 
{
	&_BIOSWE,
	&_BLE,
	&_SRC,
	&_TSS,
	&_SMM_BWP,
	NULL

};

REGISTER_FIELD *pr3_fields[] = 
{
	&_BIOSWE,
	&_BLE,
	&_SRC,
	&_TSS,
	&_SMM_BWP,
	NULL

};

REGISTER_FIELD *pr4_fields[] = 
{
	&_BIOSWE,
	&_BLE,
	&_SRC,
	&_TSS,
	&_SMM_BWP,
	NULL
};

MEMORY _LegacyDOS ={ "Legacy DOS", "dram", 0x0, 0x100000 };

MEMORY _TPM = { "TPM", "mmio", 0xFED40000, 0x10000 };

MEMORY *_memory[] =
{
	&_LegacyDOS,
	&_TPM,
	NULL
};

REGISTER _VID = 	{ "PCI0.0.0_VID",	PCICFG, 0, 0, 0, 0x0, 2, "Vendor ID", NULL };
REGISTER _DID = { "PCI0.0.0_DID", PCICFG, 0, 0, 0, 0x2, 2, "Device ID", NULL };
REGISTER _PXPEBAR = { "PCI0.0.0_PXPEPBAR", PCICFG, 0, 0, 0, 0x40, 8, "PCI Express Egress Port Base Address", NULL };
REGISTER _MCHBAR = { "PCI0.0.0_MCHBAR", PCICFG, 0, 0, 0, 0x48, 8, "MCH Base Address", NULL };
REGISTER _GGC = { "PCI0.0.0_GGC", PCICFG, 0, 0, 0, 0x50, 2, "Graphics Control", NULL };
REGISTER _DEVEN = { "PCI0.0.0_DEVEN", PCICFG, 0, 0, 0, 0x54, 4, "Device Enables", NULL };
REGISTER _DPR = { "PCI0.0.0_PAVPC", PCICFG, 0, 0, 0, 0x58, 4, "PAVP Configuration", NULL };
REGISTER _PCIEXBAR = { "PCI0.0.0_PCIEXBAR", PCICFG, 0, 0, 0, 0x60, 8, "PCIe MMCFG Base Address", NULL };
REGISTER _DMIBAR = { "PCI0.0.0_DMIBAR", PCICFG, 0, 0, 0, 0x68, 8, "DMI Base Address", NULL };
REGISTER _MESEG_BASE = { "PCI0.0.0_MESEG_BASE", PCICFG, 0, 0, 0, 0x70, 8, "Manageability Engine Base Address Register", NULL };
REGISTER _MESEG_MASK = { "PCI0.0.0_MESEG_MASK", PCICFG, 0, 0, 0, 0x78, 8, "Manageability Engine Limit Address Register", NULL };
REGISTER _PAM0 = { "PCI0.0.0_PAM0", PCICFG, 0, 0, 0, 0x80, 1, "Programmable Attribute Map 0", NULL };
REGISTER _PAM1 = { "PCI0.0.0_PAM1", PCICFG, 0, 0, 0, 0x81, 1,"Programmable Attribute Map 1", NULL };
REGISTER _PAM2 = { "PCI0.0.0_PAM2", PCICFG, 0, 0, 0, 0x82, 1,"Programmable Attribute Map 2", NULL };
REGISTER _PAM3 = { "PCI0.0.0_PAM3", PCICFG, 0, 0, 0, 0x83, 1,"Programmable Attribute Map 3", NULL };
REGISTER _PAM4 = { "PCI0.0.0_PAM4", PCICFG, 0, 0, 0, 0x84, 1,"Programmable Attribute Map 4", NULL };
REGISTER _PAM5 = { "PCI0.0.0_PAM5", PCICFG, 0, 0, 0, 0x85, 1,"Programmable Attribute Map 5", NULL };
REGISTER _PAM6 = { "PCI0.0.0_PAM6", PCICFG, 0, 0, 0, 0x86, 1,"Programmable Attribute Map 6", NULL };
REGISTER _LAC = { "PCI0.0.0_LAC", PCICFG, 0, 0, 0, 0x87, 1,"Legacy Access Control", NULL };
REGISTER _SMRAMC = { "PCI0.0.0_SMRAMC", PCICFG, 0, 0, 0, 0x88, 1,"System Management RAM Control", NULL };
REGISTER _REMAPBASE = { "PCI0.0.0_REMAPBASE", PCICFG, 0, 0, 0, 0x90, 8,"Memory Remap Base Address", NULL };
REGISTER _REMAPLIMIT = { "PCI0.0.0_REMAPLIMIT", PCICFG, 0, 0, 0, 0x98, 8,"Memory Remap Limit Address", NULL };
REGISTER _TOM = { "PCI0.0.0_TOM", PCICFG, 0, 0, 0, 0xA0, 8, "Top of Memory", NULL };
REGISTER _TOUUD = { "PCI0.0.0_TOUUD", PCICFG, 0, 0, 0, 0xA8, 8, "Top of Upper Usable DRAM", NULL };
REGISTER _BDSM = { "PCI0.0.0_BDSM", PCICFG, 0, 0, 0, 0xB0, 4, "Base of Graphics Stolen Memory", NULL };
REGISTER _BGSM = { "PCI0.0.0_BGSM", PCICFG, 0, 0, 0, 0xB4, 4, "Base of GTT Stolen Memory", NULL };
REGISTER _TSEGMB = { "PCI0.0.0_TSEGMB", PCICFG, 0, 0, 0, 0xB8, 4, "TSEG Memory Base", NULL };
REGISTER _TOLUD = { "PCI0.0.0_TOLUD", PCICFG, 0, 0, 0, 0xBC, 4, "Top of Low Usable DRAM", NULL };
REGISTER _SKPD = { "PCI0.0.0_SKPD", PCICFG, 0, 0, 0, 0xDC, 4, "Scratchpad Data", NULL };
REGISTER _CAPID0_A = { "PCI0.0.0_CAPID0_A", PCICFG, 0, 0, 0, 0xE4, 4, "Capabilities A", NULL };
REGISTER _CAPID0_B = { "PCI0.0.0_CAPID0_B", PCICFG, 0, 0, 0, 0xE8, 4, "Capabilities B", NULL };
REGISTER _BC = { "BC", PCICFG, 0, 0x1f, 0, 0xdc, 1, "BIOS Control", _bc_fields };
REGISTER _PR0 = { "PR0", MMIO, 0, 0, 0, 0x74, 1, "Protected Range 0", pr0_fields };
REGISTER _PR1 = { "PR1", MMIO, 0, 0, 0, 0x78, 4, "Protected Range 1", pr1_fields };
REGISTER _PR2 = { "PR2", MMIO, 0, 0, 0, 0x7c, 4, "Protected Range 2", pr2_fields };
REGISTER _PR3 = { "PR3", MMIO, 0, 0, 0, 0x80, 4, "Protected Range 3", pr3_fields };
REGISTER _PR4 = { "PR4", MMIO, 0, 0, 0, 0x84, 4, "Protected Range 4", pr4_fields };

REGISTER *_registers[] =
{
	&_VID,
	&_DID,
	&_PXPEBAR,
	&_MCHBAR,
	&_GGC,
	&_DEVEN,
	&_DPR,
	&_PCIEXBAR,
	&_DMIBAR,
	&_MESEG_BASE,
	&_MESEG_MASK,
	&_PAM0,
	&_PAM1,
	&_PAM2,
	&_PAM3, // { "PCI0.0.0_PAM3", "pcicfg", 0, 0, 0, 0x83, 1,"Programmable Attribute Map 3", NULL }
	&_PAM4, // { "PCI0.0.0_PAM4", "pcicfg", 0, 0, 0, 0x84, 1,"Programmable Attribute Map 4", NULL }
	&_PAM5, // { "PCI0.0.0_PAM5", "pcicfg", 0, 0, 0, 0x85, 1,"Programmable Attribute Map 5", NULL }
	&_PAM6, // { "PCI0.0.0_PAM6", "pcicfg", 0, 0, 0, 0x86, 1,"Programmable Attribute Map 6", NULL }
	&_LAC, // { "PCI0.0.0_LAC", "pcicfg", 0, 0, 0, 0x87, 1,"Legacy Access Control", NULL }
	&_SMRAMC, //{ "PCI0.0.0_SMRAMC", "pcicfg", 0, 0, 0, 0x88, 1,"System Management RAM Control", NULL }
	&_REMAPBASE, //{ "PCI0.0.0_REMAPBASE", "pcicfg", 0, 0, 0, 0x90, 8,"Memory Remap Base Address", NULL }
	&_REMAPLIMIT, // { "PCI0.0.0_REMAPLIMIT", "pcicfg", 0, 0, 0, 0x98, 8,"Memory Remap Limit Address", NULL }
	&_TOM, // { "PCI0.0.0_TOM", "pcicfg", 0, 0, 0, 0xA0, 8, "Top of Memory", NULL }
	&_TOUUD, // { "PCI0.0.0_TOUUD", "pcicfg", 0, 0, 0, 0xA8, 8, "Top of Upper Usable DRAM", NULL }
	&_BDSM, // { "PCI0.0.0_BDSM", "pcicfg", 0, 0, 0, 0xB0, 4, "Base of Graphics Stolen Memory", NULL }
	&_BGSM, // { "PCI0.0.0_BGSM", "pcicfg", 0, 0, 0, 0xB4, 4, "Base of GTT Stolen Memory", NULL }
	&_TSEGMB, // { "PCI0.0.0_TSEGMB", "pcicfg", 0, 0, 0, 0xB8, 4, "TSEG Memory Base", NULL }
	&_TOLUD, // { "PCI0.0.0_TOLUD", "pcicfg", 0, 0, 0, 0xBC, 4, "Top of Low Usable DRAM", NULL }
	&_SKPD, // { "PCI0.0.0_SKPD", "pcicfg", 0, 0, 0, 0xDC, 4, "Scratchpad Data", NULL }
	&_CAPID0_A, // { "PCI0.0.0_CAPID0_A", "pcicfg", 0, 0, 0, 0xE4, 4, "Capabilities A", NULL }
	&_CAPID0_B, // { "PCI0.0.0_CAPID0_B", "pcicfg", 0, 0, 0, 0xE8, 4, "Capabilities B", NULL }
	&_BC, // { "BC", "pcicfg", 0, 0x1f, 0, 0xdc, 1, "BIOS Control", &_bc_fields }
	&_PR0, // { "PR0", "mmio/SPIBAR", 0, 0, 0x74, 1, "Protected Range 0", &pr0_fields }
	&_PR1, // { "PR1", "mmio/SPIBAR", 0, 0, 0x78, 4, "Protected Range 1", &pr1_fields }
	&_PR2, // { "PR2", "mmio/SPIBAR", 0, 0, 0x7c, 4, "Protected Range 0", &pr2_fields }
	&_PR3, // { "PR3", "mmio/SPIBAR", 0, 0, 0x80, 4, "Protected Range 0", &pr3_fields }
	&_PR4, // { "PR4", "mmio/SPIBAR", 0, 0, 0x84, 4, "Protected Range 0", &pr4_fields }
	NULL
};

BAR _pxpepbar =
{
	"PXPEPBAR", 0, 0, 0, 0x40, 8, 0x7FFFFFF000, 0x1000, 0, "PCI Express Egress Port Register Range", 0
};

BAR _mchbar =
{
	"MCHBAR", 0, 0, 0, 0x48, 8, 0x7FFFFF8000, 0x8000, 0, "Host Memory Mapped Register Range", 0
};


BAR _mmcfg =
{
	"MMCFG", 0, 0, 0, 0x60, 8, 0x7FFFFFF000, 0x1000, 0, "PCI Express Register Range", 0
};

BAR _dmibar =
{
	"DMIBAR", 0, 0, 0, 0x68, 8, 0x7FFFFFF000, 0x1000, 0, "Root Complex Register Range", 0
};

BAR _gttmmadr =
{
	"GTTMMADR", 0, 2, 0, 0x10, 8, 0x7FFFC00000, 0, 0, "Graphics Translation Table Range", 0
};

BAR _gmadr =
{
	"GMADR", 0, 2, 0, 0x18, 8, 0x7FF8000000, 0, 0, "Graphics Memory Range", 0
};

BAR _hdabar =
{
	"HDABAR", 0, 3, 0, 0x10, 8, 0x7FFFFFF000, 0x1000, 0, "HD Audio Base", 0
};

BAR _rcba =
{
	"RCBA", 0, 0x1f, 0, 0xf0, 4, 0xffffc000, 0x4000, 0, "Root Complex Base Address", 0
};

BAR _spibar =
{
	"SPIBAR", 0, 0x1f, 0, 0xf0, 4, 0xffffc000, 0x200, 0, "SPI Controller Register Range", 0x3800
};


BAR *_mmio[] =
{
	&_pxpepbar,
	&_mchbar,
	&_mmcfg,
	&_dmibar,
	&_gttmmadr,
	&_gmadr,
	&_hdabar,
	&_rcba,
	&_spibar,
	NULL
};

IO _PMBASE = { "PMBASE", 0, 0x1f, 0x0, 0x40, 0xff80, 0x80, "ACPI Base Address"};

IO *_io[] = 
{
	&_PMBASE,
	NULL
};

CONFIGURATION _standard = 
{
	"Standard PCI Platform",			// standard platform!
	_mmio,
	_io,
	_memory,
	_registers
};

CONFIGURATION _avn =
{
	"AVN",
	_mmio,
	_io,
	_memory,
	_registers
};

CONFIGURATION _byt = 
{
	"BYT",
	_mmio,
	_io,
	_memory,
	_registers
};

// List of supported platform and their // settings
CONFIGURATION *SupportedPlatform[] =
{
	&_standard,
	&_avn,
	&_byt,
	NULL
};


BAR *get_BAR(CONFIGURATION *Platform, CHAR8* BarName)
{
	BAR	**mmio = Platform->mmio;
	int i;

	if (mmio == NULL)
		return NULL;	// no "MMIO" register defined

	i = 0;

	while(mmio[i] != NULL)
	{
		if (strcmp(mmio[i]->Name, BarName) == 0)
		{	// found barname
			return mmio[i];
		}

		i++;
	}

	// not supported!
	return NULL;
}


REGISTER *get_BAR_reg(CONFIGURATION *Platform, CHAR8* BarName)
{
	REGISTER **mmio = Platform->Register;
	int i;

	if (mmio == NULL)
		return NULL;	// no registers defined

	i = 0;

	while(mmio[i] != NULL)
	{
		if (strcmp(mmio[i]->Name, BarName) == 0)
		{	// found barname
			return mmio[i];
		}

		i++;
	}

	// not supported!
	return NULL;
}
