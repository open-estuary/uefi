/** @file
*
*  Copyright (c) 2011-2014, ARM Limited. All rights reserved.
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

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <ArmPlatform.h>

#include <Library/OemSetVirtualMapDesc.h>

#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS          32

// DDR attributes
#define DDR_ATTRIBUTES_CACHED           ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK
#define DDR_ATTRIBUTES_UNCACHED         ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED

/**
  Return the Virtual Memory Map of your platform

  This Virtual Memory Map is used by MemoryInitPei Module to initialize the MMU on your platform.

  @param[out]   VirtualMemoryMap    Array of ARM_MEMORY_REGION_DESCRIPTOR describing a Physical-to-
                                    Virtual Memory mapping. This array must be ended by a zero-filled
                                    entry

**/
VOID
ArmPlatformGetVirtualMemoryMap (
  IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
  )
{
  ARM_MEMORY_REGION_ATTRIBUTES  CacheAttributes;
  //EFI_RESOURCE_ATTRIBUTE_TYPE   ResourceAttributes;
  UINTN                         Index;
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable;
  //UINT32                        SysId;
  //BOOLEAN                       HasSparseMemory;
  //EFI_VIRTUAL_ADDRESS           SparseMemoryBase;
  //UINT64                        SparseMemorySize;
  EFI_PEI_HOB_POINTERS          NextHob;

  ASSERT (VirtualMemoryMap != NULL);

  VirtualMemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR*)AllocatePages(EFI_SIZE_TO_PAGES (sizeof(ARM_MEMORY_REGION_DESCRIPTOR) * MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS));
  if (VirtualMemoryTable == NULL) {
      return;
  }

  if (FeaturePcdGet(PcdCacheEnable) == TRUE) {
      CacheAttributes = DDR_ATTRIBUTES_CACHED;
  } else {
      CacheAttributes = DDR_ATTRIBUTES_UNCACHED;
  }
/*
  // ReMap (Either NOR Flash or DRAM)
  VirtualMemoryTable[Index].PhysicalBase = ARM_VE_REMAP_BASE;
  VirtualMemoryTable[Index].VirtualBase  = ARM_VE_REMAP_BASE;
  VirtualMemoryTable[Index].Length       = ARM_VE_REMAP_SZ;

  if (FeaturePcdGet(PcdNorFlashRemapping) == FALSE) {
    // Map the NOR Flash as Secure Memory
    if (FeaturePcdGet(PcdCacheEnable) == TRUE) {
      VirtualMemoryTable[Index].Attributes   = DDR_ATTRIBUTES_CACHED;
    } else {
      VirtualMemoryTable[Index].Attributes   = DDR_ATTRIBUTES_UNCACHED;
    }
  } else {
    // DRAM mapping
    VirtualMemoryTable[Index].Attributes   = CacheAttributes;
  }
*/

  Index = OemSetVirtualMapDesc(VirtualMemoryTable, CacheAttributes);

  // Search for System Memory Hob that contains the EFI resource system memory  s00296804
  NextHob.Raw = GetHobList ();
  while ((NextHob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, NextHob.Raw)) != NULL)
  {
    if (NextHob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)
    {
        if (NextHob.ResourceDescriptor->PhysicalStart > BASE_4GB)//只修改4G以上的属性
        {
            VirtualMemoryTable[++Index].PhysicalBase = NextHob.ResourceDescriptor->PhysicalStart;
            VirtualMemoryTable[Index].VirtualBase  = NextHob.ResourceDescriptor->PhysicalStart;
            VirtualMemoryTable[Index].Length       =NextHob.ResourceDescriptor->ResourceLength;
            VirtualMemoryTable[Index].Attributes   =  CacheAttributes;
        }
    }

    NextHob.Raw = GET_NEXT_HOB (NextHob);
  }
  
  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase = 0;
  VirtualMemoryTable[Index].VirtualBase  = 0;
  VirtualMemoryTable[Index].Length       = 0;
  VirtualMemoryTable[Index].Attributes   = (ARM_MEMORY_REGION_ATTRIBUTES)0;
  
  ASSERT((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);
  DEBUG((EFI_D_ERROR, "[%a]:[%dL] discriptor count=%d\n", __FUNCTION__, __LINE__, Index+1));

  *VirtualMemoryMap = VirtualMemoryTable;
}
