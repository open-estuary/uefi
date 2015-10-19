/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpd1z2bQhhV6cC9dn956MYMwSDGfeyc2k9r9TKK9bTrj6YPvsPEr44pJLAZWy
NJXlVegU+hOwXxwBPvY79ThZgAcnFthw71ThLsRFRu/V1MynFYMSZQZ+Bc1EF+wYTwa8JeCr
Sl8zHJWLjHeUpkQ2IIDJRNt1/fIz9vaT47rbjh4rz23kSx5SgNwseFJFz1m3SA==*/
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
    MmioWrite32(mResetRegBaseAddr + (0x16 << 1), 0x55);
  //uniBIOS_l00306713_000_start 2015-4-11 14:58:14
 //Issue ID:DTS2015040805519    Description: 【iWare V200R002C00B980 PANGEA BBIT测试】EBL下的输入复位命令后会打印部分硬盘启动信息，各单板都需要整改      
    MicroSecondDelay(2000000);  //由于写入CPLD重启命令后，重启CPU还需一段时间，有继续打印，所以加延时      
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

  // TODO: Draft Code, need to be optimized later
  MmioWrite32(0x60380154, 0);
  MmioWrite32(0x60380004, 0x78000000);
  MmioWrite32(0x60380020, 0x1c42c);
  MmioWrite32(0x60380024, 0x1c42c);
  MmioWrite32(0x60380154, 1);
  
  mResetRegBaseAddr = 0x78000000;

  Status = gDS->GetMemorySpaceDescriptor(mResetRegBaseAddr,&desp);
  if(EFI_ERROR(Status)){
      return Status;
  } 
  desp.Attributes |= EFI_MEMORY_RUNTIME;
  Status = gDS->SetMemorySpaceAttributes(mResetRegBaseAddr, 0x10000, desp.Attributes);
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

