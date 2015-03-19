#ifndef __CMOS_H_
	#define __CMOS_H_

#define CMOS_ADDR_PORT_LOW	0x70
#define CMOS_DATA_PORT_LOW	0x71
#define CMOS_ADDR_PORT_HIGH	0x72
#define CMOS_DATA_PORT_HIGH	0x73

EFI_STATUS CMOS_Read(UINTN Offset, UINT8 *Data);
EFI_STATUS CMOS_Write(UINTN Offset, UINT8 Data);

#endif
