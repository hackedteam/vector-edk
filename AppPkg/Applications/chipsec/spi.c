#include <Uefi.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "config.h"
#include "driver.h"
#include "settings.h"
#include "pci.h"
#include "spi.h"

#define SPI_READ_WRITE_MAX_DBC	64
#define SPI_READ_WRITE_DEF_DB	4

#define HSFCTL_READ_CYCLE	((PCH_RCBA_SPI_HSFCTL_FCYCLE_READ << 1) | PCH_RCBA_SPI_HSFCTL_FCYCLE_FGO)
#define HSFCTL_WRITE_CYCLE	((PCH_RCBA_SPI_HSFCTL_FCYCLE_WRITE << 1) | PCH_RCBA_SPI_HSFCTL_FCYCLE_FGO)
#define HSFCTL_ERASE_CYCLE	((PCH_RCBA_SPI_HSFCTL_FCYCLE_ERASE << 1) | PCH_RCBA_SPI_HSFCTL_FCYCLE_FGO)

#define HSFSTS_CLEAR		(PCH_RCBA_SPI_HSFSTS_AEL | PCH_RCBA_SPI_HSFSTS_FCERR | PCH_RCBA_SPI_HSFSTS_FDONE)

#define SPI_HSFSTS_OFFSET	0x04
#define SPI_HSFSTS_FLOCKDN_MASK	( 1 << 15)
#define SPI_HSFSTS_FDOPSS_MASK (1 << 13)

#define SPI_REGION_NUMBER		7
#define SPI_REGION_NUMBER_IN_FD	5

UINTN	SPI_REGION[] =
{
	PCH_RCBA_SPI_FREG0_FLASHD,
	PCH_RCBA_SPI_FREG1_BIOS,
	PCH_RCBA_SPI_FREG2_ME,
	PCH_RCBA_SPI_FREG3_GBE,
	PCH_RCBA_SPI_FREG4_PLATFORM_DATA,
	PCH_RCBA_SPI_FREG5_DEVICE_EXPANSION,
	PCH_RCBA_SPI_FREG6_SECONDARY_BIOS
};

char *SPI_REGION_NAMES[] = 
{
	"Flash Descriptor",
	"BIOS",
	"Intel ME",
	"GBe",
	"Platform Data",
	"Device Expansion",
	"Secondary BIOS"
};

//#
//# Flash Descriptor Master Defines
//#
#define SPI_MASTER_NUMBER_IN_FD

#define MASTER_HOST_CPU_BIOS    0
#define MASTER_ME               1
#define MASTER_GBE              2


char *SPI_MASTER_NAMES[] = 
{
	"CPU/BIOS",
	"ME",
	"GBe"
};

///
VOID _get_SPI_region(UINTN flreg, UINTN *range_base, UINTN *range_limit)
{
	*range_base = (flreg & PCH_RCBA_SPI_FREGx_BASE_MASK) << 12;
	*range_limit = (flreg & PCH_RCBA_SPI_FREGx_LIMIT_MASK) >> 4;

	*range_limit = *range_limit + 0xfff;

	printf("[SPI!region] %08x %04x\n", *range_base, *range_limit);
}

UINTN get_SPI_MMIO_base_fallback()
{
	UINT32 reg_value;
	UINTN spi_base;

	reg_value = pci_read_dword(SPI_MMIO_BUS, SPI_MMIO_DEV, SPI_MMIO_FUN, SPI_MMIO_REG_OFFSET);

	spi_base = ((reg_value >> SPI_BASE_ADDR_SHIFT) << SPI_BASE_ADDR_SHIFT) + SPI_MMIO_BASE_OFFSET;

	return spi_base;
}


EFI_STATUS get_SPI_MMIO_base(UINTN *SPIBASE, UINTN *SPISIZE)
{
	UINT64 _spibase;
	unsigned int _spisize;
	BAR *spibar = get_BAR(gConfiguration, "SPIBAR");	//

	printf("[SPI!get_SPI_MMIO_base] Get SPI/BAR\n");


	if (spibar == NULL)
	{
		printf("[SPI!get_SPI_MMIO_base] Get SPI/BAR via fallback\n");
		*SPIBASE = get_SPI_MMIO_base_fallback();
		*SPISIZE = 0;
	}
	else
	{
		printf("[SPI!get_SPI_MMIO_base] Get SPI/BAR via MMIO\n");
		get_MMIO_BAR_base_address(spibar, &_spibase, &_spisize);
		*SPISIZE = (UINTN) _spisize;
		*SPIBASE = (UINTN) _spibase;
	}
	return RETURN_SUCCESS;
}

EFI_STATUS SPI_Initialize(CONFIGURATION *Configuration, UINT8 *SPIData, UINTN *Size)
{
	struct SPI_obj *This;
	EFI_STATUS Status;

	if (*Size != 0 && (Configuration == NULL || SPIData == NULL))
	{
		Status = EFI_INVALID_PARAMETER;
		goto ERROR;
	}

	if (*Size < SPI_obj_size)
	{	// require X memory
		*Size = SPI_obj_size;
		Status = EFI_BUFFER_TOO_SMALL;
		goto ERROR;
	}

	if (*Size == 0)
	{
		EFI_INVALID_PARAMETER;
		*Size = SPI_obj_size;
		goto ERROR;
	}

	This = (struct SPI_obj *) SPIData;

	memset(This, 0, *Size);

	This->Configuration = Configuration;
	
	get_SPI_MMIO_base(&This->rcba_spi_base, &This->spi_size);

	This->SPImap = (struct SPI_REG *) This->rcba_spi_base;

#define BERASE_MASK 0x18
#define BERASE_BIT     3

	// Block/Sector Erase Size
	switch((This->SPImap->HSFSTS & 0x18) >> BERASE_BIT)
	{
		case 0: This->block_erase_size = 256; break;
		case 1: This->block_erase_size = 4 * 1024; break;
		case 2: This->block_erase_size = 8 * 1024; break;
		case 3: This->block_erase_size = 64 * 1024; break;
		default: This->block_erase_size = 4; break;
	};

	Status = disable_BIOS_write_protection(This);

ERROR:	// wrong object size
	return Status;
}

UINT8 *SPI_reg_addr(struct SPI_obj *This, UINTN reg)
{
	UINTN addr_value;

	addr_value = This->rcba_spi_base + reg;

	//printf("\nSPI_reg_addr %08x", addr_value);
	return (UINT8 *) addr_value;
}

UINTN SPI_reg_read(struct SPI_obj *This, UINTN reg)
{
	UINTN reg_value;

	reg_value = read_MMIO_reg(This->rcba_spi_base, reg);

	//printf("[SPI!reg_read] %08x %04x = %08x\n", This->rcba_spi_base, reg, reg_value);

	return reg_value;
}

void SPI_reg_write(struct SPI_obj *This, UINTN reg, UINTN value)
{
	//printf("[SPI!reg_write] %08x %04x %08x\n", This->rcba_spi_base, reg, value);
	write_MMIO_reg(This->rcba_spi_base, reg, value);
}

void get_SPI_region(struct SPI_obj *This, UINT16 spi_region_id, UINTN *range_base, UINTN *range_limit, UINTN* freg)
{
	*freg = SPI_reg_read(This, (UINTN) spi_region_id);

	_get_SPI_region(*freg, range_base, range_limit);
}

void get_SPI_regions()
{
	// wrong! not implemented
}

EFI_STATUS get_SPI_Protected_Range(struct SPI_obj *This, UINTN prn_num, UINTN *base, UINTN *limit, BOOLEAN *wpe, UINTN *rpe, UINTN *pr_j_reg, UINTN *pr_j)
{
	if (prn_num > 5)
	{
		return RETURN_INVALID_PARAMETER;
	}

	*pr_j_reg = PCH_RCBA_SPI_PR0 + prn_num * 4;

	*pr_j = SPI_reg_read(This, *pr_j_reg);
	*base = (*pr_j & PCH_RCBA_SPI_PR0_PRB_MASK) << 12;
	*limit = (*pr_j & PCH_RCBA_SPI_PR0_PRL_MASK) >> 4;
	*wpe = (*pr_j & PCH_RCBA_SPI_PR0_WPE) != 0;
	*rpe = (*pr_j & PCH_RCBA_SPI_PR0_RPE) != 0;

	return RETURN_SUCCESS;
}

void get_BIOS_Control_Init(struct LPC_BC_REG *BcReg, UINT8 reg_value)
{
	BcReg->reg_value = reg_value;

	// BcReg->SMM_BWP = (reg_value >> 5) & 0x01;
	BcReg->TSS = (reg_value >> 4) & 0x01;	// Top Swap Status
	BcReg->SRC = (reg_value >> 2) & 0x03;	// SPI Read Configuration
	BcReg->BLE = (reg_value >> 1) & 0x01;	// BIOS Lock Enable
	BcReg->BIOSWE = reg_value & 0x01;		// BIOS Write Enable
}

void get_BIOS_Control_fallback(struct SPI_obj *This)
{
	UINT8 reg_value = pci_read_byte(0, 31, 0, LPC_BC_REG_OFF);

	get_BIOS_Control_Init(&This->BcReg, reg_value);
}


void get_BIOS_Control(struct SPI_obj *This, struct LPC_BC_REG *BcReg)
{
	UINT8 reg_value;
		
	if (get_BAR_reg(This->Configuration, "BC") != NULL)
	{
		reg_value = (UINT8) read_register(This->Configuration, "BC");
		get_BIOS_Control_Init(BcReg, reg_value);
	}
	else
	{
		get_BIOS_Control_fallback(This);
	}
}

/***
 *
 **/
EFI_STATUS disable_BIOS_write_protection(struct SPI_obj *This)
{
	UINT8 reg_value;

	get_BIOS_Control(This, &This->BcReg);

	printf("[BcReg] %x [0]\n", This->BcReg.reg_value);

	if (This->BcReg.BLE && (!This->BcReg.BIOSWE))
	{	// BIOS write protection enabled!
		printf("[disable_BIOS_write_protection] BIOS write protection enabled!\n");
		return EFI_UNSUPPORTED;
	}
	else if (This->BcReg.BIOSWE)
	{	// BIOS write protection not enabled..
		printf("[disable_BIOS_write_protection] BIOS write protection not enabled..!\n");
		return EFI_SUCCESS;
	}

	// BIOS protection enabled but not locked!
	printf("[disable_BIOS_write_protection] BIOS protection enabled but not locked!\n");

	reg_value = This->BcReg.reg_value;

	reg_value |= 0x01;	// 

	write_register(This->Configuration, "BC", reg_value);

	get_BIOS_Control(This, &This->BcReg);
	printf("[BcReg] %x [1]\n", This->BcReg.reg_value);

	if (This->BcReg.BIOSWE)
	{	// BIOS protection disabled succesful!
		printf("[disable_BIOS_write_protection] BIOS protection disabled succesful!!\n");
		return EFI_SUCCESS;
	}

	printf("[disable_BIOS_write_protection] DISABLING failed!!\n");
	return EFI_ACCESS_DENIED;	// Access denied!
}

EFI_STATUS _wait_SPI_flash_cyclo_done(struct SPI_obj *This)
{
	UINT32 spi_base;
	UINT16 hsfsts;
	INT16 i;
	UINT8 cyclo_done;

	spi_base = (UINT32) This->rcba_spi_base;
	cyclo_done = 0;
	hsfsts = 0;

	for(i = 0; i < 1000 && cyclo_done == 0; i++)
	{	// Hardware sets this bit when software sets the Flash Cyclo GO.
		// This bit remains set until the cycle completes on the SPI interface.
		usleep(10);	// wait for 100ms
		ReadPhysMem((UINT8 *)&hsfsts, 0, spi_base + PCH_RCBA_SPI_HSFSTS, sizeof(UINT16));
		
		cyclo_done = !(hsfsts & PCH_RCBA_SPI_HSFSTS_SCIP);
	}

	if (cyclo_done == 0)
	{	// wait!
		usleep(100);	// wait for 100ms

		ReadPhysMem((UINT8 *)&hsfsts, 0, spi_base + PCH_RCBA_SPI_HSFSTS, sizeof(UINT16));
		cyclo_done = !(hsfsts & PCH_RCBA_SPI_HSFSTS_SCIP);
	}

	if (cyclo_done)
	{
		hsfsts = HSFSTS_CLEAR;

		WritePhysMem((UINT8*) &hsfsts, 0, spi_base + PCH_RCBA_SPI_HSFSTS, sizeof(UINT16));	// Set bits AEL, FCERR, FDONE

		ReadPhysMem((UINT8 *)&hsfsts, 0, spi_base + PCH_RCBA_SPI_HSFSTS, sizeof(UINT16));	// Read bits....

		cyclo_done = !((hsfsts & PCH_RCBA_SPI_HSFSTS_AEL) | (hsfsts & PCH_RCBA_SPI_HSFSTS_FCERR));
	}

	if (cyclo_done == 0)
	{
		//printf("\n[hsfsts] %04x\n", hsfsts);
		return RETURN_NOT_READY;
	}

	//printf("\n[wait spi flash] success.\n");	
	return RETURN_SUCCESS;


}

EFI_STATUS _send_spi_cycle(struct SPI_obj *This, UINT8 cmd, UINT8 dbc, UINTN spi_fla)
{
	UINTN spi_base, hsfsts, _faddr;
	//INT i;
	UINT8 cyclo_done;
	UINT16 hsfctl;
	EFI_STATUS Result;

	spi_base = This->rcba_spi_base;

	spi_fla = spi_fla & PCH_RCBA_SPI_FADDR_MASK;

	WritePhysMem((UINT8*) &spi_fla, 0, spi_base + PCH_RCBA_SPI_FADDR, 4);	// write dword
	_faddr = SPI_reg_read(This, PCH_RCBA_SPI_FADDR);

	if (cmd != HSFCTL_ERASE_CYCLE)
	{
		WritePhysMem((UINT8 *) &dbc, 0, spi_base + PCH_RCBA_SPI_HSFCTL + 0x01, 1);
	}

	WritePhysMem((UINT8 *) &cmd, 0, spi_base + PCH_RCBA_SPI_HSFCTL, 1);
	
	ReadPhysMem((UINT8 *) &hsfctl, 0, spi_base + PCH_RCBA_SPI_HSFCTL, 2);

	if (cmd == HSFCTL_ERASE_CYCLE)
	{
		while(_wait_SPI_flash_cyclo_done(This) != RETURN_SUCCESS);

		Result = RETURN_SUCCESS;
	}
	else
		Result = _wait_SPI_flash_cyclo_done(This);

	return Result;
}

EFI_STATUS erase_spi_block(struct SPI_obj *This, UINTN spi_addr)
{
	UINT8 cyclo_done;

	while(_wait_SPI_flash_cyclo_done(This) != RETURN_SUCCESS);
	
	return _send_spi_cycle(This, HSFCTL_ERASE_CYCLE, 0, spi_addr);
}


EFI_STATUS erase_spi(struct SPI_obj *This, UINTN spi_addr, UINTN size,
					void (*progress_callback)(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr),
					void (*error_callback)(UINT32 spiaddr, UINT16 command))
{
	UINT32 spi_addr_base = spi_addr;
	UINT32 spi_size = size;

	progress_callback(spi_addr_base, spi_size, spi_addr);

	while(size > 0)
	{
		if (erase_spi_block(This, spi_addr) == RETURN_ABORTED)
		{	// Error!
			if (error_callback != NULL)
				error_callback(spi_addr, HSFCTL_ERASE_CYCLE);

			//return EFI_DEVICE_ERROR;
		}

		spi_addr += This->block_erase_size;
		size -= This->block_erase_size;

		progress_callback(spi_addr_base, spi_size, spi_addr);
	}

	return EFI_SUCCESS;
}

EFI_STATUS write_spi(struct SPI_obj *This, UINTN spi_addr, UINT32 BufferSize, UINT8 *Buffer,
					void (*progress_callback)(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr),
					void (*error_callback)(UINT32 spiaddr, UINT16 command)
)
{
	EFI_STATUS Status, cyclo_done;
	UINTN spi_base;
	UINT32 dbc, n, r, *BufferDword, data;
	
	dbc = 4;	// write DWORD!
	spi_base = This->rcba_spi_base;
	Status = EFI_SUCCESS;

	n = BufferSize / dbc;
	r = BufferSize % dbc;

	BufferDword = (UINT32 *) Buffer;

	// write all dword!
	while(n > 0)
	{
		//*BufferDword = 0xDEADBEEF;
		WritePhysMem((UINT8*) BufferDword, 0, spi_base + PCH_RCBA_SPI_FDATA00, 4);	// load content into FDATA00
		BufferDword++;	// move to next dword
		/*WritePhysMem((UINT8*) BufferDword, 0, spi_base + PCH_RCBA_SPI_FDATA01, 4);	// load content into FDATA00
		BufferDword++;	// move to next dword
		WritePhysMem((UINT8*) BufferDword, 0, spi_base + PCH_RCBA_SPI_FDATA02, 4);	// load content into FDATA00
		BufferDword++;	// move to next dword
		WritePhysMem((UINT8*) BufferDword, 0, spi_base + PCH_RCBA_SPI_FDATA03, 4);	// load content into FDATA00
		BufferDword++;	// move to next dword*/
		// send the command to PCI to write data into new address!
		if (_send_spi_cycle(This, HSFCTL_WRITE_CYCLE, dbc - 1, spi_addr) != RETURN_SUCCESS)
		{
			if (error_callback != NULL)	// notify error!
				error_callback(spi_addr, HSFCTL_WRITE_CYCLE);

			Status = EFI_DEVICE_ERROR;	// an error writing in mmio
		}

		spi_addr += dbc;	// move to next address
		n--;
	}

	// write all dword!
	while(r > 0)
	{
		WritePhysMem((UINT8*) BufferDword, 0, spi_base + PCH_RCBA_SPI_FDATA00, 4);	// load content into FDATA00
		BufferDword++;
		// send the command to PCI to write data into new address!
		if (_send_spi_cycle(This, HSFCTL_WRITE_CYCLE, dbc - 1, spi_addr) != RETURN_SUCCESS)
		{
			Status = EFI_DEVICE_ERROR;	// an error writing in mmio
			if (error_callback != NULL)	// notify error!
				error_callback(spi_addr, HSFCTL_WRITE_CYCLE);
		}

		spi_addr += 4;	// move to next address
		if (r > 4) r = r - 4;
		else r = 0;
	}

	return EFI_SUCCESS;
}

EFI_STATUS read_spi(struct SPI_obj *This, UINTN spi_addr, UINT32 BufferSize, UINT8 *Buffer, 
					void (*progress_callback)(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr),
					void (*error_callback)(UINT32 spiaddr, UINT16 command)
					)
{
	EFI_STATUS Status, cyclo_done;
	UINTN spi_base;
	UINT32 dbc, n, r, *BufferDword, data;
	UINT8 *SPI_Buffer_PCI;
	UINT32 spi_addr_base = spi_addr;

	if (BufferSize == 0 || This == NULL || Buffer == NULL)
	{
		Status = EFI_INVALID_PARAMETER;
		goto ERROR;
	}

	if ((BufferSize & 0x3) != 0)
	{
		Status = EFI_INVALID_PARAMETER;
		goto ERROR;
	}

	memset(Buffer, 0xff, BufferSize);	// FF -> FV parity

	dbc = 64;	// read DWORD!
	spi_base = This->rcba_spi_base;
	Status = EFI_SUCCESS;

	n = BufferSize / dbc;
	r = BufferSize % dbc;
	r = 0;

	BufferDword = (UINT32 *) Buffer;

	progress_callback(spi_addr_base, BufferSize, spi_addr_base);	// notify first step!

	while(n > 0)
	{
		// send buffer size and flash address to SPI register
		if (_send_spi_cycle(This, HSFCTL_READ_CYCLE, dbc - 1, spi_addr) != RETURN_SUCCESS)
		{	// cannot write
			Status = EFI_DEVICE_ERROR;	// an error writing in mmio
			
			if (error_callback != NULL)	// notify error!
				error_callback(spi_addr, HSFCTL_READ_CYCLE);
		}
		
		SPI_Buffer_PCI = SPI_reg_addr(This, PCH_RCBA_SPI_FDATA00); // Ptr to DWORD!
		memcpy((void *) BufferDword, (void *) SPI_Buffer_PCI, 64);


		//*BufferDword = (UINT32) SPI_reg_read(This, PCH_RCBA_SPI_FDATA00);	// read only dword
				
		//spi_addr += dbc;	// move to next address
		//BufferDword++;	// move to next dword

		spi_addr += 64;
		BufferDword+= 0x10;	// move to next dword
		
		progress_callback((UINT32) spi_addr_base, BufferSize, (UINT32) spi_addr);

		n--;
	}

	// write latest dword
	if (r != 0)
	{
		data = 0xFFFFFFFF;	// all value must 1! -- TODO -- Align to FV parity flag

		//WritePhysMem((UINT8*) &data, 0, spi_base + PCH_RCBA_SPI_FDATA00, 4);	// load content into FDATA00
		
		if (_send_spi_cycle(This, HSFCTL_READ_CYCLE, r - 1, spi_addr) != EFI_SUCCESS)
		{
			Status = EFI_DEVICE_ERROR;	// an error writing in mmio
			
			if (error_callback != NULL)	// notify error!
				error_callback(spi_addr, HSFCTL_READ_CYCLE);

		}

		memcpy((Buffer + n * dbc), &data, r);	//
	}

	progress_callback((UINT32) spi_addr_base, BufferSize, (UINT32) spi_addr);
ERROR:
	return Status;
}

//
//EFI_STATUS read_spi_low(struct SPI_obj *This, UINTN spi_addr, UINT32 BufferSize, UINT8 *Buffer, void (*progress_callback)(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr))
//{
//	EFI_STATUS Status, cyclo_done;
//	UINTN spi_base;
//	UINT32 dbc, n, r, *BufferDword, data;
//	UINT8 *SPI_Buffer_PCI;
//
//	UINT32 spi_addr_base = spi_addr;
//
//	memset(Buffer, 0xff, BufferSize);	// FF -> FV parity
//
//	if ((BufferSize & 0x3) != 0)
//	{
//		Status = EFI_INVALID_PARAMETER;
//		goto ERROR;
//	}
//
//	dbc = 4;	// read DWORD!
//	spi_base = This->rcba_spi_base;
//	Status = EFI_SUCCESS;
//
//	n = BufferSize / dbc;
//	r = BufferSize % dbc;
//	r = 0;
//
//	BufferDword = (UINT32 *) Buffer;
//
//	//printf("\n\n\n\n");
//
//	//printf("[read_spi] %08x kb.\r", (n * dbc) / 1024);
//
//	progress_callback(spi_addr_base, BufferSize, spi_addr_base);	// notify first step!
//
//	// write all dword!
//	while(n > 0)
//	{
//		
//		// send the command to PCI to write data into new address!
//		if (_send_spi_cycle(This, HSFCTL_READ_CYCLE, dbc - 1, spi_addr) != EFI_SUCCESS)
//		{
//			Status = EFI_DEVICE_ERROR;	// an error writing in mmio
//		}
//		
//		SPI_Buffer_PCI = SPI_reg_addr(This, PCH_RCBA_SPI_FDATA00); // Ptr to DWORD!
//		memcpy((void *) BufferDword, (void *) SPI_Buffer_PCI, 64);
//
//
//		//*BufferDword = (UINT32) SPI_reg_read(This, PCH_RCBA_SPI_FDATA00);	// read only dword
//				
//		//spi_addr += dbc;	// move to next address
//		//BufferDword++;	// move to next dword
//
//		spi_addr += 64;
//		BufferDword+= 0x10;	// move to next dword
//		
//		//if ((n % 256) == 0)
//		//{
//		//	printf("[read_spi] %08x kb.\r", (n * dbc) / 1024);
//		//}
//		//n--;
//
//		progress_callback((UINT32) spi_addr_base, BufferSize, (UINT32) spi_addr);
//
//		n -= 0x10;
//	}
//
//	// write latest dword
//	if (r != 0)
//	{
//		data = 0xFFFFFFFF;	// all value must 1! -- TODO -- Align to FV parity flag
//
//		//WritePhysMem((UINT8*) &data, 0, spi_base + PCH_RCBA_SPI_FDATA00, 4);	// load content into FDATA00
//		
//		if (_send_spi_cycle(This, HSFCTL_READ_CYCLE, r - 1, spi_addr) != EFI_SUCCESS)
//		{
//			Status = EFI_DEVICE_ERROR;	// an error writing in mmio
//		}
//
//		memcpy((Buffer + n * dbc), &data, r);	//
//	}
//
//ERROR:
//	return Status;
//}
