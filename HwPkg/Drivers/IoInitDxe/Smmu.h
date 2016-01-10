/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
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

#ifndef _SMMU_H_
#define _SMMU_H_

typedef struct {
  UINTN       Base;
  UINTN       S2Cbt;
} SMMU_DEVICE;

EFI_STATUS
SmmuConfigForOS (
  SMMU_DEVICE       *Smmu
  );

EFI_STATUS
SmmuEnableTable (
  SMMU_DEVICE       *Smmu
  );


#endif

