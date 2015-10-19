/** @file
*
*  Copyright (c) 2014-2015, Linaro Limited. All rights reserved.
*  Copyright (c) 2014-2015, Hisilicon Limited. All rights reserved.
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

#include <Hi6220.h>

// The total number of descriptors, including the final "end-of-table" descriptor.
#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS 12

// DDR attributes
#define DDR_ATTRIBUTES_CACHED           ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK
#define DDR_ATTRIBUTES_UNCACHED         ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED

STATIC struct HiKeyReservedMemory {
  EFI_PHYSICAL_ADDRESS         Offset;
  EFI_PHYSICAL_ADDRESS         Size;
} HiKeyReservedMemoryBuffer [] = {
  { 0x05E00000, 0x00100000 },    // MCU
  { 0x06DFF000, 0x00001000 },    // MAILBOX
  { 0x0740F000, 0x00001000 },    // MAILBOX
  { 0x3E000000, 0x02000000 }     // TEE OS
};

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
  UINTN                         Index = 0, Count, ReservedTop;
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable;
  EFI_PEI_HOB_POINTERS          NextHob;
  EFI_RESOURCE_ATTRIBUTE_TYPE   ResourceAttributes;
  UINT64                        ResourceLength;
  EFI_PHYSICAL_ADDRESS          ResourceTop;

  NextHob.Raw = GetHobList ();
  Count = sizeof (HiKeyReservedMemoryBuffer) / sizeof (struct HiKeyReservedMemory);
  while ((NextHob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, NextHob.Raw)) != NULL)
  {
    if (Index >= Count)
      break;
    if ((NextHob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) &&
        (HiKeyReservedMemoryBuffer[Index].Offset >= NextHob.ResourceDescriptor->PhysicalStart) &&
        ((HiKeyReservedMemoryBuffer[Index].Offset + HiKeyReservedMemoryBuffer[Index].Size) <=
         NextHob.ResourceDescriptor->PhysicalStart + NextHob.ResourceDescriptor->ResourceLength))
    {
      ResourceAttributes = NextHob.ResourceDescriptor->ResourceAttribute;
      ResourceLength = NextHob.ResourceDescriptor->ResourceLength;
      ResourceTop = NextHob.ResourceDescriptor->PhysicalStart + ResourceLength;
      ReservedTop = HiKeyReservedMemoryBuffer[Index].Offset + HiKeyReservedMemoryBuffer[Index].Size;

      // Create the System Memory HOB for the reserved buffer
      BuildResourceDescriptorHob (EFI_RESOURCE_MEMORY_RESERVED,
                                  EFI_RESOURCE_ATTRIBUTE_PRESENT,
                                  HiKeyReservedMemoryBuffer[Index].Offset,
                                  HiKeyReservedMemoryBuffer[Index].Size);
      // Update the HOB
      NextHob.ResourceDescriptor->ResourceLength = HiKeyReservedMemoryBuffer[Index].Offset - NextHob.ResourceDescriptor->PhysicalStart;

      // If there is some memory available on the top of the reserved memory then create a HOB
      if (ReservedTop < ResourceTop)
      {
        BuildResourceDescriptorHob (EFI_RESOURCE_SYSTEM_MEMORY,
                                    ResourceAttributes,
                                    ReservedTop,
                                    ResourceTop - ReservedTop);
      }
      Index++;
    }
    NextHob.Raw = GET_NEXT_HOB (NextHob);
  }

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

  Index = 0;

  // Hi6220 SOC peripherals
  VirtualMemoryTable[Index].PhysicalBase    = HI6220_PERIPH_BASE;
  VirtualMemoryTable[Index].VirtualBase     = HI6220_PERIPH_BASE;
  VirtualMemoryTable[Index].Length          = HI6220_PERIPH_SZ;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DDR - 1GB
  VirtualMemoryTable[++Index].PhysicalBase  = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].VirtualBase     = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].Length          = PcdGet64 (PcdSystemMemorySize);
  VirtualMemoryTable[Index].Attributes      = CacheAttributes;

  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase  = 0;
  VirtualMemoryTable[Index].VirtualBase     = 0;
  VirtualMemoryTable[Index].Length          = 0;
  VirtualMemoryTable[Index].Attributes      = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = VirtualMemoryTable;
}
