
#ifndef _MISC_BIOS_VENDER_H
#define _MISC_BIOS_VENDER_H

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>


#define FV_CUSTDATA_ADDR            FixedPcdGet32(PcdCustDataFvAddress)


#pragma pack(1)
typedef struct _OEM_TIME{
  UINT16  Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Min;
  UINT8   Sec;
}OEM_TIME;

typedef struct {
  UINT16       structV;
  char         vendor[32]; 
  OEM_TIME     buildTime;   // 
  char         strV[3];     
  char         strR[3];    
  char         strC[3];    
  char         strB[6];     
  UINT32       biosImageSize; 
  char         vName[32];   
  UINT32       biosUpdateFlag; 
  UINT32       fileCheckSum;   
  UINT32       dataCheckSum;   
  UINT32       ckRegionAddr0;  
  UINT32       ckRegionSize0;  
  UINT32       ckRegionAddr1;  
  UINT32       ckRegionSize1;  
  UINT32       ckRegionAddr2;  
  UINT32       ckRegionSize2;  
  UINT32       ckRegionAddr3;  
  UINT32       ckRegionSize3;  
  UINT32       updateRegionNum;  
  UINT32       updateAddr0;  
  UINT32       updateSize0;  
  UINT32       updateAddr1;  
  UINT32       updateSize1;  
  UINT32       updateAddr2;  
  UINT32       updateSize2;  
  UINT32       updateAddr3;  
  UINT32       updateSize3;  
} BIOS_OEM_DATA;

typedef struct _NV_DATA_HEADER {
  EFI_GUID      DataGuid;
  UINT32        CrcCheckSum;
  UINT16        DataSize;
  UINT16        Flag;
} NV_DATA_HEADER;

#pragma pack()

#endif

