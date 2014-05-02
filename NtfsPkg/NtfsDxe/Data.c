/*++

Copyright (c) 2005 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  Data.c

Abstract:

  Global data in the FAT Filesystem driver

Revision History

--*/

#include "Ntfs.h"

//
// Globals
//
//
// NtfsFsLock - Global lock for synchronizing all requests.
//
EFI_LOCK NtfsFsLock = EFI_INITIALIZE_LOCK_VARIABLE(TPL_CALLBACK);

//
// Filesystem interface functions
//
EFI_FILE_PROTOCOL               NtfsFileInterface = {
  EFI_FILE_PROTOCOL_REVISION,
  NtfsOpen,
  NtfsClose,
  NtfsDelete,
  NtfsRead,
  NtfsWrite,
  NtfsGetPosition,
  NtfsSetPosition,
  NtfsGetInfo,
  NtfsSetInfo,
  NtfsFlush
};
