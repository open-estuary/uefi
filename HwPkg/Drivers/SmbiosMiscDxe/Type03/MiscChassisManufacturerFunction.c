/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefdzZgLdg+3AYHszahhxsyFFtBKsE+sUVO27rnVWOd9AvWHk+D8ObkTf58lLX5SPWpDkP5
C6DJnCLFD4KLfEjcBTssFTxw0GBXbkqzpdZowKg06k5TAtk38CFMXU5KBkP3Rg==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/** @file

Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscChassisManufacturerFunction.c

Abstract:

  This driver parses the mMiscSubclassDataTable structure and reports
  any generated data to smbios.

**/

#include "SmbiosMisc.h"

/**
  This function makes boot time changes to the contents of the
  MiscChassisManufacturer (Type 3).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.  

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscChassisManufacturer)
{
    CHAR8                           *OptionalStrStart;
    UINTN                           ManuStrLen;
    UINTN                           VerStrLen;
    UINTN                           AssertTagStrLen;
    UINTN                           SerialNumStrLen;
    UINTN                           ChaNumStrLen;
    EFI_STRING                      Manufacturer;
    EFI_STRING                      Version;
    EFI_STRING                      SerialNumber;
    EFI_STRING                      AssertTag;
    EFI_STRING                      ChassisSkuNumber;
    STRING_REF                      TokenToGet;
    EFI_SMBIOS_HANDLE               SmbiosHandle;
    SMBIOS_TABLE_TYPE3              *SmbiosRecord;
    SMBIOS_TABLE_TYPE3              *InputData;
    EFI_STATUS                      Status; 

    InputData = (SMBIOS_TABLE_TYPE3 *)RecordData;

    //
    // First check for invalid parameters.
    //
    if (RecordData == NULL) 
    {
        return EFI_INVALID_PARAMETER;
    }  

    //获取相关字符串的长度
    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_MANUFACTURER);
    Manufacturer = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    ManuStrLen = StrLen(Manufacturer);
    if (ManuStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_VERSION);
    Version = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    VerStrLen = StrLen(Version);
    if (VerStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_SERIAL_NUMBER);
    SerialNumber = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    SerialNumStrLen = StrLen(SerialNumber);
    if (SerialNumStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_ASSET_TAG);
    AssertTag = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    AssertTagStrLen = StrLen(AssertTag);
    if (AssertTagStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_CHASSIS_SKU_NUMBER);
    ChassisSkuNumber = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    ChaNumStrLen = StrLen(ChassisSkuNumber);
    if (ChaNumStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    //
    // Two zeros following the last string.
    //
    SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE3) + ManuStrLen      + 1  
                                                            + VerStrLen       + 1 
                                                            + SerialNumStrLen + 1 
                                                            + AssertTagStrLen + 1 
                                                            + ChaNumStrLen    + 1 + 1);
    if (NULL == SmbiosRecord)
    {
        return EFI_OUT_OF_RESOURCES;
    }

    ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE3) + ManuStrLen      + 1  
                                                      + VerStrLen       + 1
                                                      + SerialNumStrLen + 1 
                                                      + AssertTagStrLen + 1 
                                                      + ChaNumStrLen    + 1 + 1);

    SmbiosRecord->Hdr.Type   = InputData->Hdr.Type;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE3);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;  
    //
    // Manu will be the 1st optional string following the formatted structure.
    //
    SmbiosRecord->Manufacturer = 1;  
    SmbiosRecord->Type         = (UINT8)InputData->Type;
    //
    // Version will be the 2nd optional string following the formatted structure.
    //
    SmbiosRecord->Version      = 2;  
    //
    // SerialNumber will be the 3rd optional string following the formatted structure.
    //
    SmbiosRecord->SerialNumber = 3;  
    //
    // AssertTag will be the 4th optional string following the formatted structure.
    //
    SmbiosRecord->AssetTag     = 4;  

    SmbiosRecord->BootupState                  = (UINT8)InputData->BootupState;
    SmbiosRecord->PowerSupplyState             = (UINT8)InputData->PowerSupplyState;
    SmbiosRecord->ThermalState                 = (UINT8)InputData->ThermalState;
    SmbiosRecord->SecurityStatus               = (UINT8)InputData->SecurityStatus;
    //CopyMem (SmbiosRecord->OemDefined, (UINT8*)InputData->OemDefined, 4);
    (VOID)memcpy_s (SmbiosRecord->OemDefined, 4, (UINT8*)InputData->OemDefined, 4);
    SmbiosRecord->Height                       = (UINT8)InputData->Height;
    SmbiosRecord->NumberofPowerCords           = (UINT8)InputData->NumberofPowerCords;
    SmbiosRecord->ContainedElementCount        = (UINT8)InputData->ContainedElementCount;
    SmbiosRecord->ContainedElementRecordLength = (UINT8)InputData->ContainedElementRecordLength;
    //CopyMem (SmbiosRecord->ContainedElements, (UINT8*)InputData->ContainedElements, sizeof(CONTAINED_ELEMENT));
    (VOID)memcpy_s (SmbiosRecord->ContainedElements, sizeof(CONTAINED_ELEMENT), (UINT8*)InputData->ContainedElements, sizeof(CONTAINED_ELEMENT));
    //
    // ChassisSkuNumber will be the 5th optional string following the formatted structure.
    //
    SmbiosRecord->ChassisSkuNumber = 5;

    OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
    UnicodeStrToAsciiStr(Manufacturer,     OptionalStrStart);
    UnicodeStrToAsciiStr(Version,          OptionalStrStart + ManuStrLen + 1);
    UnicodeStrToAsciiStr(SerialNumber,     OptionalStrStart + ManuStrLen + 1 + VerStrLen + 1);
    UnicodeStrToAsciiStr(AssertTag,        OptionalStrStart + ManuStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1);
    UnicodeStrToAsciiStr(ChassisSkuNumber, OptionalStrStart + ManuStrLen + 1 + VerStrLen + 1 + SerialNumStrLen +1 + AssertTagStrLen + 1);
    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = LogSmbiosData( (UINT8*)SmbiosRecord, &SmbiosHandle);  
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Smbios Type03 Table Log Failed! %r \n", __FUNCTION__, __LINE__, Status));        
    }

    FreePool(SmbiosRecord);

    return Status;
}
