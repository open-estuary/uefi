/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

******************************************************************************
  文 件 名   : MiscBiosVendor.h
  版 本 号   : 初稿
  作    者   : y00216284
  生成日期   : 
  最近修改   :
  功能描述   : 
  函数列表   :   
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 创建文件  
******************************************************************************/
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

//内嵌版本信息
typedef struct {
  UINT16       structV;
  char         vendor[32];  // 厂商名
  OEM_TIME     buildTime;   // 
  char         strV[3];     // BIOS的V版本号，字符串"xxx\0"
  char         strR[3];     // BIOS的R版本号，字符串"xxx\0"
  char         strC[3];     // BIOS的C版本号，字符串"xx\0"
  char         strB[6];     // BIOS的B版本号，字符串"xxx\0"
  UINT32       biosImageSize; 
  char         vName[32];   // 版本名称
  UINT32       biosUpdateFlag; 
  UINT32       fileCheckSum;    // BIOS文件的Checksum，保证整个BIOS文件累计和为0
  UINT32       dataCheckSum;   // Checksum Region0~3的校验和
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

