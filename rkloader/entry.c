#include <Uefi.h>

//#include <Guid/FileInfo.h>
//#include <Guid/FileSystemInfo.h>
//#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/LoadedImage.h>
//#include <Ppi/FirmwareFile.h>
#include <Pi/PiMultiPhase.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>

#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
//#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

BOOLEAN		gReceived;

EFI_HANDLE	gImageHandle;
EFI_SYSTEM_TABLE	*gSystemTable;
EFI_BOOT_SERVICES	*gBootServices;
EFI_RUNTIME_SERVICES	*gRuntimeServices;
//EFI_LOADED_IMAGE_PROTOCOL *gLoadedImageProtocol;

EFI_GUID SMBIOS_TABLE_GUID =
{	0x7ce88fb3,
	0x4bd7,
	0x4679,
	{  0x87, 0xa8, 0xa8, 0xd8, 0xde, 0xe5,0xd, 0x2b } 
};


EFI_GUID LAUNCH_APP =
{
	0xeaea9aec,
	0xc9c1,
	0x46e2,
	{ 0x9d, 0x52, 0x43, 0x2a, 0xd2, 0x5a, 0x9b, 0x0b }
};

EFI_GUID LOADED_IMAGE_PROTOCOL_GUID =
{
	0x5B1B31A1,
	0x9562,
	0x11d2,
	{ 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B }
};

EFI_GUID FIRMWARE_VOLUME_PROTOCOL_GUID =
{
	0x389F751F, 0x1838, 0x4388, {0x83, 0x90, 0xCD, 0x81, 0x54, 0xBD, 0x27, 0xF8 }
};

EFI_GUID DEVICE_PATH_PROTOCOL_GUID = 
{
	0x9576e91, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b }
};

/**
 InitializeLib
 **/
VOID 
EFIAPI 
InitializeLib(
  IN EFI_HANDLE			ImageHandle,
  IN EFI_SYSTEM_TABLE	*SystemTable
  )
{
	gImageHandle = ImageHandle;
	gSystemTable = SystemTable;
	gBootServices = gSystemTable->BootServices;
	gRuntimeServices = gSystemTable->RuntimeServices;
	gBS = gBootServices;
	gST = gSystemTable;
	gRT = gRuntimeServices;
}

/**
 Callback
 **/
VOID
EFIAPI
CallbackSMI (
  IN  EFI_EVENT	Event,
  IN  VOID      *Context
  )
{
	EFI_HANDLE	ImageLoadedHandle;
	EFI_HANDLE	DeviceHandle;
	EFI_STATUS	Status;
	EFI_LOADED_IMAGE_PROTOCOL  *LoadedImage;
	EFI_FIRMWARE_VOLUME_PROTOCOL	*FirmwareProtocol;
	EFI_DEVICE_PATH_PROTOCOL	*DevicePathProtocol, *NewDevicePathProtocol, *NewFilePathProtocol, *NewDevicePathEnd;
	EFI_TPL tpl;

	VOID *Ptr;
	UINTN BufferSize;
	EFI_FV_FILETYPE FoundType;
	EFI_FV_FILE_ATTRIBUTES FileAttributes;
	UINT32 AuthenticationStatus;
	UINT32 DevicePathLength;

	if (gReceived)
		goto done;
	
	//CpuBreakpoint();

	tpl = gBootServices->RaiseTPL(TPL_HIGH_LEVEL);
	gBootServices->RestoreTPL(TPL_APPLICATION);

	DEBUG((EFI_D_INFO, "Looking protocols..."));

	Status = gBootServices->HandleProtocol(gImageHandle, &LOADED_IMAGE_PROTOCOL_GUID, &LoadedImage);

	if (EFI_ERROR(Status))
	{
		DEBUG((EFI_D_INFO, "Unsupported LOADED IMAGE PROTOCOL.\n"));
		goto done;
	}

	DEBUG((EFI_D_INFO, "Loading image..."));

	DeviceHandle = LoadedImage->DeviceHandle;

	Status = gBootServices->HandleProtocol(DeviceHandle, &FIRMWARE_VOLUME_PROTOCOL_GUID, &FirmwareProtocol);
	Status = gBootServices->HandleProtocol(DeviceHandle, &DEVICE_PATH_PROTOCOL_GUID, &DevicePathProtocol);

	DevicePathLength = DevicePathProtocol->Length[0] + DevicePathProtocol->Length[1];
	DevicePathLength += sizeof(EFI_GUID) + 4 + 4;

	gBootServices->AllocatePool(4, DevicePathLength, &NewDevicePathProtocol);

	DevicePathLength = DevicePathProtocol->Length[0] + DevicePathProtocol->Length[1];
	gBootServices->CopyMem(NewDevicePathProtocol, DevicePathProtocol, DevicePathLength);	// copy "VOLUME" descriptor

	NewFilePathProtocol = (EFI_DEVICE_PATH_PROTOCOL *) ((UINT8 *) NewDevicePathProtocol + DevicePathLength);
	NewFilePathProtocol->Type = 0x04;
	NewFilePathProtocol->SubType = 0x06;
	NewFilePathProtocol->Length[0] = 0x14;
	NewFilePathProtocol->Length[1] = 0x00;
	gBootServices->CopyMem(((CHAR8 *)(NewFilePathProtocol) + 4), &LAUNCH_APP, sizeof(EFI_GUID));


	NewDevicePathEnd = (EFI_DEVICE_PATH_PROTOCOL *) ((UINT8 *) NewDevicePathProtocol + DevicePathLength + sizeof(EFI_GUID) + 4);

	NewDevicePathEnd->Type = 0x7f;
	NewDevicePathEnd->SubType = 0xff;
	NewDevicePathEnd->Length[0] = 0x04;
	NewDevicePathEnd->Length[1] = 0x00;


	//gBootServices->FreePool(DevicePathProtocol);
	
	//UefiMain(gImageHandle, gSystemTable);

	
	Status = gBootServices->LoadImage(FALSE, gImageHandle, NewDevicePathProtocol, NULL, 0, &ImageLoadedHandle);

	if (EFI_ERROR(Status))
	{
		switch(Status)
		{
			case EFI_INVALID_PARAMETER:
				DEBUG((EFI_D_INFO, "One of the parameters has an invalid value.\n"));
				break;
			case EFI_UNSUPPORTED:
				DEBUG((EFI_D_INFO, "The image type is not supported.\n"));
				break;
			case EFI_OUT_OF_RESOURCES:
				DEBUG((EFI_D_INFO, "Image was not loaded due to insufficient resources.\n"));
				break;
			case EFI_NOT_FOUND:
				DEBUG((EFI_D_INFO, "Both SourceBuffer and DevicePath are NULL.\n"));
				break;
			case EFI_LOAD_ERROR:
				DEBUG((EFI_D_INFO, "Image was not loaded because the device returned a read error.\n"));
				break;
			case EFI_SECURITY_VIOLATION:
				DEBUG((EFI_D_INFO, "Image was not loaded because the image's signature was invalid.\n"));
				break;
			default:
				DEBUG((EFI_D_INFO, "Image was not loaded because the image format was corrupt or not understood.\n"));
				break;
		}
	}
	else
		gBootServices->StartImage(ImageLoadedHandle, NULL, NULL);


	gBootServices->FreePool(NewDevicePathProtocol);

	gReceived = TRUE;	// all done.. 

	gBootServices->RaiseTPL(TPL_HIGH_LEVEL);
	gBootServices->RestoreTPL(tpl);

done:
	return;
}


/**
  Required by the EBC compiler and identical in functionality to _ModuleEntryPoint(). 

  @param  ImageHandle  The image handle of the UEFI Application.
  @param  SystemTable  A pointer to the EFI System Table.

  @retval  EFI_SUCCESS               The UEFI Application exited normally.
  @retval  EFI_INCOMPATIBLE_VERSION  _gUefiDriverRevision is greater than SystemTable->Hdr.Revision.
  @retval  Other                     Return value from ProcessModuleEntryPointList().

**/
EFI_STATUS
EFIAPI
_ModuleEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	EFI_EVENT Event;

	DEBUG((EFI_D_INFO, "Running RK loader.\n"));
	InitializeLib(ImageHandle, SystemTable);
	
	gReceived = FALSE;	// reset event!
	
	//CpuBreakpoint();

	// wait for EFI EVENT GROUP READY TO BOOT
	gBootServices->CreateEventEx(0x200, 0x10, &CallbackSMI, NULL, &SMBIOS_TABLE_GUID, &Event);

	return EFI_SUCCESS;
}
