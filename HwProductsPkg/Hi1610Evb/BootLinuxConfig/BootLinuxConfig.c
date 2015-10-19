/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7oqucjIznazmY6pDerNNjrJRr3zqJWqMJ5g20Wh4kav+V4LjOmY/LHDS62FHF
VyAlAwwPzcbvHilrgL7bW67/8CrCSf8cXap2AoZuMiQcRFcBP1nqYsDRaAa5J8iRBIDW1G4+
ee/SlT/unup5o8Bdp7u/rHtVC7cGJxWNSV3Kl2Lfm1AASsILVnu2bz3Aze66mA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
#include <Uefi.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/CacheMaintenanceLib.h>

#include <PlatformArch.h>
#include <Library/PlatformSysCtrlLib.h>

#include <Library/OemMiscLib.h>
#include <Library/ArmLib.h>

EFI_STATUS
EFIAPI
BootLinuxConfig (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    DEBUG((EFI_D_ERROR,"SMMU CONFIG........."));
    SmmuConfigForLinux();
    DEBUG((EFI_D_ERROR,"Done\n"));

    DEBUG((EFI_D_ERROR,"ITS CONFIG........."));
    //ITSCONFIG();
    DEBUG((EFI_D_ERROR,"Done\n"));
    
    DEBUG((EFI_D_ERROR,"AP CONFIG........."));
    //解复位从核之前清MAILBOX
    MmioWrite64(FixedPcdGet64(PcdMailBoxAddress), 0x0);
    // MMU已使能, 需要增加刷cache操作
    (void)WriteBackInvalidateDataCacheRange((VOID *) FixedPcdGet64(PcdMailBoxAddress), 8);
    asm("DSB SY");
    asm("ISB");
    
    //uniBIOS_c00213799_start 2014-12-18 10:14:02
    //Issue ID:DTS2014121800997
    //Description:【iWare V200R002C00B950 BIOS】CPU L1/L2 Cache mbist功能合入
    CoreSelectBoot();
    //uniBIOS_c00213799_end 2014-12-18 10:14:02
    
    /*MmioWrite32(0x80010000 + 0x306c, 0x3ff);
    MmioWrite32(0x80010000 + 0x307c, 0x3ff);
    MmioWrite32(0x80010000 + 0x308c, 0x3ff);
    MmioWrite32(0x80010000 + 0x309c, 0x3ff);
    //单核  
    DJTAG_WRITE(0x10, 0x30F, 0xB, CDIEC_ID);   //CDIE MN1*/

    DEBUG((EFI_D_ERROR,"Done\n"));

    DEBUG((EFI_D_ERROR,"MN CONFIG........."));
    MN_CONFIG();
    DEBUG((EFI_D_ERROR,"Done\n"));

    return EFI_SUCCESS;
}
