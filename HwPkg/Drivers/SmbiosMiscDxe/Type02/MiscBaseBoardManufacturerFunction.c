/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefdyQGuGBwAX8cElpg1LdymAfo/aat9/4MgHtMVk5KLWjqVzMtFpeUZrmPmZ+Tq4BBFVzx
+7Ixr0GeTL1wq6N2QH1Qg3MjbYLCacw7Zoo8Hzkj3zXE776T1FeuFiiYApZPlw==*/
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

  MiscBaseBoardManufacturerFunction.c
  
Abstract: 

  This driver parses the mSmbiosMiscDataTable structure and reports
  any generated data using SMBIOS protocol.

**/
#include "SmbiosMisc.h"


/**
  This function makes basic board manufacturer to the contents of the
  Misc Base Board Manufacturer (Type 2).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.  

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscBaseBoardManufacturer)
{
    CHAR8                             *OptionalStrStart;
    UINTN                             ManuStrLen;
    UINTN                             ProductNameStrLen;
    UINTN                             VerStrLen;
    UINTN                             SerialNumStrLen;
    UINTN                             AssetTagStrLen; 
    UINTN                             ChassisLocaStrLen;
    UINTN                             HandleCount = 0;
    UINT16                            *HandleArray = NULL;  
    EFI_STRING                        BaseBoardManufacturer;
    EFI_STRING                        BaseBoardProductName;
    EFI_STRING                        Version;
    EFI_STRING                        SerialNumber;
    EFI_STRING                        AssetTag;
    EFI_STRING                        ChassisLocation;
    STRING_REF                        TokenToGet;
    EFI_SMBIOS_HANDLE                 SmbiosHandle;
    SMBIOS_TABLE_TYPE2                *SmbiosRecord;
    SMBIOS_TABLE_TYPE2                *InputData = NULL;
    EFI_STATUS                        Status;

    STRING_REF                        TokenToUpdate;
    CHAR16                            *ProductName;
    CHAR16                            *pVersion;
    
    InputData = (SMBIOS_TABLE_TYPE2*)RecordData; 

    //
    // First check for invalid parameters.
    //
    if (RecordData == NULL) 
    {
        return EFI_INVALID_PARAMETER;
    }

    ProductName = (CHAR16 *) PcdGetPtr (PcdBaseBoardProductName);
    if (StrLen(ProductName) > 0) 
    {
        TokenToUpdate = STRING_TOKEN (STR_MISC_BASE_BOARD_PRODUCT_NAME);
        HiiSetString (mHiiHandle, TokenToUpdate, ProductName, NULL);
    }

    pVersion = (CHAR16 *) PcdGetPtr (PcdBaseBoardVersion);
    if (StrLen(pVersion) > 0) 
    {
        TokenToUpdate = STRING_TOKEN (STR_MISC_BASE_BOARD_VERSION);
        HiiSetString (mHiiHandle, TokenToUpdate, pVersion, NULL);
    }

    //
    // Type 2 should depend on Type 3
    //
    TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_MANUFACTURER);
    BaseBoardManufacturer = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    ManuStrLen = StrLen(BaseBoardManufacturer);
    if (ManuStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_PRODUCT_NAME);
    BaseBoardProductName = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    ProductNameStrLen = StrLen(BaseBoardProductName);
    if (ProductNameStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }  

    TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_VERSION);
    Version = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    VerStrLen = StrLen(Version);
    if (VerStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_SERIAL_NUMBER);
    SerialNumber = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    SerialNumStrLen = StrLen(SerialNumber);
    if (SerialNumStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_ASSET_TAG);
    AssetTag = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    AssetTagStrLen = StrLen(AssetTag);
    if (AssetTagStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_CHASSIS_LOCATION);
    ChassisLocation = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    ChassisLocaStrLen = StrLen(ChassisLocation);
    if (ChassisLocaStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }
    //
    // Two zeros following the last string.
    //
    SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE2) + ManuStrLen        + 1 
                                                                + ProductNameStrLen + 1 
                                                                + VerStrLen         + 1 
                                                                + SerialNumStrLen   + 1 
                                                                + AssetTagStrLen    + 1 
                                                                + ChassisLocaStrLen + 1 + 1);
    if (NULL == SmbiosRecord)
    {
        return EFI_OUT_OF_RESOURCES;
    }


    ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE2)  + ManuStrLen        + 1 
                                                       + ProductNameStrLen + 1 
                                                       + VerStrLen         + 1 
                                                       + SerialNumStrLen   + 1 
                                                       + AssetTagStrLen    + 1 
                                                       + ChassisLocaStrLen + 1 + 1);

    SmbiosRecord->Hdr.Type          = InputData->Hdr.Type;
    SmbiosRecord->Hdr.Length        = sizeof (SMBIOS_TABLE_TYPE2);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;
    //
    // Manu will be the 1st optional string following the formatted structure.
    //
    SmbiosRecord->Manufacturer = 1;
    //
    // ProductName will be the 2st optional string following the formatted structure.
    //
    SmbiosRecord->ProductName  = 2;
    //
    // Version will be the 3rd optional string following the formatted structure.
    //
    SmbiosRecord->Version = 3;
    //
    // SerialNumber will be the 4th optional string following the formatted structure.
    //
    SmbiosRecord->SerialNumber = 4;
    //
    // AssertTag will be the 5th optional string following the formatted structure.
    //
    SmbiosRecord->AssetTag = 5;

    SmbiosRecord->FeatureFlag       = (*(BASE_BOARD_FEATURE_FLAGS*)&(InputData->FeatureFlag));
    //
    // LocationInChassis will be the 6th optional string following the formatted structure.
    //
    SmbiosRecord->LocationInChassis = 6;
    SmbiosRecord->BoardType         = (UINT8)InputData->BoardType;

    //
    //  Update Contained objects Handle
    //
    SmbiosRecord->NumberOfContainedObjectHandles = 0;

    GetLinkTypeHandle(EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE, &HandleArray, &HandleCount);

    if(HandleCount)
    {
        SmbiosRecord->ChassisHandle = HandleArray[0];
    }

    FreePool(HandleArray); 

    OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
    UnicodeStrToAsciiStr(BaseBoardManufacturer, OptionalStrStart);
    UnicodeStrToAsciiStr(BaseBoardProductName,  OptionalStrStart + ManuStrLen + 1);
    UnicodeStrToAsciiStr(Version,               OptionalStrStart + ManuStrLen + 1 + ProductNameStrLen + 1);
    UnicodeStrToAsciiStr(SerialNumber,          OptionalStrStart + ManuStrLen + 1 + ProductNameStrLen + 1 + VerStrLen + 1);
    UnicodeStrToAsciiStr(AssetTag,              OptionalStrStart + ManuStrLen + 1 + ProductNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 );
    UnicodeStrToAsciiStr(ChassisLocation,       OptionalStrStart + ManuStrLen + 1 + ProductNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + AssetTagStrLen + 1);

    Status = LogSmbiosData( (UINT8*)SmbiosRecord, &SmbiosHandle);  
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Smbios Type02 Table Log Failed! %r \n", __FUNCTION__, __LINE__, Status));        
    }

    FreePool(SmbiosRecord);

    return Status;
}
