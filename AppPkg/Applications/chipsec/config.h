////////

#ifndef __CONFIG_H_
#define __CONFIG_H_

typedef struct _bar
{
	CHAR8	*Name;
	UINT16		Bus;
	UINT16		Dev;
	UINT16		Fun;
	UINT8		Reg;
	UINT8		Width;
	UINT64		Mask;
	UINT16		Size;
	UINT8		EnableBit;
	CHAR8*	Desc;
	UINT16		Offset;
} BAR;

typedef struct _MMIO
{
	CHAR8	*Name;
	UINT16		Bus;
	UINT16		Dev;
	UINT16		Fun;
	INT8		Reg;
	INT8		Width;
	UINT32	Mask;
	INT8		Size;
	INT8		EnableBit;
	CHAR8*	Desc;
	INT8		Offset;

} MMIO_STRUCT;

typedef  struct _IO
{
	CHAR8	*Name;
	UINT16		Bus;
	UINT16		Dev;
	UINT16		Fun;
	INT8		Reg;
	UINT32	Mask;
	INT8		Size;
	CHAR8*	Desc;
} IO;


typedef struct _memory
{
	CHAR8*	Name;
	CHAR8*	Type;
	UINT32	Address;
	UINT32	Size;
} MEMORY;

typedef struct _register_field
{
	CHAR8*	Name;
	INT8		Bit;
	INT8		Size;
} REGISTER_FIELD;

#define REGISTER_TYPE	UINT8
#define PCICFG	0
#define MMCFG	1
#define MMIO	2
#define	MSR		3
#define PORTIO	4
#define IOBAR	5

typedef struct _register
{
	CHAR8*			Name;
	REGISTER_TYPE	Type;
	UINT16			Bus;
	UINT16			Dev;
	UINT16			Fun;
	INT8			Offset;
	INT8			Size;
	CHAR8*			Desc;
	REGISTER_FIELD	**Fields;
} REGISTER;


typedef struct _configuration
{
	CHAR8*		Platform;
	BAR			**mmio;			// MMIO array
	IO			**io;			// IO array
	MEMORY		**memory;		// MEMORY array
	REGISTER	**Register;		// Register array
} CONFIGURATION;


#ifndef BIT0
	#define BIT0  0x0001
	#define BIT1  0x0002
	#define BIT2  0x0004
	#define BIT3  0x0008
	#define BIT4  0x0010
	#define BIT5  0x0020
	#define BIT6  0x0040
	#define BIT7  0x0080
	#define BIT8  0x0100
	#define BIT9  0x0200
	#define BIT10  0x0400
	#define BIT11  0x0800
	#define BIT12  0x1000
	#define BIT13  0x2000
	#define BIT14  0x4000
	#define BIT15  0x8000
	#define BIT16  0x00010000
	#define BIT17  0x00020000
	#define BIT18  0x00040000
	#define BIT19  0x00080000
	#define BIT20  0x00100000
	#define BIT21  0x00200000
	#define BIT22  0x00400000
	#define BIT23  0x00800000
	#define BIT24  0x01000000
	#define BIT25  0x02000000
	#define BIT26  0x04000000
	#define BIT27  0x08000000
	#define BIT28  0x10000000
	#define BIT29  0x20000000
	#define BIT30  0x40000000
	#define BIT31  0x80000000
	#define BIT32  0x100000000
	#define BIT33  0x200000000
	#define BIT34  0x400000000
	#define BIT35  0x800000000
	#define BIT36  0x1000000000
	#define BIT37  0x2000000000
	#define BIT38  0x4000000000
	#define BIT39  0x8000000000
	#define BIT40  0x10000000000
	#define BIT41  0x20000000000
	#define BIT42  0x40000000000
	#define BIT43  0x80000000000
	#define BIT44  0x100000000000
	#define BIT45  0x200000000000
	#define BIT46  0x400000000000
	#define BIT47  0x800000000000
	#define BIT48  0x1000000000000
	#define BIT49  0x2000000000000
	#define BIT50  0x4000000000000
	#define BIT51  0x8000000000000
	#define BIT52  0x10000000000000
	#define BIT53  0x20000000000000
	#define BIT54  0x40000000000000
	#define BIT55  0x80000000000000
	#define BIT56  0x100000000000000
	#define BIT57  0x200000000000000
	#define BIT58  0x400000000000000
	#define BIT59  0x800000000000000
	#define BIT60  0x1000000000000000
	#define BIT61  0x2000000000000000
	#define BIT62  0x4000000000000000
	#define BIT63  0x8000000000000000
#endif


#define ALIGNED_4KB    0xFFF    
#define ALIGNED_1MB    0xFFFFF
#define ALIGNED_8MB    0x7FFFFF
#define ALIGNED_64MB   0x3FFFFFF
#define ALIGNED_128MB  0x7FFFFFF
#define ALIGNED_256MB  0xFFFFFFF

/*############################################################################
// CPU common configuration
############################################################################*/
#define PCI_BUS0  0x0

// ----------------------------------------------------------------------------
// Device 0 PCIe Config
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Device 0 MMIO BARs
// ----------------------------------------------------------------------------
#define PCI_MCHBAR_REG_OFF            0x48

#define PCI_PCIEXBAR_REG_OFF          0x60
#define PCI_PCIEXBAR_REG_LENGTH_MASK  (0x3 << 1)
#define PCI_PCIEXBAR_REG_LENGTH_256MB 0x0
#define PCI_PCIEXBAR_REG_LENGTH_128MB 0x1
#define PCI_PCIEXBAR_REG_LENGTH_64MB  0x2
#define PCI_PCIEXBAR_REG_ADMSK64      (1 << 26)
#define PCI_PCIEXBAR_REG_ADMSK128     (1 << 27)
#define PCI_PCIEXBAR_REG_ADMSK256     0xF0000000

#define PCI_DMIBAR_REG_OFF             0x68

#define PCI_SMRAMC_BUS                0
#define PCI_SMRAMC_DEV                0
#define PCI_SMRAMC_FUN                0
#define PCI_SMRAMC_REG_OFF            0x88 // 0x9D before Sandy Bridge


// ----------------------------------------------------------------------------
// Device 2 (Processor Graphics/Display) MMIO BARs
// ----------------------------------------------------------------------------
#define PCI_GTDE_DEV                  2

#define PCI_GTTMMADR_REG_OFF          0x10
#define PCI_GMADR_REG_OFF             0x18

// ----------------------------------------------------------------------------
// HD Audio device configuration
// ----------------------------------------------------------------------------
#define PCI_HDA_DEV                    0x3
#define PCI_HDA_MMC_REG_OFF            0x62
#define PCI_HDA_MMAL_REG_OFF           0x64
#define PCI_HDA_MMAH_REG_OFF           0x68
#define PCI_HDA_MMD_REG_OFF            0x6C

#define PCI_HDAUDIOBAR_REG_OFF         0x10

// ----------------------------------------------------------------------------
// CPU MSRs
// ----------------------------------------------------------------------------
#define IA32_MTRRCAP_MSR             0xFE
#define IA32_MTRRCAP_SMRR_MASK       0x800

#define IA32_FEATURE_CONTROL_MSR     0x3A
#define IA32_FEATURE_CTRL_LOCK_MASK  0x1

#define IA32_SMRR_BASE_MSR           0x1F2
#define IA32_SMRR_BASE_MEMTYPE_MASK  0x7
#define IA32_SMRR_BASE_BASE_MASK     0xFFFFF000

#define IA32_SMRR_MASK_MSR           0x1F3
#define IA32_SMRR_MASK_VLD_MASK      0x800
#define IA32_SMRR_MASK_MASK_MASK     0xFFFFF000

#define MTRR_MEMTYPE_UC  0x0
#define MTRR_MEMTYPE_WB  0x6

#define IA32_MSR_CORE_THREAD_COUNT                    0x35
#define IA32_MSR_CORE_THREAD_COUNT_THREADCOUNT_MASK   0xFFFF
#define IA32_MSR_CORE_THREAD_COUNT_CORECOUNT_MASK     0xFFFF0000

#define IA32_PLATFORM_INFO_MSR       0xCE

/*############################################################################
// PCH common configuration
############################################################################*/

//----------------------------------------------------------------------------
// SPI Host Interface Registers
//----------------------------------------------------------------------------

#define PCH_RCBA_SPI_BFPR                   0x00  // BIOS Flash Primary Region Register (= FREG1)

#define PCH_RCBA_SPI_HSFSTS                 0x04  // Hardware Sequencing Flash Status Register
#define PCH_RCBA_SPI_HSFSTS_FLOCKDN         BIT15                         // Flash Configuration Lock-Down
#define PCH_RCBA_SPI_HSFSTS_FDV             BIT14                         // Flash Descriptor Valid
#define PCH_RCBA_SPI_HSFSTS_FDOPSS          BIT13                         // Flash Descriptor Override Pin-Strap Status
#define PCH_RCBA_SPI_HSFSTS_SCIP            BIT5                          // SPI cycle in progress
#define PCH_RCBA_SPI_HSFSTS_BERASE_MASK     (BIT4 | BIT3)                 // Block/Sector Erase Size
#define PCH_RCBA_SPI_HSFSTS_BERASE_256B     0x00                          // Block/Sector = 256 Bytes
#define PCH_RCBA_SPI_HSFSTS_BERASE_4K       0x01                          // Block/Sector = 4K Bytes
#define PCH_RCBA_SPI_HSFSTS_BERASE_8K       0x10                          // Block/Sector = 8K Bytes
#define PCH_RCBA_SPI_HSFSTS_BERASE_64K      0x11                          // Block/Sector = 64K Bytes
#define PCH_RCBA_SPI_HSFSTS_AEL             BIT2                          // Access Error Log
#define PCH_RCBA_SPI_HSFSTS_FCERR           BIT1                          // Flash Cycle Error
#define PCH_RCBA_SPI_HSFSTS_FDONE           BIT0                          // Flash Cycle Done

#define PCH_RCBA_SPI_HSFCTL                 0x06  // Hardware Sequencing Flash Control Register
#define PCH_RCBA_SPI_HSFCTL_FSMIE           BIT15                         // Flash SPI SMI Enable
#define PCH_RCBA_SPI_HSFCTL_FDBC_MASK       0x3F00                        // Flash Data Byte Count, Count = FDBC + 1.
#define PCH_RCBA_SPI_HSFCTL_FCYCLE_MASK     0x0006                        // Flash Cycle
#define PCH_RCBA_SPI_HSFCTL_FCYCLE_READ     0                             // Flash Cycle Read
#define PCH_RCBA_SPI_HSFCTL_FCYCLE_WRITE    2                             // Flash Cycle Write
#define PCH_RCBA_SPI_HSFCTL_FCYCLE_ERASE    3                             // Flash Cycle Block Erase
#define PCH_RCBA_SPI_HSFCTL_FCYCLE_FGO      BIT0                          // Flash Cycle GO

#define PCH_RCBA_SPI_FADDR                0x08  // SPI Flash Address
#define PCH_RCBA_SPI_FADDR_MASK           0x07FFFFFF                      // SPI Flash Address Mask [0:26]

#define PCH_RCBA_SPI_FDATA00              0x10  // SPI Data 00 (32 bits)
#define PCH_RCBA_SPI_FDATA01              0x14  
#define PCH_RCBA_SPI_FDATA02              0x18  
#define PCH_RCBA_SPI_FDATA03              0x1C  
#define PCH_RCBA_SPI_FDATA04              0x20  
#define PCH_RCBA_SPI_FDATA05              0x24  
#define PCH_RCBA_SPI_FDATA06              0x28  
#define PCH_RCBA_SPI_FDATA07              0x2C  
#define PCH_RCBA_SPI_FDATA08              0x30  
#define PCH_RCBA_SPI_FDATA09              0x34  
#define PCH_RCBA_SPI_FDATA10              0x38  
#define PCH_RCBA_SPI_FDATA11              0x3C  
#define PCH_RCBA_SPI_FDATA12              0x40  
#define PCH_RCBA_SPI_FDATA13              0x44  
#define PCH_RCBA_SPI_FDATA14              0x48  
#define PCH_RCBA_SPI_FDATA15              0x4C  

// SPI Flash Regions Access Permisions Register
#define PCH_RCBA_SPI_FRAP                 0x50
#define PCH_RCBA_SPI_FRAP_BMWAG_MASK      0xFF000000                    
#define PCH_RCBA_SPI_FRAP_BMWAG_GBE       BIT27                         
#define PCH_RCBA_SPI_FRAP_BMWAG_ME        BIT26                         
#define PCH_RCBA_SPI_FRAP_BMWAG_BIOS      BIT25                         
#define PCH_RCBA_SPI_FRAP_BMRAG_MASK      0x00FF0000                    
#define PCH_RCBA_SPI_FRAP_BMRAG_GBE       BIT19                         
#define PCH_RCBA_SPI_FRAP_BMRAG_ME        BIT18                         
#define PCH_RCBA_SPI_FRAP_BMRAG_BIOS      BIT17                         
#define PCH_RCBA_SPI_FRAP_BRWA_MASK       0x0000FF00                    
#define PCH_RCBA_SPI_FRAP_BRWA_SB         BIT14                         
#define PCH_RCBA_SPI_FRAP_BRWA_DE         BIT13                         
#define PCH_RCBA_SPI_FRAP_BRWA_PD         BIT12                         
#define PCH_RCBA_SPI_FRAP_BRWA_GBE        BIT11                         
#define PCH_RCBA_SPI_FRAP_BRWA_ME         BIT10                         
#define PCH_RCBA_SPI_FRAP_BRWA_BIOS       BIT9                          
#define PCH_RCBA_SPI_FRAP_BRWA_FLASHD     BIT8                          
#define PCH_RCBA_SPI_FRAP_BRRA_MASK       0x000000FF                    
#define PCH_RCBA_SPI_FRAP_BRRA_SB         BIT6                          
#define PCH_RCBA_SPI_FRAP_BRRA_DE         BIT5                          
#define PCH_RCBA_SPI_FRAP_BRRA_PD         BIT4                          
#define PCH_RCBA_SPI_FRAP_BRRA_GBE        BIT3                          
#define PCH_RCBA_SPI_FRAP_BRRA_ME         BIT2                          
#define PCH_RCBA_SPI_FRAP_BRRA_BIOS       BIT1                          
#define PCH_RCBA_SPI_FRAP_BRRA_FLASHD     BIT0                          

// Flash Region Registers
#define PCH_RCBA_SPI_FREG0_FLASHD            0x54  // Flash Region 0 (Flash Descriptor)
#define PCH_RCBA_SPI_FREG1_BIOS              0x58  // Flash Region 1 (BIOS)
#define PCH_RCBA_SPI_FREG2_ME                0x5C  // Flash Region 2 (ME)
#define PCH_RCBA_SPI_FREG3_GBE               0x60  // Flash Region 3 (GbE)
#define PCH_RCBA_SPI_FREG4_PLATFORM_DATA     0x64  // Flash Region 4 (Platform Data)
#define PCH_RCBA_SPI_FREG5_DEVICE_EXPANSION  0x68  // Flash Region 5 (Device Expansion)
#define PCH_RCBA_SPI_FREG6_SECONDARY_BIOS    0x6C  // Flash Region 6 (Secondary BIOS)

#define PCH_RCBA_SPI_FREGx_LIMIT_MASK     0x7FFF0000                    // Size
#define PCH_RCBA_SPI_FREGx_BASE_MASK      0x00007FFF                    // Base

// Protected Range Registers
#define PCH_RCBA_SPI_PR0                  0x74  // Protected Region 0 Register
#define PCH_RCBA_SPI_PR0_WPE              BIT31                         // Write Protection Enable
#define PCH_RCBA_SPI_PR0_PRL_MASK         0x7FFF0000                    // Protected Range Limit Mask
#define PCH_RCBA_SPI_PR0_RPE              BIT15                         // Read Protection Enable
#define PCH_RCBA_SPI_PR0_PRB_MASK         0x00007FFF                    // Protected Range Base Mask
#define PCH_RCBA_SPI_PR1                  0x78
#define PCH_RCBA_SPI_PR1_WPE              BIT31
#define PCH_RCBA_SPI_PR1_PRL_MASK         0x7FFF0000
#define PCH_RCBA_SPI_PR1_RPE              BIT15
#define PCH_RCBA_SPI_PR1_PRB_MASK         0x00007FFF
#define PCH_RCBA_SPI_PR2                  0x7C
#define PCH_RCBA_SPI_PR2_WPE              BIT31
#define PCH_RCBA_SPI_PR2_PRL_MASK         0x7FFF0000
#define PCH_RCBA_SPI_PR2_RPE              BIT15 
#define PCH_RCBA_SPI_PR2_PRB_MASK         0x00007FFF
#define PCH_RCBA_SPI_PR3                  0x80
#define PCH_RCBA_SPI_PR3_WPE              BIT31
#define PCH_RCBA_SPI_PR3_PRL_MASK         0x7FFF0000
#define PCH_RCBA_SPI_PR3_RPE              BIT15                         
#define PCH_RCBA_SPI_PR3_PRB_MASK         0x00007FFF                    
#define PCH_RCBA_SPI_PR4                  0x84  
#define PCH_RCBA_SPI_PR4_WPE              BIT31 
#define PCH_RCBA_SPI_PR4_PRL_MASK         0x7FFF0000
#define PCH_RCBA_SPI_PR4_RPE              BIT15     
#define PCH_RCBA_SPI_PR4_PRB_MASK         0x00007FFF

#define PCH_RCBA_SPI_OPTYPE               0x96  // Opcode Type Configuration
#define PCH_RCBA_SPI_OPTYPE7_MASK        (BIT15 | BIT14)
#define PCH_RCBA_SPI_OPTYPE6_MASK        (BIT13 | BIT12)
#define PCH_RCBA_SPI_OPTYPE5_MASK        (BIT11 | BIT10)
#define PCH_RCBA_SPI_OPTYPE4_MASK        (BIT9 | BIT8)  
#define PCH_RCBA_SPI_OPTYPE3_MASK        (BIT7 | BIT6)  
#define PCH_RCBA_SPI_OPTYPE2_MASK        (BIT5 | BIT4)  
#define PCH_RCBA_SPI_OPTYPE1_MASK        (BIT3 | BIT2)  
#define PCH_RCBA_SPI_OPTYPE0_MASK        (BIT1 | BIT0)  
#define PCH_RCBA_SPI_OPTYPE_RDNOADDR      0x00
#define PCH_RCBA_SPI_OPTYPE_WRNOADDR      0x01
#define PCH_RCBA_SPI_OPTYPE_RDADDR        0x02
#define PCH_RCBA_SPI_OPTYPE_WRADDR        0x03

#define PCH_RCBA_SPI_OPMENU               0x98  // Opcode Menu Configuration

#define PCH_RCBA_SPI_FDOC                 0xB0  // Flash Descriptor Observability Control Register
#define PCH_RCBA_SPI_FDOC_FDSS_MASK      (BIT14 | BIT13 | BIT12)       // Flash Descriptor Section Select
#define PCH_RCBA_SPI_FDOC_FDSS_FSDM       0x0000                        // Flash Signature and Descriptor Map
#define PCH_RCBA_SPI_FDOC_FDSS_COMP       0x1000                        // Component
#define PCH_RCBA_SPI_FDOC_FDSS_REGN       0x2000                        // Region
#define PCH_RCBA_SPI_FDOC_FDSS_MSTR       0x3000                        // Master
#define PCH_RCBA_SPI_FDOC_FDSI_MASK       0x0FFC                        // Flash Descriptor Section Index

#define PCH_RCBA_SPI_FDOD                 0xB4  // Flash Descriptor Observability Data Register

// ----------------------------------------------------------------------------
// PCI 0/31/0: PCH LPC Root Complex
// ----------------------------------------------------------------------------
#define PCI_B0D31F0_LPC_DEV  31
#define PCI_B0D31F0_LPC_FUN  0

#define LPC_BC_REG_OFF         0xDC //  BIOS Control (BC)
#define GEN_PMCON  0xA0

#define CFG_REG_PCH_LPC_PMBASE  0x40 // ACPI I/O Base (PMBASE/ABASE)
#define CFG_REG_PCH_LPC_PMBASE_MASK  ~0x1
#define CFG_REG_PCH_LPC_ACTL    0x44 // ACPI Control  (ACTL)
#define CFG_REG_PCH_LPC_GBA     0x44 // GPIO I/O Base (GBA)
#define CFG_REG_PCH_LPC_GC      0x44 // GPIO Control  (GC)

// PMBASE registers
#define PMBASE_SMI_EN          0x30 // SMI_EN offset in PMBASE (ABASE)

#define LPC_RCBA_REG_OFFSET    0xF0
#define RCBA_BASE_ADDR_SHIFT   14

// ----------------------------------------------------------------------------
// SPI Controller MMIO
// ----------------------------------------------------------------------------
#define SPI_MMIO_BUS          PCI_BUS0
#define SPI_MMIO_DEV          PCI_B0D31F0_LPC_DEV
#define SPI_MMIO_FUN          PCI_B0D31F0_LPC_FUN
#define SPI_MMIO_REG_OFFSET    0xF0
#define SPI_BASE_ADDR_SHIFT    14
#define SPI_MMIO_BASE_OFFSET   0x3800  // Base address of the SPI host interface registers off of RCBA
//#define #SPI_MMIO_BASE_OFFSET  0x3020  // Old (ICH8 and older) SPI registers base
   

#define SPI_BIOS_CONTROL_OFFSET  0xDC // BIOS Control Register



// ----------------------------------------------------------------------------
// PCI B0:D31:F3 SMBus Controller
// ----------------------------------------------------------------------------
#define PCI_B0D31F3_SMBUS_CTRLR_DEV 31
#define PCI_B0D31F3_SMBUS_CTRLR_FUN  0x3
//#0x8C22, 0x9C22 // HSW
//#0x1C22 // SNB
//#0x1E22 // IVB 0x0154
#define PCI_B0D31F3_SMBUS_CTRLR_DID  0x1C22

#define CFG_REG_PCH_SMB_CMD   0x04                    // D31:F3 Command

#define CFG_REG_PCH_SMB_SBA   0x20                    // SMBus Base Address
#define CFG_REG_PCH_SMB_SBA_BASE_ADDRESS_MASK  0xFFE0 // Base Address
#define CFG_REG_PCH_SMB_SBA_IO                BIT0   // I/O Space Indicator

#define CFG_REG_PCH_SMB_HCFG  0x40                    // D31:F3 Host Configuration
#define CFG_REG_PCH_SMB_HCFG_SPD_WD           BIT4   // SPD_WD
#define CFG_REG_PCH_SMB_HCFG_SSRESET          BIT3   // Soft SMBus Reset
#define CFG_REG_PCH_SMB_HCFG_I2C_EN           BIT2   // I2C Enable
#define CFG_REG_PCH_SMB_HCFG_SMB_SMI_EN       BIT1   // SMBus SMI Enable
#define CFG_REG_PCH_SMB_HCFG_HST_EN           BIT0   // SMBus Host Enable

// ----------------------------------------------------------------------------
// PCH I/O Base Registers
// ----------------------------------------------------------------------------

#define TCOBASE_ABASE_OFFSET  0x60


// ----------------------------------------------------------------------------
// PCH RCBA
// ----------------------------------------------------------------------------


#define RCBA_GENERAL_CONFIG_OFFSET  0x3400  // Offset of BIOS General Configuration memory mapped registers base in RCBA

#define RCBA_GC_RC_REG_OFFSET       0x0     // RTC Configuration (RC) register

#define RCBA_GC_GCS_REG_OFFSET      0x10    // General Control and Status (GCS) register
#define RCBA_GC_GCS_REG_BILD_MASK   0x1     // BIOS Interface Lock-Down (BILD)
#define RCBA_GC_GCS_REG_BBS_MASK    0xC00   // Boot BIOS Straps (BBS) - PCI/SPI/LPC
#define RCBA_GC_BUC_REG_OFFSET      0x14    // Backup Control (BUC) register
#define RCBA_GC_BUC_REG_TS_MASK     0x1     // Top-Swap strap (TS)


// #########################################################################
// # global configuration for "standard" platform
extern CONFIGURATION *SupportedPlatform[];


BAR *get_BAR(CONFIGURATION *Platform, CHAR8* BarName);

//
BAR *get_MMIO_BAR(CONFIGURATION *Platform, CHAR8* BarName);
REGISTER *get_BAR_reg(CONFIGURATION *Platform, CHAR8* BarName);

BOOLEAN is_MMIO_BAR_defined(CONFIGURATION *Platform, CHAR8* BarName);
BOOLEAN is_REG_BAR_defined(CONFIGURATION *Platform, CHAR8 *RegName);



// MMIO bar
EFI_STATUS get_MMIO_BAR_base_address(BAR *bar, UINT64 *base, unsigned int *size);

UINTN read_MMIO_reg(UINTN bar_base, UINTN offset);
VOID write_MMIO_reg(UINTN bar_base, UINTN offset, UINTN value);


// REGISTER access!
UINTN read_register(CONFIGURATION *Configuration, CHAR8 *reg_name);
VOID write_register(CONFIGURATION *Configuration, CHAR8 *reg_name, UINTN value);

#endif
