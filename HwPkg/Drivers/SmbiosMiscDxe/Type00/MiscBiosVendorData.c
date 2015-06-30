/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefd+OMh96lLXslTQbFDvhR2mQ6tuNDf0BGu5BQ+EnM8Mli5OkvDQb0qoCyMw1C1PzYFwU0
mM/ZAoW9f/Mz9+C5Po33kxU3W9m+GI0Wo853nYVwC9MraY8TG2RpKM9CIQ8VbA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*++

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscBiosVendorData.c

Abstract:

  This file provide OEM to define Smbios Type0 Data

**/
#include "SmbiosMisc.h"


//
// Static (possibly build generated) Bios Vendor data.
//
MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE0, MiscBiosVendor) = {
    {                                          //Hdr
        EFI_SMBIOS_TYPE_BIOS_INFORMATION,          // Type,
        0,                                         // Length,
        0                                          // Handle
    },
    STRING_TOKEN (STR_MISC_BIOS_VENDOR),       //Vendor
    STRING_TOKEN (STR_MISC_BIOS_VERSION),      //BiosVersion
    0xE000,                                    //BiosSegment
    STRING_TOKEN (STR_MISC_BIOS_RELEASE_DATE), //BiosReleaseDate
    0,                                         //BiosSize
    {                                          //BiosCharacteristics
        0,                                         // Reserved                          :2
        0,                                         // Unknown                           :1
        0,                                         // BiosCharacteristicsNotSupported   :1
        0,                                         // IsaIsSupported                    :1
        0,                                         // McaIsSupported                    :1
        0,                                         // EisaIsSupported                   :1
        1,                                         // PciIsSupported                    :1
        0,                                         // PcmciaIsSupported                 :1
        0,                                         // PlugAndPlayIsSupported            :1
        0,                                         // ApmIsSupported                    :1
        1,                                         // BiosIsUpgradable                  :1
        1,                                         // BiosShadowingAllowed              :1
        0,                                         // VlVesaIsSupported                 :1
        0,                                         // EscdSupportIsAvailable            :1
        1,                                         // BootFromCdIsSupported             :1
        1,                                         // SelectableBootIsSupported         :1
        0,                                         // RomBiosIsSocketed                 :1
        0,                                         // BootFromPcmciaIsSupported         :1
        1,                                         // EDDSpecificationIsSupported       :1
        1,                                         // JapaneseNecFloppyIsSupported      :1
        1,                                         // JapaneseToshibaFloppyIsSupported  :1
        1,                                         // Floppy525_360IsSupported          :1
        1,                                         // Floppy525_12IsSupported           :1
        1,                                         // Floppy35_720IsSupported           :1
        1,                                         // Floppy35_288IsSupported           :1
        0,                                         // PrintScreenIsSupported            :1
        1,                                         // Keyboard8042IsSupported           :1
        0,                                         // SerialIsSupported                 :1
        0,                                         // PrinterIsSupported                :1
        1,                                         // CgaMonoIsSupported                :1
        0,                                         // NecPc98                           :1
        0                                          // ReservedForVendor                 :32
    },

    {
        0x03,                                        //BIOSCharacteristicsExtensionBytes[0]                                       
        //  {                                          //BiosReserved
        //    1,                                         // AcpiIsSupported                   :1 
        //    1,                                         // UsbLegacyIsSupported              :1
        //    0,                                         // AgpIsSupported                    :1
        //    0,                                         // I20BootIsSupported                :1
        //    0,                                         // Ls120BootIsSupported              :1
        //    0,                                         // AtapiZipDriveBootIsSupported      :1
        //    0,                                         // Boot1394IsSupported               :1
        //    0                                          // SmartBatteryIsSupported           :1
        //  },
        0x0D                                         //BIOSCharacteristicsExtensionBytes[1]
        //  {                                          //SystemReserved
        //    1,                                         //BiosBootSpecIsSupported            :1
        //    0,                                         //FunctionKeyNetworkBootIsSupported  :1 
        //    1,                                         //TargetContentDistributionEnabled   :1
        //    1,                                         //UefiSpecificationSupported         :1      
        //    0,                                         //VirtualMachineSupported            :1        
        //    0                                          //ExtensionByte2Reserved             :3        
        //  },
    },
    0,                                        //SystemBiosMajorRelease;
    0,                                        //SystemBiosMinorRelease;
    0xFF,                                     //EmbeddedControllerFirmwareMajorRelease;
    0xFF                                      //EmbeddedControllerFirmwareMinorRelease;
};
