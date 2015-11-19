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
