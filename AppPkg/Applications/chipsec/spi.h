#ifndef __SPI_H_

	#define __SPI_H_

struct LPC_BC_REG
{
	UINTN reg_value;
	BOOLEAN SMM_BWP;
	BOOLEAN TSS;
	BOOLEAN SRC;
	UINT8   BLE;
	BOOLEAN BIOSWE;
};

#pragma pack(push, 1)
struct SPI_REG
{
	UINT32	BFPR;		// BIOS Flash Primary Region
	UINT16	HSFSTS;		// Hardware Sequencing Flash Status
	UINT16	HSFCTL;		// Hardware Sequencing Flash Control
	UINT32	FADDR;		// Flash Address;
	UINT16	Pad00;	// Reserved -> Padding?
	UINT32	FDATA[0x10];	// Flash Data 0 .. N
	UINT32	FRACC;		// Flash Region Access Permissions
	UINT32	FREG0;		// Flash Region 0
	UINT32	FREG1;		// Flash Region 1
	UINT32	FREG2;		// Flash Region 2
	UINT32	FREG3;		// Flash Region 3
	UINT32	FREG4;		// Flash Region 4
	UINT32	FREG5;		// Reserved
	UINT32	FREG6;		// Reserved
	UINT32	FPR0;		// Flash Protected Range 0
	UINT32	FPR1;		// Flash Protected Range 1
	UINT32	FPR2;		// Flash Protected Range 2
	UINT32	FPR3;		// Flash Protected Range 3
	UINT32	FPR4;		// Flash Protected Range 4
	UINT32	Pad01[3];	// Padding
	UINT8	SSFSTS;		// Software Sequencing Flash Status;
	UINT8	SSFCTL[3];	// Software Sequencing Flash Control;
	UINT16	PREOP;		// Prefix Opcode Configuration
	UINT16	OPTYPE;		// Opcode Type Configuration
	UINT64	OPMENU;		// Opcode Menu Configuration
	UINT64	BBAR[2];	// BIOS Base Address Configuration
	UINT32	FDOC;		// Flash Descriptor Observability Control
	UINT32	FDOD;		// Flash Descriptor Observability Data
	UINT32	Pad03[3];	// Reserved
	UINT32	AFC;		// Additional Flash Control
	UINT32	LVSCC;		// Host Lower Vendor Specific Component Capabilities
	UINT32	UVSCC[2];		// Host Upper Vendor Specific Component Capabilities
	UINT32	FPB;		// Flash Partition Boundary

};
#pragma pack(pop)

struct SPI_obj
{
	CONFIGURATION*	Configuration;
	UINTN			rcba_spi_base;
	UINTN			spi_size;
	struct SPI_REG	*SPImap;	
	UINTN			block_erase_size;

	//
	struct LPC_BC_REG	BcReg;	// LPC_BC_REG
};

#define SPI_obj_size sizeof(struct SPI_obj)


EFI_STATUS SPI_Initialize(CONFIGURATION *Configuration, UINT8 *SPIData, UINTN *Size);
void get_SPI_region(struct SPI_obj *This, UINT16 spi_region_id, UINTN *range_base, UINTN *range_limit, UINTN* freg);

EFI_STATUS read_spi(struct SPI_obj *This, UINTN spi_addr, UINT32 BufferSize, UINT8 *Buffer, 
					void (*progress_callback)(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr),
					void (*error_callback)(UINT32 spiaddr, UINT16 command)
					);

EFI_STATUS write_spi(struct SPI_obj *This, UINTN spi_addr, UINT32 BufferSize, UINT8 *Buffer, 
					void (*progress_callback)(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr),
					void (*error_callback)(UINT32 spiaddr, UINT16 command)
					);

EFI_STATUS erase_spi(struct SPI_obj *This, UINTN spi_addr, UINTN size,
					void (*progress_callback)(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr),
					void (*error_callback)(UINT32 spiaddr, UINT16 command));

EFI_STATUS disable_BIOS_write_protection(struct SPI_obj *This);

#define FLASH_DESCRIPTOR	0
#define BIOS				1
#define	ME					2
#define	GBE					3
#define PLATFORM_DATA		4
#define	DEVICE_EXPANSION	5
#define SECONDARY_BIOS		6

#endif
