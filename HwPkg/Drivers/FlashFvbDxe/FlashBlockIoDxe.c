/*-----------------------------------------------------------------------*/
/*!!Warning: Huawei key information asset. No spread without permission. */
/*CODEMARK:EG4uRhTwMmgcVFBsBnYHCDadN5jJKSuVyxmmaCmKFU6eJEbB2fyHF9weu4/jer/hxLHb+S1e
E0zVg4C3NiZh4WIPvj4+Pmfbh+YGahSH5JekcNbwDJ3EondjpJjt4q4ECmDk1NoHHofliorY
nR2tpLrHP3QAWInGJGilmc65ltkqHDyYl13KZXlLbM20hQ5iTvC4s13FYkEk3+117k918qB/
3nk2RQpLnmQCbVAcemeecOx+rh9VQCIQ7GXsh7G4+IKyKUrvDDIpI8XfCooKOQ==#*/
/*--!!Warning: Deleting or modifying the preceding information is prohibited.--*/
/******************************************************************************

                版权所有 (C), 2009-2019, 华为技术有限公司

******************************************************************************
文 件 名   : FlashBlockIoDxe.c
版 本 号   : v1.0
作    者   : c00213799
生成日期   : 2015年01月07日
功能描述   :
修改历史   :
******************************************************************************/
#include "FlashFvbDxe.h"

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.ReadBlocks
//
EFI_STATUS
EFIAPI
FlashBlockIoReadBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL*     This,
    IN  UINT32                  MediaId,
    IN  EFI_LBA                     Lba,
    IN  UINTN         BufferSizeInBytes,
    OUT VOID*                     Buffer
)
{
    FLASH_INSTANCE*   Instance;
    EFI_STATUS          Status;

    Instance = INSTANCE_FROM_BLKIO_THIS(This);

    //DEBUG ((EFI_D_ERROR, "FlashBlockIoReadBlocks(MediaId=0x%x, Lba=%ld, BufferSize=0x%x bytes (%d kB), BufferPtr @ 0x%08x)\n", MediaId, Lba, BufferSizeInBytes, Buffer));

    if ( !This->Media->MediaPresent )
    {
        Status = EFI_NO_MEDIA;
    }
    else if ( This->Media->MediaId != MediaId )
    {
        Status = EFI_MEDIA_CHANGED;
    }
    else
    {
        Status = FlashReadBlocks (Instance, Lba, BufferSizeInBytes, Buffer);
    }

    return Status;
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.WriteBlocks
//
EFI_STATUS
EFIAPI
FlashBlockIoWriteBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL*     This,
    IN  UINT32                  MediaId,
    IN  EFI_LBA                     Lba,
    IN  UINTN         BufferSizeInBytes,
    IN  VOID*                     Buffer
)
{
    FLASH_INSTANCE*   Instance;
    EFI_STATUS          Status;

    Instance = INSTANCE_FROM_BLKIO_THIS(This);

    //DEBUG ((EFI_D_ERROR, "FlashBlockIoWriteBlocks(MediaId=0x%x, Lba=%ld, BufferSize=0x%x bytes (%d kB), BufferPtr @ 0x%08x)\n", MediaId, Lba, BufferSizeInBytes, Buffer));

    if ( !This->Media->MediaPresent )
    {
        Status = EFI_NO_MEDIA;
    }
    else if ( This->Media->MediaId != MediaId )
    {
        Status = EFI_MEDIA_CHANGED;
    }
    else if ( This->Media->ReadOnly )
    {
        Status = EFI_WRITE_PROTECTED;
    }
    else
    {
        Status = FlashWriteBlocks (Instance, Lba, BufferSizeInBytes, Buffer);
    }

    return Status;
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.FlushBlocks
//
EFI_STATUS
EFIAPI
FlashBlockIoFlushBlocks (
    IN EFI_BLOCK_IO_PROTOCOL*  This
)
{
    // No Flush required for the NOR Flash driver
    // because cache operations are not permitted.

    //sDEBUG ((EFI_D_ERROR, "FlashBlockIoFlushBlocks: Function NOT IMPLEMENTED (not required).\n"));

    // Nothing to do so just return without error
    return EFI_SUCCESS;
}
