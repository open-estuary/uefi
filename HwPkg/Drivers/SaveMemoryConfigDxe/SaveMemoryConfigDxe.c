/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefd1c+x33NDPwqpTmM0D8zx9Utx3JxVELqYS/9RntqHJZMlZcrHeaW/VTufJkPlZwbzEUn
W0ne0Hk3/io/uMdAelJfIlrHNpm0Bdv9cmQoKt6Tfc2uNAKf/ah/4eQvqqia0Q==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司
******************************************************************************/
#include "Uefi.h"
#include <Library/ArmLib.h>
#include <Library/DebugLib.h>  
#include <Protocol/SpiFlashProtocol.h>
#include <Library/HwMemInitLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>  
#include <Guid/MemoryMapData.h>  

/****************************************************************************
 函 数 名  : SaveMemoryConfigDxeEntry
 功能描述  : 读取Memory相关配置hob数据，存入Flash
 输入参数  : IN EFI_HANDLE         ImageHandle,
             IN EFI_SYSTEM_TABLE  *SystemTable
 输出参数  : 无
 返 回 值  : EFI_STATUS
 修改历史  :
 1.日    期   : 2014年12月18日
  作    者   : l00228991
  修改内容   : 新生成函数
****************************************************************************/
EFI_STATUS
EFIAPI SaveMemoryConfigDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    NVRAM           *Nvram;
    GBL_DATA        *Gbl_Data;
    SPI_FLASH_PROTOCOL        *Flash;
    VOID*           HobList;
    UINT32          NvramCrc;

    
    HobList = GetHobList();
    Gbl_Data = (GBL_DATA*)GetNextGuidHob(&gEfiMemoryMapGuid, HobList);
    Gbl_Data = GET_GUID_HOB_DATA(Gbl_Data);

    Nvram = &(Gbl_Data->nvram);

    Status = gBS->CalculateCrc32(((UINT8 *)Nvram+sizeof(UINT32)),(sizeof(NVRAM)-sizeof(UINT32)),&NvramCrc);
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR,"Nvram CalculateCrc32 Failed\n"));
        return Status;
    }
    
    if( Nvram->NvramCrc != NvramCrc)
    {
        Nvram->NvramCrc = NvramCrc;
        
        Status = gBS->LocateProtocol (&gSpiFlashProtocolGuid, NULL, (VOID *) &Flash);
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "%a - %d Status=%r\n", __FILE__, __LINE__, Status));
            return Status;
        }
        
        Status = Flash->Erase(Flash,NVRAM_ADDR,sizeof(NVRAM));
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "%a - %d SpiFlash Erase Error,Status=%r\n", __FILE__, __LINE__,Status));
            return Status;
        }

        Status = Flash->Write(Flash, NVRAM_ADDR, (UINT8*)(Nvram), sizeof(NVRAM));
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "%a - %d Flash Write Error,Status=%r\n", __FILE__, __LINE__,Status));
            return Status;
        }
    }

    return EFI_SUCCESS;
}



