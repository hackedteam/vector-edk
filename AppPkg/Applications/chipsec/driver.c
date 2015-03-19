/* 
CHIPSEC: Platform Security Assessment Framework
Copyright (c) 2010-2014, Intel Corporation
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; Version 2.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Contact information:
chipsec@intel.com
*/ 


//#include <ntddk.h>
//#include <wdmsec.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "BaseTypes.h"
#include "driver.h"
#include "cpu.h"


//#pragma comment(lib, "wdmsec.lib")
//#pragma comment(lib, "bufferoverflowK.lib") 

//#pragma comment(linker, "/section:chipsec_code,EWP")

//#pragma code_seg("chipsec_code$__c")

UINT32
ReadPCICfg(
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

VOID
WritePCICfg(
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
BYTE pci_read_byte(WORD bus, WORD dev, WORD func, BYTE offset )
{
  WORD target = func + ((dev & 0x1F) << 3) + ((bus & 0xFF) << 8) ;
  _outpd( 0xCF8, (DWORD)( target << 8 ) | 0x80000000UL | ((DWORD)offset & ~3 ) );
  return (BYTE)_inp( 0xCFC + (offset & 0x3) ); 
}
WORD pci_read_word(WORD bus, WORD dev, WORD func, BYTE offset )
{
  WORD target = func + ((dev & 0x1F) << 3) + ((bus & 0xFF) << 8) ;
  _outpd( 0xCF8, (DWORD)( target << 8 ) | 0x80000000UL | ((DWORD)offset & ~3 ) );
  return (WORD)_inpw( 0xCFC  + (offset & 0x2) ); 
}
DWORD pci_read_dword(WORD bus, WORD dev, WORD func, BYTE offset )
{
  WORD target = func + ((dev & 0x1F) << 3) + ((bus & 0xFF) << 8) ;
  _outpd( 0xCF8, (DWORD)( target << 8 ) | 0x80000000UL | ((DWORD)offset & ~3 ) );
  return _inpd( 0xCFC ); 
}


void _dump_buffer( unsigned char * b, unsigned int len )
{
  unsigned int i;
  unsigned int m = len / 8;
  unsigned int r = len % 8;
  unsigned char line[3*8 + 1];
  unsigned char * line_ptr = line;
  for( i = 0; i < m; i++ )
	  ;
    //DbgPrint( "%02X %02X %02X %02X %02X %02X %02X %02X : %c %c %c %c %c %c %c %c\n", b[i*8], b[i*8+1], b[i*8+2], b[i*8+3], b[i*8+4], b[i*8+5], b[i*8+6], b[i*8+7], b[i*8], b[i*8+1], b[i*8+2], b[i*8+3], b[i*8+4], b[i*8+5], b[i*8+6], b[i*8+7] );

  for( i = 0; i < r; i++ ) line_ptr += sprintf( line_ptr, "%02X ", b[m*8 + i] );
  *(line_ptr + 1) = '\0';
  //DbgPrint( "%s\n", line );
}
