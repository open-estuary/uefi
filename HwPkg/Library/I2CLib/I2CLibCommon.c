
#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

#include <Library/PlatformSysCtrlLib.h>
#include "I2CLibInternal.h"

UINTN GetI2cBase (UINT32 Socket, UINT8 Port)
{
  return PlatformGetI2cBase(Socket, Port);
}

EFI_STATUS
I2cLibRuntimeSetup (UINT32 Socket, UINT8 Port)
{
  return EFI_SUCCESS; 
}


