#ifndef _I2C_LIB_INTERNAL_H_
#define _I2C_LIB_INTERNAL_H_

#include <PlatformArch.h>
#include <Library/I2CLib.h>

UINTN GetI2cBase (UINT32 Socket, UINT8 Port);

extern UINTN gI2cBase[MAX_SOCKET][I2C_PORT_MAX];

EFI_STATUS
I2cLibRuntimeSetup (UINT32 Socket, UINT8 Port);


#endif

