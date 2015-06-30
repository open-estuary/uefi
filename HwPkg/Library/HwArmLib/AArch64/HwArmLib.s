/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Library\HwArmLib\AArch64\HwArmLib.s

Description: 

*************************************************/

#include <Uefi.h>
#include <Library/HwArmLib.h>

.text
.align 3

GCC_ASM_EXPORT(asm_write_reg)
GCC_ASM_EXPORT(asm_read_reg)
GCC_ASM_EXPORT(SmcJumpToTfBl1)

ASM_PFX(asm_write_reg):
 mrs x0, S3_1_c15_c2_1
 orr x0, x0, (1<<6)
 msr S3_1_c15_c2_1, x0
 ret

ASM_PFX(asm_read_reg):
 mrs x0, S3_1_c15_c2_1
 ret
           
ASM_PFX(SmcJumpToTfBl1):
 stp x0, x1, [SP, #-16]!
 stp x2, x3, [SP, #-16]!
 stp x4, x5, [SP, #-16]!
 stp x6, x7, [SP, #-16]!
 stp x8, x8, [SP, #-16]!
 stp x10, x11, [SP, #-16]!
 stp x12, x13, [SP, #-16]!
 stp x14, x15, [SP, #-16]!
 stp x16, x17, [SP, #-16]!
 stp x18, x19, [SP, #-16]!
 stp x20, x21, [SP, #-16]!
 stp x22, x23, [SP, #-16]!
 stp x24, x25, [SP, #-16]!
 stp x26, x27, [SP, #-16]!
 stp x28, x29, [SP, #-16]!
 stp x29, x30, [SP, #-16]!
 
 smc #0
 
 ldp x29, x30, [SP], #16
 ldp x28, x29, [SP], #16
 ldp x26, x27, [SP], #16
 ldp x24, x25, [SP], #16
 ldp x22, x23, [SP], #16
 ldp x20, x21, [SP], #16
 ldp x18, x19, [SP], #16
 ldp x16, x17, [SP], #16
 ldp x14, x15, [SP], #16
 ldp x12, x13, [SP], #16
 ldp x10, x11, [SP], #16
 ldp x8, x9, [SP], #16
 ldp x6, x7, [SP], #16
 ldp x4, x5, [SP], #16
 ldp x2, x3, [SP], #16
 ldp x0, x1, [SP], #16
 
 ret
/* UEFI BIOS_t00216239_end   2015-1-2 17:43:11 */

ASM_FUNCTION_REMOVE_IF_UNREFERENCED
