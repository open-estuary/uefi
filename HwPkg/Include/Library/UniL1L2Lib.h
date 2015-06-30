/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpat0TH65jPMlUENBvGXufQCTRBL0qDbp15usAFL4sRUNaa9PQnehq7qerrn2
LiJ41jY3vK5aXgvzJQfi10VjRjcbLPQS+oVHSf8qYNeBUwbcRGHGjWNZ4VM9RsDwqFItf0xJ
jPhxw+D6r0qcH5Rge1RFjqnOcSIvxe6T7Tm3HD5pNqV6yvbX4buz9kqEuVCe5g==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Library\UniL1L2Lib.h

Author: uniBIOS-g00179230 (Heyi Guo)    Date: 2013.5.30

Description: 

*************************************************/


#ifndef _UNI_L1_L2_LIB_H_
#define _UNI_L1_L2_LIB_H_

int BTRM_L1L2_Valid (VOID);

BOOLEAN UniIsL2BiosFail (VOID);
BOOLEAN UniIsCurrentBiosL2 (VOID);
EFI_STATUS UniResetL2Counter (VOID);
EFI_STATUS UniResetL2FailFlag (VOID);

#endif
