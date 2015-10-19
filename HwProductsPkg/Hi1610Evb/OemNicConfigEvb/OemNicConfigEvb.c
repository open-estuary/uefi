/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpd1z2bQhhV6cC9dn956MYMwSDGfeyc2k9r9TKK9bTrj6YPvsPEr44pJLAZWy
NJXlVVn+imGPUvVvApaCslZ6IA0YlQFptasYmlBIaKTuuOc9wSnqDp/zxVkqZYbec6Jpcrnd
Sm6KRYdV7nHKgBhY6CdAH3CMPlW+6+csX70XlAFiHwz6ZZNpetUbbiT7gr/rvg==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwProductsPkg\Pv660Evb\OemNicConfigEvb\OemNicConfigEvb.c

Author: uniBIOS-g00179230 (Heyi Guo)    Date: 2014.11.17

Description:

*************************************************/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/NorFlashProtocol.h>
#include <Protocol/OemNicProtocol.h>

EFI_STATUS
EFIAPI OemGetMacEvb (
  IN OUT EFI_MAC_ADDRESS *Mac,
  IN UINTN Port
  )
{
  UINTN i;

  if (NULL == Mac)
  {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Mac buffer is null!\n", __FUNCTION__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }

  //TODO: discard port number, only support one port
  // Only 6 bytes are used
  for (i =0; i < 6; i++)
  {
      Mac->Addr[i] = *(volatile unsigned char *)(UINTN)(FixedPcdGet64(PcdNORFlashBase) + 0xA00000 + i);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI OemSetMacEvb (
  IN EFI_MAC_ADDRESS *Mac,
  IN UINTN Port
  )
{
  UNI_NOR_FLASH_PROTOCOL *Flash = NULL;
  EFI_STATUS Status;
  if (NULL == Mac)
  {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Mac buffer is null!\n", __FUNCTION__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gUniNorFlashProtocolGuid, NULL, (VOID *) &Flash);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Locate Protocol failed %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }

  // 当前只使用MAC前6字节
  Status = Flash->Write(Flash, 0x00A00000, Mac->Addr, 6);
  return Status;
}


OEM_NIC_PROTOCOL mOemNicProtocolEvb = {
  .GetMac = OemGetMacEvb,
  .SetMac = OemSetMacEvb,
};

EFI_STATUS
EFIAPI
OemNicConfigEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallProtocolInterface(
    &ImageHandle,
    &gOemNicProtocolGuid,
    EFI_NATIVE_INTERFACE,
    &mOemNicProtocolEvb
    );

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Install Protocol failed %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }

  return EFI_SUCCESS;
}

