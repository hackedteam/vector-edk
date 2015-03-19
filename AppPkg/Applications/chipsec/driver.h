#ifndef DRIVER_H
#define DRIVER_H


//#include "cpu.h"
//#include "chipsec.h"
//#include "windef.h"

// function prototypes

//BYTE bPCIRead(WORD target, BYTE offset );
//WORD wPCIRead(WORD target, BYTE offset );
//DWORD lPCIRead(WORD target, BYTE offset );
//BYTE bPCIReadParam(WORD bus, WORD dev, WORD func, BYTE offset );
//WORD wPCIReadParam(WORD bus, WORD dev, WORD func, BYTE offset );
//DWORD lPCIReadParam(WORD bus, WORD dev, WORD func, BYTE offset );
//VOID bPCIWriteParam(WORD bus, WORD dev, WORD func, BYTE offset, BYTE value );
//VOID wPCIWriteParam(WORD bus, WORD dev, WORD func, BYTE offset, WORD value );
//VOID lPCIWriteParam(WORD bus, WORD dev, WORD func, BYTE offset, DWORD value );

typedef UINT8	BYTE;
typedef UINT16	WORD;
typedef UINT32	DWORD;

void ReadPhysMem(UINT8 *OutBuffer, volatile UINTN phys_address_hi, volatile UINTN phys_address_lo, UINTN length);
void WritePhysMem(UINT8 *InBuffer, volatile UINTN phys_address_hi, volatile UINTN phys_address_lo, UINTN length);

#endif	// DRIVER_H
