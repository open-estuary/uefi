/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefd2906K7BWMK86UBuLVUmsgUM/4y83F9Zab2orM6V4QjY1thvdHchX1S3M/nSL2uMr4nQ
F0kaxZ1tUsp2/3v9cKckw+ijIp2nOLMnNO2rYbbeJgX7x9R/s32UwnKMPLIikQ==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/** @file

  Copyright (c) 2009, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


Module Name:

  MiscBiosVendorData.c

Abstract:

  This driver parses the mMiscSubclassDataTable structure and reports
  any generated data to the DataHub.

--*/
#include "SmbiosMisc.h"

#include "MiscBiosVendor.h"


/**
  This function returns the value & exponent to Base2 for a given
  Hex value. This is used to calculate the BiosPhysicalDeviceSize.

  @param Value                      The hex value which is to be converted into value-exponent form
  @param Exponent                   The exponent out of the conversion

  @retval EFI_SUCCESS               All parameters were valid and *Value & *Exponent have been set.
  @retval EFI_INVALID_PARAMETER     Invalid parameter was found.

**/
EFI_STATUS
GetValueExponentBase2(
  IN OUT UINTN        *Value,
  OUT    UINTN        *Exponent
  )
{
    if ((Value == NULL) || (Exponent == NULL)) 
    {
        return EFI_INVALID_PARAMETER;
    }

    while ((*Value % 2) == 0) 
    {
        *Value=*Value/2;
        (*Exponent)++;
    }

    return EFI_SUCCESS;
}

/**
  Field Filling Function. Transform an EFI_EXP_BASE2_DATA to a byte, with '64k'
  as the unit.

  @param  Base2Data              Pointer to Base2_Data

  @retval EFI_SUCCESS            Transform successfully.
  @retval EFI_INVALID_PARAMETER  Invalid parameter was found.

**/
UINTN
Base2ToByteWith64KUnit (
  IN  UINTN  Value,
  IN  UINTN  Exponent
  )
{  
    Exponent -= 16;
    Value <<= Exponent;

    return Value;
}


/**
  This function makes boot time changes to the contents of the
  MiscBiosVendor (Type 0).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscBiosVendor)
{
    CHAR8                 *OptionalStrStart;
    UINTN                 VendorStrLen;
    UINTN                 VerStrLen;
    UINTN                 DateStrLen;
    UINTN                 BiosPhysicalSizeHexValue;
    UINTN                 BiosPhysicalSizeExponent;
    CHAR16                *Vendor;
    CHAR16                *Version;
    CHAR16                *ReleaseDate;
    EFI_STATUS            Status;
    EFI_STRING            Char16String;
    STRING_REF            TokenToUpdate;
    STRING_REF            TokenToGet;
    SMBIOS_TABLE_TYPE0    *SmbiosRecord;
    EFI_SMBIOS_HANDLE     SmbiosHandle;
    SMBIOS_TABLE_TYPE0    *InputData;

    BIOS_OEM_DATA        *pOemBiosInfo = NULL; 

    BiosPhysicalSizeHexValue = 0x0;
    BiosPhysicalSizeExponent = 0x0;
    InputData = (SMBIOS_TABLE_TYPE0 *)RecordData;

    //
    // First check for invalid parameters.
    //
    if (RecordData == NULL) 
    {
        return EFI_INVALID_PARAMETER;
    }

    //从PCD获取相关字符串，并更新相关字段
    Vendor = (CHAR16 *) PcdGetPtr (PcdFirmwareVendor);
    if (StrLen(Vendor) > 0) 
    {
        TokenToUpdate = STRING_TOKEN (STR_MISC_BIOS_VENDOR);
        HiiSetString (mHiiHandle, TokenToUpdate, Vendor, NULL);
    }

    Version = (CHAR16 *) PcdGetPtr (PcdFirmwareVersionString);
    if (StrLen (Version) > 0) 
    { 
        TokenToUpdate = STRING_TOKEN (STR_MISC_BIOS_VERSION);
        HiiSetString (mHiiHandle, TokenToUpdate, Version, NULL);
    }   

    ReleaseDate = (CHAR16 *) PcdGetPtr (PcdFirmwareReleaseDateString);
    
    pOemBiosInfo = (BIOS_OEM_DATA *)(FV_CUSTDATA_ADDR + sizeof(EFI_FIRMWARE_VOLUME_HEADER) + sizeof(NV_DATA_HEADER));
    if(pOemBiosInfo->buildTime.Year != 0xFFFF && pOemBiosInfo->buildTime.Month > 0 && pOemBiosInfo->buildTime.Month <= 0xC)
    {
        ReleaseDate = AllocateZeroPool ((sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH);
        if(NULL == ReleaseDate)
        {
            return EFI_OUT_OF_RESOURCES;
        }
        
        (VOID)UnicodeSPrintAsciiFormat( ReleaseDate, 
                              (sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH, 
                              "%02d/%02d/%4d",                               
                              pOemBiosInfo->buildTime.Month, 
                              pOemBiosInfo->buildTime.Day,
                              pOemBiosInfo->buildTime.Year
                              );  
    }
    if (StrLen(ReleaseDate) > 0) 
    {
        TokenToUpdate = STRING_TOKEN (STR_MISC_BIOS_RELEASE_DATE);
        HiiSetString (mHiiHandle, TokenToUpdate, ReleaseDate, NULL);
    }

    //
    // Now update the BiosPhysicalSize
    //
    BiosPhysicalSizeHexValue = FixedPcdGet32 (PcdFdSize);     
    Status = GetValueExponentBase2 (
             &BiosPhysicalSizeHexValue, 
             &BiosPhysicalSizeExponent
             );
    if (EFI_ERROR (Status))
    {
        //for fortify
        //return Status;
        goto Exit;
    }
    
    //获取字符串的长度
    TokenToGet = STRING_TOKEN (STR_MISC_BIOS_VENDOR);
    Char16String = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    VendorStrLen = StrLen(Char16String);
    if (VendorStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        //return EFI_UNSUPPORTED;
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_BIOS_VERSION);
    Version = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    VerStrLen = StrLen(Version);
    if (VerStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        //return EFI_UNSUPPORTED;
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }

    TokenToGet = STRING_TOKEN (STR_MISC_BIOS_RELEASE_DATE);
    ReleaseDate = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
    DateStrLen = StrLen(ReleaseDate);
    if (DateStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        //return EFI_UNSUPPORTED;
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }

    //
    // Two zeros following the last string.
    //
    SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE0) + VendorStrLen + 1 + VerStrLen + 1 + DateStrLen + 1 + 1);
    if(NULL == SmbiosRecord)
    {
        //return EFI_OUT_OF_RESOURCES;
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
    }

    SmbiosRecord->Hdr.Type = InputData->Hdr.Type;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE0);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;
    //
    // Vendor will be the 1st optional string following the formatted structure.
    //
    SmbiosRecord->Vendor = 1;  
    //
    // Version will be the 2nd optional string following the formatted structure.
    //
    SmbiosRecord->BiosVersion = 2;
    //SmbiosRecord->BiosSegment = (UINT16)InputData->BiosSegment;
    SmbiosRecord->BiosSegment = (UINT16)(FixedPcdGet32 (PcdFdBaseAddress) / 0x10000);
    //
    // ReleaseDate will be the 3rd optional string following the formatted structure.
    //
    SmbiosRecord->BiosReleaseDate = 3;
    SmbiosRecord->BiosSize = (UINT8)(Base2ToByteWith64KUnit(BiosPhysicalSizeHexValue, BiosPhysicalSizeExponent) - 1);
    SmbiosRecord->BiosCharacteristics = *(MISC_BIOS_CHARACTERISTICS*)(&InputData->BiosCharacteristics);
    //
    // CharacterExtensionBytes also store in InputData->BiosCharacteristics1 later two bytes to save size.
    //
    SmbiosRecord->BIOSCharacteristicsExtensionBytes[0] = *((UINT8 *) &InputData->BIOSCharacteristicsExtensionBytes[0]);
    SmbiosRecord->BIOSCharacteristicsExtensionBytes[1] = *((UINT8 *) &InputData->BIOSCharacteristicsExtensionBytes[1]);

    SmbiosRecord->SystemBiosMajorRelease = InputData->SystemBiosMajorRelease;
    SmbiosRecord->SystemBiosMinorRelease = InputData->SystemBiosMinorRelease;

    SmbiosRecord->EmbeddedControllerFirmwareMajorRelease = InputData->EmbeddedControllerFirmwareMajorRelease;
    SmbiosRecord->EmbeddedControllerFirmwareMinorRelease = InputData->EmbeddedControllerFirmwareMinorRelease;

    OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
    UnicodeStrToAsciiStr(Char16String, OptionalStrStart);
    UnicodeStrToAsciiStr(Version, OptionalStrStart + VendorStrLen + 1);
    UnicodeStrToAsciiStr(ReleaseDate, OptionalStrStart + VendorStrLen + 1 + VerStrLen + 1);
    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = LogSmbiosData( (UINT8*)SmbiosRecord, &SmbiosHandle);  
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Smbios Type00 Table Log Failed! %r \n", __FUNCTION__, __LINE__, Status));        
    }

    FreePool(SmbiosRecord);

Exit:
    if(ReleaseDate != NULL)
    {
        FreePool(ReleaseDate);   
    }    

    return Status;
}
