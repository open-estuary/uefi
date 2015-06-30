/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : BootLine.h
  版 本 号   : v1.0
  作    者   : 
  生成日期   : 2014年11月20日
  最近修改   :
  功能描述   : GUID used for Memory Map Data entries in the HOB list.
  修改历史   :
******************************************************************************/

#ifndef _BOOTLINE_VARIABLE_GUID_H_
#define _BOOTLINE_VARIABLE_GUID_H_

#define EFI_BOOTLINE_VARIABLE_GUID \
  { \
    0xc3253c90, 0xa24f, 0x4588, 0xa6, 0x64, 0x1f, 0x88, 0x13, 0x77, 0x8f, 0xc9 \
  }

extern EFI_GUID gBootLineVariableGuid;

#endif
