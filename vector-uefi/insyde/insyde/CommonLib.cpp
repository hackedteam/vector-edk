#include "UefiTypes.h"
#include "CommonLib.h"

/*++
  
Routine Description::

  This function calculates the value needed for a valid UINT16 checksum

Arguments:

  Buffer      Pointer to buffer containing byte data of component.
  Size        Size of the buffer

Returns:

  The 16 bit checksum value needed.

--*/
UINT16 CalculateChecksum16( UINT16 *Buffer, UINTN Size)
{
  return (UINT16) (0x10000 - CalculateSum16 (Buffer, Size));
}

UINT8 CalculateChecksum8(UINT8 *Buffer, UINTN Size)
{
	return (UINT8) (0x100 - CalculateSum8(Buffer, Size));
}


/*++
  
Routine Description:

  This function calculates the UINT16 sum for the requested region.

Arguments:

  Buffer      Pointer to buffer containing byte data of component.
  Size        Size of the buffer

Returns:

  The 16 bit checksum

--*/
UINT16 CalculateSum16 (UINT16       *Buffer, UINTN        Size)
{
  UINTN   Index;
  UINT16  Sum;

  Sum = 0;

  //
  // Perform the word sum for buffer
  //
  for (Index = 0; Index < Size; Index++) {
    Sum = (UINT16) (Sum + Buffer[Index]);
  }

  return (UINT16) Sum;
}


UINT8	CalculateSum8(UINT8 *Buffer, UINTN Size)
{
	UINTN Index;
	UINT8 Sum;

	Sum = 0;

	//
	// Perform the byte sum for buffer
	//
	for (Index = 0; Index < Size; Index++) {
		Sum = (UINT8) (Sum + Buffer[Index]);
	}

	return Sum;
}

DWORD Expand24bit(UINT8 *ptr)
{
	return ((ptr[0]) | (ptr[1] << 8) | (ptr[2] << 16));
}

void Pack24bit(UINT32 value, UINT8 *ffsSize)
{
	if (value >= 0xffffff)
	{
		ffsSize[0] = 0xff;
		ffsSize[1] = 0xff;
		ffsSize[2] = 0xff;
	}
	else
	{
		ffsSize[2] = (UINT8) ((value) >> 16);
		ffsSize[1] = (UINT8) ((value) >> 8);
		ffsSize[0] = (UINT8) ((value));
	}
}