/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpd1z2bQhhV6cC9dn956MYMwSDGfeyc2k9r9TKK9bTrj6YPvsPEr44pJLAZWy
NJXlVZyN3q1irPCsNWRZ5S+LBR2EKa9YjLuNajZ13sp37ua3GoHk+HBM4ldou5v9Ii44FOh6
CEPqvXpwa4mh2n09JCneqo6LrEKUOY7bbGDDieClYJ2slsnFkN+Hunf4A8KI4g==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: BoardFeature.c

Description: 

*************************************************/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

#include <PlatformArch.h>
#include <Library/HwMemInitLib.h>
#include <Library/OemMiscLib.h>
#include <Library/SerdesLib.h>
#include <Library/CpldApi.h>
#include <Library/CpldD02.h>
#include <Library/TimerLib.h>
#include <Library/I2CLib.h>

I2C_SLAVE_ADDR  DimmSpdAddr[MAX_SOCKET][MAX_CHANNEL][MAX_DIMM] = {
    {{{I2C_PORT1,0x50},{I2C_PORT1,0x51},{I2C_INVALIDPORT,0xFF}},      //Socket 0 TC DDRC0
    {{I2C_PORT0,0x50},{I2C_PORT0,0x51},{I2C_INVALIDPORT,0xFF}},      //Socket 0 TC DDRC1
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}},//Invalid
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}}},//Invalid
#if MAX_SOCKET > 1
    {{{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}},//Socket is Invalid
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}},
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}},//Socket is Invalid
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}}},
#endif
};

I2C_DEVICE gDS3231RtcDevice = {
    .Socket = 0,
    .Port = 7,
    .DeviceType = DEVICE_TYPE_SPD,
    .SlaveDeviceAddress = 0x68
};

 // Set Tx output polarity. Not inverting data is default. For Phosphor660 D02 Board
//if((1 == ulMacroId) && ((7 == ulDsNum)||(0 == ulDsNum))) 
SERDES_POLARITY_INVERT gSerdesPolarityTxDesc[] =
{
    {1, 7},
    {1, 0},
    {SERDES_INVALID_MACRO_ID, SERDES_INVALID_LANE_NUM}
};

// Set Rx data polarity. Not inverting data is default. For Phosphor660 D02 Board
//if((1 == ulMacroId) && ((0 == ulDsNum) || (1 == ulDsNum)))
SERDES_POLARITY_INVERT gSerdesPolarityRxDesc[] =
{
    {1, 0},
    {1, 1},
    {SERDES_INVALID_MACRO_ID, SERDES_INVALID_LANE_NUM}
};

serdes_param_t gSerdesParam = {
    .hilink0_mode = EM_HILINK0_PCIE1_8LANE,
    .hilink1_mode = EM_HILINK1_PCIE0_8LANE,
    .hilink2_mode = EM_HILINK2_PCIE2_8LANE,
    .hilink3_mode = EM_HILINK3_GE_4LANE,
    .hilink4_mode = EM_HILINK4_XGE_4LANE,
    .hilink5_mode = EM_HILINK5_SAS1_4LANE,
    };

EFI_STATUS OemGetSerdesParam (serdes_param_t *Param)
{
  if (NULL == Param)
  {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Param == NULL!\n", __FUNCTION__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }

  (VOID) CopyMem(Param, &gSerdesParam, sizeof(*Param));
  return EFI_SUCCESS;
}


VOID OemPcieResetAndOffReset(void)
  {
    WriteCpldReg(CPU0_PCIE1_RESET_REG,0x0);
    WriteCpldReg(CPU0_PCIE2_RESET_REG,0x0);
    WriteCpldReg(CPU1_PCIE1_RESET_REG,0x0);
    WriteCpldReg(CPU1_PCIE2_RESET_REG,0x0);
    MicroSecondDelay(100000);
    WriteCpldReg(CPU0_PCIE1_RESET_REG,0x55);
    WriteCpldReg(CPU0_PCIE2_RESET_REG,0x55);
    WriteCpldReg(CPU1_PCIE1_RESET_REG,0x55);
    WriteCpldReg(CPU1_PCIE2_RESET_REG,0x55);
    return;
  }

VOID BoardInformation(void)
  {
    UINT8   tCPLD_LOGIC_VERSION;
    UINT8   tPRODUCT_VERSION;
    UINT8   tCPLD_LOGIC_COMPLIER_YEAR;
    UINT8   tCPLD_LOGIC_COMPLIER_MONTH;
    UINT8   tCPLD_LOGIC_COMPLIER_DAY;
    UINT8   tCPLD_LOGIC_COMPLIER_HOUR;
    UINT8   tCPLD_LOGIC_COMPLIER_MINUTE;
    UINT8   tBOARD_ID;
    UINT8   tBOM_VERSION;
    
    DEBUG((EFI_D_ERROR, "------------------------------------------------------\n"));
    tCPLD_LOGIC_VERSION =  ReadCpldReg(CPLD_LOGIC_VERSION);
    tPRODUCT_VERSION =  ReadCpldReg(PRODUCT_VERSION);
    tCPLD_LOGIC_COMPLIER_YEAR = ReadCpldReg(CPLD_LOGIC_COMPLIER_YEAR);
    tCPLD_LOGIC_COMPLIER_MONTH = ReadCpldReg(CPLD_LOGIC_COMPLIER_MONTH);
    tCPLD_LOGIC_COMPLIER_DAY = ReadCpldReg(CPLD_LOGIC_COMPLIER_DAY);
    tCPLD_LOGIC_COMPLIER_HOUR = ReadCpldReg(CPLD_LOGIC_COMPLIER_HOUR);
    tCPLD_LOGIC_COMPLIER_MINUTE = ReadCpldReg(CPLD_LOGIC_COMPLIER_MINUTE);
    tBOARD_ID = ReadCpldReg(BOARD_ID);
    tBOM_VERSION = ReadCpldReg(BOM_VERSION);
    DEBUG((EFI_D_ERROR, "Logic Version: %02d\n",tCPLD_LOGIC_VERSION));
    DEBUG((EFI_D_ERROR, "Product Version: %02d\n",tPRODUCT_VERSION));
    DEBUG((EFI_D_ERROR, "CPLD Logic Build time: 20%02d-%02d-%02d %02d:%02d\n",tCPLD_LOGIC_COMPLIER_YEAR,tCPLD_LOGIC_COMPLIER_MONTH,
        tCPLD_LOGIC_COMPLIER_DAY,tCPLD_LOGIC_COMPLIER_HOUR,tCPLD_LOGIC_COMPLIER_MINUTE));
    DEBUG((EFI_D_ERROR, "Board ID: %02d\n",tBOARD_ID));
    DEBUG((EFI_D_ERROR, "BOM Version: %02d\n",tBOM_VERSION));
    return;
  }

ETH_PRODUCT_DESC gEthPdtDesc[ETH_MAX_PORT] = 
{   
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {TRUE,  ETH_SPEED_1000M, ETH_FULL_DUPLEX, ETH_PHY_MVL88E1512_ID, 0x0},
    {TRUE,  ETH_SPEED_1000M, ETH_FULL_DUPLEX, ETH_PHY_MVL88E1512_ID, 0x1},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID}
};

UINT32 OemEthFindFirstSP()
{
    UINT32 i;

    for (i = 0; i < ETH_DEBUG_PORT0; i++)
    {
        if (TRUE == gEthPdtDesc[i].Valid)
        {
            return i;
        }
    }
    
    return ETH_INVALID;
}

ETH_PRODUCT_DESC *OemEthInit(UINT32 port)
{
    return (ETH_PRODUCT_DESC *)(&(gEthPdtDesc[port]));
}

