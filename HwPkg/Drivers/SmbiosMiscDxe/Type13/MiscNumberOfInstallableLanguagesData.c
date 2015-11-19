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
