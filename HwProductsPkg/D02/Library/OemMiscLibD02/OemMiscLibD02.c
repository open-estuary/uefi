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


VOID OemSetSelfTestFailFlagAndAlarm()
{
    return;
}


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


EFI_STATUS OemCheckCpld(VOID)
{
    return EFI_SUCCESS;
}


EFI_STATUS OemCheckClock(VOID)
{
    return EFI_SUCCESS;
}

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



VOID CoreSelectBoot(VOID)
{
    if (!PcdGet64 (PcdTrustedFirmwareEnable))
    {
        StartupAp ();
    }

    return;
}


VOID OemBiosSwitch(UINT32 Master)
{
    return;
}

VOID SetCpldBootDeviceID(IN UINT8 Value)
{        
    return;
}


UINT8 ReadCpldBootDeviceID(void)
{  
    return 0;
}


VOID SetCpldBootDeviceCount(IN UINT8 Value)
{       
    return;
}


UINT8 ReadCpldBootDeviceCount(void)
{
    return 0;
}


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

