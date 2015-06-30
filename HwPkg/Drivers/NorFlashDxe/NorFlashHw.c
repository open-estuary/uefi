/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefd2PjWDZN+DQz+19WNnvANCqt39BE7NKeAk+F0i6xFitynKw/xfmJ4Ihj/0qYD51oshQS
kC1Tf2LvZD/JpniKkXr9Cz0u+n8chYuymK1/O5aS30/swisQ8HFSPVmb86vXhA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NorFlashHw.c
  版 本 号   : v2.0
  作    者   : c00213799
  生成日期   : 2013年03月04日
  最近修改   :
  功能描述   : Flash驱动指令操作层
  修改历史   :
1.   日	  期   : 
     作	  者   :
     修改内容  :
******************************************************************************/
#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "NorFlashHw.h"

/*全局变量定义*/
BOOLEAN  gFlashBusy = FALSE;  
FLASH_INDEX gIndex = {
    0,
    0,
    0,
    0,
    0,
    0
};

/*****************************************************************************
 函 数 名  : PortReadData
 功能描述  : 根据位宽读取Flash中的数据
 输入参数  : Index      gFlashInfo数组的索引
            FlashAddr   Flash绝对地址
 输出参数  : 无
 返 回 值  : 读取的数据
 修改历史  :
*****************************************************************************/
UINT32 PortReadData (
    UINT32 Index,
    UINT32 FlashAddr
  )
{
    /* 根据端口宽度访问Flash */
    switch (gFlashInfo[Index].ParallelNum)
    {
        case 2:
            return *(volatile UINT32*)(UINTN)FlashAddr;
        case 1:
            return *(volatile UINT16*)(UINTN)FlashAddr;
        /* 其它缺省为错误*/
        default:
            DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:illegal PortWidth!\n", __FUNCTION__,__LINE__));
            return 0xffffffff;
    }
}
/*****************************************************************************
 函 数 名  : PortWriteData
 功能描述  : 根据位宽写入数据到Flash中
 输入参数  : Index      gFlashInfo数组的索引
            FlashAddr   Flash绝对地址
            ulInputData: 需要调整数据
 输出参数  : 无
 返 回 值  : 调整后的数据
 修改历史      :
*****************************************************************************/
EFI_STATUS
PortWriteData (
    UINT32 Index,
    UINT32 FlashAddr,
    UINT32 InputData
  )
{
    /* 根据端口宽度访问Flash */
    switch (gFlashInfo[Index].ParallelNum)
    {
        case 2:
             *(volatile UINT32*)(UINTN)FlashAddr = InputData;
             break;
        case 1:
             *(volatile UINT16*)(UINTN)FlashAddr = (UINT16)InputData;
             break;
        default:
             DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:illegal PortWidth!\n", __FUNCTION__,__LINE__));
             return EFI_DEVICE_ERROR;
    }
    return EFI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : PortAdjustData
 功能描述  : 根据位宽取低位数据
 输入参数  : Index      gFlashInfo数组的索引
            ulInputData: 需要调整数据
 输出参数  : 无
 返 回 值  : 调整后的数据
 修改历史  :
*****************************************************************************/
UINT32 PortAdjustData( 
    UINT32 Index,
    UINT32 ulInputData
  )
{
    /* 根据端口宽度调整数据 */
    switch (gFlashInfo[Index].ParallelNum)
    {
        case 2:
             return ulInputData;
        case 1:
             return (0x0000ffff & ulInputData );
        default:
            DEBUG((EFI_D_ERROR,"[FLASH_S29GL256N_PortAdjustData]: Error--illegal g_ulFlashS29Gl256NPortWidth!\n\r"));
            return 0xffffffff;
    }
}

/****************************************************************************
 函 数 名 : GetCommandIndex
 功能描述  : 获取命令字，保存到全局变量gIndex中
 输入参数  : Index       - gFlashInfo的索引
 输出参数  : 无
 返 回 值  : EFI_SUCCESS        - 操作成功
           EFI_DEVICE_ERROR   - 设备错误
 修改历史  :
****************************************************************************/
EFI_STATUS GetCommandIndex(
    UINT32 Index
  )
{
    UINT32 CommandCount = 0;
    UINT32 i;
    UINT8 Flag = 1;
    
    CommandCount = sizeof(gFlashCommandReset) / sizeof(FLASH_COMMAND_RESET);
    for(i = 0;i < CommandCount; i ++ )
    {
        if(gFlashInfo[Index].CommandType & gFlashCommandReset[i].CommandType)
        {
            Flag = 0;
            gIndex.ReIndex = i;
            break;
        }
    }
    //如果没有找到复位命令字，直接返回
    if(Flag)   
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Can not Get Reset Command!\n", __FUNCTION__,__LINE__));
        return EFI_DEVICE_ERROR;
    }
    
    CommandCount = sizeof(gFlashCommandId) / sizeof(FLASH_COMMAND_ID);
    for(Flag = 1,i = 0;i < CommandCount; i ++ )
    {
        if(gFlashInfo[Index].CommandType & gFlashCommandId[i].CommandType)
        {
            Flag = 0;
            gIndex.IdIndex = i;
            break;
        }
    }
    //如果没有找到获取ID命令字，直接返回
    if(Flag)
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Can not Get ID Command!\n", __FUNCTION__,__LINE__));
        return EFI_DEVICE_ERROR;
    }

    CommandCount = sizeof(gFlashCommandWrite) / sizeof(FLASH_COMMAND_WRITE);
    for(Flag = 1, i = 0;i < CommandCount; i ++ )
    {
        if(gFlashInfo[Index].CommandType & gFlashCommandWrite[i].CommandType)
        {
            Flag = 0;
            gIndex.WIndex = i;
            break;
        }
    }
    //如果没有找到写命令字，直接返回
    if(Flag)
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Can not Get Write Command!\n", __FUNCTION__,__LINE__));
        return EFI_DEVICE_ERROR;
    }

    CommandCount = sizeof(gFlashCommandErase) / sizeof(FLASH_COMMAND_ERASE);
    for(Flag = 1, i = 0;i < CommandCount; i ++ )
    {
        if(gFlashInfo[Index].CommandType & gFlashCommandErase[i].CommandType)
        {
            Flag = 0;
            gIndex.WIndex = i;
            break;
        }
    }
    //如果没有找到擦除命令字，直接返回
    if(Flag)
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Can not Get Erase Command!\n", __FUNCTION__,__LINE__));
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/****************************************************************************
 函 数 名  : FlashReset
 功能描述  : 复位flash，恢复到读模式，调用者需要确保gIndex已初始化完成
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
****************************************************************************/
VOID FlashReset(UINT32 Base)
{
    (VOID)PortWriteData(gIndex.InfIndex, Base, gFlashCommandReset[gIndex.ReIndex].ResetData);
    (void)gBS->Stall(20000);
}

/****************************************************************************
 函 数 名  : GetManufacturerID
 功能描述  : 获取厂商ID
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 修改历史  :
****************************************************************************/
void GetManufacturerID(UINT32 Index, UINT32 Base, UINT8 *pbyData)
{
    
	UINT32 dwAddr;

    FlashReset(Base);
    
	dwAddr = Base +  (gFlashCommandId[gIndex.IdIndex].ManuIDAddressStep1 << gFlashInfo[Index].ParallelNum);
    (VOID)PortWriteData(Index, dwAddr, gFlashCommandId[gIndex.IdIndex].ManuIDDataStep1);
    
	dwAddr = Base + (gFlashCommandId[gIndex.IdIndex].ManuIDAddressStep2 << gFlashInfo[Index].ParallelNum);
    (VOID)PortWriteData(Index, dwAddr, gFlashCommandId[gIndex.IdIndex].ManuIDDataStep2);

	dwAddr = Base + (gFlashCommandId[gIndex.IdIndex].ManuIDAddressStep3 << gFlashInfo[Index].ParallelNum);
    (VOID)PortWriteData(Index, dwAddr, gFlashCommandId[gIndex.IdIndex].ManuIDDataStep3);

    *pbyData = (UINT8)PortReadData(Index, Base + (gFlashCommandId[gIndex.IdIndex].ManuIDAddress << gFlashInfo[Index].ParallelNum));     

	FlashReset(Base);	//must reset to return to the read mode
}

/****************************************************************************
 函 数 名 : FlashInit
 功能描述  : 初始化flash的基地址、匹配flash信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : EFI_SUCCESS        - 操作成功
           EFI_DEVICE_ERROR   - 设备错误
 修改历史  :
****************************************************************************/
EFI_STATUS FlashInit(UINT32 Base)
{
    UINT32 FlashCount = 0;
    UINT32 i = 0;
    EFI_STATUS Status;
    UINT8 Flag = 1;
    UINT32 TempData = 0;    
    UINT32 TempDev1 = 0;   
    UINT32 TempDev2 = 0;
    UINT32 TempDev3 = 0;    
    UINT32 dwAddr;
        
    FlashCount = sizeof(gFlashInfo) / sizeof(NOR_FLASH_INFO_TABLE);
    for(;i < FlashCount; i ++ )
    {
        //获取命令字索引，初始化gIndex
        Status = GetCommandIndex(i);
        if (EFI_ERROR(Status))
         {
             DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Get Command Index %r!\n", __FUNCTION__,__LINE__, Status));
             return Status;
         }
        
        FlashReset(Base);
        //发送获取manuID的命令字
        dwAddr = Base +  (gFlashCommandId[gIndex.IdIndex].ManuIDAddressStep1 << gFlashInfo[i].ParallelNum);
        (VOID)PortWriteData(i, dwAddr, gFlashCommandId[gIndex.IdIndex].ManuIDDataStep1);
        
        dwAddr = Base + (gFlashCommandId[gIndex.IdIndex].ManuIDAddressStep2 << gFlashInfo[i].ParallelNum);
        (VOID)PortWriteData(i, dwAddr, gFlashCommandId[gIndex.IdIndex].ManuIDDataStep2);
        
        dwAddr = Base + (gFlashCommandId[gIndex.IdIndex].ManuIDAddressStep3 << gFlashInfo[i].ParallelNum);
        (VOID)PortWriteData(i, dwAddr, gFlashCommandId[gIndex.IdIndex].ManuIDDataStep3);
        //Get manufacture ID
        TempData = PortReadData(i, Base + (gFlashCommandId[gIndex.IdIndex].ManuIDAddress << gFlashInfo[i].ParallelNum));     

        //Get Device Id
        TempDev1 = PortReadData(i, Base + (gFlashCommandId[gIndex.IdIndex].DeviceIDAddress1 << gFlashInfo[i].ParallelNum));
        TempDev2 = PortReadData(i, Base + (gFlashCommandId[gIndex.IdIndex].DeviceIDAddress2 << gFlashInfo[i].ParallelNum));
        TempDev3 = PortReadData(i, Base + (gFlashCommandId[gIndex.IdIndex].DeviceIDAddress3 << gFlashInfo[i].ParallelNum));
        DEBUG ((EFI_D_ERROR, "[cdtest]manufactor ID 0x%x!\n",TempData));
        DEBUG ((EFI_D_ERROR, "[cdtest]Device ID 1 0x%x!\n",TempDev1));
        DEBUG ((EFI_D_ERROR, "[cdtest]Device ID 2 0x%x!\n",TempDev2));
        DEBUG ((EFI_D_ERROR, "[cdtest]Device ID 3 0x%x!\n",TempDev3));
        //恢复到读模式
        FlashReset(Base);
        
        //匹配manuID与DeviceID
        if((0xffffffff != TempData)
            && (PortAdjustData(i, gFlashInfo[i].ManufacturerID) == TempData))
        {
            if((0xffffffff != TempDev1)
                && (PortAdjustData(i, gFlashInfo[i].DeviceID1) == TempDev1))
            {
                if((0xffffffff != TempDev2)
                    && (PortAdjustData(i, gFlashInfo[i].DeviceID2) == TempDev2))
                {
                    if((0xffffffff != TempDev3)
                        && (PortAdjustData(i, gFlashInfo[i].DeviceID3) == TempDev3))
                    {
                        Flag = 0;
                        gIndex.InfIndex = i;
                        break;
                    }
                }
            }
         }
    }
    //如果循环结束还没找到匹配的芯片信息，报错退出
    if(Flag)
    {
        return EFI_DEVICE_ERROR;
    }
        
    return EFI_SUCCESS;
}

/****************************************************************************
 函 数 名  : width8IsAll
 功能描述  : 判断指定区域的值是否全为某一个特定值
 输入参数  : Base               - Flash基地址
             Offset             - 偏移地址
             Length             - 输入数据
             Value              - 输入用于判断的值
 输出参数  : 无
 返 回 值  : TURE               - 是全等于输入的值
             FALSE              - 存在不等于输入值的地址
 
 修改历史  :

****************************************************************************/
static BOOLEAN width8IsAll(
    const UINT64       Base,
    const UINT64       Offset,
    const UINT64       Length,
    const UINT8        Value
)
{
    UINT64 NewAddr = Base + Offset;
    UINT64 NewLength = Length;
    while (NewLength --)
    {
        if (*(UINT8 *)(UINTN)NewAddr == Value)
        {
            NewAddr ++;
            continue;
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}


/****************************************************************************
 函 数 名  : BufferWriteCommand
 功能描述  : 发送buffer写命令字
 输入参数  : Base              基地址
             Offset            偏移地址
             pData             写入的数据
 输出参数  : 无
 返 回 值  : EFI_SUCCESS           - 成功
           EFI_NOT_READY           - 失败
 
 修改历史  :

****************************************************************************/
EFI_STATUS BufferWriteCommand(UINTN Base, UINTN Offset, void *pData)
{
    UINT32 dwCommAddr;
    UINT32 *pdwData;
    UINT16 *pwData;
    UINT32 dwLoop;
    UINT32 ulWriteWordCount;
    UINT32 dwAddr;

    if(gFlashBusy)	
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL]:Flash is busy!\n", __FUNCTION__,__LINE__));
        return EFI_NOT_READY;
    }
    gFlashBusy = TRUE;

    if(2 == gFlashInfo[gIndex.InfIndex].ParallelNum)
    {
        pdwData = (UINT32 *)pData;
      
        dwAddr = (UINT32)Base + (gFlashCommandWrite[gIndex.WIndex].BufferProgramAddressStep1 << gFlashInfo[gIndex.InfIndex].ParallelNum);
        (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandWrite[gIndex.WIndex].BufferProgramDataStep1);

        dwAddr = (UINT32)Base + (gFlashCommandWrite[gIndex.WIndex].BufferProgramAddressStep2 << gFlashInfo[gIndex.InfIndex].ParallelNum);
        (VOID)PortWriteData(gIndex.InfIndex, dwAddr,  gFlashCommandWrite[gIndex.WIndex].BufferProgramDataStep2);

        //dwAddr = Base + (Offset << gFlashInfo[gIndex.InfIndex].ParallelNum);
        dwAddr = (UINT32)Base + Offset;
        (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandWrite[gIndex.WIndex].BufferProgramDataStep3);
       
       //写入单位个数，个数固定
       ulWriteWordCount = ((gFlashInfo[gIndex.InfIndex].BufferProgramSize - 1) << 16) | (gFlashInfo[gIndex.InfIndex].BufferProgramSize - 1);
       (VOID)PortWriteData(gIndex.InfIndex, dwAddr, ulWriteWordCount);
       
       //写入数据
       for (dwLoop = 0; dwLoop < gFlashInfo[gIndex.InfIndex].BufferProgramSize; dwLoop ++)
       {
           dwCommAddr = (UINT32)Base + (UINT32)Offset + (dwLoop << gFlashInfo[gIndex.InfIndex].ParallelNum);   //16位 *2，32位 *4
           *(volatile UINT32 *)(UINTN)dwCommAddr = *pdwData;
           pdwData ++;            
       }
       
       dwAddr = (UINT32)Base + (UINT32)Offset + ((gFlashInfo[gIndex.InfIndex].BufferProgramSize - 1) << gFlashInfo[gIndex.InfIndex].ParallelNum);
       (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandWrite[gIndex.WIndex].BufferProgramtoFlash);

       
    //发送写命令字
    }
    else
    {
        pwData  = (UINT16 *)pData;
        
        dwAddr = (UINT32)Base + (gFlashCommandWrite[gIndex.WIndex].BufferProgramAddressStep1 << gFlashInfo[gIndex.InfIndex].ParallelNum);
        (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandWrite[gIndex.WIndex].BufferProgramDataStep1);
        
        dwAddr = (UINT32)Base + (gFlashCommandWrite[gIndex.WIndex].BufferProgramAddressStep2 << gFlashInfo[gIndex.InfIndex].ParallelNum);
        (VOID)PortWriteData(gIndex.InfIndex, dwAddr,  gFlashCommandWrite[gIndex.WIndex].BufferProgramDataStep2);
        
        //dwAddr = Base + (Offset << gFlashInfo[gIndex.InfIndex].ParallelNum);
        dwAddr = (UINT32)Base + Offset;
        (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandWrite[gIndex.WIndex].BufferProgramDataStep3);
        
        //写入单位个数，个数固定
        ulWriteWordCount = gFlashInfo[gIndex.InfIndex].BufferProgramSize - 1;
        (VOID)PortWriteData(gIndex.InfIndex, dwAddr, ulWriteWordCount);
        
        //写入数据
        for (dwLoop = 0; dwLoop < gFlashInfo[gIndex.InfIndex].BufferProgramSize; dwLoop ++)
        {
            dwCommAddr = (UINT32)Base + (UINT32)Offset + (dwLoop << gFlashInfo[gIndex.InfIndex].ParallelNum);   //16位 *2，32位 *4
            *(volatile UINT16 *)(UINTN)dwCommAddr = *pwData;
            pwData ++;
        }
        
        dwAddr = (UINT32)Base + (UINT32)Offset + ((gFlashInfo[gIndex.InfIndex].BufferProgramSize - 1) << gFlashInfo[gIndex.InfIndex].ParallelNum);
        (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandWrite[gIndex.WIndex].BufferProgramtoFlash);
        
    }

    (void)gBS->Stall(200);

    gFlashBusy = FALSE;
    return EFI_SUCCESS;

}

/****************************************************************************
 函 数 名  : SectorEraseCommand
 功能描述  : 擦除输入地址所在的sector
 输入参数  : UINT32 Offset     - 将被擦除的sector内任意地址
 输出参数  : 无
 返 回 值  : 无
 
 修改历史  :

****************************************************************************/
EFI_STATUS SectorEraseCommand(UINTN Base, UINTN Offset)
{
    UINT32 dwAddr;

    if(gFlashBusy)	
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL]:Flash is busy!\n", __FUNCTION__,__LINE__));
        return EFI_NOT_READY;
    }
    
    gFlashBusy = TRUE;

    dwAddr = (UINT32)Base + (gFlashCommandErase[gIndex.EIndex].SectorEraseAddressStep1 << gFlashInfo[gIndex.InfIndex].ParallelNum);
    (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandErase[gIndex.EIndex].SectorEraseDataStep1);
    
    dwAddr = (UINT32)Base + (gFlashCommandErase[gIndex.EIndex].SectorEraseAddressStep2 << gFlashInfo[gIndex.InfIndex].ParallelNum);
    (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandErase[gIndex.EIndex].SectorEraseDataStep2);
    
    dwAddr = (UINT32)Base + (gFlashCommandErase[gIndex.EIndex].SectorEraseAddressStep3 << gFlashInfo[gIndex.InfIndex].ParallelNum);
    (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandErase[gIndex.EIndex].SectorEraseDataStep3);

    dwAddr = (UINT32)Base + (gFlashCommandErase[gIndex.EIndex].SectorEraseAddressStep4 << gFlashInfo[gIndex.InfIndex].ParallelNum);
    (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandErase[gIndex.EIndex].SectorEraseDataStep4);

    dwAddr = (UINT32)Base + (gFlashCommandErase[gIndex.EIndex].SectorEraseAddressStep5 << gFlashInfo[gIndex.InfIndex].ParallelNum);
    (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandErase[gIndex.EIndex].SectorEraseDataStep5);

    dwAddr = (UINT32)Base + Offset;
    (VOID)PortWriteData(gIndex.InfIndex, dwAddr, gFlashCommandErase[gIndex.EIndex].SectorEraseDataStep6);

    (void)gBS->Stall(500000);
    
    gFlashBusy = FALSE;
    return EFI_SUCCESS;
}

/****************************************************************************
 函 数 名  : CompleteCheck
 功能描述  : 检测写或擦除操作是否完成
             只判断最后单位长度的数据
 输入参数  : Base              -flash基地址
             UINT32 Offset     - 检测用偏移地址
             void *pData       -输入值
             UINT32 Length       - 长度
 输出参数  : 无
 返 回 值  : EFI_SUCCESS			- 成功
            EFI_DEVICE_ERROR		- 错误码
 
 修改历史  :
****************************************************************************/
EFI_STATUS CompleteCheck(UINT32 Base, UINT32 Offset, void *pData, UINT32 Length)
{
    UINT32 dwTestAddr;    //指向最后一个单位的地址
    UINT32 dwTestData;    //取最后写入的一个单位数据
    UINT32 dwTemp = 0;    //读出用于判断的值        
    UINT32 dwTemp1 = 0;
    UINT32 i;
    UINT32 dwTimeOut = 3000000;  //最大循环次数(约3s)

    if(gFlashBusy)	
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL]:Flash is busy!\n", __FUNCTION__,__LINE__));
        return EFI_NOT_READY;
    }
    gFlashBusy = TRUE;
    
    if(2 == gFlashInfo[gIndex.InfIndex].ParallelNum)
    {
        dwTestAddr = Base + Offset + Length - sizeof(UINT32);
        dwTestData = *((UINT32 *)((UINT8 *)pData + Length - sizeof(UINT32)));

        while(dwTimeOut--)
        {
            dwTemp1 = *(volatile UINT32 *)(UINTN)(dwTestAddr);        
            if (dwTestData == dwTemp1)
            {
                dwTemp = *(volatile UINT32 *)(UINTN)(dwTestAddr);
                dwTemp1 = *(volatile UINT32 *)(UINTN)(dwTestAddr);
                if ((dwTemp == dwTemp1) && (dwTestData == dwTemp1))
                {
                    gFlashBusy = FALSE;
                    return EFI_SUCCESS;
                }
            }
             
            (void)gBS->Stall(1);
        }
        //判断两片并联芯片中是哪一块出错
        if((UINT16)(dwTemp1 >> 16) != (UINT16)(dwTestData >> 16))
        {
            DEBUG((EFI_D_ERROR, "CompleteCheck ERROR: chip1 address %x, buffer %x, flash %x!\n", Offset, dwTestData, dwTemp1));
        }
        if((UINT16)(dwTemp1) != (UINT16)(dwTestData))
        {
            DEBUG((EFI_D_ERROR, "CompleteCheck ERROR: chip2 address %x, buffer %x, flash %x!\n", Offset, dwTestData, dwTemp1));
        }
    }
    else
    {
        dwTestAddr = Base + Offset + Length - sizeof(UINT16);
        dwTestData = *((UINT16 *)((UINT8 *)pData + Length - sizeof(UINT16)));
        
        while(dwTimeOut--)
        {
            dwTemp1 = *(volatile UINT16 *)(UINTN)(dwTestAddr);        
            if (dwTestData == dwTemp1)
            {
                dwTemp = *(volatile UINT16 *)(UINTN)(dwTestAddr);
                dwTemp1 = *(volatile UINT16 *)(UINTN)(dwTestAddr);
                if ((dwTemp == dwTemp1) && (dwTestData == dwTemp1))
                {
                    gFlashBusy = FALSE;
                    return EFI_SUCCESS;
                }
            }
             
            (void)gBS->Stall(1);
        }
    }
               
    for(i = 0; i < 5; i ++)
    {
        DEBUG((EFI_D_ERROR, "CompleteCheck ERROR: flash %x\n",PortReadData(gIndex.InfIndex, dwTestAddr)));
    }
    
    FlashReset(Base);  //返回错误之前先复位，恢复到可读状态
    
    gFlashBusy = FALSE;
    DEBUG((EFI_D_ERROR, "CompleteCheck ERROR: timeout address %x, buffer %x, flash %x\n", Offset, dwTestData, dwTemp1));	
    return EFI_TIMEOUT;
}
/****************************************************************************
 函 数 名  : IsNeedToWrite
 功能描述  : 判断指定区域的值是否全为某一个特定值
 输入参数  : Offset             - 偏移地址
             Length             - 输入数据
             Value              - 输入用于判断的值
 输出参数  : 无
 返 回 值  : TURE               - 是全等于输入的值
            FALSE              - 存在不等于输入值的地址
 修改历史  :
****************************************************************************/
EFI_STATUS IsNeedToWrite(
    IN  UINT32         Base,
    IN  UINT32       Offset,
    IN  UINT8       *Buffer,
    IN  UINT32       Length
  )
{
    UINTN NewAddr = Base + Offset;
    UINT8 FlashData = 0;        //flash上的值
    UINT8 BufferData = 0;       //与flash值相对应的buffer值

    for(; Length > 0; Length --)
    {
        BufferData = *Buffer;
        //lint -epn -e511
        FlashData = *(UINT8 *)NewAddr;
        if (BufferData != FlashData)
        {
            return TRUE;
        }
        NewAddr ++;
        Buffer ++;
    }
    
    return FALSE;
}

/****************************************************************************
 函 数 名  : BufferWrite
 功能描述  : 写单位长度个字节，并检测是否写完成
 输入参数  : Offset     偏移地址 
             pData      写入值首地址
             Length 写入长度
 输出参数  : 无
 返 回 值  : EFI_SUCCESS           - 成功
             EFI_ABORTED        - 失败
 
 修改历史  :

****************************************************************************/
EFI_STATUS BufferWrite(UINT32 Offset, void *pData, UINT32 Length)
{
    EFI_STATUS Status;
    UINT32 dwLoop;
    UINT32 Retry = 3;
    
    if (FALSE == IsNeedToWrite(gIndex.Base, Offset, (UINT8 *)pData, Length))
    {
        return EFI_SUCCESS;
    }

    do
    {
        (void)BufferWriteCommand(gIndex.Base, Offset, pData);
        Status = CompleteCheck(gIndex.Base, Offset, pData, Length);
     
        //如果写完成，判断写入值是否正确
        if (EFI_SUCCESS == Status)
        {
            for (dwLoop = 0; dwLoop < Length; dwLoop ++)
            {
                if (*(UINT8 *)(UINTN)(gIndex.Base + Offset + dwLoop) != *((UINT8 *)pData + dwLoop))
                {
                    DEBUG((EFI_D_ERROR, "Flash_WriteUnit ERROR: address %x, buffer %x, flash %x\n", Offset, *((UINT8 *)pData + dwLoop), *(UINT8 *)(UINTN)(gIndex.Base + Offset + dwLoop)));
                    Status = EFI_ABORTED;
                    continue;
                }
            }
        }
        else
        {
            DEBUG((EFI_D_ERROR, "Flash_WriteUnit ERROR: complete check failed, %r\n", Status));
            continue;
        }
    } while ((Retry--) && EFI_ERROR(Status));

    return Status;
}

/****************************************************************************
 函 数 名  : SectorErase
 功能描述  : 擦除一个Sector，并检测是否完成，检测是否已用首字节检测
 输入参数  : Base               基地址
             Offset            偏移地址
 输出参数  : 无
 返 回 值  : EFI_SUCCESS           - 成功
            EFI_ABORTED        - 失败
 
 修改历史  :

****************************************************************************/
EFI_STATUS SectorErase(UINT32 Base, UINT32 Offset)
{
    UINT8 gTemp[FLASH_MAX_UNIT];
    UINT64 dwLoop = FLASH_MAX_UNIT - 1;
    UINT32 Retry = 3;
    EFI_STATUS Status;
    
    do
    {
        gTemp[dwLoop] = 0xFF;
    }while (dwLoop --);

    do  
    {
        (void)SectorEraseCommand(Base, Offset);
        Status = CompleteCheck(Base, Offset, (void *)gTemp, FLASH_MAX_UNIT);
        
        //如果写完成，判断写入值是否正确
        if (EFI_SUCCESS == Status)
        {
            //检测整个Sector是否已为全0xFF，Offset要指向sector的起始
            if (width8IsAll(Base,Offset - (Offset % gFlashInfo[gIndex.InfIndex].BlockSize), gFlashInfo[gIndex.InfIndex].BlockSize, 0xFF))
            {
                return EFI_SUCCESS;
            }
            else
            {
                DEBUG((EFI_D_ERROR, "Flash_SectorErase ERROR: not all address equal 0xFF\n"));
                //先修改Status再continue，否则while判断会结束
                Status = EFI_ABORTED;
                continue;
            }
        }
        else
        {
            DEBUG((EFI_D_ERROR, "Flash_SectorErase ERROR: complete check failed, %r\n", Status));
            continue;
        }
    }while ((Retry--) && EFI_ERROR(Status));

    if(Retry)
    {
        //do nothing for pclint
    }
    
    return Status;
}
