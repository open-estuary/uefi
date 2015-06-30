/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ProcessorSubClass.h
  版 本 号   : v1.0
  作    者   : 
  生成日期   : 2014年11月10日
  最近修改   :
  功能描述   : Header file for the ProcessorSubClass Driver.
  修改历史   :
******************************************************************************/

#ifndef _PROCESSOR_SUBCLASS_DRIVER_H
#define _PROCESSOR_SUBCLASS_DRIVER_H

#include <FrameworkDxe.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <PlatformArch.h>
#include <Library/PlatformSysCtrlLib.h>
#include <Library/OemMiscLib.h>
#include <Library/HwSafeOperationLib.h>

//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//

extern UINT8 ProcessorSubClassStrings[];

#define CPU_CACHE_L1_Instruction		0
#define CPU_CACHE_L1_Data		        1
#define CPU_CACHE_L2		            2
#define CPU_CACHE_L3		            3
#define MAX_CACHE_LEVEL		            4

#define EXTERNAL_CLOCK                  50     //50 MHz
#define CPU_MAX_SPEED                   2100   //2.1G

//
// Cache Info
//
typedef struct {
  UINT16					InstalledSize;    //In KB
  CACHE_TYPE_DATA			SystemCacheType;
  CACHE_ASSOCIATIVITY_DATA	Associativity;
} CACHE_INFO;

//
// Cache Configuration
//
typedef struct {
  UINT16    Level           :3;
  UINT16    Socketed        :1;
  UINT16    Reserved1       :1;
  UINT16    Location        :2;
  UINT16    Enable          :1;
  UINT16    OperationalMode :2;
  UINT16    Reserved2       :6;
} CACHE_CONFIGURATION;

//
// Processor Status
//
typedef struct {
  UINT8 CpuStatus       :3; // Indicates the status of the processor.
  UINT8 Reserved1       :3; // Reserved for future use. Should be set to zero.
  UINT8 SocketPopulated :1; // Indicates if the processor socket is populated or not.
  UINT8 Reserved2       :1; // Reserved for future use. Should be set to zero.
} PROCESSOR_STATUS_DATA;

// 
// Processor Characteristics
//
typedef struct {
  UINT16 Reserved                 :1;
  UINT16 Unknown                  :1;
  UINT16 Capable64Bit             :1;
  UINT16 MultiCore                :1;
  UINT16 HardwareThread           :1;
  UINT16 ExecuteProtection        :1;
  UINT16 EnhancedVirtualization   :1;
  UINT16 PowerPerformanceControl  :1;
  UINT16 Reserved2                :8;
} PROCESSOR_CHARACTERISTICS_DATA;

#endif
