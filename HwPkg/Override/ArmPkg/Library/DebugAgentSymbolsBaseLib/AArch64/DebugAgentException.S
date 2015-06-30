#------------------------------------------------------------------------------
#
# Copyright (c) 2011 - 2013, ARM Ltd. All rights reserved.<BR>
#
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#------------------------------------------------------------------------------

// temp code
#include <Library/PlatformSysCtrlLib.h>

GCC_ASM_EXPORT(DebugAgentVectorTable)
GCC_ASM_IMPORT(DefaultExceptionHandler)

.text
ASM_PFX(DebugAgentVectorTable):

//
// Current EL with SP0 : 0x0 - 0x180
//
.align 11
ASM_PFX(SynchronousExceptionSP0):
  b   ASM_PFX(SynchronousExceptionSP0)

.align 7
ASM_PFX(IrqSP0):
  b   ASM_PFX(IrqSP0)

.align 7
ASM_PFX(FiqSP0):
  b   ASM_PFX(FiqSP0)

.align 7
ASM_PFX(SErrorSP0):
  b   ASM_PFX(SErrorSP0)

//
// Current EL with SPx: 0x200 - 0x380
//
.align 7
ASM_PFX(SynchronousExceptionSPx):
  b   ASM_PFX(SynchronousExceptionSPx)

.align 7
ASM_PFX(IrqSPx):
  b   ASM_PFX(IrqSPx)

.align 7
ASM_PFX(FiqSPx):
  b   ASM_PFX(FiqSPx)

.align 7
ASM_PFX(SErrorSPx):
  b   ASM_PFX(SErrorSPx)

/* Lower EL using AArch64 : 0x400 - 0x580 */
.align 7
ASM_PFX(SynchronousExceptionA64):
  ldr w1, PcdGet32(PcdTrustedFirmwareMagicNum)
  cmp w0, w1
  b.ne 1f
  ldr x0, PcdGet64(PcdTrustedFirmwareBL1Base)
  blr x0
dead:
  b     dead
1:  
  msr scr_el3, xzr           //;Disable copro. traps to EL3
  msr hcr_el2, xzr           //;Disable copro. traps to EL3
  ldr x0, =0x20d400000
  ldr x1, =20
  lsrv x1, x0, x1
  ldr x2, =0x60010034
  ldr x3, =16
1:
  str w1, [x2], #8
  sub x3, x3, 1
  cmp x3, 0
  bne 1b

  mov x2, x0  // backup x0
  bl ASM_PFX(PlatformGetPackageType)
  cmp w0, PACKAGE_16CORE
  mov x0, x2  // restore x0
  b.eq 2f
  ldr x2, =0x40010034
  ldr x3, =16
1:
  str w1, [x2], #8
  sub x3, x3, 1
  cmp x3, 0
  bne 1b
2:
  blr x0
  b   ASM_PFX(SynchronousExceptionA64)

.align 7
ASM_PFX(IrqA64):
  b   ASM_PFX(IrqA64)

.align 7
ASM_PFX(FiqA64):
  b   ASM_PFX(FiqA64)

.align 7
ASM_PFX(SErrorA64):
  b   ASM_PFX(SErrorA64)

//
// Lower EL using AArch32 : 0x0 - 0x180
//
.align 7
ASM_PFX(SynchronousExceptionA32):
  b   ASM_PFX(SynchronousExceptionA32)

.align 7
ASM_PFX(IrqA32):
  b   ASM_PFX(IrqA32)

.align 7
ASM_PFX(FiqA32):
  b   ASM_PFX(FiqA32)

.align 7
ASM_PFX(SErrorA32):
  b   ASM_PFX(SErrorA32)
