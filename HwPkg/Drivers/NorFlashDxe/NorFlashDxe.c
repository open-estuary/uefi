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
  文 件 名   : NorFlashDxe.c
  版 本 号   : v2.0
  作    者   : c00213799
  生成日期   : 2013年03月04日
  最近修改   :
  功能描述   : Flash接口
  修改历史   :
1.   日	  期   : 
     作	  者   :
     修改内容  :
******************************************************************************/
#include <Uefi.h>
//#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Library/PcdLib.h>
#include <Protocol/NorFlashProtocol.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/Cpu.h>
#include "NorFlashHw.h"
extern EFI_GUID gUniNorFlashProtocolGuid;


EFI_STATUS Erase(
   IN UNI_NOR_FLASH_PROTOCOL   *This,
   IN  UINT32                   Offset,
   IN  UINT32                   Length
  );

EFI_STATUS  Write(
  IN UNI_NOR_FLASH_PROTOCOL   *This,
  IN  UINT32                  Offset,
  IN  UINT8                  *Buffer,
  UINT32                     ulLength
  );

EFI_STATUS Read(   
  IN UNI_NOR_FLASH_PROTOCOL   *This,
  IN UINT32                   Offset,
  IN OUT UINT8               *Buffer,
  IN UINT32                   ulLen
  );

UNI_NOR_FLASH_PROTOCOL gUniNorFlash = {
    Erase,
    Write,
    Read
};

/****************************************************************************
 函 数 名   :  Read
 功能描述  : 
 输入参数  : 
 输出参数  : 无
 返 回 值  : EFI_SUCCESS        - 操作成功
           EFI_INVALID_PARAMETER  -输入参数错误
 修改历史  :
****************************************************************************/
EFI_STATUS
EFIAPI Read(   
  IN UNI_NOR_FLASH_PROTOCOL   *This,
  IN UINT32                   Offset,
  IN OUT UINT8               *Buffer,
  IN UINT32                    ulLen
  )
{
    UINT32       index;
    UINT64 ullAddr;
    UINT32 ullCnt = 0;
    UINT32 *puiBuffer32 = NULL;
    UINT32 *puiDst32 = NULL;
    UINT8 *pucBuffer8 = NULL;
    UINT8 *pucDst8 = NULL;

    if (Offset + ulLen > (gFlashInfo[gIndex.InfIndex].SingleChipSize * gFlashInfo[gIndex.InfIndex].ParallelNum))
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Exceed the flash scope!\n", __FUNCTION__,__LINE__));
        return EFI_INVALID_PARAMETER;
    }
    if (0 == ulLen)
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Length is Zero!\n", __FUNCTION__,__LINE__));
        return EFI_INVALID_PARAMETER;
    }
    if (NULL == Buffer)
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Buffer is NULL!\n", __FUNCTION__,__LINE__));
        return EFI_BAD_BUFFER_SIZE;
    }


    ullAddr = gIndex.Base + Offset;

    pucBuffer8 = (UINT8 *)Buffer;
    pucDst8    = (UINT8 *)((UINTN)ullAddr);

    /* 长度小于4，直接按字节读取 */
    if (ulLen < FOUR_BYTE_UNIT)
    {
        for(index = 0; index< ulLen; index++)
        {
            *pucBuffer8++ = *pucDst8++;
        }
    }
    else /* 长度大于等于4，进行按4字节对齐读取处理 */
    {

        ullCnt = Offset % FOUR_BYTE_UNIT;
        ullCnt = FOUR_BYTE_UNIT - ullCnt;

        for(index = 0; index < ullCnt; index++)
        {
            *pucBuffer8++ = *pucDst8++;
        }

        ulLen -= ullCnt;

        puiBuffer32 = (UINT32 *)pucBuffer8;
        puiDst32    = (UINT32 *)pucDst8;
        ullCnt      = ulLen / FOUR_BYTE_UNIT;

        for(index = 0; index < ullCnt; index++)
        {
            *puiBuffer32++ = *puiDst32++;
        }

        ullCnt     = ulLen % FOUR_BYTE_UNIT;
        pucBuffer8 = (UINT8 *)puiBuffer32;
        pucDst8    = (UINT8 *)puiDst32;

        for(index = 0; index < ullCnt; index++)
        {
            *pucBuffer8++ = *pucDst8++;
        }
    }

    return EFI_SUCCESS;
}


/****************************************************************************
 函 数 名 : WriteAfterErase_Fill
 功能描述  : 补数据写入一个单元，当起始或结束基地址与数据单元不对齐时调用
             调用者必须保证所写入数据不跨单元
             调用此函数前必须保证被写区间已不需要擦除
 输入参数  : Offset             - 偏移地址
             Buffer             - 输入数据
             Length             - 长度
 输出参数  : 无
 返 回 值  : EFI_SUCCESS        - 操作成功
             EFI_DEVICE_ERROR   - 设备错误

 修改历史  :

****************************************************************************/
static EFI_STATUS WriteAfterErase_Fill(
    IN  const UINT32       Offset,
    IN  const UINT8       *Buffer,
    IN  const UINT32       Length
    )
{
    EFI_STATUS Status;
    UINT32 Loop;
    UINT32 DataOffset;       //实际写入数据在新数据单元中的起始位置
    UINT32 NewOffset;       //数据重组后新的写入地址
    UINT8 *NewDataUnit;     //新数据单元
    
    UINT32 FlashUnitLength;

    FlashUnitLength = gFlashInfo[gIndex.InfIndex].BufferProgramSize << gFlashInfo[gIndex.InfIndex].ParallelNum;
    //参数检查
    if (0 == Length)
    {
        return EFI_SUCCESS;
    }
    if ((Offset % FlashUnitLength + Length) > FlashUnitLength)
    {
        DEBUG ((EFI_D_INFO, "[%a]:[%dL]:Exceed the Flash Size!\n", __FUNCTION__,__LINE__));
        return EFI_UNSUPPORTED;
    }

    //为新数据单元分配空间
    Status = gBS->AllocatePool(EfiBootServicesData, FlashUnitLength, (VOID *)&NewDataUnit);
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Allocate Pool failed, %r!\n", __FUNCTION__,__LINE__, Status));
        return Status;
    }

    //重组新数据单元
    NewOffset = Offset - (Offset % FlashUnitLength);
    
    gBS->CopyMem((VOID *)NewDataUnit, (VOID *)(UINTN)(gIndex.Base + NewOffset), FlashUnitLength);
    
    DataOffset = Offset % FlashUnitLength;
    for (Loop = 0; Loop < Length; Loop ++)
    {
        NewDataUnit[(UINT32)(DataOffset + Loop)] = Buffer[Loop];
    }
    
    Status = BufferWrite(NewOffset, (void *)NewDataUnit, FlashUnitLength);
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:BufferWrite %r!\n", __FUNCTION__,__LINE__, Status));
        return Status;
    }

    (void)gBS->FreePool((VOID *)NewDataUnit);
    return Status;
}

/****************************************************************************
 函 数 名  : WriteAfterErase_Final
 功能描述  : 最终写操作，当保证起始地址为单元起始后，调用此函数写入所有数据
             调用此函数前必须保证被写区间已不需要擦除
             调用者必须保证StartAddress为单元起始地址
 输入参数  : Offset             - 偏移地址
             Buffer             - 输入数据
             Length             - 长度
 输出参数  : 无
 返 回 值  : EFI_SUCCESS        - 操作成功
             EFI_DEVICE_ERROR   - 设备错误

 修改历史  :

****************************************************************************/
static EFI_STATUS WriteAfterErase_Final(
    IN  UINT32       Offset,
    IN  UINT8       *Buffer,
    IN  UINT32       Length
    )
{
    EFI_STATUS Status;
    UINT32 Loop;
    UINT32 FlashUnitLength;

    FlashUnitLength = gFlashInfo[gIndex.InfIndex].BufferProgramSize << gFlashInfo[gIndex.InfIndex].ParallelNum;
    //参数检查
    if (0 == Length)
    {
        return EFI_SUCCESS;
    }
    //StartAddress必须是单元起始
    if (0 != (Offset % FlashUnitLength))
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]: Offset must be a multiple of 0x%x!\n", __FUNCTION__,__LINE__,FlashUnitLength));
        return EFI_UNSUPPORTED;
    }

    //写入前面部分，到剩余长度不足一个数据单元为止
    Loop = Length / FlashUnitLength;
    while (Loop --)
    {
        Status = BufferWrite(Offset, (void *)Buffer, FlashUnitLength);
        if (EFI_ERROR(Status))
        {
            DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:BufferWrite Failed: %r!\n", __FUNCTION__,__LINE__, Status));
            return EFI_DEVICE_ERROR;
        }
        Offset += FlashUnitLength;
        Buffer += FlashUnitLength;
    }

    //写入剩余的不足一个单元的数据
    Length = Length % FlashUnitLength;
    if (Length)
    {
        Status = WriteAfterErase_Fill(Offset, Buffer, Length);
        if (EFI_ERROR(Status))
        {
            DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:WriteAfterErase_Fill failed,%r!\n", __FUNCTION__,__LINE__, Status));
            return Status;
        }
    }
    
    return EFI_SUCCESS;
}
/****************************************************************************
 函 数 名  : WriteAfterErase
 功能描述  : 实现任意长度的写，调用此函数前必须保证被写区间已不需要擦除
 输入参数  : Offset            偏移地址
 输出参数  : 无
 返 回 值  : EFI_SUCCESS           - 成功
            EFI_ABORTED        - 失败
 
 修改历史  :

****************************************************************************/
EFI_STATUS
WriteAfterErase(
    UINT32       TempBase,
    UINT32       Offset,
    UINT8       *Buffer,
    UINT32       Length
  )
{
    EFI_STATUS Status;
    UINT32 FlashUnitLength;

    FlashUnitLength = gFlashInfo[gIndex.InfIndex].BufferProgramSize << gFlashInfo[gIndex.InfIndex].ParallelNum;
    
    if (0 == Length)
    {
        return EFI_SUCCESS;
    }

    //起始地址不是单元起始，则先写入第一个单元，使下一步写入的起始地址是单元起始
    if (Offset % FlashUnitLength)
    {
        UINT32 TempLength;

        //只写入起始地址所在的数据单元
        TempLength = FlashUnitLength - (Offset % FlashUnitLength);
        if (TempLength > Length)
        {
            TempLength = Length;
        }
        Status = WriteAfterErase_Fill(Offset, Buffer, TempLength);
        if (EFI_ERROR(Status))
        {
            DEBUG ((EFI_D_ERROR, "[%a]:[%dL]: %r!\n", __FUNCTION__,__LINE__, Status));
            return Status;
        }
        //更新参数
        Offset += TempLength;
        Length -= TempLength;
        Buffer += TempLength;

        //Desc:if Offset >= gOneFlashSize,modify base
        if (0 < (Offset / gFlashInfo[gIndex.InfIndex].SingleChipSize))
        {
            TempBase += gFlashInfo[gIndex.InfIndex].SingleChipSize;
            gIndex.Base = TempBase;
            Offset = 0;
        }
    }

    //满足起始地址为单元起始后，写入剩余部分数据
    Status = WriteAfterErase_Final(Offset, Buffer, Length);
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]: %r!\n", __FUNCTION__,__LINE__, Status));
        return Status;
    }

    return EFI_SUCCESS;
}

/****************************************************************************
 函 数 名  : FlashSectorErase
 功能描述  : 从起始地址开始，擦除指定长度的空间，不跨Sector
             注意本操作中内存的释放
             调用者必须保证所要擦除的地址在同一个Sector内
 输入参数  : Offset   - 起始偏移地址
             Length         - 长度
 输出参数  : 无
 返 回 值  : Status

 修改历史  :

****************************************************************************/
EFI_STATUS 
FlashSectorErase(
    UINT32      TempBase,
    UINT32      Offset,
    UINT32      Length
  )
{
    EFI_STATUS  Status;
    UINT32 SectorOffset;    //擦除区起始地址所在flash sector首地址(绝对地址)
    UINT8 *StaticBuffer;
    UINT8 *Buffer;          //回写数据区指针
    UINT32 TempOffset;      //回写时的起始地址
    UINT32 TempLength;       //回写时的长度
    UINT32 LeftLength;       //起始地址所在Sector的剩余空间
    

    if (0 == Length)
    {
        return EFI_SUCCESS;
    }
    //如果所要擦除的地址不在同一个Sector内，则返回错误
    LeftLength = gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum - (Offset % (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum));
    if (LeftLength < Length)
    {
        return EFI_UNSUPPORTED;
    }

    //计算起始地址所在Sector的起始地址
    SectorOffset = Offset - (Offset % (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum));
    
    Status = gBS->AllocatePool(EfiBootServicesData, gFlashInfo[gIndex.InfIndex].BlockSize * (UINTN)gFlashInfo[gIndex.InfIndex].ParallelNum, (VOID *)&StaticBuffer);
    if (EFI_ERROR(Status))
    {
        return Status;
    }
    
    Buffer = StaticBuffer;
    
    gBS->CopyMem((VOID *)Buffer, (VOID *)(UINTN)(TempBase + SectorOffset), 
                 (gFlashInfo[gIndex.InfIndex].BlockSize * (UINTN)gFlashInfo[gIndex.InfIndex].ParallelNum));

    //擦除
    Status = SectorErase(TempBase, SectorOffset);
    if (EFI_ERROR(Status))
    {
        goto DO;
    }
    
    //回写前面部分
    TempOffset = SectorOffset;
    TempLength = Offset % (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum);

    Status = WriteAfterErase(TempBase, TempOffset, Buffer, TempLength);
    if (EFI_ERROR(Status))
    {
        goto DO;
    }
    
    //回写后面部分，注意buffer的偏移
    //因前后回写用的是同一个buffer，因此要加上擦除的长度
    Buffer = Buffer + TempLength + Length;
    TempOffset = Offset + Length;
    TempLength = SectorOffset + (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum) - TempOffset;

    Status = WriteAfterErase(TempBase, TempOffset, Buffer, TempLength);
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]: %r!\n", __FUNCTION__,__LINE__,Status));
        goto DO;
    }
        
    (void)gBS->FreePool((VOID *)StaticBuffer);    
    return EFI_SUCCESS;
    
DO:
    (void)gBS->FreePool((VOID *)StaticBuffer);
    return Status;
}

/****************************************************************************
 函 数 名  : Erase
 功能描述  : 实现任意长度的擦
 输入参数  : Offset                 偏移地址
             Length                擦除长度
 输出参数  : 无
 返 回 值  : EFI_SUCCESS           - 成功
            EFI_ABORTED        - 失败
 
 修改历史  :

****************************************************************************/
EFI_STATUS
EFIAPI Erase(
   IN UNI_NOR_FLASH_PROTOCOL   *This,
   IN UINT32                   Offset,
   IN UINT32                   Length
  )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT32 Sectors;         //起始地址与结束地址之间间隔的sector数
    UINT32 TempLength;
    UINT32 TempBase;
    UINT32 Loop;

    //判断入参
    if (Offset + Length > (gFlashInfo[gIndex.InfIndex].SingleChipSize * gFlashInfo[gIndex.InfIndex].ParallelNum))
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Exceed the Flash Size!\n", __FUNCTION__,__LINE__));
        return EFI_ABORTED;
    }
    if (0 == Length)
    {
        return EFI_SUCCESS;
    }

    //计算本次擦除涉及的块数，值为结束地址所在块号 - 起始地址所在块号 + 1
    //结束地址为起始地址 + 长度 - 1
    Sectors = ((Offset + Length - 1) / (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum)) - (Offset / (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum)) + 1;
    TempBase = gIndex.Base;
    
    //if Offset >= gOneFlashSize,modify base
    if(0 < (Offset / gFlashInfo[gIndex.InfIndex].SingleChipSize))
    {
        TempBase +=  gFlashInfo[gIndex.InfIndex].SingleChipSize * (Offset/gFlashInfo[gIndex.InfIndex].SingleChipSize);
        Offset = Offset - (Offset & gFlashInfo[gIndex.InfIndex].SingleChipSize);
    }

    for (Loop = 0; Loop <= Sectors; Loop ++)
    {
        //保证所擦除的地址在同一个Sector内
        TempLength = gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum - (Offset % (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum));

        //若剩余空间足够满足所需擦除的长度，则只擦除要求的长度
        if (TempLength > Length)
        {
            TempLength = Length;
        }
        
        Status = FlashSectorErase(TempBase, Offset, TempLength);
        if (EFI_ERROR(Status))
        {
            DEBUG ((EFI_D_ERROR, "[%a]:[%dL]: FlashErase One Sector Error, Status = %r!\n", __FUNCTION__,__LINE__,Status));
            return Status;
        }
        
        Offset += TempLength;
        
         //if Offset >= gOneFlashSize,modify base
        if (0 < (Offset / gFlashInfo[gIndex.InfIndex].SingleChipSize))
        {
            TempBase += gFlashInfo[gIndex.InfIndex].SingleChipSize;
            Offset = 0;
        }
        Length -= TempLength;
    }

    return Status;
}

/****************************************************************************
 函 数 名  : Write
 功能描述  : 实现任意长度的写
 输入参数  : Offset                  写入的偏移地址
             Buffer                  写入的数据
             ulLength                长度
 输出参数  : 无
 返 回 值  : EFI_SUCCESS           - 成功
           EFI_INVALID_PARAMETER        - 失败
 修改历史  :
****************************************************************************/
EFI_STATUS
EFIAPI Write(
  IN UNI_NOR_FLASH_PROTOCOL   *This,
  IN UINT32                   Offset,
  IN UINT8                   *Buffer,
  UINT32                     ulLength
  )
{
    EFI_STATUS  Status;
    UINT32     TempLength;
    UINT32       TempBase;
    UINT32           Loop;
    UINT32        Sectors;         //起始地址与结束地址之间间隔的sector数

    if((Offset + ulLength) > (gFlashInfo[gIndex.InfIndex].SingleChipSize * gFlashInfo[gIndex.InfIndex].ParallelNum))
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Exceed the Flash Size!\n", __FUNCTION__,__LINE__));
        return EFI_INVALID_PARAMETER;
    }
    if (0 == ulLength)
    {
        return EFI_SUCCESS;
    }
    
    //计算本次擦除涉及的块数，值为结束地址所在块号 - 起始地址所在块号 + 1
    //结束地址为起始地址 + 长度 - 1
    Sectors = ((Offset + ulLength - 1) / (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum)) - (Offset / (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum)) + 1;
    TempBase = gIndex.Base;
    
    //if Offset >= gOneFlashSize,modify base
    if(0 < (Offset / gFlashInfo[gIndex.InfIndex].SingleChipSize))
    {
        TempBase +=  gFlashInfo[gIndex.InfIndex].SingleChipSize * (Offset/gFlashInfo[gIndex.InfIndex].SingleChipSize);
        Offset = Offset - (Offset & gFlashInfo[gIndex.InfIndex].SingleChipSize);
    }

    for (Loop = 0; Loop <= Sectors; Loop ++)
    {
        //保证所擦除的地址在同一个Sector内
        TempLength = gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum - (Offset % (gFlashInfo[gIndex.InfIndex].BlockSize * gFlashInfo[gIndex.InfIndex].ParallelNum));

        //若剩余空间足够满足所需擦除的长度，则只擦除要求的长度
        if (TempLength > ulLength)
        {
            TempLength = ulLength;
        }

        //判断是否需要擦除
        if (TRUE == IsNeedToWrite(TempBase, Offset, Buffer, TempLength))
        {
            Status = FlashSectorErase(TempBase, Offset, TempLength);
            if (EFI_ERROR(Status))
            {
                DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:FlashErase One Sector Error, Status = %r!\n", __FUNCTION__,__LINE__,Status));
                return Status;
            }

            //需要擦除，才需要写入，输入地址为擦除偏移地址，长度为擦除偏移长度
            Status = WriteAfterErase(TempBase, Offset, Buffer, TempLength);
            if (EFI_ERROR(Status))
            {
                DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:WriteAfterErase Status = %r!\n", __FUNCTION__,__LINE__,Status));
                return Status;
            }
        }
        
        Offset += TempLength;
        Buffer += TempLength;
        
         //if Offset >= gOneFlashSize,modify base
        if (0 < (Offset / gFlashInfo[gIndex.InfIndex].SingleChipSize))
        {
            TempBase += gFlashInfo[gIndex.InfIndex].SingleChipSize;
            Offset = 0;
        }
        ulLength -= TempLength;
    }
    
    return EFI_SUCCESS;
}


VOID SetFlashAttributeToUncache(VOID)
{
    EFI_CPU_ARCH_PROTOCOL             *gCpu           = NULL;
    EFI_STATUS Status;
    
    Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **)&gCpu);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "LocateProtocol gEfiCpuArchProtocolGuid Status = %r !\n", Status)); 
    }
    
    Status = gCpu->SetMemoryAttributes(
                     gCpu,
                     PcdGet64(PcdNORFlashBase),
                     PcdGet32(PcdNORFlashCachableSize),
                     EFI_MEMORY_UC
                     );
    
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "gCpu->SetMemoryAttributes Status = %r !\n", Status)); 
    }

}

EFI_STATUS
EFIAPI InitializeFlash (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable)
{
    EFI_STATUS Status;
        
    
    gIndex.Base = (UINT32)PcdGet64(PcdNORFlashBase);   

    SetFlashAttributeToUncache();
    Status = FlashInit(gIndex.Base);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "Init Flash Error !\n")); 
        return Status;
    }
    else
    {
        DEBUG((EFI_D_ERROR, "Init Flash OK!\n")); 
    }

    Status = gBS->InstallProtocolInterface (
                            &ImageHandle,
                            &gUniNorFlashProtocolGuid,
                            EFI_NATIVE_INTERFACE,
                            &gUniNorFlash);
    if(EFI_SUCCESS != Status)
    {
        DEBUG ((EFI_D_ERROR, "[%a]:[%dL]:Install Protocol Interface %r!\n", __FUNCTION__,__LINE__,Status));
    }
    
    return Status;
}
