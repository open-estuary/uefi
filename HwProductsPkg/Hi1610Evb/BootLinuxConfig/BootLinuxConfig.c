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
    MmioWrite64(FixedPcdGet64(PcdMailBoxAddress), 0x0);
    (void)WriteBackInvalidateDataCacheRange((VOID *) FixedPcdGet64(PcdMailBoxAddress), 8);
    asm("DSB SY");
    asm("ISB");
    
    //uniBIOS_c00213799_start 2014-12-18 10:14:02
    //Issue ID:DTS2014121800997
    CoreSelectBoot();
    //uniBIOS_c00213799_end 2014-12-18 10:14:02
    
    /*MmioWrite32(0x80010000 + 0x306c, 0x3ff);
    MmioWrite32(0x80010000 + 0x307c, 0x3ff);
    MmioWrite32(0x80010000 + 0x308c, 0x3ff);
    MmioWrite32(0x80010000 + 0x309c, 0x3ff);
    DJTAG_WRITE(0x10, 0x30F, 0xB, CDIEC_ID);   //CDIE MN1*/

    DEBUG((EFI_D_ERROR,"Done\n"));

    DEBUG((EFI_D_ERROR,"MN CONFIG........."));
    MN_CONFIG();
    DEBUG((EFI_D_ERROR,"Done\n"));

    return EFI_SUCCESS;
}
