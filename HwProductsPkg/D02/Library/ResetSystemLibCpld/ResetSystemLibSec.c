/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpd1z2bQhhV6cC9dn956MYMwSDGfeyc2k9r9TKK9bTrj6YPvsPEr44pJLAZWy
NJXlVfkbaSe+hl9RXTC9oabrF7EREUkVfNcDt2gwoBwRt3KcHmgSvQcnU+RDGmN6q0dx1r30
ZarNrai2krun4kgghZ2KPn8RaLAsb4JnlYAI6FH5BWQAbvcFNfWRn2qTqmrpAA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/** @file
  Template library implementation to support ResetSystem Runtime call.

  Fill in the templates with what ever makes you system reset.

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2013, ARM Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/EfiResetSystemLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>



/**
  Resets the entire platform.

  @param  ResetType             The type of reset to perform.
  @param  ResetStatus           The status code for the reset.
  @param  DataSize              The size, in bytes, of WatchdogData.
  @param  ResetData             For a ResetType of EfiResetCold, EfiResetWarm, or
                                EfiResetShutdown the data buffer starts with a Null-terminated
                                Unicode string, optionally followed by additional binary data.

**/
EFI_STATUS
EFIAPI
LibResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  )
{
    UINT64    mResetRegBaseAddr;

    mResetRegBaseAddr = PcdGet64(PcdCpldBaseAddress);
    switch (ResetType)
    {
        case EfiResetPlatformSpecific:
            // Map the platform specific reset as reboot
        case EfiResetWarm:
            // Map a warm reset into a cold reset
        case EfiResetCold:
        case EfiResetShutdown:

            MmioWrite8(mResetRegBaseAddr + 0x17, 0x55);
            //MmioWrite8(mResetRegBaseAddr + 0x0E, 0x08);
            break;
        default:
            return EFI_INVALID_PARAMETER;
    }


    return EFI_SUCCESS;
}
