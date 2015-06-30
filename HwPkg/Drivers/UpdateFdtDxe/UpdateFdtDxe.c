/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePcxREXVZ5dUR67fNFdcsGWwP1ecJzNoC5Nk+8B
iJvImLuolrIPUKBGd8L2wpF6y6N3BrBWhrMiUsGHALluV0im+t4T28eAdD4NhRGenqxofQly
tiYrdQUOiCbgko7X8I9R7sXS0nvwXr8GKLUB1vDslWVM+jomrqhBcmv9hcQOHQ==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : UpdateFdtDxe.c
  版 本 号   : v1.0
  作    者   : s00296804
  生成日期   : 2014年12月23日
  最近修改   :
  功能描述   : 从SPI Flash读出FDT并更新,创建FDT Configuration Table
  修改历史   :
1.   日	  期   : 
     作	  者   :
     修改内容  :
******************************************************************************/
#include <Uefi.h>
#include <Pi/PiDxeCis.h>
#include <Library/DebugLib.h>
#include <libfdt.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Guid/Fdt.h>
#include <Protocol/OemNicProtocol.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/FdtUpdateLib.h>

#include <Library/HwSafeOperationLib.h>
#include "UpdateFdtDxe.h"

STATIC
EFI_STATUS
InstallFdtIntoConfigurationTable (
  IN VOID* FdtBlob,
  IN UINTN FdtSize
  )
{
  EFI_STATUS Status;

  // Check the FDT header is valid. We only make this check in DEBUG mode in case the FDT header change on
  // production device and this ASSERT() becomes not valid.
  if(!(fdt_check_header (FdtBlob) == 0))
  {
      DEBUG ((EFI_D_ERROR,"can not find FdtBlob \n"));
      return EFI_INVALID_PARAMETER;
  }

  // Ensure the Size of the Device Tree is smaller than the size of the read file
  if(!((UINTN)fdt_totalsize (FdtBlob) <= FdtSize))
  {
      DEBUG ((EFI_D_ERROR,"FdtBlob <= FdtSize \n"));
      return EFI_INVALID_PARAMETER;
  }

  // Install the FDT into the Configuration Table
  Status = gBS->InstallConfigurationTable (&gFdtTableGuid, FdtBlob);

  return Status;
}

EFI_STATUS
EFIAPI UpdateFdt (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable)
{
    INTN                Error;
    VOID*               Fdt;      
    UINT32              Size;     
    UINTN               NewFdtBlobSize;   
    UINTN               NewFdtBlobBase; 
    EFI_STATUS          Status = EFI_SUCCESS;
    UINT32              Index = 0;
    UINTN               FDTConfigTable;
    VOID*               dMem=NULL;
    EFI_GCD_MEMORY_SPACE_DESCRIPTOR desp = {0};
    
    Fdt = (VOID*)(PcdGet64(FdtFileAddress));
    
    //给OS报一段空间base: 0x6B000000, size: 0x4000000,NVRAM 用    
    Status = gDS->GetMemorySpaceDescriptor(PcdGet64(PcdReservedNvramBase),&desp);
    if(EFI_ERROR(Status)){
         DEBUG ((EFI_D_ERROR,"get memory space error:--------- \n"));
        return Status;
    } 
    desp.Attributes |= EFI_MEMORY_RUNTIME | EFI_MEMORY_WB;
    Status = gDS->SetMemorySpaceAttributes(PcdGet64(PcdReservedNvramBase),PcdGet64(PcdReservedNvramSize), desp.Attributes);
    if(EFI_ERROR(Status)){
        DEBUG ((EFI_D_ERROR,"set memory space error:--------- \n"));
        return Status;
    } 

    //查找FTD tree
    Error = fdt_check_header ((VOID*)(PcdGet64(FdtFileAddress)));
    DEBUG ((EFI_D_ERROR,"fdtfileaddress:--------- 0x%lx\n",PcdGet64(FdtFileAddress)));
    if (Error != 0) 
    {
        DEBUG ((EFI_D_ERROR,"ERROR: Device Tree header not valid (%a)\n", fdt_strerror(Error)));
        return EFI_INVALID_PARAMETER;
    }   

    Size = (UINTN)fdt_totalsize ((VOID*)(PcdGet64(FdtFileAddress)));
    NewFdtBlobSize = Size + ADD_FILE_LENGTH;
    
    Status = gBS->AllocatePages (AllocateAnyPages, EfiRuntimeServicesData, EFI_SIZE_TO_PAGES(Size), &NewFdtBlobBase);
    if (EFI_ERROR (Status)) 
    {
        return EFI_OUT_OF_RESOURCES;
    }

    dMem = memcpy_s((VOID*)NewFdtBlobBase,NewFdtBlobSize, Fdt, Size);
    if(NULL == dMem)
    {
        DEBUG((EFI_D_ERROR, "%a(%d):MEM CPY FAIL!\n", __FUNCTION__,__LINE__));
        goto EXIT;
    }
   
    Status = EFIFdtUpdate(NewFdtBlobBase);
    if (EFI_ERROR (Status)) 
    {
        DEBUG((EFI_D_ERROR, "%a(%d):EFIFdtUpdate Fail!\n", __FUNCTION__,__LINE__));
        goto EXIT;
    }
    
    // 安装FDT Configuration Table
    Status = InstallFdtIntoConfigurationTable ((VOID*)(UINTN)NewFdtBlobBase, NewFdtBlobSize);
    DEBUG ((EFI_D_ERROR, "NewFdtBlobBase: 0x%lx  NewFdtBlobSize:0x%lx\n",NewFdtBlobBase,NewFdtBlobSize));
    if (EFI_ERROR (Status))
    {
        DEBUG ((EFI_D_ERROR, "installfdtconfiguration table fail():\n"));
        goto EXIT;
    }

    //查找FDT Configuration Table,做验证用，查看FDT Configuration Table 地址
    for (Index = 0; Index < gST->NumberOfTableEntries; Index ++) 
    {
        if (CompareGuid (&gFdtTableGuid, &(gST->ConfigurationTable[Index].VendorGuid)))
        {
            FDTConfigTable = (UINTN)gST->ConfigurationTable[Index].VendorTable;
            DEBUG ((EFI_D_ERROR, "FDTConfigTable Address: 0x%lx\n",FDTConfigTable));     
            break;
        }
    }
    
    return Status; 
    
    EXIT:
         //释放内存
         gBS->FreePages(NewFdtBlobBase,EFI_SIZE_TO_PAGES(NewFdtBlobSize));
        
    return Status;  
    
}
