/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Library\OemSetVirtualMapDesc.h


Description: 

*************************************************/

#ifndef _OEM_SET_VIRTUAL_MAP_DESC_H_
#define _OEM_SET_VIRTUAL_MAP_DESC_H_


UINTN OemSetVirtualMapDesc (
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable,
  ARM_MEMORY_REGION_ATTRIBUTES  CacheAttributes
  );

#endif

