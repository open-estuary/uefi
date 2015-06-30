/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Library\PlatformSysCtrlLib.h


Description: 

*************************************************/

#ifndef _PLATFORM_SYS_CTRL_LIB_H_
#define _PLATFORM_SYS_CTRL_LIB_H_

#define PACKAGE_16CORE      0
#define PACKAGE_32CORE      1
#define PACKAGE_RESERVED    2
#define PACKAGE_TYPE_NUM    3

UINT32 PlatformGetPackageType (VOID);

VOID DisplayCpuInfo (VOID);

UINTN PlatformGetPll (UINT32 NodeId, UINTN Pll);

#define DJTAG_READ_INVALID_VALUE 0xFFFFFFFF
#define DJTAG_CHAIN_ID_AA        1
#define DJTAG_CHAIN_ID_LLC       4

/*系统控制寄存器DJTAG部分偏移地址*/
#define SC_DJTAG_MSTR_EN_OFFSET             0x6800
#define SC_DJTAG_MSTR_START_EN_OFFSET       0x6804
#define SC_DJTAG_SEC_ACC_EN_OFFSET          0x6808
#define SC_DJTAG_DEBUG_MODULE_SEL_OFFSET    0x680C
#define SC_DJTAG_MSTR_WR_OFFSET             0x6810
#define SC_DJTAG_CHAIN_UNIT_CFG_EN_OFFSET   0x6814
#define SC_DJTAG_MSTR_ADDR_OFFSET           0x6818
#define SC_DJTAG_MSTR_DATA_OFFSET           0x681C
#define SC_DJTAG_TMOUT_OFFSET               0x6820
#define SC_TDRE_OP_ADDR_OFFSET              0x6824
#define SC_TDRE_WDATA_OFFSET                0x6828
#define SC_TDRE_REPAIR_EN_OFFSET            0x682C
#define SC_DJTAG_RD_DATA0_OFFSET            0xE800
#define SC_TDRE_RDATA0_OFFSET               0xE830


UINTN PlatformGetI2cBase(UINT32 Socket,UINT8 Port);

VOID PlatformAddressMapCleanUp (VOID);
VOID PlatformDisableDdrWindow (VOID);

VOID PlatformEnableArchTimer (VOID);

EFI_STATUS
DawFindFreeWindow (UINTN Socket, UINTN *DawIndex);

VOID DawSetWindow (UINTN Socket, UINTN WindowIndex, UINT32 Value);

VOID DJTAG_TDRE_WRITE(UINT32 Offset, UINT32 Value, UINT32 ChainID, UINT32 NodeId, BOOLEAN Repair);

UINT32 DJTAG_TDRE_READ(UINT32 Offset, UINT32 ChainID, UINT32 NodeId, BOOLEAN Repair);

VOID RemoveRoceReset(VOID);

VOID ITSCONFIG (VOID);

VOID MN_CONFIG (VOID);

VOID SmmuConfigForLinux (VOID);

VOID StartupAp (VOID);

VOID LlcCleanInvalidate (VOID);

UINTN PlatformGetCpuFreq (UINT8 Socket);

UINTN PlatformGetCoreCount (VOID);
UINTN PlatformGetDdrChannel (VOID);
#endif
