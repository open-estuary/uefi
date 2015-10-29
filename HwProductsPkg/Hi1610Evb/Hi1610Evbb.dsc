#
#  Copyright (c) 2011-2012, ARM Limited. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = Hi1610Evbb
  PLATFORM_GUID                  = D0D445F1-B2CA-4101-9986-1B23525CBEA6
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/Hi1610Evbb
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = HwProductsPkg/Hi1610Evb/$(PLATFORM_NAME).fdf
  DEFINE EDK2_SKIP_PEICORE=0

!include HwProductsPkg/Pv660.dsc.inc

[LibraryClasses.common]
  ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64Lib.inf
  ArmCpuLib|ArmPkg/Drivers/ArmCpuLib/ArmCortexAEMv8Lib/ArmCortexAEMv8Lib.inf
  #ArmPlatformLib|ArmPlatformPkg/ArmVExpressPkg/Library/ArmVExpressLibRTSM/ArmVExpressLib.inf
  ArmPlatformLib|HwPkg/Library/ArmPlatformLibPv660/ArmPlatformLib.inf

  ArmPlatformSysConfigLib|ArmPlatformPkg/ArmVExpressPkg/Library/ArmVExpressSysConfigLib/ArmVExpressSysConfigLib.inf
  NorFlashPlatformLib|ArmPlatformPkg/ArmVExpressPkg/Library/NorFlashArmVExpressLib/NorFlashArmVExpressLib.inf
  LcdPlatformLib|ArmPlatformPkg/ArmVExpressPkg/Library/PL111LcdArmVExpressLib/PL111LcdArmVExpressLib.inf

  #DebugAgentTimerLib|ArmPlatformPkg/ArmVExpressPkg/Library/DebugAgentTimerLib/DebugAgentTimerLib.inf

  #c00213799 2013.6.29 I2C库接口
  I2CLib|HwPkg/Library/I2CLib/I2CLib.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf

  #start-uniBIOS-x00131865-002 网络 2012-3-30
  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf



  #FDTUpdateLib
  FdtUpdateLib|HwProductsPkg/D02/FdtUpdateLibD02/FdtUpdateLib.inf

  # uniBIOS-y00216284 DTS2014071901841 2014-7-19
  #【HRD ARM PV660 V100R001C00B010 BIOS】提供PCIe初始化功能
  #PcieInitLib|HwPkg/PcieInitLib/PcieInitLib.inf

  CpldDriverLib|HwPkg/Library/CpldDriverLib/CpldDriverLib.inf

  SerdesLib|HwPkg/Library/Serdes/Hi1610Serdes/Hi1610SerdesLib.inf

  # ARM PL031 RTC Driver
  RealTimeClockLib|HwPkg/Library/VirtualRealTimeClockLib/RealTimeClockLib.inf
  #RealTimeClockLib|HwPkg/DS3231RealTimeClockLib/DS3231RealTimeClockLib.inf

  OemMiscLib|HwProductsPkg/Hi1610Evb/Library/OemMiscLibEvb/OemMiscLibEvbb.inf
  OemAddressMapLib|HwProductsPkg/Hi1610Evb/Library/OemAddressMapHi1610/OemAddressMapHi1610.inf
  BootLineLib|HwProductsPkg/D02/Library/BootLineLibD02/BootLineLibD02.inf
  PlatformSysCtrlLib|HwPkg/Library/PlatformSysCtrlLib/PlatformSysCtrlLibHi1610/PlatformSysCtrlLibHi1610.inf

[LibraryClasses.common.SEC]
  ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64LibSec.inf
  #ArmPlatformLib|ArmPlatformPkg/ArmVExpressPkg/Library/ArmVExpressLibRTSM/ArmVExpressLibSec.inf
  ArmPlatformLib|HwPkg/Library/ArmPlatformLibPv660/ArmPlatformLibSec.inf
  EfiResetSystemLib|HwProductsPkg/Hi1610Evb/Library/ResetSystemLibCpld/ResetSystemLibSec.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  #EfiResetSystemLib|HwProductsPkg/Hi1610Evb/Library/ResetSystemLibCpld/ResetSystemLib.inf

[BuildOptions]
  GCC:*_*_AARCH64_ARCHCC_FLAGS  = -DARM_CPU_AARCH64 -mstrict-align
  GCC:*_*_AARCH64_PP_FLAGS  = -DARM_CPU_AARCH64
  GCC:*_*_AARCH64_PLATFORM_FLAGS == -I$(WORKSPACE)/ArmPlatformPkg/ArmVExpressPkg/Include -I$(WORKSPACE)/ArmPlatformPkg/ArmVExpressPkg/Include/Platform/RTSM -I$(WORKSPACE)/HwPkg/Include/Platform/Hi1610


################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]

!if $(EDK2_SKIP_PEICORE) == 1
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryInitializeInSec|TRUE
  gArmPlatformTokenSpaceGuid.PcdSendSgiToBringUpSecondaryCores|TRUE
!endif

  ## If TRUE, Graphics Output Protocol will be installed on virtual handle created by ConsplitterDxe.
  #  It could be set FALSE to save size.
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|FALSE
  
[PcdsFixedAtBuild.common]
  gArmPlatformTokenSpaceGuid.PcdFirmwareVendor|"ARM Versatile Express"
  gEmbeddedTokenSpaceGuid.PcdEmbeddedPrompt|"Hi1610EVBb"

  gArmPlatformTokenSpaceGuid.PcdCoreCount|8

  #
  # NV Storage PCDs. Use base of 0x0C000000 for NOR1
  #
  #uniBIOS_c00213799_start 2015-1-14 15:24:40
  #Issue ID:DTS2015011302934
  #Description:【iWare V200R002C00B970】合入变量区读写功能
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase|0xa49D0000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize|0x0000E000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase|0xa49DE000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize|0x00002000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase|0xa49E0000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize|0x00010000
  #uniBIOS_c00213799_end 2015-1-14 15:24:40

  #c00213799 secure boot FVB模块
  #gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase|0x310C0000
  #gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize|0x00010000
  #gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase|0x310D0000
  #gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize|0x00010000
  #gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase|0x310E0000
  #gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize|0x00010000

  ## start-uniBIOS-y00216284-011 【iWare V2R2C00B950 BIOS】代码精简，删除不用的模块  DTS2014121700326  2014-12-17 >>>
  #c00213799 secure boot 镜像启动策略
  #gEfiSecurityPkgTokenSpaceGuid.PcdOptionRomImageVerificationPolicy|0x02
  #gEfiSecurityPkgTokenSpaceGuid.PcdRemovableMediaImageVerificationPolicy|0x02
  #gEfiSecurityPkgTokenSpaceGuid.PcdFixedMediaImageVerificationPolicy|0x02
  
  #c00213799 变量最大值
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000

  gArmTokenSpaceGuid.PcdVFPEnabled|0

  # Stacks for MPCores in Secure World
  #UEFI BIOS_t00216239_start   2014-10-29 11:00:07
  #Description: DTS2014102807906  660 BIOS DDR初始化代码代码结构调整至PEI，此处将SEC和PEI前期堆栈设置至SRAM上
  #gArmPlatformTokenSpaceGuid.PcdCPUCoresSecStackBase|0x21000000
  #gArmPlatformTokenSpaceGuid.PcdCPUCoreSecPrimaryStackSize|0x10000
  gArmPlatformTokenSpaceGuid.PcdCPUCoresSecStackBase|0x81000000
  gArmPlatformTokenSpaceGuid.PcdCPUCoreSecPrimaryStackSize|0x10000

  # Stacks for MPCores in Monitor Mode
  #gArmPlatformTokenSpaceGuid.PcdCPUCoresSecMonStackBase|0x2E008000
  #gArmPlatformTokenSpaceGuid.PcdCPUCoreSecMonStackSize|0x100
  gArmPlatformTokenSpaceGuid.PcdCPUCoresSecMonStackBase|0x8100FF00
  gArmPlatformTokenSpaceGuid.PcdCPUCoreSecMonStackSize|0x100

  # Stacks for MPCores in Normal World
  #gArmPlatformTokenSpaceGuid.PcdCPUCoresStackBase|0x2E000000
  #gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0x4000
  gArmPlatformTokenSpaceGuid.PcdCPUCoresStackBase|0x81000000
  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0xFF00
  #UEFI BIOS_t00216239_end   2014-10-29 11:00:07

  ## uniBIOS-g00179230 2013-9-13, FPGA版本同步
  ## 当前只有1GB内存, 而且BIOS放到0x30000000的位置运行, 所以将系统可用内存设置为0~512MB
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x00000000
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x3fc00000 #快速启动配置
  #gArmTokenSpaceGuid.PcdSystemMemorySize|0x20000000

  # Size of the region used by UEFI in permanent memory (Reserved 64MB)
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x10000000

  #
  # ARM Pcds
  #
  gArmTokenSpaceGuid.PcdArmUncachedMemoryMask|0x0000000040000000

  #
  # ARM PrimeCell
  #


  ## SP805 Watchdog - Motherboard Watchdog
  gArmPlatformTokenSpaceGuid.PcdSP805WatchdogBase|0x601e0000

  ## Serial Terminal
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x60300000
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200

  gHwTokenSpaceGuid.PcdUartClkInHz|200000000
## 在FPGA上需要查询真实的串口状态，因此需要有一些延时等待
  gHwTokenSpaceGuid.PcdSerialPortSendDelay|10000000

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1

  ## SMMU add by t00200952
  gHwTokenSpaceGuid.PcdM3SmmuBaseAddress|0xa0040000
  gHwTokenSpaceGuid.PcdPcieSmmuBaseAddress|0xb0040000
  gHwTokenSpaceGuid.PcdDsaSmmuBaseAddress|0xc0040000
  gHwTokenSpaceGuid.PcdAlgSmmuBaseAddress|0xd0040000

  ## BIOSversion add by s00164430
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"Hi1610 EVBb B905"

  ##uniBIOS_y00216284_049_start DTS2015050709882 2015-5-23 根据不同单板定义Version
  ##uniBIOS_y00216284_018_start DTS2015010906228 2015-1-13 根据不同单板定义ProductName
  gHwTokenSpaceGuid.PcdSystemProductName|L"Hi1610 EVBb"
  gHwTokenSpaceGuid.PcdSystemVersion|L"LINARO"
  gHwTokenSpaceGuid.PcdBaseBoardProductName|L"CH02TEVBB"
  gHwTokenSpaceGuid.PcdBaseBoardVersion|L"LINARO"
  
  #DTS2015031301099-C00227771-20150311 频点自适应功能开发
  gHwTokenSpaceGuid.PcdSlotPerChannelNum|0x3
  
  #
  # ARM PL390 General Interrupt Controller
  #

  ## uniBIOS-g00179230 2013-9-13, FPGA版本同步
  ## 更新GIC基地址
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x6D000000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0xFE000000

  ## DTB address at spi flash 15MB
  gHwTokenSpaceGuid.FdtFileAddress|0xA4F00000

  #
  # ARM OS Loader
  #
  # Versatile Express machine type (ARM VERSATILE EXPRESS = 2272) required for ARM Linux:
  #DTS2014120801648
  gArmPlatformTokenSpaceGuid.PcdDefaultBootDescription|L"Linux from SATA"
  gArmPlatformTokenSpaceGuid.PcdDefaultBootDevicePath|L""
  gArmPlatformTokenSpaceGuid.PcdDefaultBootInitrdPath|L"EFI\GRUB2\grubaa64.efi"
  gArmPlatformTokenSpaceGuid.PcdDefaultBootArgument|""
  gArmPlatformTokenSpaceGuid.PcdDefaultBootType|2

  # Use the serial console (ConIn & ConOut) and the Graphic driver (ConOut)
  ## uniBIOS-g00179230 2013-9-16, FPGA版本同步
  ## 配置成FPGA上实际的波特率9600, ConOut和ConIn的DevicePath也需要同步修改, 否则到BDS阶段串口无打印
  gArmPlatformTokenSpaceGuid.PcdDefaultConOutPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi();VenHw(407B4008-BF5B-11DF-9547-CF16E0D72085)"
  gArmPlatformTokenSpaceGuid.PcdDefaultConInPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi()"

  #
  # ARM L2x0 PCDs
  #
  gArmTokenSpaceGuid.PcdL2x0ControllerBase|0x1E00A000

  #
  # ARM Architectual Timer Frequency
  #
  # Set model tick to 120Mhz. This depends a lot on workstation performance.
  #gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|120000000

  ## 样片核内Timer频率固定为50MHz
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|66000000



  # add by t00200952
  gHwTokenSpaceGuid.PcdSysControlBaseAddress|0x60010000

  gHwTokenSpaceGuid.PcdMailBoxAddress|0x0000FFF8

  gHwTokenSpaceGuid.PcdCpldBaseAddress|0x78000000

  #定时器寄存器基地址 add by t00200952
  gHwTokenSpaceGuid.PcdTimerBaseAddress|0x60060000

  #SFC控制器寄存器地址 DTS2014021204581  z00201473 2013.10.21  新增SFC驱动
  gHwTokenSpaceGuid.PcdSFCCFGBaseAddress|0xA6000000      #配置口寄存器基址
  gHwTokenSpaceGuid.PcdSFCMEM0BaseAddress|0xA4000000      #MEM口基址

  #Ramdisk占用内存大小，单位MB add by t00200952
  gHwTokenSpaceGuid.PcdRamDiskMaxSize|128

  #sub 11 的sub ctrl地址(Peri subctrl) add by t00200952
  gHwTokenSpaceGuid.PcdPeriSubctrlAddress|0x60000000
  
  ## DTS2015032408517 z00304930 2015-04-17 网卡平台化
  gHwTokenSpaceGuid.PcdMdioSubctrlAddress|0x60000000

  ## NORFlash add by c00213799
  gHwTokenSpaceGuid.PcdNORFlashBase|0x70000000
  gHwTokenSpaceGuid.PcdNORFlashCachableSize|0x8000000

  ## 1+1
  gHwTokenSpaceGuid.PcdPlatformDefaultPackageType|0x0

  gHwTokenSpaceGuid.PcdArmPrimaryCoreTemp|0x80020000

  gHwTokenSpaceGuid.PcdTopOfLowMemory|0x40000000
  gHwTokenSpaceGuid.PcdBottomOfHighMemory|0x100000000
  
  gHwTokenSpaceGuid.PcdTrustedFirmwareEnable|0x1
  gHwTokenSpaceGuid.PcdTrustedFirmwareBL1Base|0xA4A00000

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]

  #
  # SEC
  #
  #HwPkg/Override/ArmPlatformPkg/Sec/Sec.inf {
  #  <LibraryClasses>
  #    # Use the implementation which set the Secure bits
  #    ArmGicLib|ArmPkg/Drivers/ArmGic/ArmGicSecLib.inf
  #}

  #
  # PEI Phase modules
  #
!if $(EDK2_SKIP_PEICORE) == 1
  ArmPlatformPkg/PrePi/PeiMPCore.inf {
    <LibraryClasses>
      ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64Lib.inf
      #ArmPlatformLib|ArmPlatformPkg/ArmVExpressPkg/Library/ArmVExpressLibRTSM/ArmVExpressLib.inf
      ArmPlatformLib|HwPkg/Library/ArmPlatformLibPv660/ArmPlatformLib.inf
      ArmPlatformGlobalVariableLib|ArmPlatformPkg/Library/ArmPlatformGlobalVariableLib/PrePi/PrePiArmPlatformGlobalVariableLib.inf
  }
  ArmPlatformPkg/PrePi/PeiUniCore.inf {
    <LibraryClasses>
      ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64Lib.inf
      #ArmPlatformLib|ArmPlatformPkg/ArmVExpressPkg/Library/ArmVExpressLibRTSM/ArmVExpressLib.inf
      ArmPlatformLib|HwPkg/Library/ArmPlatformLibPv660/ArmPlatformLib.inf
      ArmPlatformGlobalVariableLib|ArmPlatformPkg/Library/ArmPlatformGlobalVariableLib/PrePi/PrePiArmPlatformGlobalVariableLib.inf
  }
!else
  ArmPlatformPkg/PrePeiCore/PrePeiCoreMPCore.inf {
    <LibraryClasses>
      ArmPlatformGlobalVariableLib|ArmPlatformPkg/Library/ArmPlatformGlobalVariableLib/Pei/PeiArmPlatformGlobalVariableLib.inf
  }
  MdeModulePkg/Core/Pei/PeiMain.inf
  MdeModulePkg/Universal/PCD/Pei/Pcd.inf  {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  ArmPlatformPkg/PlatformPei/PlatformPeim.inf
  #ArmPlatformPkg/MemoryInitPei/MemoryInitPeim.inf
  #HwPkg/Override/ArmPlatformPkg/MemoryInitPei/MemoryInitPeim.inf
  HwProductsPkg/Hi1610Evb/Hi1610Evbb/MemoryInitPei/MemoryInitPeim.inf
  ArmPkg/Drivers/CpuPei/CpuPei.inf
  IntelFrameworkModulePkg/Universal/StatusCode/Pei/StatusCodePei.inf
  HwPkg/Drivers/BootModePei/BootModePei.inf  ## start-uniBIOS-y00216284-011 【iWare V2R2C00B950 BIOS】代码精简，删除不用的模块  DTS2014121700326  2014-12-17 >>>
  MdeModulePkg/Universal/FaultTolerantWritePei/FaultTolerantWritePei.inf
  MdeModulePkg/Universal/Variable/Pei/VariablePei.inf

  HwProductsPkg/Hi1610Evb/BootLinuxConfig/BootLinuxConfigPeim.inf

  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf {
    <LibraryClasses>
      NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }
!endif

  #
  # DXE
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
  }

  HwPkg/Drivers/IoInitDxe/IoInitDxe.inf

  #
  # Architectural Protocols
  #
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  #c00213799 在Flash芯片上使用
  HwPkg/Drivers/NorFlashDxe/NorFlashDxe.inf

  ## 板级网口配置驱动
  HwProductsPkg/Hi1610Evb/OemNicConfigEvb/OemNicConfigEvb.inf

  #新增SPI FLASH驱动，DTS2014021204581 z00201473   2013.10.23
  #HwPkg/Drivers/SFC/SfcDxeDriver.inf
  HwProductsPkg/Hi1610Evb/Hi1610Evbb/Drivers/SFC/SfcDxeDriver.inf

  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf
  #HwPkg/Drivers/NorFlashFvbDxe/NorFlashFvbDxe.inf  ## start-uniBIOS-y00216284-011 DTS2014121700326  2014-12-17 >>>
  HwPkg/Drivers/FlashFvbDxe/FlashFvbDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  EmbeddedPkg/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf

  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  EmbeddedPkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf

  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  EmbeddedPkg/SerialDxe/SerialDxe.inf

  # Simple TextIn/TextOut for UEFI Terminal
  EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf

  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf

  #ArmPkg/Drivers/PL390Gic/PL390GicDxe.inf
  HwPkg/Drivers/PL390Gic/PL390GicDxe.inf

  HwPkg/Drivers/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  #ArmPkg/Drivers/TimerDxe/TimerDxe
  HwPkg/Drivers/TimerDxe_SOC/TimerDxe_SOC.inf
  ArmPlatformPkg/Drivers/LcdGraphicsOutputDxe/PL111LcdGraphicsOutputDxe.inf

  HwPkg/Override/ArmPlatformPkg/Drivers/SP805WatchdogDxe/SP805WatchdogDxe.inf

  IntelFrameworkModulePkg/Universal/StatusCode/RuntimeDxe/StatusCodeRuntimeDxe.inf   #DTS:DTS2014021101224  z00201473  2014.2.11
  #
  #ACPI
  #
  #HwPkg/AcpiTables/AcpiTables.inf  ## start-uniBIOS-y00216284-011 DTS2014121700326  2014-12-17 >>>
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  HwPkg/Pv660AcpiTables/AcpiTablesHi1610.inf


  #
  #network
  #
  #HwPkg/Drivers/Network/SnpPV600DxeDebug/SnpPV600Dxe.inf
  #HwPkg/Drivers/Network/SnpPV600DxeDebug_GE6/SnpPV600Dxe.inf
  #HwPkg/Drivers/Network/SnpPV600DxeService/SnpPV600Dxe.inf
  ## DTS2015032408517 z00304930 2015-04-17 网卡平台化
  #HwPkg/Drivers/Network/SnpHi1610Dxe_PLAT/SnpPV600DxeMac6.inf
  HwProductsPkg/Hi1610Evb/Hi1610Evbb/Drivers/SnpPV600Dxe_PLAT/SnpPV600DxeMac6.inf

  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  MdeModulePkg/Universal/Network/Ip4ConfigDxe/Ip4ConfigDxe.inf
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
  MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
  MdeModulePkg/Universal/Network/VlanConfigDxe/VlanConfigDxe.inf

  #
  # Semi-hosting filesystem
  #
  ArmPkg/Filesystem/SemihostFs/SemihostFs.inf

  #
  # Multimedia Card Interface
  #
  #EmbeddedPkg/Universal/MmcDxe/MmcDxe.inf
  #ArmPlatformPkg/Drivers/PL180MciDxe/PL180MciDxe.inf

  #
  # FAT filesystem + GPT/MBR partitioning
  #
  #l00228991 增加RAMDISK
  HwPkg/Drivers/ramdisk/ramdisk.inf
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf

  #HwPkg/Override/EmbeddedPkg/Ebl/Ebl.inf
  HwProductsPkg/Hi1610Evb/Hi1610Evbb/Ebl/Ebl.inf
  #c00213799 secure boot test
  MdeModulePkg/Application/HelloWorld/HelloWorld.inf
  #
  # Bds
  #
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf

  #HwPkg/Network/OemFtp/OemFtp.inf


  #x00216252, DTS2014042301731, SATA驱动代码上传， 2014-04-23
  #HwPkg/Drivers/AtaAtapiPassThru/AtaAtapiPassThru.inf
  #MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  #HwPkg/Drivers/SasDxe/SasDxeDriver.inf
  HwProductsPkg/Hi1610Evb/Hi1610Evbb/Drivers/SasDxe/SasDxeDriver.inf

  # uniBIOS-y00216284 DTS2014071901841 2014-7-19
  #【HRD ARM PV660 V100R001C00B010 BIOS】提供PCIe初始化功能
  #HwPkg/Pcie/PcieInitDxe.inf   ## start-uniBIOS-y00216284-011 DTS2014121700326  2014-12-17 >>>

  # uniBIOS-y00216284  2014-10-20
  # 【HRD ARM PV660 V100R001C00B010 BIOS】Smbiso模块
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  HwPkg/Drivers/SmbiosMiscDxe/SmbiosMiscDxe.inf

  #s00296804 新增更新FDT并创建FDT Configuration Table 驱动
  HwPkg/Drivers/UpdateFdtDxe/UpdateFdtDxe.inf
  # uniBIOS-y00216284-002  2014-10-31
  # 【HRD ARM PV660 V100R001C00B010 BIOS】Memory Subclass Driver模块
  HwPkg/Drivers/MemorySubClassDxe/MemorySubClassDxe.inf
  
  HwPkg/Drivers/ProcessorSubClassDxe/ProcessorSubClassDxe.inf

  HwPkg/Bds/Bds.inf


