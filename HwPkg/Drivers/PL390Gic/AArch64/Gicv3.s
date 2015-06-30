/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\PL390Gic\AArch64\Gicv3.s

Description: 

*************************************************/

#define ICC_EOIR1_EL1   S3_0_C12_C12_1
#define ICC_EOIR0_EL1   S3_0_C12_C8_1
#define ICC_SRE_EL3     S3_6_C12_C12_5

.text
.align 2


GCC_ASM_EXPORT(WriteIccEoirNs)

ASM_PFX(WriteIccEoirNs):
  msr ICC_EOIR1_EL1, x0
  isb
  ret


