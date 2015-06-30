/*-----------------------------------------------------------------------*/
/*!!Warning: Huawei key information asset. No spread without permission. */
/*CODEMARK:EG4uRhTwMmgcVFBsBnYHCDadN5jJKSuVyxmmaCmKFU6eJEbB2fyHF9weu4/jer/hxLHb+S1e
E0zVg4C3NiZh4bPwYvCdjdVpqIFpLgARMXYjreBf//8/f5Q3OZWBrJs7rTqjWIcFzf/y20MJ
X2UYcgt6bDPtak8sllNL2WUUKA8/NOF/sAW9uTC6skGYSFCOHPg46dcKXU6xAzHraqQejcME
hETceAciAlKy0OF4M1YNXMSW2fsMAlRsGzpSkfyIDI+DzP7tVayGg/5Ko0VLEA==#*/
/*--!!Warning: Deleting or modifying the preceding information is prohibited.--*/
/** @file
 
Copyright (c) 2009 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmbiosMisc.h"

#if 0
EFI_TYPE13_BIOS_LANGUAGE_INFORMATION_STRING  SmbiosType13tringArray [] = 
{
  {"zh-CN", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_SIMPLECH_LONG),   STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_SIMPLECH_ABBREVIATE)},
  {"en-US", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_ENG_LONG),        STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_ENG_ABBREVIATE)},
  {"fr-FR", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_FRA_LONG),        STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_FRA_ABBREVIATE)},
  {"zh-TW", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_CHN_LONG),        STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_CHN_ABBREVIATE)},
  {"ja-JP", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_JPN_LONG),        STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_JPN_ABBREVIATE)},
  {"it-IT", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_ITA_LONG),        STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_ITA_ABBREVIATE)},
  {"es-ES", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_SPA_LONG),        STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_SPA_ABBREVIATE)},
  {"de-DE", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_GER_LONG),        STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_GER_ABBREVIATE)},
  {"pt-PT", STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_POR_LONG),        STRING_TOKEN (STR_MISC_BIOS_LANGUAGES_POR_ABBREVIATE)}
};

UINT8 EFI_TYPE13_BIOS_LANGUAGE_INFORMATION_ARRAY_COUNT = (sizeof(SmbiosType13tringArray) / sizeof(EFI_TYPE13_BIOS_LANGUAGE_INFORMATION_STRING));
#endif


/*++
  Check whether the language is supported for given HII handle

  @param   HiiHandle     The HII package list handle.
  @param   Offset        The offest of current lanague in the supported languages.
  @param   CurrentLang   The language code.

  @retval  TRUE          Supported.
  @retval  FALSE         Not Supported.

--*/
VOID
EFIAPI
CurrentLanguageMatch (
  IN  EFI_HII_HANDLE                   HiiHandle,
  OUT UINT16                           *Offset,
  OUT CHAR8                            *CurrentLang
  )
{
  CHAR8     *DefaultLang;
  CHAR8     *BestLanguage;
  CHAR8     *Languages;
  CHAR8     *MatchLang;
  CHAR8     *EndMatchLang;
  UINTN     CompareLength;
  
  Languages = HiiGetSupportedLanguages (HiiHandle);
  if (Languages == NULL) {
    return;
  }

  (VOID)GetEfiGlobalVariable2 (L"PlatformLang", (VOID**)&CurrentLang, NULL);
  DefaultLang  = (CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang);
  BestLanguage = GetBestLanguage (
                   Languages,
                   FALSE,
                   (CurrentLang != NULL) ? CurrentLang : "",
                   DefaultLang,
                   NULL
                   );
  if (BestLanguage != NULL) {
    //
    // Find the best matching RFC 4646 language, compute the offset.
    //
    CompareLength = AsciiStrLen (BestLanguage);
    for (MatchLang = Languages, (*Offset) = 0; *MatchLang != '\0'; (*Offset)++) {
      //
      // Seek to the end of current match language. 
      //
      for (EndMatchLang = MatchLang; *EndMatchLang != '\0' && *EndMatchLang != ';'; EndMatchLang++);
  
      if ((EndMatchLang == MatchLang + CompareLength) && AsciiStrnCmp(MatchLang, BestLanguage, CompareLength) == 0) {
        //
        // Find the current best Language in the supported languages
        //
        break;
      }
      //
      // best language match be in the supported language.
      //
      ASSERT (*EndMatchLang == ';');
      MatchLang = EndMatchLang + 1;
    }
    FreePool (BestLanguage);
  }

  FreePool (Languages);
  if (CurrentLang != NULL) {
    FreePool (CurrentLang);
  }
  return ;
}


/**
  Get next language from language code list (with separator ';').

  @param  LangCode       Input: point to first language in the list. On
                         Otput: point to next language in the list, or
                                NULL if no more language in the list.
  @param  Lang           The first language in the list.

**/
VOID
EFIAPI
GetNextLanguage (
  IN OUT CHAR8      **LangCode,
  OUT CHAR8         *Lang
  )
{
  UINTN  Index;
  CHAR8  *StringPtr;

  if(NULL == LangCode || NULL == *LangCode || NULL == Lang) {
    return;
  }

  Index     = 0;
  StringPtr = *LangCode;
  while (StringPtr[Index] != 0 && StringPtr[Index] != ';') {
    Index++;
  }

  //CopyMem (Lang, StringPtr, Index);
  (VOID)memcpy_s(Lang, Index, StringPtr, Index);
  Lang[Index] = 0;

  if (StringPtr[Index] == ';') {
    Index++;
  }
  *LangCode = StringPtr + Index;
}

/**
  This function returns the number of supported languages on HiiHandle.

  @param   HiiHandle    The HII package list handle.

  @retval  The number of supported languages.

**/
UINT16
EFIAPI
GetSupportedLanguageNumber (
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  CHAR8   *Lang;
  CHAR8   *Languages;
  CHAR8   *LanguageString;
  UINT16  LangNumber;
  
  Languages = HiiGetSupportedLanguages (HiiHandle);
  if (Languages == NULL) {
    return 0;
  }

  LangNumber = 0;
  Lang = AllocatePool (AsciiStrSize (Languages));
  if (Lang != NULL) {
    LanguageString = Languages;
    while (*LanguageString != 0) {
      GetNextLanguage (&LanguageString, Lang);
      LangNumber++;
    }
    FreePool (Lang);
  }
  FreePool (Languages);
  return LangNumber;
}


/**
  This function makes boot time changes to the contents of the
  MiscNumberOfInstallableLanguages (Type 13).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscNumberOfInstallableLanguages)
{
  UINTN                                     LangStrLen;
  CHAR8                                     CurrentLang[SMBIOS_STRING_MAX_LENGTH + 1];
  CHAR8                                     *OptionalStrStart;
  UINT16                                    Offset;
  EFI_STATUS                                Status;
  EFI_SMBIOS_HANDLE                         SmbiosHandle;
  SMBIOS_TABLE_TYPE13                       *SmbiosRecord;
  SMBIOS_TABLE_TYPE13                       *InputData = NULL;;
 
  InputData = (SMBIOS_TABLE_TYPE13 *)RecordData;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  InputData->InstallableLanguages = GetSupportedLanguageNumber (mHiiHandle);

  //
  // Try to check if current langcode matches with the langcodes in installed languages
  //
  ZeroMem(CurrentLang, SMBIOS_STRING_MAX_LENGTH + 1);
  CurrentLanguageMatch (mHiiHandle, &Offset, CurrentLang);
  LangStrLen = AsciiStrLen(CurrentLang);

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE13) + LangStrLen + 1 + 1);
  if(NULL == SmbiosRecord) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE13) + LangStrLen + 1 + 1);

  SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_BIOS_LANGUAGE_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE13);
  SmbiosRecord->Hdr.Handle = 0;  

  SmbiosRecord->InstallableLanguages = InputData->InstallableLanguages;
  SmbiosRecord->Flags                = InputData->Flags;
  SmbiosRecord->CurrentLanguages     = 1;
  OptionalStrStart                   = (CHAR8 *)(SmbiosRecord + 1);
  AsciiStrCpy(OptionalStrStart, CurrentLang);
  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  Status = LogSmbiosData((UINT8*)SmbiosRecord, &SmbiosHandle);  
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Smbios Type13 Table Log Failed! %r \n", __FUNCTION__, __LINE__, Status));        
  }
  
  FreePool(SmbiosRecord);
  return Status;
}
