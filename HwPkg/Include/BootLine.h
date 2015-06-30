/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : BootLine.h
  版 本 号   : v2.0
  作    者   : c00213799
  生成日期   : 2015年03月15日
  最近修改   :
  功能描述   :
  修改历史   :
******************************************************************************/

#ifndef __BOOT_LINE_H__
#define __BOOT_LINE_H__


#define PRODUCT_NAME_LENGTH		0x10  
#define SN_LENGTH	         	0x10  	
#define BIOS_VERSION_LENGTH		0x10  
#define BOOT_ORDER_LENGTH	    0x10

#define MaxInputNum             255

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
  OEM_TIME     buildTime; 
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

typedef struct {
  UINT32  CpuNum; 
}CPU_INFO;

typedef struct {
  UINT32    DdrCapacity; 
  UINT16    DdrFrequency;
  UINT8     NUMAOptimized; 
  UINT16    FrequencyChoice; 
  UINT8     ChannelInterleaving;
  UINT8     RankInterleaving;
  UINT8     RankMarginTool; 
  UINT32    RMTPatternLength; 
  UINT8     PerBitMargin;      
  UINT8     MemoryEccSupport;    
  UINT8     PatrolScrub;         
  UINT8     PatrolScrubInterval; 
  UINT8     DemandScrub;         
}DDR_INFO;

typedef struct			
{
  CHAR8     ProductName[PRODUCT_NAME_LENGTH];
  CHAR8     SerialNum[SN_LENGTH];
  CHAR8     BiosVersion[BIOS_VERSION_LENGTH]; 
  OEM_TIME  BiosBuildTime; 
  CPU_INFO  CpuInfo;
  DDR_INFO  DdrInfo; 
  CHAR8     BootSequence[BOOT_ORDER_LENGTH];
} SETUP_PARAMS;

VOID EblInitializeBootLineDefault(VOID);
EFI_STATUS PrintBootlineInfo(VOID);
EFI_STATUS ChangeBootlineInfo(VOID);
EFI_STATUS SaveBootlineInfo(VOID);
EFI_STATUS VerifyBootLineEntry(VOID);

#endif

