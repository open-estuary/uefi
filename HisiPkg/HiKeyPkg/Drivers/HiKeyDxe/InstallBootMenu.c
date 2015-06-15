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

#include <Protocol/DevicePathFromText.h>

#include <Guid/ArmGlobalVariableHob.h>
#include <Guid/EventGroup.h>
#include <Guid/GlobalVariable.h>

#include "HiKeyDxeInternal.h"

STATIC CONST BOOLEAN mIsEndOfDxeEvent = TRUE;

STATIC
EFI_STATUS
EFIAPI
HiKeyCreateBootEntry (
  IN CHAR16          *DevicePathText,
  IN CHAR16          *BootArgs,
  IN CHAR16          *BootDescription
  )
{
  BDS_LOAD_OPTION                    *BdsLoadOption;
  EFI_STATUS                          Status;
  UINTN                               DescriptionSize;
  UINTN                               BootOrderSize;
  UINT16                              BootOrder;
  UINT8*                              EfiLoadOptionPtr;
  EFI_DEVICE_PATH_PROTOCOL           *DevicePathNode;
  UINTN                               NodeLength;
  EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL *DevicePathFromTextProtocol;

  if ((DevicePathText == NULL) || (BootArgs == NULL) || (BootDescription == NULL)) {
    DEBUG ((EFI_D_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Status = gRT->GetVariable (
                  (CHAR16*)L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BootOrderSize,
                  (VOID*)&BootOrder
                  );
  if (EFI_ERROR (Status) == EFI_NOT_FOUND) {
    BootOrder = 0;
  }
  BdsLoadOption = (BDS_LOAD_OPTION*)AllocateZeroPool (sizeof(BDS_LOAD_OPTION));
  ASSERT (BdsLoadOption != NULL);

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathFromTextProtocolGuid,
                  NULL,
                  (VOID**)&DevicePathFromTextProtocol
                  );
  ASSERT_EFI_ERROR(Status);

  BdsLoadOption->FilePathList = DevicePathFromTextProtocol->ConvertTextToDevicePath (DevicePathText);
  ASSERT (BdsLoadOption->FilePathList != NULL);
  BdsLoadOption->FilePathListLength = GetDevicePathSize (BdsLoadOption->FilePathList);
  BdsLoadOption->Attributes = LOAD_OPTION_ACTIVE | LOAD_OPTION_CATEGORY_BOOT;

  BdsLoadOption->OptionalDataSize = StrSize (BootArgs);
  BdsLoadOption->OptionalData = (CHAR16*)AllocateZeroPool (BdsLoadOption->OptionalDataSize);
  ASSERT (BdsLoadOption->OptionalData != NULL);
  StrCpy (BdsLoadOption->OptionalData, BootArgs);

  BdsLoadOption->LoadOptionIndex = BootOrder;
  DescriptionSize = StrSize (BootDescription);
  BdsLoadOption->Description = (VOID*)AllocateZeroPool (DescriptionSize);
  StrCpy (BdsLoadOption->Description, BootDescription);

  BdsLoadOption->LoadOptionSize = sizeof(UINT32) + sizeof(UINT16) + DescriptionSize + BdsLoadOption->FilePathListLength + BdsLoadOption->OptionalDataSize;
  BdsLoadOption->LoadOption = (EFI_LOAD_OPTION)AllocateZeroPool (BdsLoadOption->LoadOptionSize);
  ASSERT (BdsLoadOption->LoadOption != NULL);

  EfiLoadOptionPtr = BdsLoadOption->LoadOption;

  //
  // Populate the EFI Load Option and BDS Boot Option structures
  //

  // Attributes fields
  *(UINT32*)EfiLoadOptionPtr = BdsLoadOption->Attributes;
  EfiLoadOptionPtr += sizeof(UINT32);

  // FilePath List fields
  *(UINT16*)EfiLoadOptionPtr = BdsLoadOption->FilePathListLength;
  EfiLoadOptionPtr += sizeof(UINT16);

  // Boot description fields
  CopyMem (EfiLoadOptionPtr, BdsLoadOption->Description, DescriptionSize);
  EfiLoadOptionPtr += DescriptionSize;

  // File path fields
  DevicePathNode = BdsLoadOption->FilePathList;
  while (!IsDevicePathEndType (DevicePathNode)) {
    NodeLength = DevicePathNodeLength(DevicePathNode);
    CopyMem (EfiLoadOptionPtr, DevicePathNode, NodeLength);
    EfiLoadOptionPtr += NodeLength;
    DevicePathNode = NextDevicePathNode (DevicePathNode);
  }

  // Set the End Device Path Type
  SetDevicePathEndNode (EfiLoadOptionPtr);
  EfiLoadOptionPtr += sizeof(EFI_DEVICE_PATH);

  // Fill the Optional Data
  if (BdsLoadOption->OptionalDataSize > 0) {
    CopyMem (EfiLoadOptionPtr, BdsLoadOption->OptionalData, BdsLoadOption->OptionalDataSize);
  }

  Status = gRT->SetVariable (
                  (CHAR16*)L"Boot0000",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BdsLoadOption->LoadOptionSize,
                  BdsLoadOption->LoadOption
                  );

  return Status;
}

STATIC
VOID
EFIAPI
HiKeyOnEndOfDxe (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{
  EFI_STATUS          Status;
  UINT16              BootIndex;
  UINT16             *BootNext;

  Status = HiKeyCreateBootEntry (
             L"VenHw(B549F005-4BD4-4020-A0CB-06F42BDA68C3)/HD(6,GPT,5C0F213C-17E1-4149-88C8-8B50FB4EC70E,0x7000,0x20000)/Image",
             L"console=ttyAMA0,115200 earlycon=pl011,0xf8015000 root=/dev/disk/by-partlabel/system rw rootwait initrd=initrd.img efi=noruntime",
             L"Debian on eMMC"
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "%a: failed to create new boot entry\n", __func__));
    return;
  }
  BootIndex = 0;
  BootNext = &BootIndex;
  Status = gRT->SetVariable (
                  (CHAR16*)L"BootNext",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (UINT16),
                  BootNext
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "%a: failed to set BootNext variable\n", __func__));
    return;
  }
}

EFI_STATUS
HiKeyBootMenuInstall (
  IN VOID
  )
{
  EFI_STATUS          Status;
  EFI_EVENT           EndOfDxeEvent;

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  HiKeyOnEndOfDxe,
                  &mIsEndOfDxeEvent,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

