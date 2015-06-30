/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Protocol\OemNicProtocol.h


Description:

*************************************************/

#ifndef _OEM_NIC_PROTOCOL_H_
#define _OEM_NIC_PROTOCOL_H_

#define OEM_NIC_PROTOCOL_GUID   \
    { 0xb5903955, 0x31e9, 0x4aaf, { 0xb2, 0x83, 0x7, 0x9f, 0x3c, 0xc4, 0x71, 0x66 } }

typedef
EFI_STATUS
(EFIAPI *OEM_NIC_GET_MAC_ADDRESS) (
  IN OUT EFI_MAC_ADDRESS *Mac,
  IN UINTN Port
  );

typedef
EFI_STATUS
(EFIAPI *OEM_NIC_SET_MAC_ADDRESS) (
  IN EFI_MAC_ADDRESS *Mac,
  IN UINTN Port
  );

typedef struct {
  OEM_NIC_GET_MAC_ADDRESS GetMac;
  OEM_NIC_SET_MAC_ADDRESS SetMac;
} OEM_NIC_PROTOCOL;

extern EFI_GUID gOemNicProtocolGuid;


#endif
