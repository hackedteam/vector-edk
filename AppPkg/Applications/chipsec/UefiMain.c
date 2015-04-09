#include <Uefi.h>
//#include <Guid/FileInfo.h>
//#include <Protocol/LoadedImage.h>
//#include <Library/UefiBootServicesTableLib.h>
//#include <Library/BaseLib.h>
//#include <Library/BaseMemoryLib.h>
//#include <Library/DebugLib.h>
//#include <Library/MemoryAllocationLib.h>
//#include <Library/DevicePathLib.h>
//#include <Library/FileHandleLib.h>
//#include <Library/PrintLib.h>
//#include <Library/UefiLib.h>

extern int main(int argc, char *argv[]);

EFI_STATUS                                     // Entry Point
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS                    Status = EFI_SUCCESS;
	//EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;  

	main(0, NULL);

	return Status;
}
