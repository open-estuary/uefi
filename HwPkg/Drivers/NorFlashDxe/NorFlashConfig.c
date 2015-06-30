/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePcxREXVZ5dUR67fNFdcsGWwP1ecJzNoC5Nk+8B
iJvImHvqoFBb8vMj8qae+asma/TE90PHeQ1tn3GI1rb0YeJFw+Is1WpJiJp6owJzkXtfxIfF
HKSz6lc3gH5grRBTS4d4IaHpAvNwlozbSskoaYi9JK43lj0T6BRmgUIJWNRGiw==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NorFlashConfig.c
  版 本 号   : v1.2
  作    者   : c00213799
  生成日期   : 2013年03月04日
  最近修改   :
  功能描述   : NorFlash信息配置，包括flash大小、命令字等
  修改历史   :
1.   日	  期   : 2013/4/7
     作	  者   : c00213799
******************************************************************************/


#include "NorFlashHw.h"

//定义支持的命令字类型
#define COMMAND_TYPE1 0x1
#define COMMAND_TYPE2 0x2
#define COMMAND_TYPE3 0x4
#define COMMAND_TYPE4 0x8
#define COMMAND_TYPE5 0x10


//芯片信息定义
NOR_FLASH_INFO_TABLE gFlashInfo[]=
{

    {//S29GL512m
      0x00010001,     // 32位ManuID，以32位表示
      0x227E227E,     // 32位DeviceID1
      0x22232223,     // 32位DeviceID2
      0x22012201,     // 32位DeviceID3
      1,              // 并联数
      0x20000000,     // 单片NorFlash大小  
      0x20000,        // 单片NorFlash块大小   
      0x0010,         // 支持Buffer Write大小
      COMMAND_TYPE1   // 支持的命令字类型
    },
    {//S29GL1g
      0x00010001,     // 32位ManuID，以32位表示
      0x227E227E,     // 32位DeviceID1
      0x22282228,     // 32位DeviceID2
      0x22012201,     // 32位DeviceID3
      1,              // 并联数
      0x40000000,     // 单片NorFlash大小  
      0x20000,        // 单片NorFlash块大小   
      0x0020,         // 支持Buffer Write大小
      COMMAND_TYPE1   // 支持的命令字类型
    },
    {//M29ew512m
      0x00890089,     // 32位ManuID，以32位表示
      0x227E227E,     // 32位DeviceID1
      0x22232223,     // 32位DeviceID2
      0x22012201,     // 32位DeviceID3，表示512M
      1,              // 并联数
      0x20000000,     // 单片NorFlash大小  
      0x20000,        // 单片NorFlash块大小   
      0x0010,         // 支持Buffer Write大小
      COMMAND_TYPE1   // 支持的命令字类型
    },
    {//M29EW2g
      0x00890089,     // 32位ManuID，以32位表示
      0x227E227E,     // 32位DeviceID1
      0x22482248,     // 32位DeviceID2
      0x22012201,     // 32位DeviceID3
      1,              // 并联数
      0x80000000,      // 单片NorFlash大小，2g  
      0x20000,        // 单片NorFlash块大小   
      0x0020,         // 支持Buffer Write大小
      COMMAND_TYPE1   // 支持的命令字类型
    },
    {//支持16位
      0x00890089,
      0x227E227E,
      0x22282228,
      0x22012201,     // 设备ID,1g
      1,
      0x10000000,
      0x20000,
      0x0020,
      COMMAND_TYPE1
    },
    {//支持32位，以下数据定义同上
      0x00890089,
      0x227E227E,
      0x22282228,
      0x22012201,     // 设备ID,1g
      2,
      0x10000000,
      0x20000,
      0x0020,
      COMMAND_TYPE1
    }
};

//芯片命令字定义
/*reset命令字*/ 
FLASH_COMMAND_RESET gFlashCommandReset[]=
{
    { 
       COMMAND_TYPE1,   // 命令字类型，与芯片信息对应
       (0x00F000F0)     // 复位命令字中发送的32位数据
    }
    
};

/*manufacture ID & Device ID 命令字*/
FLASH_COMMAND_ID gFlashCommandId[]=
{
     {
       COMMAND_TYPE1,   // 命令字类型，与芯片信息对应
       (0x0555),        // 获取ID命令字，第一步地址
       (0x00AA00AA),    // 获取ID命令字，第一步32位数据
       (0x02AA),        // 获取ID命令字，第二步地址                    
       (0x00550055),    // 获取ID命令字，第二步32位数据
       (0x0555),        // 获取ID命令字，第三步地址
       (0x00900090),    // 获取ID命令字，第三步32位数据
       (0x0000),        // 获取ManuID命令字，第四步地址                            
       
       (0x0001),        // 获取DeviceID命令字，第四步地址
       (0x000E),        // 获取DeviceID命令字，第五步地址
       (0x000F)         // 获取DeviceID命令字，第六步地址
    }
};

/*Write Buffer 命令字*/
FLASH_COMMAND_WRITE gFlashCommandWrite[]=
{
    {
       COMMAND_TYPE1,   // 命令字类型，与芯片信息对应
       (0x0555),        // 发送写命令字，第一步地址
       (0x00AA00AA),    // 发送写命令字，第一步32位数据
       (0x02AA),        // 发送写命令字，第二步地址                     
       (0x00550055),    // 发送写命令字，第二步32位数据
       (0x00250025),    // 发送写命令字，第三步32位数据
       (0x00290029)     // 发送写命令字，第四步32位数据
    }
   
};

/*erase 命令字*/
FLASH_COMMAND_ERASE gFlashCommandErase[]=
{
    {
       COMMAND_TYPE1,   // 命令字类型，与芯片信息对应
       (0x0555),        // 发送块擦命令字，第一步地址
       (0x00AA00AA),    // 发送块擦命令字，第一步32位数据
       (0x02AA),        // 发送块擦命令字，第二步地址                    
       (0x00550055),    // 发送块擦命令字，第二步32位数据
       (0x0555),        // 发送块擦命令字，第三步地址
       (0x00800080),    // 发送块擦命令字，第三步32位数据
       (0x0555),        // 发送块擦命令字，第四步地址
       (0x00AA00AA),    // 发送块擦命令字，第四步32位数据
       (0x02AA),        // 发送块擦命令字，第五步地址                     
       (0x00550055),    // 发送块擦命令字，第五步32位数据
       (0x00300030)     // 发送块擦命令字，第六步32位数据
    }
  
};

