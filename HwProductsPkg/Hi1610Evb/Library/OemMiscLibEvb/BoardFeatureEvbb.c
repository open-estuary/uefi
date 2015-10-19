/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpd1z2bQhhV6cC9dn956MYMwSDGfeyc2k9r9TKK9bTrj6YPvsPEr44pJLAZWy
NJXlVedespbxTjZ1vyhERcXhe5efbuRMKmuY6KO5Lg8wq5Y88LfOVhFiIbNw3+1oyK7nIGc9
8jZ28fn8DcOERn9JauEIxXqkzvvlDlHKSKs1ljczvaLPSX6FRZ820CehbF3Ozg==*/
/*--------------------------------------------------------------------------------------------------------------------------*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

#include <PlatformArch.h>
#include <Library/HwMemInitLib.h>
#include <Library/OemMiscLib.h>
#include <Library/SerdesLib.h>

// For PV660 HRD EVB B Board
I2C_SLAVE_ADDR  DimmSpdAddr[MAX_SOCKET][MAX_CHANNEL][MAX_DIMM] = {
    {{{I2C_PORT1,0x50},{I2C_PORT1,0x51},{I2C_PORT1,0x52}},      //Socket 0 TC DDRC0
    {{I2C_PORT0,0x50},{I2C_PORT0,0x51},{I2C_PORT0,0x52}},      //Socket 0 TC DDRC1
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}},//Invalid
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}}},//Invalid

#if MAX_SOCKET > 1
    {{{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}},//Socket is Invalid
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}},
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}},//Socket is Invalid
    {{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF},{I2C_INVALIDPORT,0xFF}}},
#endif
}; 

// Set Tx output polarity. Not inverting data is default. For Phosphor660 16 core EVB Board
//if(((3 == ulMacroId) && (3 == ulDsNum)) || ((4 == ulMacroId) && ((0 == ulDsNum) || (2 == ulDsNum))))
SERDES_POLARITY_INVERT gSerdesPolarityTxDesc[] =
{
    {3, 3},
    {4, 0},
    {4, 2},
    {SERDES_INVALID_MACRO_ID, SERDES_INVALID_LANE_NUM}
};

// Set Rx data polarity. Not inverting data is default. For Phosphor660 16 core EVB Board
//if((4 == ulMacroId) && ((0 == ulDsNum) || (1 == ulDsNum)|| (2 == ulDsNum)))
SERDES_POLARITY_INVERT gSerdesPolarityRxDesc[] =
{
    {4, 0},
    {4, 1},
    {4, 2},
    {SERDES_INVALID_MACRO_ID, SERDES_INVALID_LANE_NUM}
};

serdes_param_t gSerdesParam = {
    .hilink0_mode = EM_HILINK0_PCIE1_8LANE,
    .hilink1_mode = EM_HILINK1_PCIE0_8LANE,
    .hilink2_mode = EM_HILINK2_PCIE2_8LANE,
    .hilink3_mode = 0xC,
    .hilink4_mode = 0xF,
    .hilink5_mode = EM_HILINK5_SAS1_4LANE,
    .hilink6_mode = 0xF,
    .use_ssc      = 0,
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


//uniBIOS_l00306713_000_start 2014-12-19 10:23:43
 //Issue ID:DTS2014121901209 //盘古加了，防止编译问题
 //Description:serdes初始化后复位解复位PCIE，PCIE枚举
VOID OemPcieResetAndOffReset(void)
{
  return;
}
//uniBIOS_l00306713_000_end   2014-12-19 10:23:43

// start-uniBIOS-s00296804  2015-2-13 >>
//增加boardinformation信息
VOID BoardInformation(void)
{
    return;
}
// end-uniBIOS-s00296804  2015-2-13 <<<

UINTN OemGetDimmSlot(UINTN Socket, UINTN Channel)
{
    return 3;
}

//uniBIOS_z00304930_000_start 2015-3-23 14:42:15
 //Issue ID:DTS2015032408517 Description: ?????
ETH_PRODUCT_DESC gEthPdtDesc[ETH_MAX_PORT] = 
{   
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID},
    {TRUE,  ETH_SPEED_1000M, ETH_FULL_DUPLEX, ETH_PHY_MVL88E1512_ID, 0x0},
    {FALSE, ETH_INVALID,     ETH_INVALID,     ETH_INVALID,           ETH_INVALID}
};

/*****************************************************************************
 ? ? ?  : OemEthFindFirstSP
 ????  : ????????????
 ????  : ?
 ????  : ?????
 ? ? ?  : 
 ????  : 
 ????  : 
 
 ????      :
  1.?    ?   : 2015?4?17?
    ?    ?   : ZhuGeSheng
    ????   : ?????

*****************************************************************************/
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

/*****************************************************************************
 ? ? ?  : OemEthInit
 ????  : ???????????
 ????  : UINT port  
 ????  : ?
 ? ? ?  : ETH_PRODUCT_DESC
 ????  : 
 ????  : 
 
 ????      :
  1.?    ?   : 2015?3?23?
    ?    ?   : ZhuGeSheng
    ????   : ?????

*****************************************************************************/
ETH_PRODUCT_DESC *OemEthInit(UINT32 port)
{
    return (ETH_PRODUCT_DESC *)(&(gEthPdtDesc[port]));
}
//uniBIOS_z00304930_000_end   2015-3-23 14:42:15

