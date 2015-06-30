/*-----------------------------------------------------------------------*/
/*!!Warning: Huawei key information asset. No spread without permission. */
/*CODEMARK:EG4uRhTwMmgcVFBsBnYHCDadN5jJKSuVyxmmaCmKFU6eJEbB2fyHF9weu4/jer/hxLHb+S1e
E0zVg4C3NiZh4bPwYvCdjdVpqIFpLgARMXYjreBf//8/f5Q3OZWBrJs7rTqjWIcFzf/y20MJ
X2UYciBTSushWMD3N7WUrmBKnvEa+kuZHPXgGnuP0mg1LkUMPH5kcRIFxkPE8+sA6mqTKFaE
+rhDEmOsLbU1yXSZNTxedH+EY6WbXmxTnid2LKnQBt9eWaar2QhzXjTeS2Gvfw==#*/
/*--!!Warning: Deleting or modifying the preceding information is prohibited.--*/
/*++

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscIpmiDeviceInformationData.c

Abstract:

  This file provide OEM to define Smbios Type38 Data
**/

#include "SmbiosMisc.h"

MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE38, MiscIpmiDeviceInformation) = 
{
  {                                                       // Header
   EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION,    				// Type;
   0,                                                       // Length;
   0                                                        // Handle;
  },                                                       
  IPMIDeviceInfoInterfaceTypeKCS,                         // InterfaceType
  0x10,                                                   // Ipmi Specification Revision
  0,                                                      // I2CSlaveAddress
  0xFF,                                                   // NvStorageDeviceAddress  
  0,                                                      // BaseAddress  
  0,													  // BaseAddressModifier/InterruptInfo    
  0                                                       // InterruptNumber
};

