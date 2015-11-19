
#ifndef _UNI_L1_L2_LIB_H_
#define _UNI_L1_L2_LIB_H_

int BTRM_L1L2_Valid (VOID);

BOOLEAN UniIsL2BiosFail (VOID);
BOOLEAN UniIsCurrentBiosL2 (VOID);
EFI_STATUS UniResetL2Counter (VOID);
EFI_STATUS UniResetL2FailFlag (VOID);

#endif
