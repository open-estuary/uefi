
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

