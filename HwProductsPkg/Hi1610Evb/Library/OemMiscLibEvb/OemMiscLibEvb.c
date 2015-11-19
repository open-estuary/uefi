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

UINTN OemGetDdrChannel (VOID)
{
    return 2;
}
// uniBIOS_y00216284_053_end 2015-7-4 <<<

//uniBIOS_z00304930_000_start 2014-12-17 10:55:57
 
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
 //Issue ID:DTS2014120902872  Description:
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
 
VOID OemBiosSwitch(UINT32 Master)
{
    return;
}
//uniBIOS_z00304930_000_end   2014-12-18 10:32:10


VOID OemWriteProtectSet(BOOLEAN val)
{
    return;
}

BOOLEAN OemIsSpiFlashWriteProtected(VOID)
{
    return FALSE;
}

VOID OpenAlarmLed(VOID)
{
    //UINT8 Value;
    //Value = ReadCpldReg(CPLD_BIOS_WR_REG);
    //Value |= CPLD_ALARM_LED_FIG;
    //WriteCpldReg(CPLD_BIOS_WR_REG, Value);
    
    return ;
}

//uniBIOS_z00304930_000_start 2015-1-23 14:33:20
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
BOOLEAN IsNeedSkipVariableSpaceOperation()
{
    return FALSE;
}
//uniBIOS_l00306713_000_end   2015-6-16 17:34:20

//uniBIOS__l00228991_start DTS2015072808846 2015-8-6 18:33:18
BOOLEAN OemIsLoadDefault()
{
    return FALSE;
}
//uniBIOS__l00228991_end   2015-8-6 18:33:18

BOOLEAN OemIsInitEth(UINT32 Port)
{
    return TRUE;
}

