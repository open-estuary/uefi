/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Library\OemMiscLib.h


Description:

*************************************************/
#ifndef _OEM_MISC_LIB_H_
#define _OEM_MISC_LIB_H_

// TODO: 后续删除
#include <PlatformArch.h>
#include <Library/HwMemInitLib.h>
#include <BootLine.h>

#define I2C_PORT0   0
#define I2C_PORT1   1
#define I2C_INVALIDPORT   0xFF

#define ETH_NUM             2
#define ETH_MODTYPE_GE      0
#define ETH_MODTYPE_10GE    1
#define ETH_INVALID_MOD     0xFF

//暂放在此处，后续可能需要整改
#define I2C_SLAVEADDR_DCP       0x3E  
#define I2C_SLAVEADDR_FRU       0x52
#define I2C_SLAVEADDR_SFP_CONV  0x50
#define I2C_SLAVEADDR_SFP_PHY   0x56
#define I2C_SLAVEADDR_SFP_ENHA  0x51
#define I2C_SLAVEADDR_PCA9545   0x70

#define I2C_SLAVEADDR_CK420     0x69

#define CK420_SPREAD_ENABLE     0x9f  //CK420展频使能配置的值
#define CK420_SPREAD_DISABLE    0x9e  //CK420展频去使能配置的值
#define CK420_SPREAD_REG        0x1   //CK420展频寄存器地址
#define CK420_MIN_REG           0x0   //CK420最小寄存器地址
#define CK420_MAX_REG           0x9   //CK420最大寄存器地址
#define CK420_MAX_REG_NUM       0xa   //CK420有10个寄存器可访问
#define PLL_BACKUP

#define INVAILD_BOOT_OPTION    0xFFFF

EFI_STATUS OemPreStartBootOptionAction(UINT16 BootOptionId);
EFI_STATUS OemBootOrderSeting(IN OUT UINT16* BootOrderFinal, UINTN BootOrderSize, BOOLEAN *IsFlashBootFirst, BOOLEAN *FlashBootSupport);
EFI_STATUS OemGetSataBootNum(UINTN SataDesSize);
EFI_STATUS OemGetPXEBootNum(UINTN PXESize);

#define ETH_MAX_PORT          8   /* 最大网口个数 */
#define ETH_DEBUG_PORT0       6   /* 起始调试网口号 */
#define ETH_DEBUG_PORT1       7   /* 结束调试网口号 */

#define ETH_SPEED_10M     6
#define ETH_SPEED_100M    7
#define ETH_SPEED_1000M   8
#define ETH_SPEED_10KM    9
#define ETH_HALF_DUPLEX   0   /* 半双工  */
#define ETH_FULL_DUPLEX   1   /* 全双工  */

#define ETH_GDD_ID                          0x001378e0       /* 对接的GDD ID , 实际的ID是 0x001378e2 */
#define ETH_PHY_BCM5241_ID                  0x0143bc30       /* 对接的BCM5241 PHY */
#define ETH_PHY_MVL88E1145_ID               0x01410cd0       /* 对接的Marvell 88e1145 PHY */
#define ETH_PHY_MVL88E1119_ID               0x01410e80       /* 对接的Marvell 88e1119 PHY */
#define ETH_PHY_MVL88E1512_ID               0x01410dd0       /* 对接的Marvell 88e1512 PHY */
#define ETH_PHY_MVL88E1543_ID               0x01410ea0       /* 对接的Marvell 88e1543 PHY */
#define ETH_PHY_NLP3142_ID                  0x00000412       /* 对接XGE的phy */

#define ETH_INVALID                         0xffffffff

typedef struct EthProductDesc{
    UINT32 Valid;    /* 产品网卡是否有效，TRUE表示有效，FALSE表示不存在 */
    UINT32 Speed;
    UINT32 Duplex;
    UINT32 PhyId;
    UINT32 PhyAddr;
}ETH_PRODUCT_DESC;
typedef struct _I2C_SLAVE_ADDR{
    UINT8  Port;
    UINT8  DevAddr;
}I2C_SLAVE_ADDR;

typedef union
{
  struct
  {
    UINT32   Result   :  8;
    UINT32   Reserved :  24;
  } clock;         //时钟自检
  struct
  {
    UINT32   Result   :  8;
    UINT32   Reserved :  24;
  } cpld;          //CPLD自检
  struct
  {
    UINT32   Result   :  8;
    UINT32   Reserved :  24;
  } bios_crc;      //BIOS CRC自检
  struct
  {
    UINT32 Socket    :   8;
    UINT32 Channel   :   8;
    UINT32 Dimm      :   8;
    UINT32 Result    :   8;
  } dimm_absent;  //DIMM在位状态自检
  struct
  {
    UINT32 Socket    :   8;
    UINT32 Channel   :   8;
    UINT32 Dimm      :   8;
    UINT32 Result    :   8;
  } dimm_isolate;  //DIMM隔离状态自检

  UINT32    Data;
} U_SELF_TEST_PARA;

typedef enum
{
    SELF_TEST_CPU_BIST  = 0,
    SELF_TEST_CLOCK,    
    SELF_TEST_CPLD,
    SELF_TEST_BIOS_CRC,
    SELF_TEST_DIMM_ABSENT,
    SELF_TEST_DIMM_ISOLATE,

    SELF_TEST_BUTT
} E_SELF_TEST_ITEM;

typedef VOID (*SELF_TEST_REPORT) (IN UINT32 SelfTestPara);
typedef enum
{
    RESET_CAUSE_KEYPRESS       = 0x1,   //面板按键复位
    RESET_CAUSE_WDT            = 0x2,   //看门狗复位
    RESET_CAUSE_SOFT           = 0x3,   //软件reboot复位
    RESET_CAUSE_SHUT           = 0x4,   //关机复位
    RESET_CAUSE_UNKNOWN        = 0x6,   //Unkown复位
    RESET_CAUSE_TEMP           = 0x7,   //CPU过热保护复位
    RESET_CAUSE_POWER          = 0xa,   //上电复位
    RESET_CAUSE_MCE            = 0xb,   //MCE复位，相当于ARM的上电复位
    RESET_CAUSE_BIOS_SWITCH    = 0xc,   //BIOS切换复位
    RESET_CAUSE_CPLD_UPDATA    = 0xd,   //CPLD升级复位
    RESET_CAUSE_BIOS           = 0xf,   //BIOS复位系统
    RESET_CAUSE_SOFT_FAIL      = 0xa0,  //软件S3复位失败后，Cpld发硬复位
    RESET_CAUSE_WDT_FAIL       = 0xa1,  //看门狗S3复位失败后，Cpld发硬复位
    RESET_CAUSE_KEYPRESS_FAIL  = 0xa2,  //面板按钮S3复位失败后，Cpld发硬复位
    RESET_CAUSE_SES            = 0xa3,  //SES发起的大系统复位；

    RESET_CAUSE_CPLD_BUTT      = 0xff
} E_RESET_CAUSE;

VOID OemPostStartIndicator (VOID);
VOID OemPostEndIndicator (VOID);

extern I2C_SLAVE_ADDR  DimmSpdAddr[MAX_SOCKET][MAX_CHANNEL][MAX_DIMM];

BOOLEAN OemIsSocketPresent (UINTN Socket);

EFI_STATUS OemMemoryTest (UINTN Base, UINTN Size);

EFI_STATUS OemSelfTestReport(IN E_SELF_TEST_ITEM Item, IN U_SELF_TEST_PARA Para);
VOID OemSetSelfTestFailFlagAndAlarm(VOID);
UINT32 OemGetCurrentBiosChannel(VOID);
EFI_STATUS OemCheckCpld(VOID);
EFI_STATUS OemCheckClock(VOID);

E_RESET_CAUSE OemGetCurrentResetCause(VOID);
E_RESET_CAUSE OemGetPer2ResetCause(VOID);
E_RESET_CAUSE OemGetPer3ResetCause(VOID);
UINT32 OemIsWarmBoot();
VOID OemBiosSwitch(UINT32 Master);

VOID CoreSelectBoot(VOID);

VOID SetCpldBootDeviceID(IN UINT8 Value);
UINT8 ReadCpldBootDeviceID(void);
VOID SetCpldBootDeviceCount(IN UINT8 Value);
UINT8 ReadCpldBootDeviceCount(void);

VOID OemWriteProtectSet(BOOLEAN val);
BOOLEAN OemIsSpiFlashWriteProtected(VOID);
VOID OemPcieResetAndOffReset(void);
VOID OpenAlarmLed(VOID);
VOID OemPcieCardReset(UINT32 Reset);
EFI_STATUS OemCk420Read(UINT16 InfoOffset, UINT32 ulLength, UINT8 *pBuf);
EFI_STATUS OemCk420Write(UINT16 InfoOffset, UINT32 ulLength, UINT8 *pBuf);
UINT32  SelDdrFreq(pGBL_DATA pGblData);

VOID BoardInformation(void);

UINT8 OemAhciStartPort(VOID);
EFI_STATUS GetSetupData(SETUP_PARAMS *Setup);

VOID CpldRamWriteCpuBist(IN UINT32 SelfTestPara);
VOID CpldRamWriteClockCheck(IN UINT32 SelfTestPara);
VOID CpldRamWriteCpldCheck(IN UINT32 SelfTestPara);
VOID CpldRamWriteBiosCrcCheck(IN UINT32 SelfTestPara);
VOID CpldRamWriteDimmAbsent(IN UINT32 SelfTestPara);
VOID CpldRamWriteDimmIsolate(IN UINT32 SelfTestPara);


extern CHAR8 *sTokenList[];
UINT32 OemEthFindFirstSP();
ETH_PRODUCT_DESC *OemEthInit(UINT32 port);

UINTN OemGetSocketNumber(VOID);
UINTN OemGetDimmSlot(UINTN Socket, UINTN Channel);

#endif

