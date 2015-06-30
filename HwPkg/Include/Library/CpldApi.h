/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : CpldApi.h
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 
  最近修改   :
  功能描述   : CpldDriver.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014/10/20
    作    者   : y00216284
    修改内容   : 创建文件
  2.日    期   : 
    作    者   : 
    修改内容   : 

******************************************************************************/

#ifndef __CPLDAPI_H__
#define __CPLDAPI_H__

#include <Uefi/UefiBaseType.h>

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

#define MAX_LOCALBUS_ADDR_REG   8

#define CPLD_LOGIC_VER_REG_1                (0x00) 
#define CPLD_LOGIC_TEST_VER                 (0x0F)
#define CPLD_LPC_DEBUG_REG                  (0x01)
#define CPLD_BP_PCB_VER_REG                 (0x02)
#define CPLD_CTRL_BOARD_TYPE_REG            (0x03)
#define CPLD_CTRL_PRSNT_RE_REG              (0x04)
#define CPLD_PCIE_CARD_PRSNT_REG            (0x05)
#define CPLD_SYS_BOOT_FLG_REG               (0x06)
#define CPLD_CTR_PCB_VER_REG                (0x07)
#define CPLD_CTR_CPU_TEMP_REG               (0x09)
#define CPLD_WIFI_CTRL_REG                  (0x0A)
#define CPLD_BSP_INT_DET_EG                 (0x0B)
#define CPLD_SINGNAL_DETECT_REG             (0x0C)
#define CPLD_SYS_PWR_REBOOT_WP_REG          (0x0D)
#define CPLD_SYS_PWR_REBBOT_CONGTROL_REG    (0x0E)

#define CPLD_BIOS_SWITCH_SET_REG            (0x10)
#define CPLD_BIOS_CURRENT_CHANNEL_REG       (0x11)
#define CPLD_BIOS_CNT_REG                   (0x12)
#define CPLD_BIOS_WP_REG                    (0x13)
#define CPLD_FIRMWARE_UPDATE_REG            (0x14)
#define CPLD_FIRMWARE_UPDATE_WP_REG         (0x15)

#define CPLD_DOG_EN_REG                     (0x20)
#define CPLD_SYS_FEED_DOG_REG               (0x21)
#define CPLD_SYS_WTD_TIMESET_REG            (0x22)
#define CPLD_SYS_WTD_CONFIG_WP_REG          (0x23)

#define CPLD_CURRENT_REDET_REASON_REG       (0x30)
#define CPLD_PRE2_RESET_REASON_REG          (0x31)
#define CPLD_PRE3_RESET_REASON_REG          (0x32)
#define CPLD_IC_RST_REG_A                   (0x33)
#define CPLD_IC_RST_REG_B                   (0x34)
#define CPLD_SYS_RESET_CNT_REG              (0x35)
#define CPLD_BSP_CLR_RESET_CNT_REG          (0x36)
#define CPLD_CURRENT_CPLD_ASS_REG_A         (0x37)
#define CPLD_IC_RST_REG_WP                  (0x39)
#define CPLD_BIOS_RESET_SYS_REG             (0x38)
#define CPLD_LAST_8_TIMES_RST_REASON_REG_1  (0x18)
#define CPLD_LAST_8_TIMES_RST_REASON_REG_2  (0x19)
#define CPLD_LAST_8_TIMES_RST_REASON_REG_3  (0x1A)
#define CPLD_LAST_8_TIMES_RST_REASON_REG_4  (0x1B)
#define CPLD_LAST_8_TIMES_RST_REASON_REG_5  (0x1C)
#define CPLD_LAST_8_TIMES_RST_REASON_REG_6  (0x1D)
#define CPLD_LAST_8_TIMES_RST_REASON_REG_7  (0x1E)
#define CPLD_LAST_8_TIMES_RST_REASON_REG_8  (0x1F)
#define CPLD_LAST_8_TIMES_RST_TYPE_REG_1    (0x28)
#define CPLD_LAST_8_TIMES_RST_TYPE_REG_2    (0x29)
#define CPLD_LAST_8_TIMES_RST_TYPE_REG_3    (0x2A)
#define CPLD_LAST_8_TIMES_RST_TYPE_REG_4    (0x2B)
#define CPLD_LAST_8_TIMES_RST_TYPE_REG_5    (0x2C)
#define CPLD_LAST_8_TIMES_RST_TYPE_REG_6    (0x2D)
#define CPLD_LAST_8_TIMES_RST_TYPE_REG_7    (0x2E)
#define CPLD_LAST_8_TIMES_RST_TYPE_REG_8    (0x2F)
#define CPLD_TCC_TEMP_REG                   (0x3A)

#define CPLD_PER_OFF_REG                    (0x50)
#define CPLD_PWR_OFF_WP_REG                 (0x51)
#define CPLD_BIOS_WR_REG                    (0x52)
#define CPLD_CLK_DET_REG                    (0x53)
#define CPLD_FREQ_CTRL_INT_REG              (0x54)
#define CPLD_MEM_TEST_EN_REG                (0x55)
#define CPLD_PCIE_PWR_OFF_REG               (0x56)
#define CPLD_PCIE_PWR_OFF_WP_REG            (0x57)
#define CPLD_PCIE_WSW_EEP_WP_REG            (0x58)
#define CPLD_PWR_DOWN_LED_FLASH_REG         (0x59)
#define CPLD_LBS_INT_REG                    (0x5A)
#define CPLD_CARD_TYPE_REG                  (0x5C)

#define CPLD_BSP2SES_FIFO_DATA_REG          (0xF0)
#define CPLD_BSP2SES_FIFO_CTRL_REG          (0xF1)

#define CPLD_INT_CTRL_REG_0                 (0x100)
#define CPLD_INT_CTRL_REG_1                 (0x101)
#define CPLD_INT_STATUS_REG_0               (0x110)
#define CPLD_INT_STATUS_REG_1               (0x111)
#define CPLD_HOTPLUG_CTRL_REG_0             (0x120) 
#define CPLD_HOTPLUG_CTRL_REG_1             (0x121) 
#define CPLD_HOTPLUG_CTRL_REG_2             (0x122) 
#define CPLD_HOTPLUG_CTRL_REG_3             (0x123) 
#define CPLD_HOTPLUG_CTRL_REG_4             (0x124) 
#define CPLD_HOTPLUG_CTRL_REG_5             (0x125) 
#define CPLD_HOTPLUG_CTRL_REG_6             (0x126) 
#define CPLD_HOTPLUG_CTRL_REG_7             (0x127) 
#define CPLD_HOTPLUG_CTRL_REG_8             (0x128) 
#define CPLD_HOTPLUG_CTRL_REG_9             (0x129) 
#define CPLD_HOTPLUG_CTRL_REG_10            (0x12A) 
#define CPLD_HOTPLUG_CTRL_REG_11            (0x12B) 
#define CPLD_HOTPLUG_CTRL_REG_12            (0x12C) 
#define CPLD_HOTPLUG_CTRL_REG_13            (0x12D) 
#define CPLD_HOTPLUG_CTRL_REG_14            (0x12E) 
#define CPLD_HOTPLUG_CTRL_REG_15            (0x12F) 
#define CPLD_HOTPLUG_STATUS_REG_0           (0x180) 
#define CPLD_HOTPLUG_STATUS_REG_1           (0x181) 
#define CPLD_HOTPLUG_STATUS_REG_2           (0x182) 
#define CPLD_HOTPLUG_STATUS_REG_3           (0x183) 
#define CPLD_HOTPLUG_STATUS_REG_4           (0x184) 
#define CPLD_HOTPLUG_STATUS_REG_5           (0x185) 
#define CPLD_HOTPLUG_STATUS_REG_6           (0x186) 
#define CPLD_HOTPLUG_STATUS_REG_7           (0x187) 
#define CPLD_HOTPLUG_STATUS_REG_8           (0x188) 
#define CPLD_HOTPLUG_STATUS_REG_9           (0x189) 
#define CPLD_HOTPLUG_STATUS_REG_10          (0x18A) 
#define CPLD_HOTPLUG_STATUS_REG_11          (0x18B) 
#define CPLD_HOTPLUG_STATUS_REG_12          (0x18C) 
#define CPLD_HOTPLUG_STATUS_REG_13          (0x18D) 
#define CPLD_HOTPLUG_STATUS_REG_14          (0x18E) 
#define CPLD_HOTPLUG_STATUS_REG_15          (0x18F) 
#define CPLD_CARD_SLOT_RST_REG_0            (0x1E0) 
#define CPLD_CARD_SLOT_RST_REG_1            (0x1E1) 
#define CPLD_CARD_SLOT_RST_WP_REG_0         (0x1F0) 
#define CPLD_CARD_SLOT_RST_WP_REG_1         (0x1F1) 

#define CPLD_LED_XGE_P0_REG                 (0x200) 
#define CPLD_LED_XGE_P1_REG                 (0x201) 
#define CPLD_LED_XGE_P2_REG                 (0x202) 
#define CPLD_LED_XGE_P3_REG                 (0x203) 
#define CPLD_CTL_SF_P0_REG                  (0x20C) 
#define CPLD_CTL_SF_P1_REG                  (0x20D)
#define CPLD_CTL_SF_P2_REG                  (0x20E)
#define CPLD_CTL_SF_P3_REG                  (0x20F)

#define CPLD_MINI_SAS1_PRSNT_REG            (0x218) 
#define CPLD_LED_MINI_SAS1_REG              (0x219) 

#define CPLD_BOOT_PLNST_ADDR                (0xC000)
#define CPLD_BOOT_ID_ADDR                   (0xC001) 
#define CPLD_BOOT_CNT_ADDR                  (0xC002) 
#define CPLD_BOOT_RESENT_REG                (0xC006) 
#define CPLD_SATA_NUM_REG                   (0xC007) 
#define CPLD_BOOT_SATA_NUM_REG              (0xC008) 

#define CPLD_DDR_DIMM_ABSENT_REG            (0xC104) 
#define CPLD_DDR_DIMM_ISOLATE_REG           (0xC10A)

#define CPLD_SELFTEST_FAILURE_REG           (0xC110)
#define CPLD_UNITED_TESTINFO_REG1           (0xC11D) 
#define CPLD_UNITED_TESTINFO_REG2           (0xC11F) 

#define CPLD_CLOCK_CHECK_MASK            BIT4 
#define CPLD_SELF_CHECK_MASK             BIT5 
#define CPLD_BIOS_CURRENT_CHANNEL_MASK   BIT1 
#define CPLD_MASTER_BIOS_CRC_CHECK_MASK  BIT4 
#define CPLD_SLAVE_BIOS_CRC_CHECK_MASK   BIT5 

#define CPLD_ALARM_LED_FIG               0x01
#define CPLD_SELFTEST_FAILURE_FLAG       0x5A  
#define CPLD_CLOCK_CHECK_FLAG            0x03 
#define CPLD_CPLD_CHECK_FLAG             0x55 

#define BIT_CLR(_value, _mask) ((_value) = ((_value) & (~(_mask))))
#define BIT_SET(_value, _mask) ((_value) = ((_value) | (_mask)))
#define BIT_GET(_value, _mask) ((_value) & (_mask))

#define CPLD_MAX_CHANNEL     4
#define CPLD_MAX_DIMM        3
#define XGE_CARD  0x30
#define GE_CARD  0x31

VOID WriteCpldReg(UINTN ulRegAddr, UINT8 ulValue);
UINT8 ReadCpldReg(UINTN ulRegAddr);
UINT32 GetCurrentBiosChannel(VOID);
EFI_STATUS CheckCpld(VOID);
EFI_STATUS CheckClock(VOID);
VOID SetSelfTestFailFlagAndAlarm(VOID);


#endif /* __CPLDAPI_H__ */
