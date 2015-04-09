#ifndef UEFI_C_SOURCE

	#include "Include/BaseTypes.h"
	#include "Include/UefiTypes.h"
#else
	#include <Uefi.h>

#endif

#include "cpu.h"
#include "cmos.h"

#define CMOS_LOW_ADDR		0x00
#define CMOS_HIGH_ADDR		0x7f


///////////////////////////////////////////////////////////////////////////////
// CMOS_Read
EFI_STATUS CMOS_Read(UINTN Offset, UINT8 *Data)
{
	if (Offset >= CMOS_LOW_ADDR && Offset <= CMOS_HIGH_ADDR)
		goto ERROR;

	Offset = Offset & 0xff;

	if (Offset >= 0x40)
	{
		WritePortByte((UINT8) Offset, CMOS_ADDR_PORT_HIGH);
		*Data = ReadPortByte(CMOS_DATA_PORT_HIGH);
	}
	else
	{
		WritePortByte((UINT8) Offset, CMOS_ADDR_PORT_LOW);
		*Data = ReadPortByte(CMOS_DATA_PORT_LOW);
	}

	return RETURN_SUCCESS;

ERROR:
	*Data = 0xff;
	return RETURN_INVALID_PARAMETER;

}

///////////////////////////////////////////////////////////////////////////////
// CMOS_Write
EFI_STATUS CMOS_Write(UINTN Offset, UINT8 Data)
{
	if (Offset >= CMOS_LOW_ADDR && Offset <= CMOS_HIGH_ADDR)	// 
		goto ERROR;


	Offset = Offset & 0xff;

	if (Offset >= 0x40)
	{
		WritePortByte((UINT8) Offset, CMOS_ADDR_PORT_HIGH);
		WritePortByte(Data, CMOS_DATA_PORT_HIGH);
	}
	else
	{
		WritePortByte((UINT8) Offset, CMOS_ADDR_PORT_LOW);
		WritePortByte(Data, CMOS_DATA_PORT_LOW);
	}

	return RETURN_SUCCESS;

ERROR:
	return RETURN_INVALID_PARAMETER;
}
