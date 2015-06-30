/*-----------------------------------------------------------------------*/
/*!!Warning: Huawei key information asset. No spread without permission. */
/*CODEMARK:EG4uRhTwMmgcVFBsBnYHCDadN5jJKSuVyxmmaCmKFU6eJEbB2fyHF9weu4/jer/hxLHb+S1e
E0zVg4C3NiZh4bPwYvCdjdVpqIFpLgARMXYjreBf//8/f5Q3OZWBrJs7rTqjWIcFzf/y20MJ
X2UYciBTSushWMD3N7WUrmBKnvG0h1Hc6UfU86Mf5RER8TavlcNiJsyQVgaQ2ocTEie/AUNI
0FmH2FvdYJ2BEzZlS3kwZW00d/R9ro/kKxWlt+Nq3NzFS6dp1fspuhiypUAYoA==#*/
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

  MiscIpmiDeviceInformationFunction.c

Abstract:

  This driver parses the mMiscSubclassDataTable structure and reports
  any generated data to smbios.

**/

#include "SmbiosMisc.h"

/**
  This function makes the attributes of IPMI to the contents of the 
  MiscChassisManufacturer structure. 

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.  

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscIpmiDeviceInformation)
{
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  SMBIOS_TABLE_TYPE38               *SmbiosRecord;
  SMBIOS_TABLE_TYPE38               *InputData = NULL;

  InputData = (SMBIOS_TABLE_TYPE38*)RecordData;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
    
  SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE38) + 1 + 1);
  if(NULL == SmbiosRecord) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE38) + 1 + 1);
  
  SmbiosRecord->Hdr.Type   = InputData->Hdr.Type;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE38);
  SmbiosRecord->Hdr.Handle = 0;  

  SmbiosRecord->InterfaceType                       = InputData->InterfaceType;
  SmbiosRecord->IPMISpecificationRevision           = InputData->IPMISpecificationRevision;
  SmbiosRecord->I2CSlaveAddress                     = InputData->I2CSlaveAddress;
  SmbiosRecord->NVStorageDeviceAddress              = InputData->NVStorageDeviceAddress;
  SmbiosRecord->BaseAddress                         = InputData->BaseAddress;
  SmbiosRecord->BaseAddressModifier_InterruptInfo   = InputData->NVStorageDeviceAddress;
  SmbiosRecord->InterruptNumber                     = InputData->InterruptNumber;

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  Status = LogSmbiosData((UINT8*)SmbiosRecord, &SmbiosHandle);  
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Smbios Type38 Table Log Failed! %r \n", __FUNCTION__, __LINE__, Status));        
  }

  FreePool(SmbiosRecord);
  return Status;
}
