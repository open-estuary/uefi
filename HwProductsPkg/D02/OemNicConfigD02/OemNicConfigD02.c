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

