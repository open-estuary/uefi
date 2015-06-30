#ifndef _OEM_DEVICE_PATH_H_
#define _OEM_DEVICE_PATH_H_
#include <Protocol/DevicePath.h>


typedef enum
{
    C_PXE  = 1,
    C_SATA = 2,
    C_SAS  = 3,
    C_USB  = 4,
} CONTROLLER_TYPE;


typedef struct{
  VENDOR_DEVICE_PATH Vender;
  
  UINT8 ControllerType;
  
  UINT8 Socket;
  
  UINT8 Port;
  
} EXT_VENDOR_DEVICE_PATH;



#endif

