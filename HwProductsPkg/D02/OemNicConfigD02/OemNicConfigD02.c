/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpd1z2bQhhV6cC9dn956MYMwSDGfeyc2k9r9TKK9bTrj6YPvsPEr44pJLAZWy
NJXlVVe1Zr9/gtteWy0L8B4LdSodtP3H6AEC9vz0Fjm76jab4u72zHmPo2Lpv+S6/F4Sg1jF
DUroMQVPLNXCAEaIvBAEJpa+9rFgEPX+1f4l1GK7dZiZ5NgxXR1EhaQOcpkjEg==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: OemNicConfig.c


Description:

*************************************************/

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/OemNicProtocol.h>
#include <Library/I2CLib.h>
#include <OemNicConfig.h>

#define EEPROM_I2C_PORT 7

EFI_STATUS
EFIAPI OemGetMac (IN OUT EFI_MAC_ADDRESS *Mac, IN UINTN Port);

EFI_STATUS
EFIAPI OemSetMac (IN EFI_MAC_ADDRESS *Mac, IN UINTN Port);

OEM_NIC_PROTOCOL mOemNicProtocol = {
  .GetMac = OemGetMac,
  .SetMac = OemSetMac,
};

/*******************************************************************************
 函 数 名  : OemGetMacE2prom
 功能描述  : 从EEPROM中获取mac地址
 输入参数  : Port    
             *pucAddr  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年11月24日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数

*******************************************************************************/
EFI_STATUS OemGetMacE2prom(IN UINT32 Port, OUT UINT8 *pucAddr)
{
    I2C_DEVICE stI2cDev = {0};
    EFI_STATUS Status;
    UINT16     I2cOffset;
    
    Status = I2CInit(0, EEPROM_I2C_PORT, Normal);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Call I2CInit failed! p1=0x%x.\n", __FUNCTION__, __LINE__, Status));
        return Status;
    } 

    I2cOffset = I2C_OFFSET_EEPROM_ETH0 + (Port * 6);
    
    stI2cDev.DeviceType = DEVICE_TYPE_E2PROM;
    stI2cDev.Port = EEPROM_I2C_PORT;
    stI2cDev.SlaveDeviceAddress = I2C_SLAVEADDR_EEPROM;
    stI2cDev.Socket = 0;
    Status = I2CRead(&stI2cDev, I2cOffset, 6, pucAddr);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Call I2cRead failed! p1=0x%x.\n", __FUNCTION__, __LINE__, Status));
        return Status;
    } 

    return EFI_SUCCESS;
}

/*******************************************************************************
 函 数 名  : OemSetMacE2prom
 功能描述  : 向EEPROM中写入MAC地址
 输入参数  : Port    
             *pucAddr  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年11月24日
    作    者   : ZhuGeSheng
    修改内容   : 新生成函数

*******************************************************************************/
EFI_STATUS OemSetMacE2prom(IN UINT32 Port, IN UINT8 *pucAddr)
{
    I2C_DEVICE stI2cDev = {0};
    EFI_STATUS Status;
    UINT16     I2cOffset;
    
    Status = I2CInit(0, EEPROM_I2C_PORT, Normal);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Call I2CInit failed! p1=0x%x.\n", __FUNCTION__, __LINE__, Status));
        return Status;
    } 

    I2cOffset = I2C_OFFSET_EEPROM_ETH0 + (Port * 6);
    
    stI2cDev.DeviceType = DEVICE_TYPE_E2PROM;
    stI2cDev.Port = EEPROM_I2C_PORT;
    stI2cDev.SlaveDeviceAddress = I2C_SLAVEADDR_EEPROM;
    stI2cDev.Socket = 0;
    Status = I2CWrite(&stI2cDev, I2cOffset, 6, pucAddr);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] Call I2cWrite failed! p1=0x%x.\n", __FUNCTION__, __LINE__, Status));
        return Status;
    } 

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI OemGetMac (
  IN OUT EFI_MAC_ADDRESS *Mac,
  IN UINTN Port
  )
{
  EFI_STATUS Status;

  if (NULL == Mac)
  {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Mac buffer is null!\n", __FUNCTION__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }

  //TODO: discard port number, only support one port
  // Only 6 bytes are used
  Status = OemGetMacE2prom(Port, Mac->Addr);
  if ((EFI_ERROR(Status)))
  {
      DEBUG((EFI_D_ERROR, "[%a]:[%dL] Get mac failed!\n", __FUNCTION__, __LINE__));
      return Status;
  }
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI OemSetMac (
  IN EFI_MAC_ADDRESS *Mac,
  IN UINTN Port
  )
{
  EFI_STATUS Status;

  if (NULL == Mac)
  {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Mac buffer is null!\n", __FUNCTION__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }
  
  Status = OemSetMacE2prom(Port, Mac->Addr);
  if ((EFI_ERROR(Status)))
  {
      DEBUG((EFI_D_ERROR, "[%a]:[%dL] Set mac failed!\n", __FUNCTION__, __LINE__));
      return Status;
  }
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
OemNicConfigEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallProtocolInterface(
    &ImageHandle,
    &gOemNicProtocolGuid,
    EFI_NATIVE_INTERFACE,
    &mOemNicProtocol
    );

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Install Protocol failed %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }

  return EFI_SUCCESS;
}

