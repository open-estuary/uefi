/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpat0TH65jPMlUENBvGXufQCm4LaY1Scm2hXUYwJOrTRw72N6o5uLoxSzomTX
lSLrv86M0e0U/kRcVIZQ9gsGzXR/5XRzovQIyRGZ6hpFhM7h1IE+l6EW61nbLC6NRVX2fJSh
InuzJt5rIAjdKuMdMPx6Fvd09ecHNN3SB5W3Shqk8jy1RkbJrkabhzeDm8E2BA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NorFlashHw.h
  版 本 号   : v2.0
  作    者   :
  生成日期   : 2013年03月04日
  最近修改   :
  功能描述   : 头文件
  修改历史   :
1.   日	  期   : 
     作	  者   :
     修改内容  :
******************************************************************************/

#ifndef _NOR_FLASH_HW_H_
#define _NOR_FLASH_HW_H_

#include <Uefi/UefiBaseType.h>


#define FOUR_BYTE_UNIT    4
#define FLASH_MAX_UNIT    4

#define FLASH_DEVICE_NUM  0x10


/*芯片信息定义*/
typedef struct {
    UINT32 ManufacturerID;         // 厂商 ID
    UINT32 DeviceID1;              // 设备ID cycle1
    UINT32 DeviceID2;              // 设备ID cycle2
    UINT32 DeviceID3;              // 设备ID cycle3
    UINT8  ParallelNum;            // 并联数
    UINT32 SingleChipSize;         // 单片Flash大小
    UINT32 BlockSize;              // 块大小    
    UINT32 BufferProgramSize;      // Buffer Write 大小 
    UINT32 CommandType;            //指定使用那组命令字
}NOR_FLASH_INFO_TABLE;

/*Define Command Address And Data*/
/*reset*/
typedef struct {
    UINT32 CommandType;
    UINT32 ResetData;
}FLASH_COMMAND_RESET;

/*manufacture ID & Device ID*/
typedef struct {
    UINT32 CommandType;
    UINT32 ManuIDAddressStep1;
    UINT32 ManuIDDataStep1;   
    UINT32 ManuIDAddressStep2; 
    UINT32 ManuIDDataStep2; 
    UINT32 ManuIDAddressStep3;
    UINT32 ManuIDDataStep3; 
    UINT32 ManuIDAddress;
    
    UINT32 DeviceIDAddress1;
    UINT32 DeviceIDAddress2;          
    UINT32 DeviceIDAddress3;
}FLASH_COMMAND_ID;

/*Write Buffer*/
typedef struct {
    UINT32 CommandType;
    UINT32 BufferProgramAddressStep1;
    UINT32 BufferProgramDataStep1;
    UINT32 BufferProgramAddressStep2;
    UINT32 BufferProgramDataStep2;
    UINT32 BufferProgramDataStep3;
    UINT32 BufferProgramtoFlash;
}FLASH_COMMAND_WRITE;

/*erase*/
typedef struct {
    UINT32 CommandType;
    UINT32 SectorEraseAddressStep1;
    UINT32 SectorEraseDataStep1;
    UINT32 SectorEraseAddressStep2;
    UINT32 SectorEraseDataStep2;
    UINT32 SectorEraseAddressStep3;
    UINT32 SectorEraseDataStep3;
    UINT32 SectorEraseAddressStep4;
    UINT32 SectorEraseDataStep4;
    UINT32 SectorEraseAddressStep5;
    UINT32 SectorEraseDataStep5;
    UINT32 SectorEraseDataStep6;
}FLASH_COMMAND_ERASE;

/*定义索引结构体，可变化*/
typedef struct {
    UINT32 Base;
    UINT32 InfIndex;
    UINT32 ReIndex;
    UINT32 IdIndex;
    UINT32 WIndex;
    UINT32 EIndex;
}FLASH_INDEX;

/*定义外部调用函数*/
extern EFI_STATUS FlashInit(UINT32 Base);
extern EFI_STATUS SectorErase(UINT32 Base, UINT32 Offset);
extern EFI_STATUS BufferWrite(UINT32 Offset, void *pData, UINT32 Length);
extern EFI_STATUS IsNeedToWrite(UINT32 Base, UINT32 Offset, UINT8 *Buffer, UINT32 Length);

/*定义外部调用全局函数*/
extern NOR_FLASH_INFO_TABLE gFlashInfo[FLASH_DEVICE_NUM];
extern FLASH_COMMAND_RESET gFlashCommandReset[FLASH_DEVICE_NUM];
extern FLASH_COMMAND_ID gFlashCommandId[FLASH_DEVICE_NUM];
extern FLASH_COMMAND_WRITE gFlashCommandWrite[FLASH_DEVICE_NUM];
extern FLASH_COMMAND_ERASE gFlashCommandErase[FLASH_DEVICE_NUM];
extern FLASH_INDEX gIndex;


#endif
