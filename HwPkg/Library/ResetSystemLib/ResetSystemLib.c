/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefd4rOq7ySRbLaGdgl9oycrerqHPv8AKO1RZRxA4+SX5JSei5ARjG0pAV+Sem3nSr6XF7i
F3PkK29/U2BIQMHNqj7n+DxKBJuFUzu8VNiEhX8aIEEr2U16z6SsTauC5kYUiw==*/
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
#include <Library/UefiRuntimeLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#define SC_SYSSOFTRST_CTRL_OFFSET   0x3004
#define SC_SYSSOFTRST_CTRL_VALUE    0x55555555

UINT64    mResetRegBaseAddr;
EFI_EVENT   mResetVirtualAddressChangeEvent = NULL;

VOID
EFIAPI
ResetVirtualAddressChangeCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EfiConvertPointer (0, (VOID **) &mResetRegBaseAddr);

  return;
}


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
  switch (ResetType) {
  case EfiResetPlatformSpecific:
    // Map the platform specific reset as reboot
  case EfiResetWarm:
    // Map a warm reset into a cold reset
  case EfiResetCold: 

  case EfiResetShutdown:
    MmioWrite32(mResetRegBaseAddr + SC_SYSSOFTRST_CTRL_OFFSET, SC_SYSSOFTRST_CTRL_VALUE);
    MicroSecondDelay(2000000);    
    break;
  
  }

 
  return EFI_SUCCESS;
}

/**
  Initialize any infrastructure required for LibResetSystem () to function.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
LibInitializeResetSystem (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR desp = {0}; 
  EFI_STATUS                      Status;
  
  mResetRegBaseAddr = PcdGet64(PcdSysControlBaseAddress);

  Status = gDS->GetMemorySpaceDescriptor(PcdGet64(PcdSysControlBaseAddress),&desp);
  if(EFI_ERROR(Status)){
      return Status;
  } 
  desp.Attributes |= EFI_MEMORY_RUNTIME;
  Status = gDS->SetMemorySpaceAttributes(PcdGet64(PcdSysControlBaseAddress),0x10000, desp.Attributes);
  if(EFI_ERROR(Status)){
      return Status;
  } 
  Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_NOTIFY,
                ResetVirtualAddressChangeCallBack,
                NULL,
                &gEfiEventVirtualAddressChangeGuid,
                &mResetVirtualAddressChangeEvent
                );
  
  if(EFI_ERROR(Status)){
      mResetVirtualAddressChangeEvent = NULL;
  }


  return EFI_SUCCESS;
}

