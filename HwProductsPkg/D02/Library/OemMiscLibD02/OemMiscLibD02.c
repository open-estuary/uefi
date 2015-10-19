/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpd1z2bQhhV6cC9dn956MYMwSDGfeyc2k9r9TKK9bTrj6YPvsPEr44pJLAZWy
NJXlVR48rJ8J8IPnmjE+kMhzoFHqWonLCYLUWmp+lFB2eXwM4liKIpp/a7eMxJTXsE5QLTFK
+VM/w2lJ6nD/IM6t8FbcpxWb+0DscgG2TvJWrCPVHsBkHUc1IiHmBbhdEJVPmQ==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwProductsPkg\Pv660Evb\Library\OemMiscLibEvb\OemMiscLibEvb.c


Description: 

*************************************************/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Library/CpldApi.h>
#include <Library/OemMiscLib.h>
#include <PlatformArch.h>
#include <Library/PlatformSysCtrlLib.h>
#include <Library/OemAddressMapLib.h>
#include <Library/SerialPortLib.h>
#include <Library/CpldD02.h>

// Right now we only support 1P
BOOLEAN OemIsSocketPresent (UINTN Socket)
{
  if (0 == Socket)
  {
    return TRUE;
  }

  return FALSE;
}

UINTN OemGetSocketNumber (VOID)
{
    return 1;
}

UINTN OemGetDimmSlot(UINTN Socket, UINTN Channel)
{
    return 2;
}

VOID OemPostEndIndicator (VOID)
{
}

EFI_STATUS OemMemoryTest (UINTN Base, UINTN Size)
{
  return EFI_SUCCESS;
}

EFI_STATUS OemSelfTestReport(IN E_SELF_TEST_ITEM Item, IN U_SELF_TEST_PARA Para)
{
    return EFI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : OemSetSelfTestFailFlagAndAlarm
 功能描述  : 设置自检失败标记位并点Alarm灯
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月4日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数

*****************************************************************************/
VOID OemSetSelfTestFailFlagAndAlarm()
{
    return;
}

/*****************************************************************************
 函 数 名  : OemGetCurrentBiosChannel
 功能描述  : 
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 0 主BIOS
             1 备BIOS
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月4日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数

*****************************************************************************/
UINT32 OemGetCurrentBiosChannel(VOID)
{
    UINTN RegAddr = CPLD_BIOS_CURRENT_CHANNEL_REG_D02;
    UINT8 Value;

    Value = ReadCpldReg(RegAddr);
    if (1==(Value&0x01))
    {
        return 1;
    }
    else
    {
        return 0;
    }
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
EFI_STATUS OemCheckCpld(VOID)
{
    return EFI_SUCCESS;
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
EFI_STATUS OemCheckClock(VOID)
{
    return EFI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : OemGetCurrentResetCause
 功能描述  : 获取当前复位原因
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数
*****************************************************************************/
E_RESET_CAUSE OemGetCurrentResetCause(VOID)
{
    return RESET_CAUSE_CPLD_BUTT;
}

/*****************************************************************************
 函 数 名  : OemGetCurrentResetCause
 功能描述  : 获取前一次复位原因
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数
*****************************************************************************/
E_RESET_CAUSE OemGetPer2ResetCause(VOID)
{
    return RESET_CAUSE_CPLD_BUTT;
}

/*****************************************************************************
 函 数 名  : OemGetCurrentResetCause
 功能描述  : 获取上上次次复位原因
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数
*****************************************************************************/
E_RESET_CAUSE OemGetPer3ResetCause(VOID)
{
    return RESET_CAUSE_CPLD_BUTT;
}

/*****************************************************************************
 函 数 名  : OemIsWarmBoot
 功能描述  : 获取上上次次复位原因
 输入参数  : UINT32  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月26日
    作    者   : L00228991
    修改内容   : 新生成函数
*****************************************************************************/
UINT32 OemIsWarmBoot(VOID)
{
    return 0;
}


/*******************************************************************************
 函 数 名  : CoreSelectBoot
 功能描述  : 判断某个核CPU BIST结果，失败就隔离
 输入参数  : 无 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月15日
    作    者   : c00213799
    修改内容   : 新生成函数
*******************************************************************************/
VOID CoreSelectBoot(VOID)
{
    if (!PcdGet64 (PcdTrustedFirmwareEnable))
    {
        StartupAp ();
    }

    return;
}

/*****************************************************************************
 函 数 名  : OemBiosSwitch
 功能描述  : 切换主备BIOS
 输入参数  : UINT32 Master 0 切换到主BIOS
                           1 切换到备BIOS 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数

*****************************************************************************/
VOID OemBiosSwitch(UINT32 Master)
{
    return;
}
/*******************************************************************************
 函 数 名  : SetCpldBootDeviceID
 功能描述  : 
 输入参数  : Result  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : l00306713
    修改内容   : 新生成函数
*******************************************************************************/
VOID SetCpldBootDeviceID(IN UINT8 Value)
{        
    return;
}

/*******************************************************************************
 函 数 名  : ReadCpldBootDeviceID
 功能描述  : 
 输入参数  : Result  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : l00306713
    修改内容   : 新生成函数
*******************************************************************************/
UINT8 ReadCpldBootDeviceID(void)
{  
    return 0;
}


/*******************************************************************************
 函 数 名  : SetCpldBootDeviceCount
 功能描述  : 
 输入参数  : Result  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : l00306713
    修改内容   : 新生成函数
*******************************************************************************/
VOID SetCpldBootDeviceCount(IN UINT8 Value)
{       
    return;
}

/*******************************************************************************
 函 数 名  : ReadCpldBootDeviceCount
 功能描述  : 
 输入参数  : Result  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : l00306713
    修改内容   : 新生成函数
*******************************************************************************/
UINT8 ReadCpldBootDeviceCount(void)
{
    return 0;
}


VOID OemWriteProtectSet(BOOLEAN val)
{
    return;
}
/*******************************************************************************
 函 数 名  : OemIsSpiFlashWriteProtected
 功能描述  : 是否本项目需要spi flash写保护功能，是请返回TRUE,否返回FALSE
 输入参数  : 无  
 输出参数  : TRUE/FALSE
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : l00306713
    修改内容   : 新生成函数
*******************************************************************************/
BOOLEAN OemIsSpiFlashWriteProtected(VOID)
{
    return FALSE;
}

VOID OpenAlarmLed(VOID)
{
    //UINT8 Value;
    //点亮单板告警灯
    //Value = ReadCpldReg(CPLD_BIOS_WR_REG);
    //Value |= CPLD_ALARM_LED_FIG;
    //WriteCpldReg(CPLD_BIOS_WR_REG, Value);
    
    return ;
}

EFI_STATUS OemCk420Read(UINT16 InfoOffset, UINT32 ulLength, UINT8 *pBuf)
{
    return EFI_SUCCESS;
}

EFI_STATUS OemCk420Write(UINT16 InfoOffset, UINT32 ulLength, UINT8 *pBuf)
{
    return EFI_SUCCESS;
}

VOID OemPcieCardReset(UINT32 Reset)
{
    return;
}

UINT32  SelDdrFreq(pGBL_DATA pGblData)
{
    return DDR_MAX;
}

UINT8 OemAhciStartPort(VOID)
{
    return 0;
}

EFI_STATUS GetSetupData(SETUP_PARAMS *Setup)
{
    return EFI_UNSUPPORTED;
}



UINT16 OemSataNum = 0;
UINT16 OemSataDesSize = 0;
UINT16 OemPXENum = 0;
UINT16 OemPXEDesSize = 0;
UINT8 OemFirstBootId = 0;

EFI_STATUS OemGetSataBootNum(UINTN SataDesSize)
{
    if( SataDesSize/sizeof(UINT16) > 0)
    {
        OemSataDesSize = SataDesSize;
        OemSataNum = OemSataDesSize/sizeof(UINT16);
    }
    else{    
        OemSataNum = 0;    
    }

    return EFI_SUCCESS;
}

EFI_STATUS OemGetPXEBootNum(UINTN PXESize)
{
    if( PXESize/sizeof(UINT16) > 0)
    {
        OemPXEDesSize = PXESize;
        OemPXENum = OemPXEDesSize/sizeof(UINT16);
    }else{
        OemPXENum = 0;
    }

    return EFI_SUCCESS;
}

EFI_STATUS OemPreStartBootOptionAction(UINT16 BootOptionId)
{
    return EFI_SUCCESS;
}

EFI_STATUS OemBootOrderSeting(IN OUT UINT16* BootOrderFinal, UINTN BootOrderSize, BOOLEAN *IsFlashBootFirst, BOOLEAN *FlashBootSupport)
{
    *FlashBootSupport = FALSE;
    if(BootOrderFinal == NULL || BootOrderSize == 0)
    {
        return EFI_INVALID_PARAMETER;
    }
    return EFI_SUCCESS;    
}

