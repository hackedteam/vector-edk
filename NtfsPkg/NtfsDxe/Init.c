/*++

Copyright (c) 2005 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  Init.c

Abstract:

  Initialization routines

--*/

#include "Ntfs.h"
#include "ntfs/ntfsinternal.h"
#include "ntfs/logging.h"
#include "ntfs/layout.h"

EFI_STATUS
NtfsOpenDevice (
  IN OUT NTFS_VOLUME           *Volume
  );

void DiagnosticSizeOf()
{
	ATTR_RECORD *attr_record;

	attr_record = AllocateZeroPool(sizeof(ATTR_RECORD));

}

VOID NtfsCreateVolumeName(CHAR8 *RootFileString, UINTN Address)
{
	CHAR8 *szHex = "0123456789abcdef";

	RootFileString[4] = szHex[Address % 0x10];
	Address = Address >> 4;

	RootFileString[5] = szHex[Address % 0x10];
	Address = Address >> 4;

	RootFileString[6] = szHex[Address % 0x10];
	Address = Address >> 4;

	RootFileString[7] = szHex[Address % 0x10];
	Address = Address >> 4;

	RootFileString[9] = szHex[Address % 0x10];
	Address = Address >> 4;

	RootFileString[9] = szHex[Address % 0x10];
	Address = Address >> 4;

	RootFileString[10] = szHex[Address % 0x10];
	Address = Address >> 4;
}

EFI_STATUS
NtfsAllocateVolume (
  IN  EFI_HANDLE                Handle,
  IN  EFI_DISK_IO_PROTOCOL      *DiskIo,
  IN  EFI_BLOCK_IO_PROTOCOL     *BlockIo
  )
/*++

Routine Description:

  Allocates volume structure, detects FAT file system, installs protocol,
  and initialize cache.

Arguments:

  Handle                - The handle of parent device.
  DiskIo                - The DiskIo of parent device.
  BlockIo               - The BlockIo of parent devicel

Returns:

  EFI_SUCCESS           - Allocate a new volume successfully.
  EFI_OUT_OF_RESOURCES  - Can not allocate the memory.
  Others                - Allocating a new volume failed.

--*/
{
  EFI_STATUS  Status;
  NTFS_VOLUME  *Volume;
  UINT32 MediaId;

  //Print(L"NtfsAllocateVolume\n");

  ntfsInit();
  //
  // Allocate a volume structure
  //
  Volume = AllocateZeroPool (sizeof (NTFS_VOLUME));
  if (Volume == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }


  //CpuBreakpoint();

  //
  // Initialize the structure
  //
  Volume->Signature                   = NTFS_VOLUME_SIGNATURE;
  Volume->Handle                      = Handle;
  Volume->DiskIo                      = DiskIo;
  Volume->BlockIo                     = BlockIo;
  Volume->MediaId                     = BlockIo->Media->MediaId;
  Volume->ReadOnly                    = BlockIo->Media->ReadOnly;
  Volume->VolumeInterface.Revision    = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REVISION;
  Volume->VolumeInterface.OpenVolume  = NtfsOpenVolume;

  
  //InitializeListHead (&Volume->CheckRef);
  //InitializeListHead (&Volume->DirCacheList);
  //
  // Initialize Root Directory entry
  //
  //Volume->RootDirEnt.FileString       = Volume->RootFileString;
  //Volume->RootDirEnt.Entry.Attributes = ;
  //
  // Check to see if there's a file system on the volume
  //
  Status = NtfsOpenDevice (Volume);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  MediaId = BlockIo->Media->MediaId;

  Volume->RootFileString[0] = 'n';
  Volume->RootFileString[1] = 't';
  Volume->RootFileString[2] = 'f';
  Volume->RootFileString[3] = 's';

  NtfsCreateVolumeName(Volume->RootFileString, (UINTN) BlockIo);
  
  Volume->vd = ntfsMount(Volume->RootFileString, Volume, 0, 0, 0, 0, 0);	// 
  Volume->vol = Volume->vd->vol;

  if (Volume->vd == NULL)
  {
	  //Print(L"Allocation of 'volume' failed.\n\r");
	  goto Done;
  }
  else
  {
		//Print(L"Allocation of 'volume' successful.\n\r");
  }

  //
  // Install our protocol interfaces on the device's handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Volume->Handle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  &Volume->VolumeInterface,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    //Print(L"InstallMultipleProtocolInterfaces %x", Status);
    goto Done;
  }
  //
  // Volume installed
  //
  DEBUG ((EFI_D_INIT, "Installed NTFS filesystem on %p\n", Handle));
  //Print(L"Installed NTFS filesystem on %p\n", Handle);
  Volume->Valid = TRUE;

Done:
  if (EFI_ERROR (Status)) {
    //Print(L"FreePool(volume)\n");
    FreePool(Volume);
  }

  //DiagnosticSizeOf();
  //Print(L"Leave NtfsAllocateVolume\n");
  return Status;
}

EFI_STATUS
NtfsAbandonVolume (
  IN NTFS_VOLUME *Volume
  )
/*++

Routine Description:

  Called by NtfsAbandonVolume(), Abandon the volume.

Arguments:

  Volume                - The volume to be abandoned.

Returns:

  EFI_SUCCESS           - Abandoned the volume successfully.
  Others                - Can not uninstall the protocol interfaces.

--*/
{
  EFI_STATUS  Status;
  BOOLEAN     LockedByMe;

  //
  // Uninstall the protocol interface.
  //
  if (Volume->Handle != NULL) {
    Status = gBS->UninstallMultipleProtocolInterfaces (
                    Volume->Handle,
                    &gEfiSimpleFileSystemProtocolGuid,
                    &Volume->VolumeInterface,
                    NULL
                    );

	if (Volume->vol != NULL) {
		ntfs_umount(Volume->vol, 1);
		
	}

    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  LockedByMe = FALSE;

  //
  // Acquire the lock.
  // If the caller has already acquired the lock (which
  // means we are in the process of some Fat operation),
  // we can not acquire again.
  //
  Status = NtfsAcquireLockOrFail ();
  if (!EFI_ERROR (Status)) {
    LockedByMe = TRUE;
  }
  //
  // The volume is still being used. Hence, set error flag for all OFiles still in
  // use. In two cases, we could get here. One is EFI_MEDIA_CHANGED, the other is
  // EFI_NO_MEDIA.
  //
  if (Volume->root != NULL) {
		//Print(L"Destruction of volume->root");
		ntfs_inode_close(Volume->root);
		Volume->root = NULL;
		ntfs_umount(Volume->vol, false);
  }

  Volume->Valid = FALSE;

  //
  // Release the lock.
  // If locked by me, this means DriverBindingStop is NOT
  // called within an on-going Fat operation, so we should
  // take responsibility to cleanup and free the volume.
  // Otherwise, the DriverBindingStop is called within an on-going
  // Fat operation, we shouldn't check reference, so just let outer
  // FatCleanupVolume do the task.
  //
  if (LockedByMe) {
    //FatCleanupVolume (Volume, NULL, EFI_SUCCESS);
    //FatReleaseLock ();
	  NtfsReleaseLock();
  }

  return EFI_SUCCESS;
}

EFI_STATUS
NtfsOpenDevice (
  IN OUT NTFS_VOLUME           *Volume
  )
/*++

Routine Description:

  Detects NTFS file system on Disk and set relevant fields of Volume

Arguments:

  Volume                - The volume structure.

Returns:

  EFI_SUCCESS           - The NTFS File System is detected successfully
  EFI_UNSUPPORTED       - The volume is not NTFS file system.
  EFI_VOLUME_CORRUPTED  - The volume is corrupted.

--*/
{
  EFI_STATUS            Status;
  UINT32                BlockSize;
  UINT32                DirtyMask;
  EFI_DISK_IO_PROTOCOL  *DiskIo;

  NTFS_BOOT_SECTOR		NtfsBs;

  //
  // Read the FAT_BOOT_SECTOR BPB info
  // This is the only part of FAT code that uses parent DiskIo,
  // Others use FatDiskIo which utilizes a Cache.
  //

  //Print(L"[Init]\n");

  DiskIo  = Volume->DiskIo;
  Status  = DiskIo->ReadDisk (DiskIo, Volume->MediaId, 0, sizeof (NtfsBs), &NtfsBs);
  //Print(L"[Init] ReadDisk (%x, %x, %x, %x)\n\r", DiskIo, Volume->MediaId, 0, sizeof(NtfsBs));
  
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INIT, "NtfsOpenDevice: read of part_lba failed %r\n", Status));
	//gST->ConOut->OutputString(gST->ConOut, L"NtfsOpenDevice: read of part_lba failed\n\n");
    return Status;
  }

  if (ntfs_boot_sector_is_ntfs(&NtfsBs) == FALSE) {
	  DEBUG ((EFI_D_INIT, "NtfsOpenDevice: ntfs_boot_sector_is_ntfs FALSE\n", Status));
	  //gST->ConOut->OutputString(gST->ConOut, L"NtfsOpenDevice: ntfs_boot_sector_is_ntfs FALSE\n\n");
	  return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
