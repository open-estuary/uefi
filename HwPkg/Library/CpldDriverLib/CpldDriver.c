
#include <Library/CpldApi.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <PlatformArch.h>
#include <Library/OemAddressMapLib.h>
#include <Library/OemMiscLib.h>


VOID WriteCpldReg(UINTN ulRegAddr, UINT8 ulValue)
{
    *(volatile UINT8 *)(ulRegAddr + PcdGet64(PcdCpldBaseAddress)) = ulValue;
}


UINT8 ReadCpldReg(UINTN ulRegAddr)
{    
    volatile UINT8 ulValue = 0;
    
    ulValue = *(volatile UINT8 *)(ulRegAddr + PcdGet64(PcdCpldBaseAddress));
    
    return (ulValue); 
}


EFI_STATUS CheckCpld(VOID)
{
    UINTN RegAddr = CPLD_LPC_DEBUG_REG;
    UINT8 TestFlag = CPLD_CPLD_CHECK_FLAG;
    UINT8 Value;

    WriteCpldReg(RegAddr, TestFlag);
    Value = ReadCpldReg(RegAddr);
    Value = ~Value; 
    if (TestFlag == Value)
    {
        return EFI_SUCCESS;    
    }
    else
    {
        return EFI_ABORTED;    
    }
}


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

