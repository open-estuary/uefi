/** @file
*
*  Copyright (c) 2015, Linaro Ltd. All rights reserved.
*  Copyright (c) 2015, Hisilicon Ltd. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/IoLib.h>

#include "Hi6220RegsPeri.h"

VOID
EFIAPI
HiKeyInitPeripherals (
  IN VOID
  )
{
  UINT32     Data;

  /* make I2C2 out of reset */
  MmioWrite32 (SC_PERIPH_RSTDIS3, PERIPH_RST3_I2C2);

  do {
    Data = MmioRead32 (SC_PERIPH_RSTSTAT3);
  } while (Data & PERIPH_RST3_I2C2);
}
