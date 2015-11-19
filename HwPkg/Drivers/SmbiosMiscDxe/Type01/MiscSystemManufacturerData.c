
/*++

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscSystemManufacturerData.c

Abstract:

  This file provide OEM to define Smbios Type1 Data

**/
#include "SmbiosMisc.h"


//
// Static (possibly build generated) System Manufacturer data.
//
MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE1, MiscSystemManufacturer) = {
    {                                               // Hdr
        EFI_SMBIOS_TYPE_SYSTEM_INFORMATION,             // Type,
        0,                                              // Length,
        0                                               // Handle
    },
    STRING_TOKEN(STR_MISC_SYSTEM_MANUFACTURER),     // Manufacturer
    STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME),     // ProductName
    STRING_TOKEN(STR_MISC_SYSTEM_VERSION),          // Version
    STRING_TOKEN(STR_MISC_SYSTEM_SERIAL_NUMBER),    // SerialNumber
    {                                               // Uuid
        0x12345678, 0x1234, 0x5678, {0x90, 0xab, 0xcd, 0xde, 0xef, 0xaa, 0xbb, 0xcc}    
    },
    SystemWakeupTypePowerSwitch,                    // SystemWakeupType
    STRING_TOKEN(STR_MISC_SYSTEM_SKU_NUMBER),       // SKUNumber,
    STRING_TOKEN(STR_MISC_SYSTEM_FAMILY)            // Family
};
