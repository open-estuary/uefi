/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefd7ChSYey4rrduWdp2yGuedHGSxMdJY1wyFfZWLCNEsol6A6UkbhuegX0W1y6Ex0AQAlL
f+hC5VtxcD5v0tVrJ3YWj0F5Wb7KUD/qY3qkXGyI394fsvD6eUjD3eZJTUfNrA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/**@file

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  SmbiosMiscDataTable.c
  
Abstract: 

  This file provide OEM to config SMBIOS Misc Type.

**/

#include "SmbiosMisc.h"

  MISC_SMBIOS_TABLE_EXTERNS(SMBIOS_TABLE_TYPE0,  MiscBiosVendor, MiscBiosVendor) 
  MISC_SMBIOS_TABLE_EXTERNS(SMBIOS_TABLE_TYPE1,  MiscSystemManufacturer, MiscSystemManufacturer) 
  MISC_SMBIOS_TABLE_EXTERNS(SMBIOS_TABLE_TYPE3,  MiscChassisManufacturer, MiscChassisManufacturer) 
  MISC_SMBIOS_TABLE_EXTERNS(SMBIOS_TABLE_TYPE2,  MiscBaseBoardManufacturer, MiscBaseBoardManufacturer)
  MISC_SMBIOS_TABLE_EXTERNS(SMBIOS_TABLE_TYPE13, MiscNumberOfInstallableLanguages, MiscNumberOfInstallableLanguages) 
  MISC_SMBIOS_TABLE_EXTERNS(SMBIOS_TABLE_TYPE32, MiscBootInformation, MiscBootInformation) 
  MISC_SMBIOS_TABLE_EXTERNS(SMBIOS_TABLE_TYPE38, MiscIpmiDeviceInformation, MiscIpmiDeviceInformation)
  
  
EFI_MISC_SMBIOS_DATA_TABLE mSmbiosMiscDataTable[] = {
  // Type0
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscBiosVendor, MiscBiosVendor),
  // Type1
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscSystemManufacturer, MiscSystemManufacturer), 
  // Type3
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscChassisManufacturer, MiscChassisManufacturer),
  // Type2
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscBaseBoardManufacturer, MiscBaseBoardManufacturer),
  // Type13
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscNumberOfInstallableLanguages, MiscNumberOfInstallableLanguages), 
  // Type32
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscBootInformation, MiscBootInformation),
  // Type38
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscIpmiDeviceInformation, MiscIpmiDeviceInformation), 
};


//
// Number of Data Table entries.
//
UINTN mSmbiosMiscDataTableEntries =
  (sizeof mSmbiosMiscDataTable) / sizeof(EFI_MISC_SMBIOS_DATA_TABLE);
