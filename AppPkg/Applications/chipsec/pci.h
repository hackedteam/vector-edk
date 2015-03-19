#ifndef __PCI_H_
#define __PCI_H_

UINT32  
ReadPCICfg(
  UINT8 bus,
  UINT8 dev,
  UINT8 fun,
  UINT8 off,
  UINT8 len // 1, 2, 4 bytes
  );

VOID  
WritePCICfg(
  UINT8 bus,
  UINT8 dev,
  UINT8 fun,
  UINT8 off,
  UINT8 len, // 1, 2, 4 bytes
  UINT32 val
  );


BYTE  pci_read_byte(WORD bus, WORD dev, WORD func, BYTE offset );
WORD  pci_read_word(WORD bus, WORD dev, WORD func, BYTE offset );
DWORD  pci_read_dword(WORD bus, WORD dev, WORD func, BYTE offset );

#endif
