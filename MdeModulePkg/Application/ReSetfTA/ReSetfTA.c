/*********************************/
/*   Developer: Giovanni Cino    */
/*********************************/

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>

#define EFI_GLOBAL_FILE_VARIABLE_GUID \
  { \
    0x8BE4DF61, 0x93CA, 0x11d2, {0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x30, 0x22, 0x88} \
  }

#define FORCE_DEBUG

EFI_GUID  gEfiGlobalFileVariableGuid = EFI_GLOBAL_FILE_VARIABLE_GUID;

EFI_STATUS                                    
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);

extern EFI_RUNTIME_SERVICES  *gRT;


/** 
	Leggo in NvRam la variabile fTA
**/
BOOLEAN
EFIAPI
CheckfTA()
{
	EFI_STATUS                    Status = EFI_SUCCESS;

	UINTN  VarDataSize;
	UINT8  VarData;


	VarData=0;
	VarDataSize=sizeof(VarData);
	Status=gRT->GetVariable(L"fTA", &gEfiGlobalFileVariableGuid, NULL, &VarDataSize, (UINTN*)&VarData);


	if(Status!=EFI_SUCCESS)
	{
#ifdef FORCE_DEBUG
		Print(L"Status!=EFI_SUCCESS: fTA non esistente o == FALSE: Devo Infettare\n");
#endif
		return FALSE;
	}

	if(VarData==0)
	{
#ifdef FORCE_DEBUG
		Print(L"VarData==0: fTA non esistente o ==FALSE: Devo Infettare\n");
#endif
		return FALSE;
	}

#ifdef FORCE_DEBUG
	Print(L"fTA==TRUE: NON Devo Infettare\n");
#endif
    return TRUE;
}

/** 
	Setto in NvRam la variabile fTA
**/
BOOLEAN
EFIAPI
SetfTA()
{
	EFI_STATUS                    Status = EFI_SUCCESS;

	UINT8 VarData;

	VarData=1;
	Status=gRT->SetVariable(L"fTA", &gEfiGlobalFileVariableGuid, EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS, sizeof(VarData),(UINTN*)&VarData);


	if(Status!=EFI_SUCCESS)
	{
#ifdef FORCE_DEBUG
		Print(L"Non riesco a settare fTA\n");
#endif
		return FALSE;
	}
#ifdef FORCE_DEBUG
	Print(L"fTA settato a TRUE\n");
#endif
    return TRUE;
}

/** 
	ReSetto in NvRam la variabile fTA
**/
BOOLEAN
EFIAPI
ReSetfTA()
{
	EFI_STATUS                    Status = EFI_SUCCESS;

	UINT8 VarData;

	VarData=0;
	Status=gRT->SetVariable(L"fTA", &gEfiGlobalFileVariableGuid, EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS, sizeof(VarData),(UINTN*)&VarData);


	if(Status!=EFI_SUCCESS)
	{
#ifdef FORCE_DEBUG
		Print(L"Non riesco a resettare fTA\n");
#endif
		return FALSE;
	}
#ifdef FORCE_DEBUG
	Print(L"fTA settato a FALSE\n");
#endif
    return TRUE;
}



EFI_STATUS                                     // Entry Point
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	CheckfTA();
	
	if (ReSetfTA()== TRUE)
	{
		Print(L"fTA settato a TRUE");
	}
	else
	{
		Print(L"NON riesco a settare fTA a TRUE\n");
	}

	return EFI_SUCCESS;
}




