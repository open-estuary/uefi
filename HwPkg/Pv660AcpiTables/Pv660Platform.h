/** @file
*
*  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
*  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/
*
**/


#ifndef _PV660_PLATFORM_H_
#define _PV660_PLATFORM_H_

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_ARM_OEM_ID           'H','I','S','I'   // OEMID 6 bytes long
#define EFI_ACPI_ARM_OEM_TABLE_ID     SIGNATURE_64('H','I','S','I','-','D','0','2') // OEM table id 8 bytes long
#define EFI_ACPI_ARM_OEM_REVISION     0x20140727
#define EFI_ACPI_ARM_CREATOR_ID       SIGNATURE_32('H','I','S','I')
#define EFI_ACPI_ARM_CREATOR_REVISION 0x00000099

// A macro to initialise the common header part of EFI ACPI tables as defined by
// EFI_ACPI_DESCRIPTION_HEADER structure.
#define ARM_ACPI_HEADER(Signature, Type, Revision) {              \
    Signature,                      /* UINT32  Signature */       \
    sizeof (Type),                  /* UINT32  Length */          \
    Revision,                       /* UINT8   Revision */        \
    0,                              /* UINT8   Checksum */        \
    { EFI_ACPI_ARM_OEM_ID },        /* UINT8   OemId[6] */        \
    EFI_ACPI_ARM_OEM_TABLE_ID,      /* UINT64  OemTableId */      \
    EFI_ACPI_ARM_OEM_REVISION,      /* UINT32  OemRevision */     \
    EFI_ACPI_ARM_CREATOR_ID,        /* UINT32  CreatorId */       \
    EFI_ACPI_ARM_CREATOR_REVISION   /* UINT32  CreatorRevision */ \
  }

#define PV660_WATCHDOG_COUNT  2

#endif
