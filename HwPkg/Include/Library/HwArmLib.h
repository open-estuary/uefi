/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Library\HwArmLib.h

Description: Huawei library for CPU core
*************************************************/

#ifndef _HW_ARM_LIB_H_
#define _HW_ARM_LIB_H_

VOID
EFIAPI
asm_write_reg (
  VOID
  );

UINTN
EFIAPI
asm_read_reg (
  VOID
  );

UINTN
EFIAPI
SmcJumpToTfBl1 (
  IN  UINT32   arg0
  );

#endif
