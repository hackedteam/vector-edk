#include <Uefi.h>
#include <stdio.h>

#include "config.h"
#include "driver.h"
#include "pci.h"

BOOLEAN is_MMIO_BAR_defined(CONFIGURATION *Platform, CHAR8* BarName)
{
	if (get_BAR(Platform, BarName) != NULL)
		return TRUE;

	return FALSE;
}

EFI_STATUS get_MMIO_BAR_base_address(BAR *bar, UINT64 *base, unsigned int *size)
{
	UINT32 base_lo, base_hi;

	*base = (UINT64) -1;
	*size = (UINTN) -1;

	if (bar->Width == 8)
	{
		base_lo = pci_read_dword(bar->Bus, bar->Dev, bar->Fun, bar->Reg);
		base_hi = pci_read_dword(bar->Bus, bar->Dev, bar->Fun, bar->Reg + 4);

		*base = base_hi;
		*base <<= 32;
		*base |= base_lo;

		printf("[MMIO!get_MMIO_BAR_base_address] Width 8 (hi) %x (lo) %x\n", base_hi, base_lo);
	}
	else
	{
		*base = 0;
		*base = pci_read_dword(bar->Bus, bar->Dev, bar->Fun, bar->Reg);
		printf("[MMIO!get_MMIO_BAR_base_address] Width 4 %x\n", *base);
	}

	if (bar->EnableBit != 0)
	{
		//UINT8 en_mask = 1 << bar->EnableBit;

		//if (*base == 0 && en_mask) 
	}

	if (bar->Mask != 0)
		*base &= bar->Mask;

	if (bar->Offset != 0)
		*base = *base + bar->Offset;

	if (bar->Size == 0)
		*size = 0x1000;
	else
		*size = bar->Size;

	printf("[MMIO!-> %x %x", *base, *size);

	return RETURN_SUCCESS;
}

BOOLEAN is_MMIO_BAR_enabled(BAR *bar)
{
	UINT64 base;
	unsigned int size;

	get_MMIO_BAR_base_address(bar, &base, &size);
	
	if (bar->EnableBit != 0)
	{
		UINT8 en_mask = 1 << bar->EnableBit;

		return ((base != 0) & en_mask);
	}

	return TRUE;
}

BOOLEAN is_MMIO_BAR_programmed(BAR *bar)
{
	UINT64 base;
	unsigned int size;

	get_MMIO_BAR_base_address(bar, &base, &size);

	return (base != 0) ? TRUE : FALSE;
}

VOID write_MMIO_reg(UINTN bar_base, UINTN offset, UINTN value)
{
	memcpy((UINT8*) bar_base + offset, &value, 4);	// dword!
}

UINTN read_MMIO_reg(UINTN bar_base, UINTN offset)
{
	UINTN value;

	value = 0;

	memcpy(&value, (UINT8*) (bar_base + offset), 4);

	return value;
}

UINTN read_MMIO_BAR_reg(BAR *bar, UINTN offset)
{
	UINT64 bar_base;

	UINTN result;
	unsigned int bar_size;

	get_MMIO_BAR_base_address(bar, &bar_base, &bar_size);

	return read_MMIO_reg((UINTN) bar_base, offset);
}

VOID write_MMIO_BAR_reg(BAR *bar, UINTN offset, UINTN value)
{
	UINT64 bar_base;
	UINTN result;
	unsigned int bar_size;

	get_MMIO_BAR_base_address(bar, &bar_base, &bar_size);

	write_MMIO_reg((UINTN) bar_base, offset, value);
}

VOID read_MMIO(UINTN bar_base, UINTN size, UINT8 *buffer)
{
	UINT32 *regs = (UINT32 *) buffer;
	UINT32 r = 0;

	while(r < size)
	{
		regs[r / 4] = (UINT32) read_MMIO_reg(bar_base, r);

		r += 4;
	}
}

UINTN get_MMCFG_base_address(CONFIGURATION *Configuration)
{
	UINT64 bar_address;
	unsigned int bar_size;

	BAR *mmcfg = get_BAR(Configuration, "MMCFG");

	get_MMIO_BAR_base_address(mmcfg, &bar_address, &bar_size);

	if (PCI_PCIEXBAR_REG_LENGTH_256MB == (bar_address & PCI_PCIEXBAR_REG_LENGTH_MASK) >> 1)
	{
		bar_address &= ~ (PCI_PCIEXBAR_REG_ADMSK128 | PCI_PCIEXBAR_REG_ADMSK64);
	}
	else if (PCI_PCIEXBAR_REG_LENGTH_128MB == (bar_address & PCI_PCIEXBAR_REG_LENGTH_MASK) >> 1)
	{
		bar_address &= ~PCI_PCIEXBAR_REG_ADMSK64;
	}

	return (UINTN) bar_address;

}

UINTN read_MMCFG_reg(CONFIGURATION *Configuration, UINT16 bus, UINT16 dev, UINT16 fun, UINT16 off, UINT16 size)
{
	UINTN bar_address, bar_address_off;
	UINTN value;

	bar_address = get_MMCFG_base_address(Configuration);
	bar_address_off = (bus * 32 * 8 + dev * 8 + fun) * 0x1000 + off;

	value = read_MMIO_reg(bar_address, bar_address_off);

	if (size == 1)
		value &= 0xff;
	else if (size == 2)
		value &= 0xffff;
	
	return value;
}


VOID write_MMCFG_reg(CONFIGURATION *Configuration, UINT16 bus, UINT16 dev, UINT16 fun, UINT16 off, UINT32 value)
{
	UINTN bar_address, bar_address_off;

	bar_address = get_MMCFG_base_address(Configuration);
	bar_address_off = (bus * 32 * 8 + dev * 8 + fun) * 0x1000 + off;

	write_MMIO_reg(bar_address, bar_address_off, value);
}

UINTN read_register(CONFIGURATION *Configuration, CHAR8 *reg_name)
{
	REGISTER *reg;
	UINTN Result;

	reg = get_BAR_reg(Configuration, reg_name);

	Result = 0;

	if (reg->Type == PCICFG)
	{
		Result = ReadPCICfg(reg->Bus, reg->Dev, reg->Fun, reg->Offset, reg->Size);
		//printf("\n[ReadPCICfg] (%x:%x:%x:%x:%x) = %x", reg->Bus, reg->Dev, reg->Fun, reg->Offset, reg->Size, Result); 
	}
	else
	{
		// none!
	}

	return Result;
}

VOID write_register(CONFIGURATION *Configuration, CHAR8 *reg_name, UINTN value)
{
	REGISTER *reg;

	reg = get_BAR_reg(Configuration, reg_name);

	if (reg->Type == PCICFG)
	{
		//printf("\n[write_register] %s %x", reg_name, value);
		WritePCICfg(reg->Bus, reg->Dev, reg->Fun, reg->Offset, reg->Size, value);
	}
	else
	{
		// none!
	}

	return;
}