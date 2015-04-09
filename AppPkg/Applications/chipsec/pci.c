#include <Uefi.h>

#include "driver.h"
#include "cpu.h"


//#pragma comment(lib, "wdmsec.lib")
//#pragma comment(lib, "bufferoverflowK.lib") 

//#pragma comment(linker, "/section:chipsec_code,EWP")

//#pragma code_seg("chipsec_code$__c")

UINT32 ReadPCICfg(
  UINT8 bus,
  UINT8 dev,
  UINT8 fun,
  UINT8 off,
  UINT8 len // 1, 2, 4 bytes
  )
{
  unsigned int result = 0;
  unsigned int pci_addr = (0x80000000 | (bus << 16) | (dev << 11) | (fun << 8) | (off & ~3));
  unsigned short cfg_data_port = (UINT16)(0xCFC + ( off & 0x3 ));
  if     ( 1 == len ) result = (ReadPCIByte ( pci_addr, cfg_data_port ) & 0xFF);
  else if( 2 == len ) result = (ReadPCIWord ( pci_addr, cfg_data_port ) & 0xFFFF);
  else if( 4 == len ) result =  ReadPCIDword( pci_addr, cfg_data_port );
  return result;
}

VOID WritePCICfg(
  UINT8 bus,
  UINT8 dev,
  UINT8 fun,
  UINT8 off,
  UINT8 len, // 1, 2, 4 bytes
  UINT32 val
  )
{
  UINT32 pci_addr = (0x80000000 | (bus << 16) | (dev << 11) | (fun << 8) | (off & ~3));
  UINT16 cfg_data_port = (UINT16)(0xCFC + ( off & 0x3 ));
  if     ( 1 == len ) WritePCIByte ( pci_addr, cfg_data_port, (UINT8)(val&0xFF) );
  else if( 2 == len ) WritePCIWord ( pci_addr, cfg_data_port, (UINT16)(val&0xFFFF) );
  else if( 4 == len ) WritePCIDword( pci_addr, cfg_data_port, val );
}

VOID 
WriteIOPort(
  UINT32 value,
  UINT16 io_port,
  UINT8 len // 1, 2, 4 bytes
  )
{
  if     ( 1 == len ) WritePortByte ( (UINT8)(value&0xFF), io_port );
  else if( 2 == len ) WritePortWord ( (UINT16)(value&0xFFFF), io_port );
  else if( 4 == len ) WritePortDword( value, io_port );
}

UINT32 
ReadIOPort(
  UINT16 io_port,
  UINT8 len // 1, 2, 4 bytes
  )
{
  if     ( 1 == len ) return (ReadPortByte( io_port ) & 0xFF);
  else if( 2 == len ) return (ReadPortWord( io_port ) & 0xFFFF);
  else if( 4 == len ) return ReadPortDword( io_port );
  return 0;
}

// pci_read_.. ARE NOT USED	
BYTE  pci_read_byte(WORD bus, WORD dev, WORD func, BYTE offset )
{
  DWORD target = func + ((dev & 0x1F) << 3) + ((bus & 0xFF) << 8) ;

  WritePortDword((DWORD)( target << 8 ) | 0x80000000UL | ((DWORD)offset & ~3 ), (UINT16) 0xCF8 );

  return (BYTE) ReadIOPort( (UINT16) 0xCFC + (offset & 0x3), 1 ); 
}
WORD  pci_read_word(WORD bus, WORD dev, WORD func, BYTE offset )
{
  WORD target = func + ((dev & 0x1F) << 3) + ((bus & 0xFF) << 8) ;
  WriteIOPort((DWORD)( target << 8 ) | 0x80000000UL | ((DWORD)offset & ~3 ) , 0xCF8, 4);
  return (WORD) ReadIOPort( (UINT16) 0xCFC  + (offset & 0x2), 2);
}
DWORD  pci_read_dword(WORD bus, WORD dev, WORD func, BYTE offset )
{
  WORD target = func + ((dev & 0x1F) << 3) + ((bus & 0xFF) << 8) ;

  WritePortDword((DWORD)( target << 8 ) | 0x80000000UL | ((DWORD)offset & ~3 ), (UINT16) 0xCF8 );
  return ReadIOPort( 0xCFC, 4); 
}
