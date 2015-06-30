/*************************************************

Copyright (C), 1988-2015, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Library\AcpiNextLib.h

Description: Utilities to construct ACPI tables for next generation
             specification.

*************************************************/

#ifndef __ACPI_NEXT_LIB_H__
#define __ACPI_NEXT_LIB_H__

#define EFI_ACPI_6_0_GIC_ITS_INIT(GicITSHwId, GicITSBase) \
  { \
    EFI_ACPI_6_0_ITS, sizeof (EFI_ACPI_6_0_GIC_ITS_STRUCTURE), EFI_ACPI_RESERVED_WORD, \
    GicITSHwId, GicITSBase, EFI_ACPI_RESERVED_DWORD\
  }

#define EFI_ACPI_HuaWei_GIC_MBI_INIT(GicMBIHwId,GicITSHwId, GicMBIBase,Size) \
  { \
    EFI_ACPI_HuaWei_MBI, sizeof (EFI_ACPI_HuaWei_GIC_MBI_STRUCTURE), EFI_ACPI_RESERVED_WORD, \
    GicMBIHwId,GicITSHwId, GicMBIBase,Size\
  }

#define EFI_ACPI_5_1_GICR_STRUCTURE_INIT(    \
    GicRBase, GicRlength)                                   \
  {                                                                                  \
    EFI_ACPI_5_1_GICR, sizeof (EFI_ACPI_5_1_GICR_STRUCTURE), EFI_ACPI_RESERVED_WORD,   \
     GicRBase, GicRlength \
  }

#define EFI_ACPI_5_1_GICC_AFFINITY_STRUCTURE_INIT(                                              \
    ProximityDomain, ACPIProcessorUID, Flags, ClockDomain)                                      \
  {                                                                                             \
    3, sizeof (EFI_ACPI_5_1_GICC_AFFINITY_STRUCTURE),ProximityDomain ,                          \
     ACPIProcessorUID,  Flags,  ClockDomain                                                     \
  }

#define EFI_ACPI_5_0_MEMORY_AFFINITY_STRUCTURE_INIT(                                              \
    ProximityDomain, AddressBaseLow, AddressBaseHigh, LengthLow, LengthHigh, Flags)               \
  {                                                                                               \
    1, sizeof (EFI_ACPI_5_0_MEMORY_AFFINITY_STRUCTURE),ProximityDomain , EFI_ACPI_RESERVED_WORD,  \
    AddressBaseLow, AddressBaseHigh, LengthLow, LengthHigh, EFI_ACPI_RESERVED_DWORD, Flags,       \
    EFI_ACPI_RESERVED_QWORD                                                                       \
  }


#endif

