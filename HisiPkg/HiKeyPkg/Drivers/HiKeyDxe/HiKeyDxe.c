/** @file
*
*  Copyright (c) 2015, Linaro Ltd. All rights reserved.
*  Copyright (c) 2015, Hisilicon Ltd. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/BaseMemoryLib.h>
#include <Library/BdsLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Guid/ArmGlobalVariableHob.h>

#include "HiKeyDxeInternal.h"

#define SERIAL_NUMBER_LENGTH        16

STATIC
VOID
EFIAPI
HiKeyInitSerialNo (
  IN   VOID
  )
{
  EFI_STATUS           Status;
  UINTN                VariableSize;
  CHAR16               DefaultSerialNo[] = L"0123456789abcdef";

  VariableSize = SERIAL_NUMBER_LENGTH * sizeof (CHAR16);
  Status = gRT->GetVariable (
                  (CHAR16 *)L"SerialNo",
                  &gArmGlobalVariableGuid,
                  NULL,
                  &VariableSize,
                  &DefaultSerialNo
                  );
  if (Status == EFI_NOT_FOUND) {
    Status = gRT->SetVariable (
                    (CHAR16*)L"SerialNo",
                    &gArmGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE       |
                    EFI_VARIABLE_BOOTSERVICE_ACCESS |
                    EFI_VARIABLE_RUNTIME_ACCESS,
                    VariableSize,
                    DefaultSerialNo
                    );
  }
}

STATIC
VOID
EFIAPI
HiKeyInitBootDevice (
  IN VOID
  )
{
  EFI_STATUS            Status;
  UINTN                 VariableSize;
  CHAR16                DefaultBootDevice[] = L"emmc";

  VariableSize = BOOT_DEVICE_LENGTH * sizeof (CHAR16);
  Status = gRT->GetVariable (
                  (CHAR16 *)L"HiKeyBootDevice",
                  &gArmGlobalVariableGuid,
                  NULL,
                  &VariableSize,
                  &DefaultBootDevice
                  );
  if (Status == EFI_NOT_FOUND) {
    Status = gRT->SetVariable (
                    (CHAR16*)L"HiKeyBootDevice",
                    &gArmGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE       |
                    EFI_VARIABLE_BOOTSERVICE_ACCESS |
                    EFI_VARIABLE_RUNTIME_ACCESS,
                    VariableSize,
                    DefaultBootDevice
                    );
  }
}

EFI_STATUS
EFIAPI
HiKeyEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS           Status;

  HiKeyInitSerialNo ();
  HiKeyInitBootDevice ();

  // Try to install the Flat Device Tree (FDT). This function actually installs the
  // UEFI Driver Binding Protocol.
  Status = HiKeyFdtInstall (ImageHandle);
  if (EFI_ERROR (Status))
    return Status;

  Status = HiKeyBootMenuInstall ();

  return Status;
}
