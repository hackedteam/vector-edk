/////

#include <Uefi.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"

void
 _ReadPhysMem(UINT8 *OutBuffer, volatile UINTN phys_address_lo, UINTN length)
{
	memcpy(OutBuffer, (UINT8*) phys_address_lo, length);
}


void
  ReadPhysMem(UINT8 *OutBuffer, volatile UINTN phys_address_hi, volatile UINTN phys_address_lo, UINTN length)
{
  _ReadPhysMem(OutBuffer, phys_address_lo, length);
}

void
  _WritePhysMem(UINT8 *InBuffer, volatile UINTN phys_address_lo, UINTN length)
{
	memcpy((UINT8 *) phys_address_lo, InBuffer, length);
}


void
  WritePhysMem(UINT8 *InBuffer, volatile UINTN phys_address_hi, volatile UINTN phys_address_lo, UINTN length)
{
  _WritePhysMem(InBuffer, phys_address_lo, length);
}

UINT64 ReadMSR(UINT32 ThreadId, UINT32 MsrAddr)
{
	INT32 low, high;
    INT64 result;

	_rdmsr(MsrAddr, &low, &high);

	result = high;
	result <<= 32;
	result |= low;

	return result;
}


void WriteMSR(UINT32 ThreadId, UINT32 MsrAddr, INT64 msr)
{
	UINT32 low, high;
    
	low = msr & 0xffffffff;
	high = msr >> 32;

	_wrmsr(MsrAddr, low, high);
}
