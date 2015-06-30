/*-----------------------------------------------------------------------*/
/*!!Warning: Huawei key information asset. No spread without permission. */
/*CODEMARK:EG4uRhTwMmgcVFBsBnYHCDadN5jJKSuVyxmmaCmKFU6eJEbB2fyHF9weu4/jer/hxLHb+S1e
E0zVg4C3NiZh4bPwYvCdjdVpqIFpLgARMXYjreBf//8/f5Q3OZWBrJs7rTqjWIcFzf/y20MJ
X2UYcn63OCaMjQwcH0cMqSsBHYJKikaCtFZCZBGqtcHyklaTPIP/i3WJRURF7nr9dAO0Znrf
cDNKlxP+H+/BpjLrzx0CzFbcyIS/XEHS7imDK1+gvhn6dO8TlwFc4DjbHqQiMw==#*/
/*--!!Warning: Deleting or modifying the preceding information is prohibited.--*/
/**@file

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  MiscNumberOfInstallableLanguagesData.c
  
Abstract: 

  This file provide OEM to define Smbios Type13 Data

**/

#include "SmbiosMisc.h"

//
// Static (possibly build generated) Bios Vendor data.
//

MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE13, MiscNumberOfInstallableLanguages) = 
{
  {                                                     // Hdr
    EFI_SMBIOS_TYPE_BIOS_LANGUAGE_INFORMATION,            // Type,
    0,                                                    // Length,
    0                                                     // Handle
  },
  0,                                                    // InstallableLanguages
  0,                                                    // Flags
  {                                                     
    0                                                   // Reserved[15]
  },
  0,                                                    // CurrentLanguageNumber
};

/* eof - MiscNumberOfInstallableLanguagesData.c */
