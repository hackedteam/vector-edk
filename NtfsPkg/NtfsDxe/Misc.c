/*++

Copyright (c) 2005 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  Misc.c

Abstract:

  Miscellaneous functions

Revision History

--*/

#include "Ntfs.h"


VOID
NtfsAcquireLock (
  VOID
  )
/*++

Routine Description:

  Lock the volume.

Arguments:

  None.

Returns:

  None.

--*/
{
  EfiAcquireLock (&NtfsFsLock);
}

EFI_STATUS
NtfsAcquireLockOrFail (
  VOID
  )
/*++

Routine Description:

  Lock the volume.
  If the lock is already in the acquired state, then EFI_ACCESS_DENIED is returned.
  Otherwise, EFI_SUCCESS is returned.

Arguments:

  None.

Returns:

  EFI_SUCCESS           - The volume is locked.
  EFI_ACCESS_DENIED     - The volume could not be locked because it is already locked.

--*/
{
  return EfiAcquireLockOrFail (&NtfsFsLock);
}

VOID
NtfsReleaseLock (
  VOID
  )
/*++

Routine Description:

  Unlock the volume.

Arguments:

  Null.

Returns:

  None.

--*/
{
  EfiReleaseLock (&NtfsFsLock);
}

//
// time conversion
//
// Adopted from public domain code in FreeBSD libc.
//

#define SECSPERMIN      60
#define MINSPERHOUR     60
#define HOURSPERDAY     24
#define DAYSPERWEEK     7
#define DAYSPERNYEAR    365
#define DAYSPERLYEAR    366
#define SECSPERHOUR     (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY      ((long) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR     12

#define EPOCH_YEAR      1970
#define EPOCH_WDAY      TM_THURSDAY

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define LEAPS_THRU_END_OF(y)    ((y) / 4 - (y) / 100 + (y) / 400)

static const int mon_lengths[2][MONSPERYEAR] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};
static const int year_lengths[2] = {
    DAYSPERNYEAR, DAYSPERLYEAR
};


VOID 
	fsw_efi_decode_time(
		OUT EFI_TIME *EfiTime, IN UINT32 UnixTime
	)
{
    long        days, rem;
    int         y, newy, yleap;
    const int   *ip;
    
    ZeroMem(EfiTime, sizeof(EFI_TIME));
    
    days = UnixTime / SECSPERDAY;
    rem = UnixTime % SECSPERDAY;
    
    EfiTime->Hour = (int) (rem / SECSPERHOUR);
    rem = rem % SECSPERHOUR;
    EfiTime->Minute = (int) (rem / SECSPERMIN);
    EfiTime->Second = (int) (rem % SECSPERMIN);
    
    y = EPOCH_YEAR;
    while (days < 0 || days >= (long) year_lengths[yleap = isleap(y)]) {
        newy = y + days / DAYSPERNYEAR;
        if (days < 0)
            --newy;
        days -= (newy - y) * DAYSPERNYEAR +
            LEAPS_THRU_END_OF(newy - 1) -
            LEAPS_THRU_END_OF(y - 1);
        y = newy;
    }
    EfiTime->Year = y;
    ip = mon_lengths[yleap];
    for (EfiTime->Month = 0; days >= (long) ip[EfiTime->Month]; ++(EfiTime->Month))
        days = days - (long) ip[EfiTime->Month];
    EfiTime->Month++;  // adjust range to EFI conventions
    EfiTime->Day = (int) (days + 1);
}
