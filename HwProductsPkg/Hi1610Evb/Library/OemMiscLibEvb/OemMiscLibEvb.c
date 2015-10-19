/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpd1z2bQhhV6cC9dn956MYMwSDGfeyc2k9r9TKK9bTrj6YPvsPEr44pJLAZWy
NJXlVTULGA1ISf9ltqbAGfrSahFBeWVPU8rl/Z2i37aV2wlo/vUTcWxtXwzwiBpTg7ibYk5i
yPWT1Ab0J2Cb4SwFnWpn5lJinT6ELWKC9JV6zoOhgNGWG6BTr0DPHcNhDc0Lmg==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwProductsPkg\Pv660Evb\Library\OemMiscLibEvb\OemMiscLibEvb.c

Author: uniBIOS-g00179230 (Heyi Guo)    Date: 2014.11.4

Description: 
DTS2015012400081-C00227771-20150124 pangea单板默认启动1866的频率！各单板各自实现函数！
DTS2015031709185-C00227771-20150317 各单板频点自适应选择。
// uniBIOS_y00216284_043_start DTS2015051309310 BIOS自研代码PCLint整改  2015-5-13 >>>
*************************************************/
//uniBIOS_y00216284_013_start 2014-12-19 DTS2014121808254 PC-LINT告警消除 
#include <Uefi.h>
//uniBIOS_z00304930_000_start 2014-12-4 16:17:49
 //Issue ID:DTS2014120204227 Description:
#include <Library/OemMiscLib.h>
//uniBIOS_z00304930_000_end   2014-12-4 16:17:49
#include <Library/DebugLib.h>
#include <Library/SerialPortLib.h>
#include <PlatformArch.h>
#include <Library/PlatformSysCtrlLib.h>
#include <Library/IoLib.h>
#include <Library/OemAddressMapLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>

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

// uniBIOS_y00216284_053_start DTS2015070301857 2015-7-4 >>>
// Description:根据实际单板情况，获取内存通道数
UINTN OemGetDdrChannel (VOID)
{
    return 2;
}
// uniBIOS_y00216284_053_end 2015-7-4 <<<

//uniBIOS_z00304930_000_start 2014-12-17 10:55:57
 //Issue ID:DTS2014120400124 Description: 使用CPLD接口
VOID OemPostEndIndicator (VOID)
{
}
//uniBIOS_z00304930_000_end   2014-12-17 10:55:57

//uniBIOS_z00304930_000_start 2014-12-4 10:42:42
 //Issue ID:DTS2014120204227 Description:
EFI_STATUS OemSelfTestReport(IN E_SELF_TEST_ITEM Item, IN U_SELF_TEST_PARA Para)
{
    return EFI_SUCCESS;
}

VOID OemSetSelfTestFailFlagAndAlarm()
{
    return;
}

UINT32 OemGetCurrentBiosChannel(VOID)
{
    return 0;
}

//uniBIOS_z00304930_000_start 2014-12-16 10:01:13
 //Issue ID:DTS2014120902872  Description:′ú???ìêóòa??DT??
EFI_STATUS OemCheckCpld(VOID)
{
    return EFI_SUCCESS;
}

EFI_STATUS OemCheckClock(VOID)
{
    return EFI_SUCCESS;
}
//uniBIOS_z00304930_000_end   2014-12-16 10:01:13
//uniBIOS_z00304930_000_end   2014-12-4 10:42:42

//uniBIOS_z00304930_000_start 2014-12-17 11:32:55
 //Issue ID:DTS2014120801893 Description:重启原因上报
E_RESET_CAUSE OemGetCurrentResetCause(VOID)
{
    return RESET_CAUSE_CPLD_BUTT;
}

E_RESET_CAUSE OemGetPer2ResetCause(VOID)
{
    return RESET_CAUSE_CPLD_BUTT;
}

E_RESET_CAUSE OemGetPer3ResetCause(VOID)
{
    return RESET_CAUSE_CPLD_BUTT;
}

UINT32 OemIsWarmBoot(VOID)
{
    return 0;
}

//uniBIOS_z00304930_000_end   2014-12-17 11:32:55

//uniBIOS_c00213799_start 2014-12-18 10:12:13
//Issue ID:DTS2014121800997
//Description:【iWare V200R002C00B950 BIOS】CPU L1/L2 Cache mbist功能合入
VOID CoreSelectBoot(VOID)
{
    if (!PcdGet64 (PcdTrustedFirmwareEnable))
    {
        StartupAp ();
    }

    return;
}
//uniBIOS_c00213799_end 2014-12-18 10:12:13

//uniBIOS_z00304930_000_start 2014-12-18 10:32:10
 //Issue ID:DTS2014121801315 Description:biosupdate命令增加主备bios切换
VOID OemBiosSwitch(UINT32 Master)
{
    return;
}
//uniBIOS_z00304930_000_end   2014-12-18 10:32:10

 //Issue ID:DTS2015042006493    Description:【iWare V200R002C00B980 BIOS】按照开源要求对代码结构进行整改
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

//uniBIOS_z00304930_000_start 2015-1-23 14:33:20
 //Issue ID:DTS2015012306969  Description:CK420展频命令行合入
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
//uniBIOS_z00304930_000_end   2015-1-23 14:33:20


//uniBIOS_y00216284_023_start DTS2015030207984 2015-3-9 15:27:28
//Description: 盘古 C02单板hilink5和hilink6共用一个SAS控制器SAS1，
//在加载Sata驱动时SAS1对应的8个phy全部被使能，造成hilink5对应的phy出现多次phychange
UINT8 OemAhciStartPort(VOID)
{
    return 0;
}
//uniBIOS_y00216284_023_end 2015-3-9 15:27:28




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

VOID OemScrubFlagConfig(pGBL_DATA pGblData)
{
    return;     
}
//uniBIOS_y00216284_044_end 2015-5-14 <<<
//uniBIOS_l00306713_000_start 2015-6-16 17:34:20
 //Issue ID:DTS2015051503878    Description:【iWare V200R002C00B980 BIOS】修改lemon单板BIOS升级与开源冲突 
BOOLEAN IsNeedSkipVariableSpaceOperation()
{
    return FALSE;
}
//uniBIOS_l00306713_000_end   2015-6-16 17:34:20

//uniBIOS__l00228991_start DTS2015072808846 2015-8-6 18:33:18
//Description:增加Lemon单板BMC重置Setup默认值的功能
BOOLEAN OemIsLoadDefault()
{
    return FALSE;
}
//uniBIOS__l00228991_end   2015-8-6 18:33:18

BOOLEAN OemIsInitEth(UINT32 Port)
{
    return TRUE;
}

