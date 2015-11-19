
/*++

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscBaseBoardManufacturerData.c

Abstract:

  This file provide OEM to define Smbios Type2 Data

**/
#include "SmbiosMisc.h"

//
// Static (possibly build generated) Chassis Manufacturer data.
//
MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE2, MiscBaseBoardManufacturer) = {
    {                                                     // Hdr
        EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION,                // Type,
        0,                                                    // Length,
        0                                                     // Handle
    },
    STRING_TOKEN(STR_MISC_BASE_BOARD_MANUFACTURER),       // BaseBoardManufacturer
    STRING_TOKEN(STR_MISC_BASE_BOARD_PRODUCT_NAME),       // BaseBoardProductName
    STRING_TOKEN(STR_MISC_BASE_BOARD_VERSION),            // BaseBoardVersion
    STRING_TOKEN(STR_MISC_BASE_BOARD_SERIAL_NUMBER),      // BaseBoardSerialNumber
    STRING_TOKEN(STR_MISC_BASE_BOARD_ASSET_TAG),          // BaseBoardAssetTag
    {                                                     // FeatureFlag
        1,                                                    // Motherboard           :1
        0,                                                    // RequiresDaughterCard  :1
        0,                                                    // Removable             :1
        1,                                                    // Replaceable           :1
        0,                                                    // HotSwappable          :1
        0                                                     // Reserved              :3
    },
    STRING_TOKEN(STR_MISC_BASE_BOARD_CHASSIS_LOCATION),   // BaseBoardChassisLocation  
    0,                                                    // ChassisHandle;
    BaseBoardTypeMotherBoard,                             // BoardType;        
    0,                                                    // NumberOfContainedObjectHandles;
    {
        0
    }                                                     // ContainedObjectHandles[1];
};
