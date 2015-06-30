/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePRXyTmI6pqqZK/VsgQAFF+TLEhrrYdasNpB+ZM
CUefdwBH47fZ4gjZvDGN+zzpHpxJcNxoZWKO0gznpNtFOGViUTuCCd2gpYiTBWRDcNmsuBsu
V4aqpAZeekGk84yLthM+R1aKZELh+gMRRpg9/JSsyJZrdCCu01rBcuyu1aGZdA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/** @file
*
*  Copyright (c) 2011-2012, ARM Limited. All rights reserved.
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

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/ArmGicLib.h>
#include <Library/PcdLib.h>

UINTN
EFIAPI
ArmGicGetMaxNumInterrupts (
  IN  INTN          GicDistributorBase
  )
{
  return 32 * ((MmioRead32 (GicDistributorBase + ARM_GIC_ICDICTR) & 0x1F) + 1);
}
