/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpat0TH65jPMlUENBvGXufQCTRBL0qDbp15usAFL4sRUNaa9PQnehq7qerrn2
LiJ41pGVXWrwm7fRrbOc7cv/RBvOWTC9GA4mpF1u09KPOQHRzbSSGFi+UOh/kVSFoTYcCgdL
Qz4ak5JrQUxd/woakscmfyVEovHuopf1/nl9CjWtwWRhfYb5eX+JiXFqt3weuA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NorFlashProtocol.h
  版 本 号   : v2.0
  作    者   :
  生成日期   : 2013年03月04日
  最近修改   :
  功能描述   : Flash协议
  修改历史   :
1.   日	  期   : 
     作	  者   :
     修改内容  :
******************************************************************************/
#ifndef _NOR_FLASH_PROTOCOL_H_
#define _NOR_FLASH_PROTOCOL_H_

#define UNI_NOR_FLASH_PROTOCOL_GUID   \
    {0x86F305EA, 0xDFAC, 0x4A6B, {0x92, 0x77, 0x47, 0x31, 0x2E, 0xCE, 0x42, 0xA}} 

typedef struct _UNI_NOR_FLASH_PROTOCOL UNI_NOR_FLASH_PROTOCOL;

typedef 
EFI_STATUS
(EFIAPI *UNI_FLASH_ERASE_INTERFACE) (
    IN UNI_NOR_FLASH_PROTOCOL   *This,
    IN UINT32                  Offset,
    IN UINT32                  Length
    );
typedef 
EFI_STATUS
(EFIAPI *UNI_FLASH_WRITE_INTERFACE) (
    IN UNI_NOR_FLASH_PROTOCOL   *This,
    IN  UINT32                 Offset,
    IN  UINT8                 *Buffer,
    UINT32                   ulLength
    );

typedef 
EFI_STATUS
(EFIAPI *UNI_FLASH_READ_INTERFACE) (
    IN UNI_NOR_FLASH_PROTOCOL   *This,
    IN UINT32                  Offset,
    IN OUT UINT8              *Buffer,
    IN UINT32                   ulLen
    );

// 修改协议接口头文件，便于EDKII的驱动调用
struct _UNI_NOR_FLASH_PROTOCOL {
    UNI_FLASH_ERASE_INTERFACE             Erase;
    UNI_FLASH_WRITE_INTERFACE             Write;
    UNI_FLASH_READ_INTERFACE              Read;
};

extern EFI_GUID gUniNorFlashProtocolGuid;

#endif
