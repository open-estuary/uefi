/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7olUFxOmtN3B1EGXlDjDOIHS2BtBe4K/qeFNYwA0dcNVVgoWNacK46c+nqzHS
sG1eIg8F8jmAKvpmvjDLo4CzhvoDD5TSdDw21WaaR0seoe2E5CZcg8c62ZCHxtay7MCrOTmB
SKi2Z0uaBhaqhhqyjC2rmkhZVX9eVt1zKdh1ug1496VSBOrgGFgMeOIld4EjDw==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : I2CLib.c
  版 本 号   : v2.0
  作    者   : c00213799
  生成日期   : 2013年03月15日
  最近修改   :
  功能描述   :
  修改历史   :
******************************************************************************/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/I2CLib.h>
#include <Library/TimerLib.h>

#include <PlatformArch.h>
#include <Library/PlatformSysCtrlLib.h>

#include "I2CHw.h"

/*****************************************************************************
 函 数 名  : I2C_Delay
 功能描述  : 延时函数
 输入参数  : UINT32 ulCount
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
VOID I2C_Delay(UINT32 ulCount)
{
    MicroSecondDelay(ulCount);
    return;
}

/*****************************************************************************
 函 数 名  :  I2c_Disable
 功能描述  : 
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
EFI_STATUS
EFIAPI
I2C_Disable(UINT32 Socket,UINT8 Port)
{
    UINT32                  ulTimeCnt = I2C_READ_TIMEOUT;
    I2C0_STATUS_U           I2cStatusReg;
    I2C0_ENABLE_U           I2cEnableReg;
    I2C0_ENABLE_STATUS_U    I2cEnableStatusReg;

    UINTN Base = PlatformGetI2cBase(Socket, Port);
    
    I2C_REG_READ((Base + I2C_STATUS_OFFSET), I2cStatusReg.Val32);
    
    while (I2cStatusReg.bits.activity)     /* 表示此时正在进行I2C访问，需要等待 */
    {
        I2C_Delay(10000);     /* 具体时间待定  */

        ulTimeCnt--;
        I2C_REG_READ(Base + I2C_STATUS_OFFSET, I2cStatusReg.Val32);
        if (0 == ulTimeCnt)         /* 表示已经超时，此时应该报错  */
        {
            return EFI_DEVICE_ERROR;
        }
    }

    //关闭I2C
    I2C_REG_READ(Base + I2C_ENABLE_OFFSET, I2cEnableReg.Val32);
    I2cEnableReg.bits.enable = 0;
    I2C_REG_WRITE(Base + I2C_ENABLE_OFFSET,I2cEnableReg.Val32);
    //检查是否关闭成功
    I2C_REG_READ(Base + I2C_ENABLE_OFFSET,I2cEnableStatusReg.Val32);
    if (0 == I2cEnableStatusReg.bits.ic_en)
    {
        return EFI_SUCCESS;
    }
    else
    {
        return EFI_DEVICE_ERROR;
    }
}

/*****************************************************************************
 函 数 名  :  I2C_Enable
 功能描述  : 
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
EFI_STATUS
EFIAPI
I2C_Enable(UINT32 Socket,UINT8 Port)
{
    I2C0_ENABLE_U           I2cEnableReg;
    I2C0_ENABLE_STATUS_U    I2cEnableStatusReg;
    
    UINTN Base = PlatformGetI2cBase(Socket, Port);
    
    //打开I2C
    I2C_REG_READ(Base + I2C_ENABLE_OFFSET, I2cEnableReg.Val32);
    I2cEnableReg.bits.enable = 1;
    I2C_REG_WRITE(Base + I2C_ENABLE_OFFSET, I2cEnableReg.Val32);

    //检查是否成功
    I2C_REG_READ(Base + I2C_ENABLE_STATUS_OFFSET, I2cEnableStatusReg.Val32);
    if (1 == I2cEnableStatusReg.bits.ic_en)
    {
        return EFI_SUCCESS;
    }
    else
    {
        return EFI_DEVICE_ERROR;
    }
}
/*****************************************************************************
 函 数 名  :  I2C_SetTarget
 功能描述  : 
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void I2C_SetTarget(UINT32 Socket,UINT8 Port,UINT32 I2cDeviceAddr)
{
    I2C0_TAR_U    I2cTargetReg;
    UINTN Base = PlatformGetI2cBase(Socket, Port);

    //设置slave地址
    I2C_REG_READ(Base + I2C_TAR_OFFSET, I2cTargetReg.Val32);
    I2cTargetReg.bits.ic_tar = I2cDeviceAddr;
    I2C_REG_WRITE(Base + I2C_TAR_OFFSET, I2cTargetReg.Val32);

    return;
}

/*****************************************************************************
 函 数 名  :  I2CInit
 功能描述  : 初始化，以便读取SPD信息
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
*****************************************************************************/
EFI_STATUS
EFIAPI
I2CInit(UINT32 Socket, UINT32 Port, SPEED_MODE SpeedMode)
{
    I2C0_CON_U              I2cControlReg;
    I2C0_SS_SCL_HCNT_U      I2cStandardSpeedSclHighCount;
    I2C0_SS_SCL_LCNT_U      I2cStandardSpeedSclLowCount;
    I2C0_RX_TL_U            I2cRxFifoReg;
    I2C0_TX_TL_U            I2cTxFifoReg;
    I2C0_INTR_MASK_U        I2cIntrMask;
    EFI_STATUS              Status;

    UINTN Base = PlatformGetI2cBase(Socket, Port);

    if((Socket >= MAX_SOCKET) || (Port >= I2C_PORT_MAX) || (SpeedMode >= SPEED_MODE_MAX)){
        return EFI_INVALID_PARAMETER;
    }

    //关闭I2C
    Status = I2C_Disable(Socket,Port);
    if(EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;
    }

    //配置传输模式、速率等
    I2C_REG_READ(Base + I2C_CON_OFFSET, I2cControlReg.Val32);
    I2cControlReg.bits.master = 1;      //主模式使能，默认值
    I2cControlReg.bits.spedd = 0x1;    //标准模式，标准模式下I2C总线的速率为100kb/s
    I2cControlReg.bits.restart_en = 1;  //主模式下restart指令发送使能
    I2cControlReg.bits.slave_disable = 1;   //从模式不使能
    I2C_REG_WRITE(Base + I2C_CON_OFFSET,I2cControlReg.Val32);

    //设置I2C总线时钟周期
    if(Normal == SpeedMode)
    {
        I2C_REG_READ(Base + I2C_SS_SCL_HCNT_OFFSET,I2cStandardSpeedSclHighCount.Val32);
        I2cStandardSpeedSclHighCount.bits.ic_ss_scl_hcnt = I2C_SS_SCLHCNT;
        I2C_REG_WRITE(Base + I2C_SS_SCL_HCNT_OFFSET, I2cStandardSpeedSclHighCount.Val32);
        I2C_REG_READ(Base + I2C_SS_SCL_LCNT_OFFSET, I2cStandardSpeedSclLowCount.Val32);
        I2cStandardSpeedSclLowCount.bits.ic_ss_scl_lcnt = I2C_SS_SCLLCNT;
        I2C_REG_WRITE(Base + I2C_SS_SCL_LCNT_OFFSET, I2cStandardSpeedSclLowCount.Val32);
    }
    else
    {
        I2C_REG_READ(Base + I2C_FS_SCL_HCNT_OFFSET,I2cStandardSpeedSclHighCount.Val32);
        I2cStandardSpeedSclHighCount.bits.ic_ss_scl_hcnt = I2C_SS_SCLHCNT; 
        I2C_REG_WRITE(Base + I2C_FS_SCL_HCNT_OFFSET, I2cStandardSpeedSclHighCount.Val32);
        I2C_REG_READ(Base + I2C_FS_SCL_LCNT_OFFSET, I2cStandardSpeedSclLowCount.Val32);
        I2cStandardSpeedSclLowCount.bits.ic_ss_scl_lcnt = I2C_SS_SCLLCNT; 
        I2C_REG_WRITE(Base + I2C_FS_SCL_LCNT_OFFSET, I2cStandardSpeedSclLowCount.Val32);
    }

    //设置发送、接收FIFO阈值
    I2C_REG_READ(Base + I2C_RX_TL_OFFSET, I2cRxFifoReg.Val32);
    I2cRxFifoReg.bits.rx_tl = I2C_TXRX_THRESHOLD;
    I2C_REG_WRITE(Base + I2C_RX_TL_OFFSET, I2cRxFifoReg.Val32);
    I2C_REG_READ(Base + I2C_TX_TL_OFFSET,I2cTxFifoReg.Val32);
    I2cTxFifoReg.bits.tx_tl = I2C_TXRX_THRESHOLD;
    I2C_REG_WRITE(Base + I2C_TX_TL_OFFSET, I2cTxFifoReg.Val32);

    //禁止所有中断
    I2C_REG_READ(Base + I2C_INTR_MASK_OFFSET, I2cIntrMask.Val32);
    I2cIntrMask.Val32 = 0x0;
    I2C_REG_WRITE(Base + I2C_INTR_MASK_OFFSET, I2cIntrMask.Val32);

    //使能I2C
    Status = I2C_Enable(Socket,Port);
    if(EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
I2CSdaConfig(UINT32 Socket, UINT32 Port)
{
    
    UINTN Base = PlatformGetI2cBase(Socket, Port);

    if((Socket >= MAX_SOCKET) || (Port >= I2C_PORT_MAX)){
        return EFI_INVALID_PARAMETER;
    }
    //SDA延迟时间
    I2C_REG_WRITE(Base + I2C_SDA_HOLD, 0x14);
    
    return EFI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : I2C_GetTxStatus
 功能描述  : 获取TX FIFO状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
UINT32 I2C_GetTxStatus(UINT32 Socket,UINT8 Port)
{
    I2C0_TXFLR_U ulFifo;
    UINTN Base = PlatformGetI2cBase(Socket, Port);

    I2C_REG_READ(Base + I2C_TXFLR_OFFSET, ulFifo.Val32);
    return ulFifo.bits.txflr;
}

/*****************************************************************************
 函 数 名  : I2C_GetRxStatus
 功能描述  : 获取TX FIFO状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :
*****************************************************************************/
UINT32
I2C_GetRxStatus(UINT32 Socket,UINT8 Port)
{
    I2C0_RXFLR_U ulFifo;
    UINTN Base = PlatformGetI2cBase(Socket, Port);

    I2C_REG_READ(Base + I2C_RXFLR_OFFSET, ulFifo.Val32);
    return ulFifo.bits.rxflr;
}
/*****************************************************************************
 函 数 名  : WriteBeforeRead
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
          1. 2013.4.12 c00213799 对接器件读时序差异
*****************************************************************************/
EFI_STATUS
EFIAPI
WriteBeforeRead(I2C_DEVICE *I2cInfo, UINT32 ulLength, UINT8 *pBuf)
{
    UINT32 ulFifo;
    UINT32 ulCnt;
    UINT32 ulTimes = 0;

    UINTN  Base = PlatformGetI2cBase(I2cInfo->Socket, I2cInfo->Port);

    //设置slave地址为DIMM0
    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
        
    ulTimes = 0;
    for(ulCnt = 0; ulCnt < ulLength; ulCnt++)
    {
        while(ulFifo > I2C_TXRX_THRESHOLD)
        {
            if(++ulTimes > I2C_READ_TIMEOUT)
            {
                return EFI_TIMEOUT;
            }
            ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
        }
   
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, *pBuf++);
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
      }

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    ulTimes = 0;
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }

    return EFI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : I2CWrite
 功能描述  : 支持多字节写，目前支持对控制器1连接的设备的写，即E2PROM
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
         
*****************************************************************************/
EFI_STATUS
EFIAPI
I2CWrite(I2C_DEVICE *I2cInfo, UINT16 InfoOffset, UINT32 ulLength, UINT8 *pBuf)
{
    UINT32 ulFifo;
    UINT32 ulTimes = 0;
    UINT32  Idx;
    UINTN  Base;

    //参数判断
    if(I2cInfo->Port >= I2C_PORT_MAX)
    {
        return EFI_INVALID_PARAMETER;
    }

    Base = PlatformGetI2cBase(I2cInfo->Socket, I2cInfo->Port);

    (VOID)I2C_Enable(I2cInfo->Socket, I2cInfo->Port);

    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    
    //往总线上发送写时序
    if(I2cInfo->DeviceType)
    {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 8) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, InfoOffset & 0xff);
    }
    else
    {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, InfoOffset & 0xff);
    }
    
    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    
    ulTimes = 0;
    for(Idx = 0; Idx < ulLength; Idx++)
    {
        ulTimes = 0;
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port); 
        while(ulFifo > I2C_TXRX_THRESHOLD)
        {
            I2C_Delay(2);
            if(++ulTimes > I2C_READ_TIMEOUT)
            {
                (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
                return EFI_TIMEOUT;
            }
            ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
        }
 
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, *pBuf++);
    }

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    ulTimes = 0;
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            DEBUG ((EFI_D_ERROR, "I2C Write try to finished,time out!\n"));
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);

    return EFI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : I2CRead
 功能描述  : 支持对指定地址的多字节读，对控制器1与0都有效
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
           1. 2013.4.12 c00213799 对接器件读时序差异
*****************************************************************************/
EFI_STATUS
EFIAPI
I2CRead(I2C_DEVICE *I2cInfo, UINT16 InfoOffset,UINT32 ulRxLen,UINT8 *pBuf)
{
    UINT32 ulCnt;
    UINT16 usTotalLen = 0;
    UINT32 ulFifo;
    UINT32 ulTimes = 0;
    UINT8  I2CWAddr[2];
    EFI_STATUS  Status;
    UINT32  BytesLeft;
    UINT32  Idx = 0;
    UINTN Base;
  
    //参数判断
    //if((0 != I2cInfo->Port) && (1 != I2cInfo->Port) && (2 != I2cInfo->Port))
    if(I2cInfo->Port >= I2C_PORT_MAX)
    {
        return EFI_INVALID_PARAMETER;
    }

    (VOID)I2C_Enable(I2cInfo->Socket, I2cInfo->Port);
    Base = PlatformGetI2cBase(I2cInfo->Socket, I2cInfo->Port);
    //往总线上发送写时序
    if(I2cInfo->DeviceType)
    {
        I2CWAddr[0] = (InfoOffset >> 8) & 0xff;
        I2CWAddr[1] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 2,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }
    else
    {
        I2CWAddr[0] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 1,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }

    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);
    
    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        while(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    
    usTotalLen = ulRxLen;
    BytesLeft = usTotalLen;

    while(BytesLeft >= I2C_DRV_ONCE_READ_BYTES_NUM){
        
        //发送读命令
        for(ulCnt = 0; ulCnt < I2C_DRV_ONCE_READ_BYTES_NUM; ulCnt++) {
            I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, I2C_READ_SIGNAL);        
        }
        
        /*接收数据*/
        for(ulCnt = 0; ulCnt < I2C_DRV_ONCE_READ_BYTES_NUM; ulCnt++) {   
            ulTimes = 0;
            do {
                I2C_Delay(2);
        
                while(++ulTimes > I2C_READ_TIMEOUT) {
                    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
                    return EFI_TIMEOUT;
                }
                ulFifo = I2C_GetRxStatus(I2cInfo->Socket,I2cInfo->Port);
                
            }while(0 == ulFifo);
        
            I2C_REG_READ(Base + I2C_DATA_CMD_OFFSET, pBuf[Idx++]);
        
            //pBuf[Idx++] = (UINT8)ulTempValue;
        }
        BytesLeft -= I2C_DRV_ONCE_READ_BYTES_NUM;
    }
    
    //发送读命令
    for(ulCnt = 0; ulCnt < BytesLeft; ulCnt++) {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, I2C_READ_SIGNAL);        
    }
    
    /*接收数据*/
    for(ulCnt = 0; ulCnt < BytesLeft; ulCnt++) {   
        ulTimes = 0;
        do {
            I2C_Delay(2);
    
            while(++ulTimes > I2C_READ_TIMEOUT) {
                (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
                return EFI_TIMEOUT;
            }
            ulFifo = I2C_GetRxStatus(I2cInfo->Socket,I2cInfo->Port);
        }while(0 == ulFifo);
    
        I2C_REG_READ(Base + I2C_DATA_CMD_OFFSET, pBuf[Idx++]);
    }
    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);

    return EFI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : I2CReadMultiByte
 功能描述  : 支持对指定地址的多字节读，对控制器1与0都有效
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
           
*****************************************************************************/
EFI_STATUS
EFIAPI
I2CReadMultiByte(I2C_DEVICE *I2cInfo, UINT32 InfoOffset,UINT32 ulRxLen,UINT8 *pBuf)
{
    UINT32 ulCnt;
    UINT16 usTotalLen = 0;
    UINT32 ulFifo;
    UINT32 ulTimes = 0;
    UINT8  I2CWAddr[4];
    EFI_STATUS  Status;
    UINT32  BytesLeft;
    UINT32  Idx = 0;
    UINTN Base;
  
    //参数判断
    //if((0 != I2cInfo->Port) && (1 != I2cInfo->Port) && (2 != I2cInfo->Port))
    if(I2cInfo->Port >= I2C_PORT_MAX)
    {
        return EFI_INVALID_PARAMETER;
    }

    (VOID)I2C_Enable(I2cInfo->Socket, I2cInfo->Port);
    Base = PlatformGetI2cBase(I2cInfo->Socket, I2cInfo->Port);
    //往总线上发送写时序
    if(I2cInfo->DeviceType == DEVICE_TYPE_E2PROM)
    {
        I2CWAddr[0] = (InfoOffset >> 8) & 0xff;
        I2CWAddr[1] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 2,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }

    else if(I2cInfo->DeviceType == DEVICE_TYPE_CPLD_3BYTE_OPERANDS)
    {
        I2CWAddr[0] = (InfoOffset >> 16) & 0xff;
        I2CWAddr[1] = (InfoOffset >> 8) & 0xff;
        I2CWAddr[2] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 3,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }

    else if(I2cInfo->DeviceType == DEVICE_TYPE_CPLD_4BYTE_OPERANDS)
    {
        I2CWAddr[0] = (InfoOffset >> 24) & 0xff;
        I2CWAddr[1] = (InfoOffset >> 16) & 0xff;
        I2CWAddr[2] = (InfoOffset >> 8) & 0xff;
        I2CWAddr[3] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 4,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }
    
    else
    {
        I2CWAddr[0] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 1,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }

    
    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);
    #if 0
    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        while(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    #endif
    usTotalLen = ulRxLen;
    BytesLeft = usTotalLen;

    #if 0
    while(BytesLeft >= I2C_DRV_ONCE_READ_BYTES_NUM){
        
        //发送读命令
        for(ulCnt = 0; ulCnt < I2C_DRV_ONCE_READ_BYTES_NUM; ulCnt++) {
            I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, I2C_READ_SIGNAL);        
        }
        
        /*接收数据*/
        for(ulCnt = 0; ulCnt < I2C_DRV_ONCE_READ_BYTES_NUM; ulCnt++) {   
            ulTimes = 0;
            do {
                I2C_Delay(2);
        
                while(++ulTimes > I2C_READ_TIMEOUT) {
                    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
                    return EFI_TIMEOUT;
                }
                ulFifo = I2C_GetRxStatus(I2cInfo->Socket,I2cInfo->Port);
                
            }while(0 == ulFifo);
        
            I2C_REG_READ(Base + I2C_DATA_CMD_OFFSET, pBuf[Idx++]);
        
            //pBuf[Idx++] = (UINT8)ulTempValue;
        }
        BytesLeft -= I2C_DRV_ONCE_READ_BYTES_NUM;
    }
    #endif
    //发送读命令
    for(ulCnt = 0; ulCnt < BytesLeft; ulCnt++) {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, I2C_READ_SIGNAL);        
    }
    
    /*接收数据*/
    for(ulCnt = 0; ulCnt < BytesLeft; ulCnt++) {   
        ulTimes = 0;
        do {
            I2C_Delay(2);
    
            while(++ulTimes > I2C_READ_TIMEOUT) {
                (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
                return EFI_TIMEOUT;
            }
            ulFifo = I2C_GetRxStatus(I2cInfo->Socket,I2cInfo->Port);
        }while(0 == ulFifo);
    
        I2C_REG_READ(Base + I2C_DATA_CMD_OFFSET, pBuf[Idx++]);
    }
    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
I2CWriteMultiByte(I2C_DEVICE *I2cInfo, UINT32 InfoOffset, UINT32 ulLength, UINT8 *pBuf)
{
    UINT32 ulFifo;
    UINT32 ulTimes = 0;
    UINT32  Idx;
    UINTN  Base;

    //参数判断
    if(I2cInfo->Port >= I2C_PORT_MAX)
    {
        return EFI_INVALID_PARAMETER;
    }

    Base = PlatformGetI2cBase(I2cInfo->Socket, I2cInfo->Port);

    (VOID)I2C_Enable(I2cInfo->Socket, I2cInfo->Port);

    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    
    //往总线上发送写时序
    if(I2cInfo->DeviceType == DEVICE_TYPE_CPLD_3BYTE_OPERANDS)
    {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 16) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 8) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, InfoOffset & 0xff);
    }

    else if(I2cInfo->DeviceType == DEVICE_TYPE_CPLD_4BYTE_OPERANDS)
    {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 24) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 16) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 8) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, InfoOffset & 0xff);
    }
    
    else
    {
  
    }

    #if 0
    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    #endif
    
    ulTimes = 0;
    for(Idx = 0; Idx < ulLength; Idx++)
    {

       I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, *pBuf++);
        
    }

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    ulTimes = 0;
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            DEBUG ((EFI_D_ERROR, "I2C Write try to finished,time out!\n"));
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);

    return EFI_SUCCESS;
}

