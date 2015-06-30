/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefd5O3CKzZLiXtIrTLAeeBo9hiUBDFRtWLfmmDPz3NygbZMNYFwguXWufTDQfwe/5xekn/
MpfbcbvR7H7enWOwlsFuKL6B2OX2ZJUIM8AQ0Nd9DEIKQ+yN2O0UJFzCHsSUAw==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*++

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscSystemManufacturerFunction.c

Abstract:

  This driver parses the mMiscSubclassDataTable structure and reports
  any generated data to smbios.

**/
#include "SmbiosMisc.h"


/**
  This function makes boot time changes to the contents of the
  MiscSystemManufacturer (Type 1).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.  

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscSystemManufacturer)
{
    CHAR8                           *OptionalStrStart;
    UINTN                           ManuStrLen;
    UINTN                           VerStrLen;
    UINTN                           PdNameStrLen;
    UINTN                           SerialNumStrLen;
    UINTN                           SKUNumStrLen;
    UINTN                           FamilyStrLen;
    EFI_STRING                      Manufacturer;
    EFI_STRING                      ProductName;
    EFI_STRING                      Version;
    EFI_STRING                      SerialNumber;
    EFI_STRING                      SKUNumber;
    EFI_STRING                      Family;
    STRING_REF                      TokenToGet;
    EFI_SMBIOS_HANDLE               SmbiosHandle;
    SMBIOS_TABLE_TYPE1              *SmbiosRecord;
    SMBIOS_TABLE_TYPE1              *InputData;
    EFI_STATUS                      Status; 
    STRING_REF                      TokenToUpdate;
    CHAR16                          *Product;
    CHAR16                          *pVersion;
    
    InputData = (SMBIOS_TABLE_TYPE1 *)RecordData;

    //
    // First check for invalid parameters.
    //
    if (RecordData == NULL) 
    {
        return EFI_INVALID_PARAMETER;
    }  

    Product = (CHAR16 *) PcdGetPtr (PcdSystemProductName);
    if (StrLen(Product) > 0) 
    {
        TokenToUpdate = STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME);
        HiiSetString (mHiiHandle, TokenToUpdate, Product, NULL);
    }

    pVersion = (CHAR16 *) PcdGetPtr (PcdSystemVersion);
    if (StrLen(pVersion) > 0) 
    {
        TokenToUpdate = STRING_TOKEN (STR_MISC_SYSTEM_VERSION);
        HiiSetString (mHiiHandle, TokenToUpdate, pVersion, NULL);
    }
    
    //获取相关字符串的长度
    TokenToGet   = STRING_TOKEN (STR_MISC_SYSTEM_MANUFACTURER);
    Manufacturer = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    ManuStrLen   = StrLen(Manufacturer);
    if (ManuStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet   = STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME);
    ProductName  = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    PdNameStrLen = StrLen(ProductName);
    if (PdNameStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION);
    Version    = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    VerStrLen  = StrLen(Version);
    if (VerStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet      = STRING_TOKEN (STR_MISC_SYSTEM_SERIAL_NUMBER);
    SerialNumber    = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    SerialNumStrLen = StrLen(SerialNumber);
    if (SerialNumStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet   = STRING_TOKEN (STR_MISC_SYSTEM_SKU_NUMBER);
    SKUNumber    = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    SKUNumStrLen = StrLen(SKUNumber);
    if (SKUNumStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    TokenToGet   = STRING_TOKEN (STR_MISC_SYSTEM_FAMILY);
    Family       = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    FamilyStrLen = StrLen(Family);
    if (FamilyStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        return EFI_UNSUPPORTED;
    }

    //
    // Two zeros following the last string.
    //
    SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE1) + ManuStrLen      + 1 
                                                            + PdNameStrLen    + 1 
                                                            + VerStrLen       + 1 
                                                            + SerialNumStrLen + 1 
                                                            + SKUNumStrLen    + 1 
                                                            + FamilyStrLen    + 1 + 1);

    if (NULL == SmbiosRecord)
    {
        return EFI_OUT_OF_RESOURCES;
    }
    ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE1) + ManuStrLen      + 1 
                                                      + PdNameStrLen    + 1 
                                                      + VerStrLen       + 1 
                                                      + SerialNumStrLen + 1 
                                                      + SKUNumStrLen    + 1 
                                                      + FamilyStrLen    + 1 + 1);

    SmbiosRecord->Hdr.Type   = InputData->Hdr.Type;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE1);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;
    //
    // Manu will be the 1st optional string following the formatted structure.
    //
    SmbiosRecord->Manufacturer = 1;
    //
    // ProductName will be the 2nd optional string following the formatted structure.
    //
    SmbiosRecord->ProductName = 2;
    //
    // Version will be the 3rd optional string following the formatted structure.
    //
    SmbiosRecord->Version = 3;
    //
    // Version will be the 4th optional string following the formatted structure.
    //
    SmbiosRecord->SerialNumber = 4;

    //CopyMem ((UINT8 *) (&SmbiosRecord->Uuid),&InputData->Uuid, sizeof(EFI_GUID));
    (VOID)memcpy_s((UINT8 *) (&SmbiosRecord->Uuid), sizeof(EFI_GUID), &InputData->Uuid, sizeof(EFI_GUID));
    SmbiosRecord->WakeUpType   = (UINT8)InputData->WakeUpType;  
    //
    // SKUNumber will be the 5th optional string following the formatted structure.
    //
    SmbiosRecord->SKUNumber    = 5;
    //
    // Family will be the 6th optional string following the formatted structure.
    //
    SmbiosRecord->Family       = 6;

    OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
    UnicodeStrToAsciiStr(Manufacturer, OptionalStrStart);
    UnicodeStrToAsciiStr(ProductName,  OptionalStrStart + ManuStrLen + 1);
    UnicodeStrToAsciiStr(Version,      OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1);
    UnicodeStrToAsciiStr(SerialNumber, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1);
    UnicodeStrToAsciiStr(SKUNumber,    OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1);
    UnicodeStrToAsciiStr(Family,       OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + SKUNumStrLen + 1);

    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = LogSmbiosData( (UINT8*)SmbiosRecord, &SmbiosHandle);  
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Smbios Type01 Table Log Failed! %r \n", __FUNCTION__, __LINE__, Status));        
    }
    
    FreePool(SmbiosRecord);
    
    return Status;
}
