/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpat0TH65jPMlUENBvGXufQCTRBL0qDbp15usAFL4sRUNaa9PQnehq7qerrn2
LiJ41sgI/2XUpk4v894ZtyudRHpYbeXqMz8dwFUWw8SjMfWuBjZhyOOBIbCnVkk3Bpwf5IDY
eF2WOvvg0P79aOoT9VI/pKmi08xqu+X9zwV0+ZYZ/fYvrhZ2mgwwlW40ecvWDw==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\FdtUpdateLib\FdtUpdateLib.h

Author: 

Description: 

*************************************************/

#ifndef _FDTUPDATELIB_H_
#define _FDTUPDATELIB_H_

#define E820_OSP_NVRAM 6      /* NVRAM内存 */
#define E820_OSP_MEM 7        /* cache内存 */
#define E820_PCIE_OPP 8       /* pcie opp cache virtual address */
#define E820_BIOS_MEM 9       /* RAM中bios 内存 */
#define E820_PBS 0x0a         /* PBS内存 */
#define E820_BIOS_FLASH 0x0b  /* BIOS FLASH 内存 */
#define E820_PCIE_MEM 0x0c    /* 对端镜像PCIE映射属性 */
#define E820_MEM_VOS 0X0d     /* 内存分区管理的内存 */

#define ADD_FILE_LENGTH  0x400

typedef struct
{
  UINT32 BaseHigh;    
  UINT32 BaseLow;
  UINT32 LengthHigh;    
  UINT32 LengthLow;
}PHY_MEM_REGION;

typedef struct 
{
  UINT8 data0;    
  UINT8 data1;
  UINT8 data2;    
  UINT8 data3;
  UINT8 data4;
  UINT8 data5;
}MAC_ADDRESS;

MAC_ADDRESS gMacAddress[1];

typedef struct region
{
    UINT32 BaseHigh;    
    UINT32 BaseLow;
    UINT32 LengthHigh;    
    UINT32 LengLow;
    UINT32 Type;
}RESERVED_MEM_REGION;

//Attention: This is not produced, but consumed by this lib!!
extern  RESERVED_MEM_REGION gExtMemRegion[];

extern  EFI_STATUS EFIFdtUpdate(UINTN FdtFileAddr);

#endif


