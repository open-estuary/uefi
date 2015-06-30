/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7oqucjIznazmY6pDerNNjrJRr3zqJWqMJ5g20Wh4kav+V4LjOmY/LHDS62FHF
VyAlAx7njIhP2ajqOJoJMnWikMp6aeZYsDDNCTEd6durgfJhGERE0UQ4K0IX5pjCzmhx5KT8
JLJLy94mv/xl5MsurpJd8GDGiPjUNa6QjAd1O/3TrizuqbFyPIj/+5eqpagAdg==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

******************************************************************************
  文 件 名   : CpldDriver.c
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 
  最近修改   :
  功能描述   : CPLD驱动
  
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 创建文件
******************************************************************************/

#include <Library/CpldApi.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <PlatformArch.h>
#include <Library/OemAddressMapLib.h>
#include <Library/OemMiscLib.h>

/*****************************************************************************
  函 数 名  : CPLD_WriteReg
  功能描述  : CPLD寄存器写
  输入参数  : UINT32 ulAddr
            UINT32 ulValue
  输出参数  : 无
  返 回 值  : VOID
*****************************************************************************/
VOID WriteCpldReg(UINTN ulRegAddr, UINT8 ulValue)
{
    *(volatile UINT8 *)(ulRegAddr + PcdGet64(PcdCpldBaseAddress)) = ulValue;
}

/*****************************************************************************
  函 数 名  : CPLD_ReadReg
  功能描述  : CPLD寄存器读
  输入参数  : void  
  输出参数  : 无
  返 回 值  : UINT32
*****************************************************************************/
UINT8 ReadCpldReg(UINTN ulRegAddr)
{    
    volatile UINT8 ulValue = 0;
    
    ulValue = *(volatile UINT8 *)(ulRegAddr + PcdGet64(PcdCpldBaseAddress));
    
    return (ulValue); 
}

/*******************************************************************************
 函 数 名  : CheckCpld
 功能描述  : CPLD自检
 输入参数  : 无 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年11月24日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数

*******************************************************************************/
EFI_STATUS CheckCpld(VOID)
{
    UINTN RegAddr = CPLD_LPC_DEBUG_REG;
    UINT8 TestFlag = CPLD_CPLD_CHECK_FLAG;
    UINT8 Value;

    WriteCpldReg(RegAddr, TestFlag);
    Value = ReadCpldReg(RegAddr);
    Value = ~Value;  //取反
    if (TestFlag == Value)
    {
        return EFI_SUCCESS;    
    }
    else
    {
        return EFI_ABORTED;    
    }
}

/*******************************************************************************
 函 数 名  : CheckClock
 功能描述  : 时钟自检
 输入参数  : 无 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年11月24日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数

*******************************************************************************/
EFI_STATUS CheckClock(VOID)
{
    UINTN RegAddr = CPLD_CLK_DET_REG;
    UINT8 Mask = CPLD_CLOCK_CHECK_FLAG;
    UINT8 Value;

    Value = ReadCpldReg(RegAddr);
    if (CPLD_CLOCK_CHECK_FLAG == BIT_GET(Value, Mask))
    {
        return EFI_SUCCESS;    
    }
    else
    {
        return EFI_ABORTED;    
    }
}

/*****************************************************************************
 函 数 名  : GetCurrentBiosChannel
 功能描述  : 获取当前启动为主bios还是备bios
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 0 主BIOS
             1 备BIOS
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月3日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数

*****************************************************************************/
UINT32 GetCurrentBiosChannel(VOID)
{
    UINTN RegAddr = CPLD_BIOS_CURRENT_CHANNEL_REG;
    UINT8 Value;

    Value = ReadCpldReg(RegAddr);
    if (CPLD_BIOS_CURRENT_CHANNEL_MASK == BIT_GET(Value, CPLD_BIOS_CURRENT_CHANNEL_MASK))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**************************************************
函数名称  :   SetSelfTestFailFlagAndAlarm()
函数说明  :   设置自检失败标志并点亮单板告警灯告警；
输入参数  :   新增函数
**************************************************/
VOID
SetSelfTestFailFlagAndAlarm(VOID)
{
    UINT8 uLedFlag   = 0;
    UINT8 uEventFlag = CPLD_SELFTEST_FAILURE_FLAG;
    
    WriteCpldReg(CPLD_SELFTEST_FAILURE_REG, uEventFlag);

    uLedFlag = ReadCpldReg(CPLD_BIOS_WR_REG);
    uLedFlag |= CPLD_ALARM_LED_FIG;
    WriteCpldReg(CPLD_BIOS_WR_REG, uLedFlag);
}

