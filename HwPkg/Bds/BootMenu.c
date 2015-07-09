/** @file
*
*  Copyright (c) 2011 - 2015, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include "BdsInternal.h"

#include <Guid/ArmGlobalVariableHob.h>
    

#ifdef SECURE_BOOT
#include <Protocol/FirmwareVolume2.h>
#include <Library/BaseCryptLib.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/ImageAuthentication.h>

STATIC UINT16 *FILENAME_TEST_DB_SET = L"DBRoot.cer";
//STATIC UINT16 *FILENAME_TEST_DB_MFST_SET = L"db_MSFTproductionWindowsSigningCA2011.cer";
STATIC UINT16 *FILENAME_TEST_DB_RSA_SET = L"Rsa2048DB.dat";

STATIC UINT16 *FILENAME_TEST_KEK_SET = L"KekRoot.cer";
STATIC UINT16 *FILENAME_TEST_KEK_MFST_SET = L"KEK_MSFTproductionKekCA.cer";

STATIC UINT16 *FILENAME_TEST_PK_SET = L"PkRoot.cer";
#endif

extern BDS_LOAD_OPTION_SUPPORT *BdsLoadOptionSupportList;

/**
  Worker function that displays the list of boot options that is passed in.

  The function loops over the entries of the list of boot options that is passed
  in. For each entry, the boot option description is displayed on a single line
  along with the position of the option in the list. In debug mode, the UEFI
  device path and the arguments of the boot option are displayed as well in
  subsequent lines.

  @param[in]  BootOptionsList  List of the boot options

**/
//STATIC
VOID
DisplayBootOptions (
  IN  LIST_ENTRY*   BootOptionsList
  )
{
  EFI_STATUS        Status;
  UINTN             BootOptionCount;
  LIST_ENTRY       *Entry;
  BDS_LOAD_OPTION  *BdsLoadOption;
  BOOLEAN           IsUnicode;

  BootOptionCount = 0 ;
  for (Entry = GetFirstNode (BootOptionsList);
       !IsNull (BootOptionsList, Entry);
       Entry = GetNextNode (BootOptionsList, Entry)
      ) {

    BdsLoadOption = LOAD_OPTION_FROM_LINK (Entry);
    Print (L"[%d] %s\n", ++BootOptionCount, BdsLoadOption->Description);

    DEBUG_CODE_BEGIN ();
      CHAR16*                           DevicePathTxt;
      EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* DevicePathToTextProtocol;
      ARM_BDS_LOADER_TYPE               LoaderType;
      ARM_BDS_LOADER_OPTIONAL_DATA*     OptionalData;

      Status = gBS->LocateProtocol (
                     &gEfiDevicePathToTextProtocolGuid,
                     NULL,
                     (VOID **)&DevicePathToTextProtocol
                     );
      ASSERT_EFI_ERROR (Status);
      DevicePathTxt = DevicePathToTextProtocol->ConvertDevicePathToText (
                                                  BdsLoadOption->FilePathList,
                                                  TRUE,
                                                  TRUE
                                                  );
      Print (L"\t- %s\n", DevicePathTxt);

      OptionalData = BdsLoadOption->OptionalData;
      if (IS_ARM_BDS_BOOTENTRY (BdsLoadOption)) {
        LoaderType = (ARM_BDS_LOADER_TYPE)ReadUnaligned32 ((CONST UINT32*)&OptionalData->Header.LoaderType);
        if ((LoaderType == BDS_LOADER_KERNEL_LINUX_ATAG) ||
            (LoaderType == BDS_LOADER_KERNEL_LINUX_FDT )   ) {
          Print (L"\t- Arguments: %a\n", &OptionalData->Arguments.LinuxArguments + 1);
        }
      } else if (OptionalData != NULL) {
        if (IsPrintableString (OptionalData, &IsUnicode)) {
          if (IsUnicode) {
            Print (L"\t- Arguments: %s\n", OptionalData);
          } else {
            AsciiPrint ("\t- Arguments: %a\n", OptionalData);
          }
        }
      }

      FreePool (DevicePathTxt);
    DEBUG_CODE_END ();
  }
}

/**
  Worker function that asks for a boot option to be selected and returns a
  pointer to the structure describing the selected boot option.

  @param[in]  BootOptionsList  List of the boot options

  @retval     EFI_SUCCESS      Selection succeeded
  @retval     !EFI_SUCCESS     Input error or input cancelled

**/
STATIC
EFI_STATUS
SelectBootOption (
  IN  LIST_ENTRY*               BootOptionsList,
  IN  CONST CHAR16*             InputStatement,
  OUT BDS_LOAD_OPTION_ENTRY**   BdsLoadOptionEntry
  )
{
  EFI_STATUS                    Status;
  UINTN                         BootOptionCount;
  UINT16                       *BootOrder;
  LIST_ENTRY*                   Entry;
  UINTN                         BootOptionSelected;
  UINTN                         Index;

  // Get the number of boot options
  Status = GetGlobalEnvironmentVariable (
            L"BootOrder", NULL, &BootOptionCount, (VOID**)&BootOrder
            );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  FreePool (BootOrder);
  BootOptionCount /= sizeof (UINT16);

  // Check if a valid boot option(s) is found
  if (BootOptionCount == 0) {
    if (StrCmp (InputStatement, DELETE_BOOT_ENTRY) == 0) {
      Print (L"Nothing to remove!\n");
    } else if (StrCmp (InputStatement, UPDATE_BOOT_ENTRY) == 0) {
      Print (L"Nothing to update!\n");
    } else if (StrCmp (InputStatement, MOVE_BOOT_ENTRY) == 0) {
      Print (L"Nothing to move!\n");
    } else {
      Print (L"No supported Boot Entry.\n");
    }
    return EFI_NOT_FOUND;
  }

  // Get the index of the boot device to delete
  BootOptionSelected = 0;
  while (BootOptionSelected == 0) {
    Print (InputStatement);
    Status = GetHIInputInteger (&BootOptionSelected);
    if (EFI_ERROR (Status)) {
      Print (L"\n");
      goto ErrorExit;
    } else if ((BootOptionSelected == 0) || (BootOptionSelected > BootOptionCount)) {
      Print (L"Invalid input (max %d)\n", BootOptionCount);
      BootOptionSelected = 0;
    }
  }

  // Get the structure of the Boot device to delete
  Index = 1;
  for (Entry = GetFirstNode (BootOptionsList);
       !IsNull (BootOptionsList, Entry);
       Entry = GetNextNode (BootOptionsList,Entry)
       )
  {
    if (Index == BootOptionSelected) {
      *BdsLoadOptionEntry = LOAD_OPTION_ENTRY_FROM_LINK (Entry);
      break;
    }
    Index++;
  }

ErrorExit:
  return Status;
}

STATIC
EFI_STATUS
SelectBootDevice (
  OUT BDS_SUPPORTED_DEVICE** SupportedBootDevice
  )
{
  EFI_STATUS  Status;
  LIST_ENTRY  SupportedDeviceList;
  UINTN       SupportedDeviceCount;
  LIST_ENTRY* Entry;
  UINTN       SupportedDeviceSelected;
  UINTN       Index;

  //
  // List the Boot Devices supported
  //

  // Start all the drivers first
  BdsConnectAllDrivers ();

  // List the supported devices
  Status = BootDeviceListSupportedInit (&SupportedDeviceList);
  ASSERT_EFI_ERROR(Status);

  SupportedDeviceCount = 0;
  for (Entry = GetFirstNode (&SupportedDeviceList);
       !IsNull (&SupportedDeviceList,Entry);
       Entry = GetNextNode (&SupportedDeviceList,Entry)
       )
  {
    *SupportedBootDevice = SUPPORTED_BOOT_DEVICE_FROM_LINK(Entry);
    Print(L"[%d] %s\n",SupportedDeviceCount+1,(*SupportedBootDevice)->Description);

    DEBUG_CODE_BEGIN();
      CHAR16*                           DevicePathTxt;
      EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* DevicePathToTextProtocol;

      Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **)&DevicePathToTextProtocol);
      ASSERT_EFI_ERROR(Status);
      DevicePathTxt = DevicePathToTextProtocol->ConvertDevicePathToText ((*SupportedBootDevice)->DevicePathProtocol,TRUE,TRUE);

      Print(L"\t- %s\n",DevicePathTxt);

      FreePool(DevicePathTxt);
    DEBUG_CODE_END();

    SupportedDeviceCount++;
  }

  if (SupportedDeviceCount == 0) {
    Print(L"There is no supported device.\n");
    Status = EFI_ABORTED;
    goto EXIT;
  }

  //
  // Select the Boot Device
  //
  SupportedDeviceSelected = 0;
  while (SupportedDeviceSelected == 0) {
    Print(L"Select the Boot Device: ");
    Status = GetHIInputInteger (&SupportedDeviceSelected);
    if (EFI_ERROR(Status)) {
      Status = EFI_ABORTED;
      goto EXIT;
    } else if ((SupportedDeviceSelected == 0) || (SupportedDeviceSelected > SupportedDeviceCount)) {
      Print(L"Invalid input (max %d)\n",SupportedDeviceCount);
      SupportedDeviceSelected = 0;
    }
  }

  //
  // Get the Device Path for the selected boot device
  //
  Index = 1;
  for (Entry = GetFirstNode (&SupportedDeviceList);
       !IsNull (&SupportedDeviceList,Entry);
       Entry = GetNextNode (&SupportedDeviceList,Entry)
       )
  {
    if (Index == SupportedDeviceSelected) {
      *SupportedBootDevice = SUPPORTED_BOOT_DEVICE_FROM_LINK(Entry);
      break;
    }
    Index++;
  }

EXIT:
  BootDeviceListSupportedFree (&SupportedDeviceList, *SupportedBootDevice);
  return Status;
}

EFI_STATUS IsFoundTheSameBootOption(UINT16 BootOptionId, BOOLEAN *IsFound)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  VOID*         Value = NULL;
  UINTN         Size = 0;
  CHAR16        BootOptionName[9];
  
  UnicodeSPrint (BootOptionName, 9 * sizeof(CHAR16), L"Boot%04X", BootOptionId);       
  Status = gRT->GetVariable (BootOptionName, &gEfiGlobalVariableGuid, NULL, &Size, Value);
  if (Status == EFI_NOT_FOUND) {
    //AsciiPrint("Variable name '%s' not found.\n",BootOptionName);
  } else if (Status == EFI_BUFFER_TOO_SMALL) {
    // Get the environment variable value
    Value = AllocatePool (Size);
    if (Value == NULL) {
      AsciiPrint("%a(%d) AllocatePool Error: '%r'\n",__FUNCTION__,__LINE__,Status);        
      return EFI_OUT_OF_RESOURCES;
    }

    Status = gRT->GetVariable ((CHAR16 *)BootOptionName, &gEfiGlobalVariableGuid, NULL, &Size, Value);
    if (EFI_ERROR (Status)) {
      AsciiPrint("%a(%d) Error: '%r'\n",__FUNCTION__,__LINE__,Status);
    } else {
      //AsciiPrint("%a(%d) %a=%a\n",__FUNCTION__,__LINE__,BootOptionName,Value);
      *IsFound = TRUE;
    }
    Status = EFI_SUCCESS;
    FreePool(Value);
  } else {
    AsciiPrint("%a(%d) Error: '%r'\n",__FUNCTION__,__LINE__,Status);
  }

  return Status;
}

EFI_STATUS DeleteNoUseOldBootOption(LIST_ENTRY *BdsBootOptionsList, UINT8 OldBootOptionCnt, UINT8 NewBootOptionCnt)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINTN                   Index;
    LIST_ENTRY*             BootEntry; 
    BDS_LOAD_OPTION_ENTRY*  BootOptionEntry = NULL;    
    
    if(OldBootOptionCnt > NewBootOptionCnt)
    {
      Index = 1; 
      for (BootEntry = GetFirstNode (BdsBootOptionsList);
           !IsNull (BdsBootOptionsList, BootEntry);
           BootEntry = GetNextNode (BdsBootOptionsList,BootEntry)
           )
      {                     
          if (Index > NewBootOptionCnt) {
            BootOptionEntry = LOAD_OPTION_ENTRY_FROM_LINK (BootEntry);
            Print(L"BootOptionList get the entry, the entry is %d! so delete it!\n",Index);
            //break;

             // Delete the BDS Load option structures
            Status = BootOptionDelete (BootOptionEntry->BdsLoadOption);
            if (EFI_ERROR(Status)) {
                Print(L"BootOptionDelete fail!\n");
                Status = EFI_ABORTED;
            }
          }
          Index++;
      }

   }
  return Status;
}

EFI_STATUS ReOrderBootList (VOID)
{
  EFI_STATUS                    Status;
  UINT16*                       BootOrder;
  UINTN                         BootOrderSize;
  UINT8 i,j;
  UINT16 temp;

  // Get the Boot Option Order from the environment variable
  Status = GetGlobalEnvironmentVariable (L"BootOrder", NULL, &BootOrderSize, (VOID**)&BootOrder);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  for(i=0; i<BootOrderSize / sizeof (UINT16); i++)
  {
      for(j=0; j<BootOrderSize / sizeof (UINT16) -1; j++)
      {
          if(BootOrder[j] > BootOrder[j+1])
          {
              temp = BootOrder[j];
              BootOrder[j] = BootOrder[j+1];
              BootOrder[j+1] = temp;
          }
      }
  }
  Status = gRT->SetVariable (
      L"BootOrder",
      &gEfiGlobalVariableGuid,
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
      BootOrderSize,
      BootOrder
      );
  if (EFI_ERROR(Status))
  {
      DEBUG((EFI_D_ERROR, "%a(%d):SetVariable failed!\n", __FUNCTION__,__LINE__));
  } 

  FreePool (BootOrder);
  return EFI_SUCCESS;
}



EFI_STATUS
ListAllSupportedBootDevice (VOID)
{
  EFI_STATUS                Status = EFI_SUCCESS;
  LIST_ENTRY                SupportedDeviceList;
  UINTN                     SupportedDeviceCount;
  LIST_ENTRY*               Entry;
  LIST_ENTRY*               OldEntry;  
  BDS_SUPPORTED_DEVICE*     SupportedBootDevice = NULL;
  UINT32                    Attributes = 0;
  ARM_BDS_LOADER_TYPE       BootType;
  BDS_LOAD_OPTION_ENTRY     *BdsLoadOptionEntry = NULL;
  BDS_LOAD_OPTION_ENTRY     *OldBdsLoadOptionEntry = NULL;  
  EFI_DEVICE_PATH           *DevicePath = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathNodes = NULL;
  UINT8*                    OptionalData = NULL;
  UINTN                     OptionalDataSize = 0;
  LIST_ENTRY                BdsBootOptionsList;
  CHAR16                    BootOptionName[9];
  BDS_LOAD_OPTION           *BdsLoadOptionOld;
  CHAR16*                   DevicePathTxtNew = NULL;
  CHAR16*                   DevicePathTxtOld = NULL;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* DevicePathToTextProtocol;
  UINT8                     OldBootOptionNum = 0;
  UINTN                     Index = 0;
  BOOLEAN                   IsFound = FALSE;
  
  BootOptionOrderList (&BdsBootOptionsList);
  for (Entry = GetFirstNode (&BdsBootOptionsList);
     !IsNull (&BdsBootOptionsList, Entry);
     Entry = GetNextNode (&BdsBootOptionsList,Entry)
     )
  {
    OldBootOptionNum++;
  }     
  // Start all the drivers first
  BdsConnectAllDrivers ();  
  // Initialize list of supported devices
  InitializeListHead (&SupportedDeviceList);    
  // List the supported BDS_DEVICE_FILESYSTEM && BDS_DEVICE_PXE devices
  Status =  BootDeviceTypeListSupportedInit (&SupportedDeviceList,BDS_DEVICE_FILESYSTEM); 
  if (EFI_ERROR(Status)) {
        Print(L" SATA BOOT DEVICE LIST fail!\n");
    }
  Status = BootDeviceTypeListSupportedInit (&SupportedDeviceList,BDS_DEVICE_PXE);
  if (EFI_ERROR(Status)) {
      Print(L" PXE BOOT DEVICE LIST fail!\n");
  }    
  // List the supported BDS_DEVICE_FILESYSTEM && BDS_DEVICE_PXE devices
  Print(L"List All Supported Boot Devices:\n");
  SupportedDeviceCount = 0;
  for (Entry = GetFirstNode (&SupportedDeviceList);
       !IsNull (&SupportedDeviceList,Entry);
       Entry = GetNextNode (&SupportedDeviceList,Entry)
       )
  {
    SupportedBootDevice = SUPPORTED_BOOT_DEVICE_FROM_LINK(Entry);
    DevicePath = SupportedBootDevice->DevicePathProtocol;
    BootType = BDS_LOADER_EFI_APPLICATION;
    if((0x1 == SupportedBootDevice->DevicePathProtocol->Type) &&  (0x1 == SupportedBootDevice->DevicePathProtocol->SubType))
    {
        // Create the specific device path node  
        Status = BdsLoadOptionFileSystemCreatePcdDevicePath(&DevicePathNodes); 
        if (EFI_ERROR(Status)) {
          Status = EFI_ABORTED;
          goto EXIT;
        }
  
        // Append the Device Path to the selected device path
        DevicePath = AppendDevicePath (SupportedBootDevice->DevicePathProtocol, (CONST EFI_DEVICE_PATH_PROTOCOL *)DevicePathNodes);
        if (DevicePath == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto EXIT;
        }
  
        OptionalData = NULL;
        OptionalDataSize = 0;
    }

    Print(L"[%d] %s\n",SupportedDeviceCount+1,SupportedBootDevice->Description);
    DEBUG_CODE_BEGIN();
      CHAR16*                           DevicePathTxt;
      Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **)&DevicePathToTextProtocol);
      ASSERT_EFI_ERROR(Status);
      DevicePathTxt = DevicePathToTextProtocol->ConvertDevicePathToText (SupportedBootDevice->DevicePathProtocol,TRUE,TRUE);
      Print(L"\t- %s\n",DevicePathTxt);
      FreePool(DevicePathTxt);
    DEBUG_CODE_END();
     
    // Create new entry
    BdsLoadOptionEntry = (BDS_LOAD_OPTION_ENTRY*)AllocatePool (sizeof(BDS_LOAD_OPTION_ENTRY));
    if (NULL == BdsLoadOptionEntry) {
        Print(L"Boot Option Allocate fail!\n");
        Status = EFI_OUT_OF_RESOURCES;
        goto EXIT;
    }
    IsFound = FALSE;
    IsFoundTheSameBootOption(SupportedDeviceCount,&IsFound);   
    if(IsFound) {
        UnicodeSPrint (BootOptionName, 9 * sizeof(CHAR16), L"Boot%04X", (UINT16)SupportedDeviceCount);
        BootOptionFromLoadOptionVariable (BootOptionName, &BdsLoadOptionOld);
//        Print(L"Found the same BootOption! \n");
        Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **)&DevicePathToTextProtocol);
        ASSERT_EFI_ERROR(Status);
        DevicePathTxtNew = DevicePathToTextProtocol->ConvertDevicePathToText (DevicePath,TRUE,TRUE);
        DevicePathTxtOld = DevicePathToTextProtocol->ConvertDevicePathToText (BdsLoadOptionOld->FilePathList,TRUE,TRUE);
                     
        if((StrnCmp (DevicePathTxtNew, DevicePathTxtOld, StrLen(DevicePathTxtOld)) != 0)) 
        {
            Print(L"DevicePath is not the same!\n");
            // Get the structure of the Boot device to delete

             Index = 1;  
            for (OldEntry = GetFirstNode (&BdsBootOptionsList);
               !IsNull (&BdsBootOptionsList, OldEntry);
               OldEntry = GetNextNode (&BdsBootOptionsList,OldEntry)
               )
            {                     
              if (Index == SupportedDeviceCount+1) {
                OldBdsLoadOptionEntry = LOAD_OPTION_ENTRY_FROM_LINK (OldEntry);
                Print(L"BootOptionList get the entry, the entry is %d!\n",SupportedDeviceCount+1);
                break;
              }
              Index++;
            }

            // Delete the BDS Load option structures
            Status = BootOptionDelete (OldBdsLoadOptionEntry->BdsLoadOption);
            if (EFI_ERROR(Status)) {
              Print(L"BootOptionDelete fail!\n");
              Status = EFI_ABORTED;
              goto EXIT;
            }

            Status = BootOptionCreate (Attributes, SupportedBootDevice->Description, DevicePath, BootType, OptionalData, OptionalDataSize, &BdsLoadOptionEntry->BdsLoadOption);
            if (EFI_ERROR(Status)) {
              Print(L"Boot Option Create fail!\n");
              Status = EFI_ABORTED;
              goto EXIT;
            }       
       }
    }else{
//        Print(L"not Found the same BootOption, so creat it! \n");
        Status = BootOptionCreate (Attributes, SupportedBootDevice->Description, DevicePath, BootType, OptionalData, OptionalDataSize, &BdsLoadOptionEntry->BdsLoadOption);
        if (EFI_ERROR(Status)) {
            Print(L"Boot Option Create fail!\n");
            Status = EFI_ABORTED;
            goto EXIT;
        }
   }
   SupportedDeviceCount++;  
   if(DevicePathTxtNew != NULL)
   {
     FreePool(DevicePathTxtNew);
   }
   if(DevicePathTxtOld != NULL)
   {
     FreePool(DevicePathTxtOld);  
   }
  }
  Status = DeleteNoUseOldBootOption(&BdsBootOptionsList,OldBootOptionNum,SupportedDeviceCount);
  if (EFI_ERROR(Status)) {
      Print(L"Delete No Use Old BootOption fail!\n");
      Status = EFI_ABORTED;
  }  
  if (SupportedDeviceCount == 0) {
    Print(L"There is no supported device.\n");
    Status = EFI_ABORTED;
    goto EXIT;
  }
  
  Status = ReOrderBootList();
  if (EFI_ERROR(Status)) {
    Print(L"Reorder BootOrder List Fail!\n");
  } 
  
EXIT:    
  BootDeviceListSupportedFree (&SupportedDeviceList, SupportedBootDevice);
  if (NULL != BdsLoadOptionEntry) {
      FreePool(BdsLoadOptionEntry);
  }  
  return Status;
}


EFI_STATUS
BootMenuAddBootOption (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS                Status;
  BDS_SUPPORTED_DEVICE*     SupportedBootDevice;
  ARM_BDS_LOADER_ARGUMENTS* BootArguments;
  CHAR16                    BootDescription[BOOT_DEVICE_DESCRIPTION_MAX];
  CHAR8                     AsciiCmdLine[BOOT_DEVICE_OPTION_MAX];
  CHAR16                    CmdLine[BOOT_DEVICE_OPTION_MAX];
  UINT32                    Attributes;
  ARM_BDS_LOADER_TYPE       BootType;
  BDS_LOAD_OPTION_ENTRY     *BdsLoadOptionEntry;
  EFI_DEVICE_PATH           *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathNodes;
  EFI_DEVICE_PATH_PROTOCOL  *InitrdPathNodes;
  EFI_DEVICE_PATH_PROTOCOL  *InitrdPath;
  UINTN                     CmdLineSize;
  BOOLEAN                   InitrdSupport;
  UINTN                     InitrdSize;
  UINT8*                    OptionalData;
  UINTN                     OptionalDataSize;

  Attributes                = 0;
  SupportedBootDevice = NULL;

  // List the Boot Devices supported
  Status = SelectBootDevice (&SupportedBootDevice);
  if (EFI_ERROR(Status)) {
    Status = EFI_ABORTED;
    goto EXIT;
  }

  // Create the specific device path node
  Status = SupportedBootDevice->Support->CreateDevicePathNode (L"EFI Application or the kernel", &DevicePathNodes);
  if (EFI_ERROR(Status)) {
    Status = EFI_ABORTED;
    goto EXIT;
  }
  // Append the Device Path to the selected device path
  DevicePath = AppendDevicePath (SupportedBootDevice->DevicePathProtocol, (CONST EFI_DEVICE_PATH_PROTOCOL *)DevicePathNodes);
  if (DevicePath == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  if (SupportedBootDevice->Support->RequestBootType) {
    Status = BootDeviceGetType (DevicePath, &BootType, &Attributes);
    if (EFI_ERROR(Status)) {
      Status = EFI_ABORTED;
      goto EXIT;
    }
  } else {
    BootType = BDS_LOADER_EFI_APPLICATION;
  }

  if ((BootType == BDS_LOADER_KERNEL_LINUX_ATAG) || (BootType == BDS_LOADER_KERNEL_LINUX_FDT)) {
    Print(L"Add an initrd: ");
    Status = GetHIInputBoolean (&InitrdSupport);
    if (EFI_ERROR(Status)) {
      Status = EFI_ABORTED;
      goto EXIT;
    }

    if (InitrdSupport) {
      // Create the specific device path node
      Status = SupportedBootDevice->Support->CreateDevicePathNode (L"initrd", &InitrdPathNodes);
      if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) { // EFI_NOT_FOUND is returned on empty input string, but we can boot without an initrd
        Status = EFI_ABORTED;
        goto EXIT;
      }

      if (InitrdPathNodes != NULL) {
        // Append the Device Path to the selected device path
        InitrdPath = AppendDevicePath (SupportedBootDevice->DevicePathProtocol, (CONST EFI_DEVICE_PATH_PROTOCOL *)InitrdPathNodes);
        // Free the InitrdPathNodes created by Support->CreateDevicePathNode()
        FreePool (InitrdPathNodes);

        if (InitrdPath == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto EXIT;
        }
      } else {
        InitrdPath = NULL;
      }
    } else {
      InitrdPath = NULL;
    }

    Print(L"Arguments to pass to the binary: ");
    Status = GetHIInputAscii (AsciiCmdLine, BOOT_DEVICE_OPTION_MAX);
    if (EFI_ERROR(Status)) {
      Status = EFI_ABORTED;
      goto FREE_DEVICE_PATH;
    }

    CmdLineSize = AsciiStrSize (AsciiCmdLine);
    InitrdSize = GetDevicePathSize (InitrdPath);

    OptionalDataSize = sizeof(ARM_BDS_LOADER_ARGUMENTS) + CmdLineSize + InitrdSize;
    BootArguments = (ARM_BDS_LOADER_ARGUMENTS*)AllocatePool (OptionalDataSize);

    BootArguments->LinuxArguments.CmdLineSize = CmdLineSize;
    BootArguments->LinuxArguments.InitrdSize = InitrdSize;
    CopyMem ((VOID*)(&BootArguments->LinuxArguments + 1), AsciiCmdLine, CmdLineSize);
    CopyMem ((VOID*)((UINTN)(&BootArguments->LinuxArguments + 1) + CmdLineSize), InitrdPath, InitrdSize);

    OptionalData = (UINT8*)BootArguments;
  } else {
    Print (L"Arguments to pass to the EFI Application: ");
    Status = GetHIInputStr (CmdLine, BOOT_DEVICE_OPTION_MAX);
    if (EFI_ERROR (Status)) {
      Status = EFI_ABORTED;
      goto EXIT;
    }

    OptionalData = (UINT8*)CmdLine;
    OptionalDataSize = StrSize (CmdLine);
  }

  Print(L"Description for this new Entry: ");
  Status = GetHIInputStr (BootDescription, BOOT_DEVICE_DESCRIPTION_MAX);
  if (EFI_ERROR(Status)) {
    Status = EFI_ABORTED;
    goto FREE_DEVICE_PATH;
  }

  // Create new entry
  BdsLoadOptionEntry = (BDS_LOAD_OPTION_ENTRY*)AllocatePool (sizeof(BDS_LOAD_OPTION_ENTRY));
  Status = BootOptionCreate (Attributes, BootDescription, DevicePath, BootType, OptionalData, OptionalDataSize, &BdsLoadOptionEntry->BdsLoadOption);
  if (!EFI_ERROR(Status)) {
    InsertTailList (BootOptionsList, &BdsLoadOptionEntry->Link);
  }

FREE_DEVICE_PATH:
  FreePool (DevicePath);

EXIT:
  if (Status == EFI_ABORTED) {
    Print(L"\n");
  }
  FreePool(SupportedBootDevice);
  return Status;
}

EFI_STATUS
BootMenuRemoveBootOption (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS                    Status;
  BDS_LOAD_OPTION_ENTRY*        BootOptionEntry;

  DisplayBootOptions (BootOptionsList);
  Status = SelectBootOption (BootOptionsList, DELETE_BOOT_ENTRY, &BootOptionEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // If the Boot Option was attached to a list remove it
  if (!IsListEmpty (&BootOptionEntry->Link)) {
    // Remove the entry from the list
    RemoveEntryList (&BootOptionEntry->Link);
  }

  // Delete the BDS Load option structures
  BootOptionDelete (BootOptionEntry->BdsLoadOption);

  return EFI_SUCCESS;
}

EFI_STATUS
BootMenuUpdateBootOption (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS                    Status;
  BDS_LOAD_OPTION_ENTRY         *BootOptionEntry;
  BDS_LOAD_OPTION               *BootOption;
  BDS_LOAD_OPTION_SUPPORT*      DeviceSupport;
  ARM_BDS_LOADER_ARGUMENTS*     BootArguments;
  CHAR16                        BootDescription[BOOT_DEVICE_DESCRIPTION_MAX];
  CHAR8                         CmdLine[BOOT_DEVICE_OPTION_MAX];
  CHAR16                        UnicodeCmdLine[BOOT_DEVICE_OPTION_MAX];
  EFI_DEVICE_PATH               *DevicePath;
  EFI_DEVICE_PATH               *TempInitrdPath;
  ARM_BDS_LOADER_TYPE           BootType;
  ARM_BDS_LOADER_OPTIONAL_DATA* LoaderOptionalData;
  ARM_BDS_LINUX_ARGUMENTS*      LinuxArguments;
  EFI_DEVICE_PATH               *InitrdPathNodes;
  EFI_DEVICE_PATH               *InitrdPath;
  UINTN                         InitrdSize;
  UINTN                         CmdLineSize;
  BOOLEAN                       InitrdSupport;
  UINT8*                        OptionalData;
  UINTN                         OptionalDataSize;
  BOOLEAN                       IsPrintable;
  BOOLEAN                       IsUnicode;

  DisplayBootOptions (BootOptionsList);
  Status = SelectBootOption (BootOptionsList, UPDATE_BOOT_ENTRY, &BootOptionEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  BootOption = BootOptionEntry->BdsLoadOption;

  // Get the device support for this Boot Option
  Status = BootDeviceGetDeviceSupport (BootOption->FilePathList, &DeviceSupport);
  if (EFI_ERROR(Status)) {
    Print(L"Not possible to retrieve the supported device for the update\n");
    return EFI_UNSUPPORTED;
  }

  Status = DeviceSupport->UpdateDevicePathNode (BootOption->FilePathList, L"EFI Application or the kernel", &DevicePath);
  if (EFI_ERROR(Status)) {
    Status = EFI_ABORTED;
    goto EXIT;
  }

  if (DeviceSupport->RequestBootType) {
    Status = BootDeviceGetType (DevicePath, &BootType, &BootOption->Attributes);
    if (EFI_ERROR(Status)) {
      Status = EFI_ABORTED;
      goto EXIT;
    }
  }

  LoaderOptionalData = BootOption->OptionalData;
  if (LoaderOptionalData != NULL) {
    BootType = (ARM_BDS_LOADER_TYPE)ReadUnaligned32 ((UINT32 *)(&LoaderOptionalData->Header.LoaderType));
  } else {
    BootType = BDS_LOADER_EFI_APPLICATION;
  }

  if ((BootType == BDS_LOADER_KERNEL_LINUX_ATAG) || (BootType == BDS_LOADER_KERNEL_LINUX_FDT)) {
    LinuxArguments = &LoaderOptionalData->Arguments.LinuxArguments;

    CmdLineSize = ReadUnaligned16 ((CONST UINT16*)&LinuxArguments->CmdLineSize);

    InitrdSize = ReadUnaligned16 ((CONST UINT16*)&LinuxArguments->InitrdSize);
    if (InitrdSize > 0) {
      Print(L"Keep the initrd: ");
    } else {
      Print(L"Add an initrd: ");
    }
    Status = GetHIInputBoolean (&InitrdSupport);
    if (EFI_ERROR(Status)) {
      Status = EFI_ABORTED;
      goto EXIT;
    }

    if (InitrdSupport) {
      if (InitrdSize > 0) {
        // Case we update the initrd device path
        Status = DeviceSupport->UpdateDevicePathNode ((EFI_DEVICE_PATH*)((UINTN)(LinuxArguments + 1) + CmdLineSize), L"initrd", &InitrdPath);
        if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) {// EFI_NOT_FOUND is returned on empty input string, but we can boot without an initrd
          Status = EFI_ABORTED;
          goto EXIT;
        }
        InitrdSize = GetDevicePathSize (InitrdPath);
      } else {
        // Case we create the initrd device path

        Status = DeviceSupport->CreateDevicePathNode (L"initrd", &InitrdPathNodes);
        if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) { // EFI_NOT_FOUND is returned on empty input string, but we can boot without an initrd
          Status = EFI_ABORTED;
          goto EXIT;
        }

        if (InitrdPathNodes != NULL) {
          // Duplicate Linux kernel Device Path
          TempInitrdPath = DuplicateDevicePath (BootOption->FilePathList);
          // Replace Linux kernel Node by EndNode
          SetDevicePathEndNode (GetLastDevicePathNode (TempInitrdPath));
          // Append the Device Path to the selected device path
          InitrdPath = AppendDevicePath (TempInitrdPath, (CONST EFI_DEVICE_PATH_PROTOCOL *)InitrdPathNodes);
          FreePool (TempInitrdPath);
          // Free the InitrdPathNodes created by Support->CreateDevicePathNode()
          FreePool (InitrdPathNodes);
          if (InitrdPath == NULL) {
            Status = EFI_OUT_OF_RESOURCES;
            goto EXIT;
          }
          InitrdSize = GetDevicePathSize (InitrdPath);
        } else {
          InitrdPath = NULL;
        }
      }
    } else {
      InitrdSize = 0;
    }

    Print(L"Arguments to pass to the binary: ");
    if (CmdLineSize > 0) {
      AsciiStrnCpy (CmdLine, (CONST CHAR8*)(LinuxArguments + 1), sizeof (CmdLine));
      CmdLine[sizeof (CmdLine) - 1] = '\0';
    } else {
      CmdLine[0] = '\0';
    }
    Status = EditHIInputAscii (CmdLine, BOOT_DEVICE_OPTION_MAX);
    if (EFI_ERROR(Status)) {
      Status = EFI_ABORTED;
      goto FREE_DEVICE_PATH;
    }

    CmdLineSize = AsciiStrSize (CmdLine);

    OptionalDataSize = sizeof(ARM_BDS_LOADER_ARGUMENTS) + CmdLineSize + InitrdSize;
    BootArguments = (ARM_BDS_LOADER_ARGUMENTS*)AllocatePool (OptionalDataSize);
    if (NULL == BootArguments)
    {
        goto FREE_DEVICE_PATH;
    }
    BootArguments->LinuxArguments.CmdLineSize = CmdLineSize;
    BootArguments->LinuxArguments.InitrdSize = InitrdSize;
    CopyMem (&BootArguments->LinuxArguments + 1, CmdLine, CmdLineSize);
    CopyMem ((VOID*)((UINTN)(&BootArguments->LinuxArguments + 1) + CmdLineSize), InitrdPath, InitrdSize);

    OptionalData = (UINT8*)BootArguments;
  } else {
    Print (L"Arguments to pass to the EFI Application: ");

    if (BootOption->OptionalDataSize > 0) {
      IsPrintable = IsPrintableString (BootOption->OptionalData, &IsUnicode);
      if (IsPrintable) {
          //
          // The size in bytes of the string, final zero included, should
          // be equal to or at least lower than "BootOption->OptionalDataSize"
          // and the "IsPrintableString()" has already tested that the length
          // in number of characters is smaller than BOOT_DEVICE_OPTION_MAX,
          // final '\0' included. We can thus copy the string for editing
          // using "CopyMem()". Furthermore, note that in the case of an Unicode
          // string "StrnCpy()" and "StrCpy()" can not be used to copy the
          // string because the data pointed to by "BootOption->OptionalData"
          // is not necessarily 2-byte aligned.
          //
        if (IsUnicode) {
          CopyMem (
            UnicodeCmdLine, BootOption->OptionalData,
            MIN (sizeof (UnicodeCmdLine),
                 BootOption->OptionalDataSize)
            );
        } else {
          CopyMem (
            CmdLine, BootOption->OptionalData,
            MIN (sizeof (CmdLine),
                 BootOption->OptionalDataSize)
            );
        }
      }
    } else {
      UnicodeCmdLine[0] = L'\0';
      IsPrintable = TRUE;
      IsUnicode = TRUE;
    }

    // We do not request arguments for OptionalData that cannot be printed
    if (IsPrintable) {
      if (IsUnicode) {
        Status = EditHIInputStr (UnicodeCmdLine, BOOT_DEVICE_OPTION_MAX);
        if (EFI_ERROR (Status)) {
          Status = EFI_ABORTED;
          goto FREE_DEVICE_PATH;
        }

        OptionalData = (UINT8*)UnicodeCmdLine;
        OptionalDataSize = StrSize (UnicodeCmdLine);
      } else {
        Status = EditHIInputAscii (CmdLine, BOOT_DEVICE_OPTION_MAX);
        if (EFI_ERROR (Status)) {
          Status = EFI_ABORTED;
          goto FREE_DEVICE_PATH;
        }

        OptionalData = (UINT8*)CmdLine;
        OptionalDataSize = AsciiStrSize (CmdLine);
      }
    } else {
      // We keep the former OptionalData
      OptionalData = BootOption->OptionalData;
      OptionalDataSize = BootOption->OptionalDataSize;
    }
  }

  Print(L"Description for this new Entry: ");
  StrnCpy (BootDescription, BootOption->Description, BOOT_DEVICE_DESCRIPTION_MAX);
  Status = EditHIInputStr (BootDescription, BOOT_DEVICE_DESCRIPTION_MAX);
  if (EFI_ERROR(Status)) {
    Status = EFI_ABORTED;
    goto FREE_DEVICE_PATH;
  }

  // Update the entry
  Status = BootOptionUpdate (BootOption, BootOption->Attributes, BootDescription, DevicePath, BootType, OptionalData, OptionalDataSize);

FREE_DEVICE_PATH:
  FreePool (DevicePath);

EXIT:
  if (Status == EFI_ABORTED) {
    Print(L"\n");
  }
  return Status;
}

/**
  Reorder boot options

  Ask for the boot option to move and then move it when up or down arrows
  are pressed. This function is called when the user selects the "Reorder Boot
  Device Entries" entry in the boot manager menu.
  The order of the boot options in BootOptionList and in the UEFI BootOrder
  global variable are kept coherent until the user confirm his reordering (ie:
  he does not exit by pressing escape).

  @param[in]  BootOptionsList  List of the boot devices constructed in
                               BootMenuMain()

  @retval  EFI_SUCCESS   No error encountered.
  @retval  !EFI_SUCCESS  An error has occured either in the selection of the
                         boot option to move or while interacting with the user.

**/
STATIC
EFI_STATUS
BootMenuReorderBootOptions (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS              Status;
  BDS_LOAD_OPTION_ENTRY  *BootOptionEntry;
  LIST_ENTRY             *SelectedEntry;
  LIST_ENTRY             *PrevEntry;
  BOOLEAN                 Move;
  BOOLEAN                 Save;
  BOOLEAN                 Cancel;
  UINTN                   WaitIndex;
  EFI_INPUT_KEY           Key;
  LIST_ENTRY             *SecondEntry;
  UINTN                   BootOrderSize;
  UINT16                 *BootOrder;
  LIST_ENTRY             *Entry;
  UINTN                   Index;

  DisplayBootOptions (BootOptionsList);

  // Ask to select the boot option to move
  while (TRUE) {
    Status = SelectBootOption (BootOptionsList, MOVE_BOOT_ENTRY, &BootOptionEntry);
    if (EFI_ERROR (Status)) {
      goto ErrorExit;
    }

    SelectedEntry = &BootOptionEntry->Link;
    SecondEntry = NULL;
    // Note down the previous entry in the list to be able to cancel changes
    PrevEntry = GetPreviousNode (BootOptionsList, SelectedEntry);

    //  Start of interaction
    while (TRUE) {
      Print (
        L"* Use up/down arrows to move the entry '%s'",
        BootOptionEntry->BdsLoadOption->Description
        );

      // Wait for a move, save or cancel request
      Move   = FALSE;
      Save   = FALSE;
      Cancel = FALSE;
      do {
        Status = gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &WaitIndex);
        if (!EFI_ERROR (Status)) {
          Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
        }
        if (EFI_ERROR (Status)) {
          Print (L"\n");
          goto ErrorExit;
        }

        switch (Key.ScanCode) {
        case SCAN_NULL:
          Save = (Key.UnicodeChar == CHAR_LINEFEED)        ||
                 (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) ||
                 (Key.UnicodeChar == 0x7f);
          break;

        case SCAN_UP:
          SecondEntry = GetPreviousNode (BootOptionsList, SelectedEntry);
          Move = SecondEntry != BootOptionsList;
          break;

        case SCAN_DOWN:
          SecondEntry = GetNextNode (BootOptionsList, SelectedEntry);
          Move = SecondEntry != BootOptionsList;
          break;

        case SCAN_ESC:
          Cancel = TRUE;
          break;
        }
      } while ((!Move) && (!Save) && (!Cancel));

      if (Move) {
        if ((SelectedEntry != NULL) && (SecondEntry != NULL)) {
          SwapListEntries (SelectedEntry, SecondEntry);
        }
      } else {
        if (Save) {
          Status = GetGlobalEnvironmentVariable (
                    L"BootOrder", NULL, &BootOrderSize, (VOID**)&BootOrder
                    );
          BootOrderSize /= sizeof (UINT16);

          if (!EFI_ERROR (Status)) {
            // The order of the boot options in the 'BootOptionsList' is the
            // new order that has been just defined by the user. Save this new
            // order in "BootOrder" UEFI global variable.
            Entry = GetFirstNode (BootOptionsList);
            for (Index = 0; Index < BootOrderSize; Index++) {
              BootOrder[Index] = (LOAD_OPTION_FROM_LINK (Entry))->LoadOptionIndex;
              Entry = GetNextNode (BootOptionsList, Entry);
            }
            Status = gRT->SetVariable (
                           (CHAR16*)L"BootOrder",
                           &gEfiGlobalVariableGuid,
                           EFI_VARIABLE_NON_VOLATILE       |
                           EFI_VARIABLE_BOOTSERVICE_ACCESS |
                           EFI_VARIABLE_RUNTIME_ACCESS,
                           BootOrderSize * sizeof (UINT16),
                           BootOrder
                           );
            FreePool (BootOrder);
          }

          if (EFI_ERROR (Status)) {
            Print (L"\nAn error occurred, move not completed!\n");
            Cancel = TRUE;
          }
        }

        if (Cancel) {
          //
          // Restore initial position of the selected boot option
          //
          RemoveEntryList (SelectedEntry);
          InsertHeadList (PrevEntry, SelectedEntry);
        }
      }

      Print (L"\n");
      DisplayBootOptions (BootOptionsList);
      // Saved or cancelled, back to the choice of boot option to move
      if (!Move) {
        break;
      }
    }
  }

ErrorExit:
  return Status ;
}

/**
  Set boot timeout

  Ask for the boot timeout in seconds and if the input succeeds assign the
  input value to the UEFI global variable "Timeout". This function is called
  when the user selects the "Set Boot Timeout" of the boot manager menu.

  @param[in]  BootOptionsList  List of the boot devices, not used here

  @retval  EFI_SUCCESS   Boot timeout in second retrieved from the standard
                         input and assigned to the UEFI "Timeout" global
                         variable
  @retval  !EFI_SUCCESS  Either the input or the setting of the UEFI global
                         variable "Timeout" has failed.
**/
EFI_STATUS
STATIC
BootMenuSetBootTimeout (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS  Status;
  UINTN       Input;
  UINT16      Timeout;

  Print (L"Timeout duration (in seconds): ");
  Status = GetHIInputInteger (&Input);
  if (EFI_ERROR (Status)) {
    Print (L"\n");
    goto ErrorExit;
  }

  Timeout = Input;
  Status = gRT->SetVariable (
                 (CHAR16*)L"Timeout",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE       |
                 EFI_VARIABLE_BOOTSERVICE_ACCESS |
                 EFI_VARIABLE_RUNTIME_ACCESS,
                 sizeof (UINT16),
                 &Timeout
                 );
  ASSERT_EFI_ERROR (Status);

ErrorExit:
  return Status;
}

struct BOOT_MANAGER_ENTRY {
  CONST CHAR16* Description;
  EFI_STATUS (*Callback) (IN LIST_ENTRY *BootOptionsList);
} BootManagerEntries[] = {
    { L"Add Boot Device Entry", BootMenuAddBootOption },
    { L"Update Boot Device Entry", BootMenuUpdateBootOption },
    { L"Remove Boot Device Entry", BootMenuRemoveBootOption },
    { L"Reorder Boot Device Entries", BootMenuReorderBootOptions },
    { L"Set Boot Timeout", BootMenuSetBootTimeout },
};

EFI_STATUS
BootMenuManager (
  IN LIST_ENTRY *BootOptionsList
  )
{
  UINTN Index;
  UINTN OptionSelected;
  UINTN BootManagerEntryCount;
  EFI_STATUS Status;

  BootManagerEntryCount = sizeof(BootManagerEntries) / sizeof(struct BOOT_MANAGER_ENTRY);

  while (TRUE) {
    // Display Boot Manager menu
    for (Index = 0; Index < BootManagerEntryCount; Index++) {
      Print(L"[%d] %s\n",Index+1,BootManagerEntries[Index]);
    }
    Print(L"[%d] Return to main menu\n",Index+1);

    // Select which entry to call
    Print(L"Choice: ");
    Status = GetHIInputInteger (&OptionSelected);
    if (EFI_ERROR(Status) || (OptionSelected == (BootManagerEntryCount+1))) {
      if (EFI_ERROR(Status)) {
        Print(L"\n");
      }
      return EFI_SUCCESS;
    } else if ((OptionSelected > 0) && (OptionSelected <= BootManagerEntryCount))  {
      BootManagerEntries[OptionSelected-1].Callback (BootOptionsList);
    }
  }
  // Should never go here
}

EFI_STATUS
BootShell (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS Status;

  // Start EFI Shell
  Status = BdsLoadApplication (gImageHandle, L"Shell", 0, NULL);
  if (Status == EFI_NOT_FOUND) {
    Print (L"Error: EFI Application not found.\n");
  } else if (EFI_ERROR(Status)) {
    Print (L"Error: Status Code: 0x%X\n",(UINT32)Status);
  }

  return Status;
}

EFI_STATUS
Reboot (
  IN LIST_ENTRY *BootOptionsList
  )
{
  gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
  return EFI_UNSUPPORTED;
}


EFI_STATUS
BootEBL (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS Status;

  // Start EFI Shell
  Status = BdsLoadApplication (gImageHandle, L"Ebl", 0, NULL);
  if (Status == EFI_NOT_FOUND) {
    Print (L"Error: EFI Application not found.\n");
  } else if (EFI_ERROR(Status)) {
    Print (L"Error: Status Code: 0x%X\n",(UINT32)Status);
  }

  return Status;
}


EFI_STATUS
ESLBOOT (
  IN LIST_ENTRY *BootOptionsList
  )
{
  ESL_Start_Kernel();
  return EFI_SUCCESS;
}

EFI_STATUS
SRELOAD (
  IN LIST_ENTRY *BootOptionsList
  )
{
  FPGA_Start_SRE();
  return EFI_SUCCESS;
}


EFI_STATUS
FLASHBOOT (
  IN LIST_ENTRY *BootOptionsList
  )
{
  Flash_Start_Kernel();
  return EFI_SUCCESS;
}
#ifdef SECURE_BOOT
EFI_STATUS ReadFromFV (
  IN  CONST CHAR16 *UiString,
  OUT VOID        **DataBuffer,
  OUT UINTN        *DataSize
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *Buffer = NULL;
  EFI_GUID                      NameGuid;
  EFI_STATUS                    NextStatus;
  UINTN                         NoHandles;
  UINTN                         Index;
  EFI_FV_FILETYPE               FileType;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv = NULL;
  VOID                          *Key = NULL;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         UiStringLen = 0;
  CHAR16                        *UiSection = NULL;
  UINTN                         UiSectionSize;
  UINT32                        Authentication;
  BOOLEAN                       bFound = FALSE;

  if ((UiString == NULL) ||
      (DataBuffer == NULL) ||
      (DataSize == NULL) 
     )
  {
    return EFI_INVALID_PARAMETER;
  }
  UiStringLen = StrLen (UiString);
  
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiFirmwareVolume2ProtocolGuid, NULL, &NoHandles, &Buffer);
  if (EFI_ERROR (Status) || (Buffer == NULL))
  {
    goto clean_up;
  }

  for (Index = 0; Index < NoHandles; Index++)
  {
    Status = gBS->HandleProtocol (Buffer[Index], &gEfiFirmwareVolume2ProtocolGuid, (VOID **)&Fv);
    if (EFI_ERROR (Status))
    {
      /* Move to the next firmware volume. */
      continue;
    }

    Status = gBS->AllocatePool(EfiBootServicesData, Fv->KeySize, (VOID**)&Key);
    if (EFI_ERROR (Status))
    {
       goto clean_up;
    }
    
    ZeroMem(Key, Fv->KeySize);
    
    FileType = EFI_FV_FILETYPE_FREEFORM;
    
    do 
    {
      NextStatus = Fv->GetNextFile (Fv, Key, &FileType, &NameGuid, &Attributes, &UiSectionSize);
      if (!EFI_ERROR (NextStatus))
      {

        UiSection = NULL;
        UiSectionSize = 0;
        Status = Fv->ReadSection (
                      Fv, 
                      &NameGuid, 
                      EFI_SECTION_USER_INTERFACE, 
                      0,
                      (VOID **)&UiSection,
                      &UiSectionSize,
                      &Authentication
                      );
        if (EFI_ERROR (Status))
        {
          /* Move to the next file in the firmware volume. */
          continue;
        }

        if (StrnCmp (UiString, UiSection, UiStringLen) == 0)
        {
          //
          // We found a UiString match. 
          //

          //
          // Read first instance found of raw section content in NameGuid file
          //
          if ((DataBuffer != NULL) && (DataSize != NULL))
          {
            *DataBuffer     = NULL;
            *DataSize       = 0;
            Status          = Fv->ReadSection (
                                Fv,
                                &NameGuid,
                                EFI_SECTION_RAW,
                                0,
                                DataBuffer,
                                DataSize,
                                &Authentication
                                );
          
            bFound = TRUE;
          } 
          goto clean_up;
        }
        
        gBS->FreePool (UiSection);
        UiSection = NULL;

      }
    } while (!EFI_ERROR (NextStatus));
    
    gBS->FreePool (Key);
    Key = NULL;
  }

clean_up:

  if (UiSection != NULL)
  {
     gBS->FreePool (UiSection);
     UiSection = NULL;
  }

  if (Key != NULL)
  {
     gBS->FreePool (Key);
     Key = NULL;
  }

  if (Buffer != NULL)
  {
     gBS->FreePool (Buffer);
     Buffer = NULL;
  }

  if (EFI_ERROR(Status))
  {
    return Status;
  }
  else
  {
    if (bFound == FALSE)
    {
      return EFI_NOT_FOUND;
    }
    else
    {
      return EFI_SUCCESS;
    }
  }

}

EFI_STATUS
PKSet (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  VOID                            *DataBuffer = NULL;
  UINTN                            DataSize = 0;

    DEBUG(( EFI_D_ERROR, "It can update key without authenticated!"));
    Status = ReadFromFV(FILENAME_TEST_PK_SET, &DataBuffer, &DataSize);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "PK Config is fail£¬ReadFromFV\n"));
        return Status;
    }
    
    Status = EnrollPlatformKey (FILENAME_TEST_PK_SET,DataSize,DataBuffer); 
    if (EFI_ERROR(Status))
    {
       Print(L"PK Config is fail\n");
       return Status;
    }

    Print(L"PK Config is ok\n");

    return Status;
}

EFI_STATUS
DBSet (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  VOID                            *DataBuffer = NULL;
  UINTN                            DataSize = 0;
  CHAR16                          *VarName = NULL;
    VarName = EFI_IMAGE_SECURITY_DATABASE;
    
    Status = ReadFromFV(FILENAME_TEST_DB_RSA_SET, &DataBuffer, &DataSize);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "DB1 Config is fail£¬ReadFromFV\n"));
        return Status;
    }
    
    DEBUG(( EFI_D_ERROR,"DB1 DataSize 0x%x\n",DataSize));
    
    Status = EnrollSignatureDatabase (FILENAME_TEST_DB_RSA_SET,VarName,DataSize,DataBuffer);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "DB1 Config is fail£¬EnrollSignatureDatabase\n"));
        return Status;
    }

    Print(L"DB1 Config is ok\n");
    
    VarName = EFI_IMAGE_SECURITY_DATABASE;
    
    Status = ReadFromFV(FILENAME_TEST_DB_SET, &DataBuffer, &DataSize);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "DB2 Config is fail£¬ReadFromFV\n"));
        return Status;
    }
    
    DEBUG(( EFI_D_ERROR,"DB2 DataSize 0x%x\n",DataSize));
    
    Status = EnrollSignatureDatabase (FILENAME_TEST_DB_SET,VarName,DataSize,DataBuffer);
    if (EFI_ERROR(Status))
    {
        DEBUG(( EFI_D_ERROR,"DB2 Config is fail£¬EnrollSignatureDatabase\n"));
        return Status;
    }

    DEBUG(( EFI_D_ERROR,"DB2 Config is ok\n"));

  return Status;
}

EFI_STATUS
DBGet (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  VOID                            *DataBuffer = NULL;
  UINTN                            DataSize = 0;
  CHAR16                          *VarName = NULL;
  EFI_GUID                        *VarGuid = NULL;
  STATIC EFI_GUID DBGuid = EFI_IMAGE_SECURITY_DATABASE_GUID;
  
  EFI_SIGNATURE_LIST        *CertList;
  EFI_SIGNATURE_DATA        *Cert;
  UINTN                     CertCount;


    VarName = EFI_IMAGE_SECURITY_DATABASE;
    VarGuid = &DBGuid;

    Status = gRT->GetVariable(VarName, VarGuid, NULL, &DataSize, NULL);
    if (Status == EFI_NOT_FOUND)
    {
        DEBUG(( EFI_D_ERROR,"GetVariable db11 EFI_NOT_FOUND\n"));
    }
    else if (EFI_ERROR(Status))
    {
        DEBUG(( EFI_D_ERROR,"GetVariable db 11%r\n",Status));
    }
    
    DataBuffer = (UINT8 *) AllocateZeroPool (DataSize);
    Status = gRT->GetVariable(VarName, VarGuid, NULL, &DataSize, DataBuffer);
    if (Status == EFI_NOT_FOUND)
    {
        DEBUG(( EFI_D_ERROR,"GetVariable db22 EFI_NOT_FOUND\n"));
    }
    else if (EFI_ERROR(Status))
    {
        DEBUG(( EFI_D_ERROR,"GetVariable db22 %r\n",Status));
    }
    
    //DEBUG(( EFI_D_ERROR,"GetVariable db DataSize 0x%x\n",DataSize));
    
    CertList = (EFI_SIGNATURE_LIST *) DataBuffer;
    //DEBUG(( EFI_D_ERROR,"GetVariable db CertList->SignatureListSize 0x%x\n",CertList->SignatureListSize));
    while ((DataSize > 0) && (DataSize >= CertList->SignatureListSize)) {
        DEBUG ((EFI_D_ERROR, "[cdtest] Certs!\n"));
      if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
        Cert          = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
        CertCount     = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
        DEBUG ((EFI_D_ERROR, "[cdtest] X509 Cert 0x%x!\n",CertCount));
          Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) Cert + CertList->SignatureSize);
        }
      DataSize -= CertList->SignatureListSize;
      CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
      }
    
    if (DataBuffer!= NULL) {
      FreePool (DataBuffer);
    }
    
  return Status;
}

EFI_STATUS
KEKSet (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  VOID                            *DataBuffer = NULL;
  UINTN                            DataSize = 0;
                         
  Status = ReadFromFV(FILENAME_TEST_KEK_SET, &DataBuffer, &DataSize);
  if (EFI_ERROR(Status))
  {
      DEBUG(( EFI_D_ERROR,"KEK1 Config is fail£¬ReadFromFV\n"));
      return Status;
  }
  
  DEBUG(( EFI_D_ERROR,"KEK1 DataSize 0x%x\n",DataSize));
  
  
  Status = EnrollKeyExchangeKey (FILENAME_TEST_KEK_SET,DataSize,DataBuffer);
  if (EFI_ERROR(Status))
  {
      DEBUG(( EFI_D_ERROR,"KEK1 Config is fail£¬EnrollKeyExchangeKey\n"));
      return Status;
  }
  
  Print(L"KEK1 Config is ok\n");  

  Status = ReadFromFV(FILENAME_TEST_KEK_MFST_SET, &DataBuffer, &DataSize);
  if (EFI_ERROR(Status))
  {
      DEBUG(( EFI_D_ERROR,"KEK2 Config is fail£¬ReadFromFV\n"));
      return Status;
  }
  
  DEBUG(( EFI_D_ERROR,"KEK2 DataSize 0x%x\n",DataSize));
  
  
  Status = EnrollKeyExchangeKey (FILENAME_TEST_KEK_MFST_SET,DataSize,DataBuffer);
  if (EFI_ERROR(Status))
  {
      DEBUG(( EFI_D_ERROR,"KEK2 Config is fail£¬EnrollKeyExchangeKey\n"));
      return Status;
  }
  
  DEBUG(( EFI_D_ERROR,"KEK2 Config is ok\n")); 

  return Status;
}

EFI_STATUS
SecureBootEnable (
  IN LIST_ENTRY *BootOptionsList
  )
{
  UINT8         SetupMode;
  UINT8         SecureBoot;
  EFI_STATUS    Status = EFI_SUCCESS;
  UINTN                            DataSize = 0;
  DataSize = sizeof (UINT8);

  Status = gRT->GetVariable (EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, &SecureBoot);  
  if (EFI_ERROR(Status))
  {
      DEBUG((EFI_D_ERROR, "SecureBoot Variable does not exist!"));
  }

  if(SecureBoot)
  {
      DEBUG((EFI_D_ERROR, "SecureBoot is already Enabled!"));
  }
  else
  {
      SecureBoot = 1;
      Status = gRT->SetVariable (EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid,
                               EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                               sizeof (UINT8), &SecureBoot);
  }

  /* Assume by default we are in setupmode and Secure Boot is not enabled */
  Status = gRT->GetVariable (EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid,
                             NULL, &DataSize, &SetupMode);
  
  if (EFI_ERROR(Status))
  {
      DEBUG(( EFI_D_ERROR, "SetupMode Variable does not exist!"));
  }

  DEBUG(( EFI_D_ERROR, "SetupMode is 0x%x!",SetupMode));
  Print(L"ok\n");
  
  return Status;
}

EFI_STATUS
DBDelete (
  IN LIST_ENTRY *BootOptionsList
  )
{
    
    EFI_STATUS                       Status = EFI_SUCCESS;
    CHAR16                          *VarName = NULL;
    
    VarName = EFI_IMAGE_SECURITY_DATABASE;
    
    Status = DeleteSignatureDatabase (VarName,1);
    if (EFI_ERROR(Status))
    {
        Print(L"DB Delete is fail!\n");
        return Status;
    }

    Print(L"DB Delete is OK!\n");
    return Status;
}

EFI_STATUS
KEKDelete (
  IN LIST_ENTRY *BootOptionsList
  )
{
    EFI_STATUS                       Status = EFI_SUCCESS;
    
    Status = DeleteKeyExchangeKey (0);
    if (EFI_ERROR(Status))
    {
        Print(L"KEK Delete is fail!\n");
        return Status;
    }

    Print(L"KEK Delete is OK!\n");
    return Status;
}

EFI_STATUS
PKDelete (
  IN LIST_ENTRY *BootOptionsList
  )
{
    EFI_STATUS                       Status = EFI_SUCCESS;
    Status = DeletePlatformKey();
    if (EFI_ERROR(Status))
    {
        Print(L"PK Delete is fail!\n");
        return Status;
    }

    Print(L"PK Delete is OK!\n");
    return Status;
}

EFI_STATUS
SecureBootDisable (
  IN LIST_ENTRY *BootOptionsList
  )
{
  EFI_STATUS Status;
  UINT8         SetupMode;
  UINT8         SecureBoot;
  UINTN DataSize;
  DataSize = sizeof (UINT8);
  
  Status = gRT->GetVariable (EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, &SecureBoot);  
  if (EFI_ERROR(Status))
  {
      DEBUG(( EFI_D_ERROR, "SecureBoot Variable does not exist!"));
  }

  if(SecureBoot)
  {
    SecureBoot = 0;
    Status = gRT->SetVariable (EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid,
                              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                sizeof (UINT8), &SecureBoot);
 
  }
  else
  {
    DEBUG(( EFI_D_ERROR, "SecureBoot is already Disabled!"));
  }

  /* Assume by default we are in setupmode and Secure Boot is not enabled */
  Status = gRT->GetVariable (EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid,
                             NULL, &DataSize, &SetupMode);
  
  if (EFI_ERROR(Status))
  {
      DEBUG(( EFI_D_ERROR, "SetupMode Variable does not exist!"));
  }

  DEBUG(( EFI_D_ERROR, "SetupMode is 0x%x!",SetupMode));

  return Status;
}
#endif

#ifndef SECURE_BOOT
struct BOOT_MAIN_ENTRY {
  CONST CHAR16* Description;
  EFI_STATUS (*Callback) (IN LIST_ENTRY *BootOptionsList);
} BootMainEntries[] = {
    { L"Shell", BootShell },
    { L"EBL", BootEBL },
    { L"Boot Manager", BootMenuManager },
    { L"ESL Start OS", ESLBOOT },
    { L"FLASH Start OS", FLASHBOOT },
    { L"FPGA Start SRE", SRELOAD },
    { L"Reboot", Reboot },
};
#else
struct BOOT_MAIN_ENTRY {
  CONST CHAR16* Description;
  EFI_STATUS (*Callback) (IN LIST_ENTRY *BootOptionsList);
} BootMainEntries[] = {
    { L"Shell", BootShell },
    { L"EBL", BootEBL },
    { L"Boot Manager", BootMenuManager },
    { L"ESL Start OS", ESLBOOT },
    { L"FLASH Start OS", FLASHBOOT },
    { L"PKSet", PKSet},
    { L"DBSet", DBSet},
    { L"KEKSet", KEKSet},
    { L"DBGet", DBGet},
    { L"SecureBootEnable", SecureBootEnable},
    { L"PKDelete", PKDelete},
    { L"DBDelete", DBDelete},
    { L"KEKDelete", KEKDelete},
    { L"SecureBootDsiable", SecureBootDisable},
    { L"Reboot", Reboot },
};
#endif



EFI_STATUS
BootMenuMain (
  VOID
  )
{
  LIST_ENTRY                    BootOptionsList;
  UINTN                         OptionCount;
  UINTN                         BootOptionCount;
  EFI_STATUS                    Status;
  LIST_ENTRY*                   Entry;
  BDS_LOAD_OPTION*              BootOption;
  UINTN                         BootOptionSelected;
  UINTN                         Index;
  UINTN                         BootMainEntryCount;
  BOOLEAN                       IsUnicode;

  BootOption         = NULL;
  BootMainEntryCount = sizeof(BootMainEntries) / sizeof(struct BOOT_MAIN_ENTRY);

  while (TRUE) {
    // Get Boot#### list
    BootOptionList (&BootOptionsList);

    OptionCount = 1;

    // Display the Boot options
    for (Entry = GetFirstNode (&BootOptionsList);
         !IsNull (&BootOptionsList,Entry);
         Entry = GetNextNode (&BootOptionsList,Entry)
         )
    {
      BootOption = LOAD_OPTION_FROM_LINK(Entry);

      Print(L"[%d] %s\n", OptionCount, BootOption->Description);

      DEBUG_CODE_BEGIN();
        CHAR16*                           DevicePathTxt;
        EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* DevicePathToTextProtocol;
        ARM_BDS_LOADER_OPTIONAL_DATA*     OptionalData;
        UINTN                             CmdLineSize;
        ARM_BDS_LOADER_TYPE               LoaderType;

        Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **)&DevicePathToTextProtocol);
        if (EFI_ERROR(Status)) {
          // You must provide an implementation of DevicePathToTextProtocol in your firmware (eg: DevicePathDxe)
          DEBUG((EFI_D_ERROR,"Error: Bds requires DevicePathToTextProtocol\n"));
          return Status;
        }
        DevicePathTxt = DevicePathToTextProtocol->ConvertDevicePathToText (BootOption->FilePathList, TRUE, TRUE);

        Print(L"\t- %s\n",DevicePathTxt);

        // If it is a supported BootEntry then print its details
        if (IS_ARM_BDS_BOOTENTRY (BootOption)) {
          OptionalData = BootOption->OptionalData;
          LoaderType = (ARM_BDS_LOADER_TYPE)ReadUnaligned32 ((CONST UINT32*)&OptionalData->Header.LoaderType);
          if ((LoaderType == BDS_LOADER_KERNEL_LINUX_ATAG) || (LoaderType == BDS_LOADER_KERNEL_LINUX_FDT)) {
            if (ReadUnaligned16 (&OptionalData->Arguments.LinuxArguments.InitrdSize) > 0) {
              CmdLineSize = ReadUnaligned16 (&OptionalData->Arguments.LinuxArguments.CmdLineSize);
              DevicePathTxt = DevicePathToTextProtocol->ConvertDevicePathToText (
                  GetAlignedDevicePath ((EFI_DEVICE_PATH*)((UINTN)(&OptionalData->Arguments.LinuxArguments + 1) + CmdLineSize)), TRUE, TRUE);
              Print(L"\t- Initrd: %s\n", DevicePathTxt);
            }
            if (ReadUnaligned16 (&OptionalData->Arguments.LinuxArguments.CmdLineSize) > 0) {
              Print(L"\t- Arguments: %a\n", (&OptionalData->Arguments.LinuxArguments + 1));
            }
          }

          switch (LoaderType) {
            case BDS_LOADER_EFI_APPLICATION:
              Print(L"\t- LoaderType: EFI Application\n");
              break;

            case BDS_LOADER_KERNEL_LINUX_ATAG:
              Print(L"\t- LoaderType: Linux kernel with ATAG support\n");
              break;

            case BDS_LOADER_KERNEL_LINUX_FDT:
              Print(L"\t- LoaderType: Linux kernel with FDT support\n");
              break;

            default:
              Print(L"\t- LoaderType: Not recognized (%d)\n", LoaderType);
          }
        } else if (BootOption->OptionalData != NULL) {
          if (IsPrintableString (BootOption->OptionalData, &IsUnicode)) {
            if (IsUnicode) {
              Print (L"\t- Arguments: %s\n", BootOption->OptionalData);
            } else {
              AsciiPrint ("\t- Arguments: %a\n", BootOption->OptionalData);
            }
          }
        }
        FreePool(DevicePathTxt);
      DEBUG_CODE_END();

      OptionCount++;
    }
    BootOptionCount = OptionCount-1;

    // Display the hardcoded Boot entries
    for (Index = 0; Index < BootMainEntryCount; Index++) {
      Print(L"[%d] %s\n",OptionCount,BootMainEntries[Index]);
      OptionCount++;
    }

    // Request the boot entry from the user
    BootOptionSelected = 0;
    while (BootOptionSelected == 0) {
      Print(L"Start: ");
      Status = GetHIInputInteger (&BootOptionSelected);
      if (EFI_ERROR(Status) || (BootOptionSelected == 0) || (BootOptionSelected > OptionCount)) {
        Print(L"Invalid input (max %d)\n",(OptionCount-1));
        BootOptionSelected = 0;
      }
    }

    // Start the selected entry
    if (BootOptionSelected > BootOptionCount) {
      // Start the hardcoded entry
      Status = BootMainEntries[BootOptionSelected - BootOptionCount - 1].Callback (&BootOptionsList);
    } else {
      // Find the selected entry from the Boot#### list
      Index = 1;
      for (Entry = GetFirstNode (&BootOptionsList);
           !IsNull (&BootOptionsList,Entry);
           Entry = GetNextNode (&BootOptionsList,Entry)
           )
      {
        if (Index == BootOptionSelected) {
          BootOption = LOAD_OPTION_FROM_LINK(Entry);
          break;
        }
        Index++;
      }

      Status = BootOptionStart (BootOption);
    }
  }
  // Should never go here
}
