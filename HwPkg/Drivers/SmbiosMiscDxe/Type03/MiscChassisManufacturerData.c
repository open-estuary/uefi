/*++

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscChassisManufacturerData.c

Abstract:

  This file provide OEM to define Smbios Type3 Data

**/
#include "SmbiosMisc.h"


//
// Static (possibly build generated) Chassis Manufacturer data.
//
MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE3, MiscChassisManufacturer) = {
    {                                                     // Hdr
        EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE  ,                   // Type,
        0,                                                    // Length,
        0                                                     // Handle
    },
    STRING_TOKEN(STR_MISC_CHASSIS_MANUFACTURER),          // Manufactrurer
    MiscChassisTypeOther,                                 // Type
    STRING_TOKEN(STR_MISC_CHASSIS_VERSION),               // Version
    STRING_TOKEN(STR_MISC_CHASSIS_SERIAL_NUMBER),         // SerialNumber
    STRING_TOKEN(STR_MISC_CHASSIS_ASSET_TAG),             // AssetTag
    ChassisStateSafe,                                     // BootupState
    ChassisStateSafe,                                     // PowerSupplyState
    ChassisStateSafe,                                     // ThermalState
    ChassisSecurityStatusNone,                            // SecurityState
    {
        0,                                                    // OemDefined[0]
        0,                                                    // OemDefined[1]
        0,                                                    // OemDefined[2]
        0                                                     // OemDefined[3]
    },                                                    
    0,                                                    // Height
    1,                                                    // NumberofPowerCords
    0,                                                    // ContainedElementCount
    0,                                                    // ContainedElementRecordLength
    {                                                     // ContainedElements[0]
        {
            0,                                                    // ContainedElementType
            0,                                                    // ContainedElementMinimum
            0                                                     // ContainedElementMaximum
        }
    }, 
    STRING_TOKEN (STR_MISC_CHASSIS_SKU_NUMBER)            // Chassis SKU Number
};
