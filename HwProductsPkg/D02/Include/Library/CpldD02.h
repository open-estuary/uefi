/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : CpldD02.h
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 
  最近修改   :
  功能描述   : Linaro D02 CPLD相关寄存器和cpldapi.h中定义不一样的地方先放在这里
  函数列表   :
  修改历史   :
  1.日    期   : 2015/1/26
    作    者   : s00296804
    修改内容   : 创建文件
  2.日    期   : 
    作    者   : 
    修改内容   : 

******************************************************************************/

#ifndef __CPLDD02_H__
#define __CPLDD02_H__
#define CPLD_LOGIC_VERSION                (0x52)
#define PRODUCT_VERSION                   (0x53)
#define CPLD_LOGIC_COMPLIER_YEAR          (0x54)
#define CPLD_LOGIC_COMPLIER_MONTH         (0x55)
#define CPLD_LOGIC_COMPLIER_DAY           (0x56)
#define CPLD_LOGIC_COMPLIER_HOUR          (0x57)
#define CPLD_LOGIC_COMPLIER_MINUTE        (0x58)
#define BOARD_ID                          (0x59)
#define BOM_VERSION                       (0x5A)
#define CPLD_BIOS_CURRENT_CHANNEL_REG_D02 (0x5B)

//PCIE解复位 针对Linaro D02板
#define CPU0_PCIE1_RESET_REG                (0x12)
#define CPU0_PCIE2_RESET_REG                (0x13)
#define CPU1_PCIE1_RESET_REG                (0x14)
#define CPU1_PCIE2_RESET_REG                (0x15)

#endif /* __CPLDD02_H__ */
