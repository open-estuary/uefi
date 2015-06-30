/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Library\OemAddressMapLib.h

Description:

*************************************************/

#ifndef _OEM_ADDRESS_MAP_LIB_H_
#define _OEM_ADDRESS_MAP_LIB_H_

// Invalid address, will cause exception when accessed by bug code
#define ADDRESS_MAP_INVALID ((UINTN)(-1))

UINTN OemGetPoeSubBase (UINT32 NodeId);
UINTN OemGetPeriSubBase (UINT32 NodeId);
UINTN OemGetAlgSubBase (UINT32 NodeId);
UINTN OemGetM3SubBase (UINT32 NodeId);

VOID OemAddressMapInit(VOID);

#endif

