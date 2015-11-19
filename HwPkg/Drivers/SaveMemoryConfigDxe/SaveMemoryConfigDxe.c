
#include "Uefi.h"
#include <Library/ArmLib.h>
#include <Library/DebugLib.h>  
#include <Protocol/SpiFlashProtocol.h>
#include <Library/HwMemInitLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>  
#include <Guid/MemoryMapData.h>  


EFI_STATUS
EFIAPI SaveMemoryConfigDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    NVRAM           *Nvram;
    GBL_DATA        *Gbl_Data;
    SPI_FLASH_PROTOCOL        *Flash;
    VOID*           HobList;
    UINT32          NvramCrc;

    
    HobList = GetHobList();
    Gbl_Data = (GBL_DATA*)GetNextGuidHob(&gEfiMemoryMapGuid, HobList);
    Gbl_Data = GET_GUID_HOB_DATA(Gbl_Data);

    Nvram = &(Gbl_Data->nvram);

    Status = gBS->CalculateCrc32(((UINT8 *)Nvram+sizeof(UINT32)),(sizeof(NVRAM)-sizeof(UINT32)),&NvramCrc);
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR,"Nvram CalculateCrc32 Failed\n"));
        return Status;
    }
    
    if( Nvram->NvramCrc != NvramCrc)
    {
        Nvram->NvramCrc = NvramCrc;
        
        Status = gBS->LocateProtocol (&gSpiFlashProtocolGuid, NULL, (VOID *) &Flash);
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "%a - %d Status=%r\n", __FILE__, __LINE__, Status));
            return Status;
        }
        
        Status = Flash->Erase(Flash,NVRAM_ADDR,sizeof(NVRAM));
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "%a - %d SpiFlash Erase Error,Status=%r\n", __FILE__, __LINE__,Status));
            return Status;
        }

        Status = Flash->Write(Flash, NVRAM_ADDR, (UINT8*)(Nvram), sizeof(NVRAM));
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "%a - %d Flash Write Error,Status=%r\n", __FILE__, __LINE__,Status));
            return Status;
        }
    }

    return EFI_SUCCESS;
}



