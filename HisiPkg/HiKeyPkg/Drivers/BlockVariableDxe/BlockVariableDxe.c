/** @file
  This file implement the Variable Protocol for the block device.

  Copyright (c) 2015, Linaro Limited. All rights reserved.
  Copyright (c) 2015, Hisilicon Limited. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Guid/VariableFormat.h>
#include <Guid/SystemNvDataGuid.h>

#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "BlockVariableDxe.h"


STATIC EFI_PHYSICAL_ADDRESS      mMapNvStorageVariableBase;

EFI_STATUS
EFIAPI
FvbGetAttributes (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL    *This,
  OUT       EFI_FVB_ATTRIBUTES_2                   *Attributes
  )
{
  EFI_FVB_ATTRIBUTES_2 FvbAttributes;
  FvbAttributes = (EFI_FVB_ATTRIBUTES_2) (

      EFI_FVB2_READ_ENABLED_CAP | // Reads may be enabled
      EFI_FVB2_READ_STATUS      | // Reads are currently enabled
      EFI_FVB2_STICKY_WRITE     | // A block erase is required to flip bits into EFI_FVB2_ERASE_POLARITY
      EFI_FVB2_MEMORY_MAPPED    | // It is memory mapped
      EFI_FVB2_ERASE_POLARITY     // After erasure all bits take this value (i.e. '1')

      );
  FvbAttributes |= EFI_FVB2_WRITE_STATUS      | // Writes are currently enabled
                   EFI_FVB2_WRITE_ENABLED_CAP;  // Writes may be enabled

  *Attributes = FvbAttributes;

  DEBUG ((DEBUG_BLKIO, "FvbGetAttributes(0x%X)\n", *Attributes));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FvbSetAttributes(
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  IN OUT    EFI_FVB_ATTRIBUTES_2                 *Attributes
  )
{
  DEBUG ((DEBUG_BLKIO, "FvbSetAttributes(0x%X) is not supported\n",*Attributes));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FvbGetPhysicalAddress (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  OUT       EFI_PHYSICAL_ADDRESS                 *Address
  )
{
  *Address = mMapNvStorageVariableBase;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FvbGetBlockSize (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  IN        EFI_LBA                              Lba,
  OUT       UINTN                                *BlockSize,
  OUT       UINTN                                *NumberOfBlocks
  )
{
  BLOCK_VARIABLE_INSTANCE       *Instance;

  Instance = CR (This, BLOCK_VARIABLE_INSTANCE, FvbProtocol, BLOCK_VARIABLE_SIGNATURE);
  *BlockSize = (UINTN) Instance->Media.BlockSize;
  *NumberOfBlocks = PcdGet32 (PcdNvStorageVariableBlockCount);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FvbRead (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL   *This,
  IN        EFI_LBA                               Lba,
  IN        UINTN                                 Offset,
  IN OUT    UINTN                                 *NumBytes,
  IN OUT    UINT8                                 *Buffer
  )
{
  BLOCK_VARIABLE_INSTANCE       *Instance;
  EFI_BLOCK_IO_PROTOCOL         *BlockIo;
  EFI_STATUS                    Status;
  UINTN                         Bytes;
  VOID                          *DataPtr;

  Instance = CR (This, BLOCK_VARIABLE_INSTANCE, FvbProtocol, BLOCK_VARIABLE_SIGNATURE);
  BlockIo = Instance->BlockIoProtocol;
  Bytes = (Offset + *NumBytes + Instance->Media.BlockSize - 1) / Instance->Media.BlockSize * Instance->Media.BlockSize;
  DataPtr = AllocateZeroPool (Bytes);
  if (DataPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "FvbWrite: failed to allocate buffer.\n"));
    return EFI_BUFFER_TOO_SMALL;
  }
  WriteBackDataCacheRange (DataPtr, Bytes);
  InvalidateDataCacheRange (Buffer, *NumBytes);
  Status = BlockIo->ReadBlocks (BlockIo, BlockIo->Media->MediaId, Instance->StartLba + Lba,
		                Bytes, DataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FvbRead StartLba:%x, Lba:%x, Offset:%x, Status:%x\n",
	    Instance->StartLba, Lba, Offset, Status));
    goto exit;
  }
  CopyMem (Buffer, DataPtr + Offset, *NumBytes);
  WriteBackDataCacheRange (Buffer, *NumBytes);
exit:
  FreePool (DataPtr);
  return Status;
}

EFI_STATUS
EFIAPI
FvbWrite (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL   *This,
  IN        EFI_LBA                               Lba,
  IN        UINTN                                 Offset,
  IN OUT    UINTN                                 *NumBytes,
  IN        UINT8                                 *Buffer
  )
{
  BLOCK_VARIABLE_INSTANCE       *Instance;
  EFI_BLOCK_IO_PROTOCOL         *BlockIo;
  EFI_STATUS                    Status;
  UINTN                         Bytes;
  VOID                          *DataPtr;

  Instance = CR (This, BLOCK_VARIABLE_INSTANCE, FvbProtocol, BLOCK_VARIABLE_SIGNATURE);
  BlockIo = Instance->BlockIoProtocol;
  Bytes = (Offset + *NumBytes + Instance->Media.BlockSize - 1) / Instance->Media.BlockSize * Instance->Media.BlockSize;
  DataPtr = AllocateZeroPool (Bytes);
  if (DataPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "FvbWrite: failed to allocate buffer.\n"));
    return EFI_BUFFER_TOO_SMALL;
  }
  WriteBackDataCacheRange (DataPtr, Bytes);
  Status = BlockIo->ReadBlocks (BlockIo, BlockIo->Media->MediaId, Instance->StartLba + Lba,
                                Bytes, DataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FvbWrite: failed on reading blocks.\n"));
    goto exit;
  }
  CopyMem (DataPtr + Offset, Buffer, *NumBytes);
  WriteBackDataCacheRange (DataPtr, Bytes);
  Status = BlockIo->WriteBlocks (BlockIo, BlockIo->Media->MediaId, Instance->StartLba + Lba,
                                 Bytes, DataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FvbWrite StartLba:%x, Lba:%x, Offset:%x, Status:%x\n",
            Instance->StartLba, Lba, Offset, Status));
  }
  // Sometimes the variable isn't flushed into block device if it's the last flush operation.
  // So flush it again.
  Status = BlockIo->WriteBlocks (BlockIo, BlockIo->Media->MediaId, Instance->StartLba + Lba,
                                 Bytes, DataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FvbWrite StartLba:%x, Lba:%x, Offset:%x, Status:%x\n",
            Instance->StartLba, Lba, Offset, Status));
  }
exit:
  FreePool (DataPtr);
  return Status;
}

EFI_STATUS
EFIAPI
FvbEraseBlocks (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL *This,
  ...
  )
{
  return EFI_SUCCESS;
}

STATIC BLOCK_VARIABLE_INSTANCE   mBlockVariableInstance = {
  .Signature      = BLOCK_VARIABLE_SIGNATURE,
  .Media          = {
    .MediaId                       = 0,
    .RemovableMedia                = FALSE,
    .MediaPresent                  = TRUE,
    .LogicalPartition              = TRUE,
    .ReadOnly                      = FALSE,
    .WriteCaching                  = FALSE,
    .BlockSize                     = 0,
    .IoAlign                       = 4,
    .LastBlock                     = 0,
    .LowestAlignedLba              = 0,
    .LogicalBlocksPerPhysicalBlock = 0,
  },
  .FvbProtocol    = {
    .GetAttributes        = FvbGetAttributes,
    .SetAttributes        = FvbSetAttributes,
    .GetPhysicalAddress   = FvbGetPhysicalAddress,
    .GetBlockSize         = FvbGetBlockSize,
    .Read                 = FvbRead,
    .Write                = FvbWrite,
    .EraseBlocks          = FvbEraseBlocks,
  }
};

EFI_STATUS
InitNonVolatileVariableStore (
  IN BLOCK_VARIABLE_INSTANCE      *Instance
  )
{
  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader;
  UINT8                                 *NvStorageData;
  UINT32                                NvStorageSize;
  UINT32                                Count;
  EFI_LBA				Lba;
  EFI_DEVICE_PATH_PROTOCOL              *NvBlockDevicePath;
  EFI_BLOCK_IO_PROTOCOL                 *NvStorageBlockIo;
  EFI_STATUS                            Status;
  UINTN                                 HeadersLength;
  UINTN                                 HeadersBlockLength;
  VARIABLE_STORE_HEADER                 *VariableStoreHeader;

  Lba = (EFI_LBA) PcdGet32 (PcdNvStorageVariableBlockLba);
  Count = PcdGet32 (PcdNvStorageVariableBlockCount);
  Instance->Media.BlockSize = PcdGet32 (PcdNvStorageVariableBlockSize);
  NvStorageSize = Count * Instance->Media.BlockSize;
  Instance->StartLba = Lba;
  HeadersLength = sizeof(EFI_FIRMWARE_VOLUME_HEADER) + sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER);
  if (NvStorageSize < HeadersLength) {
    return EFI_BAD_BUFFER_SIZE;
  }
  HeadersBlockLength = (HeadersLength + Instance->Media.BlockSize - 1) / Instance->Media.BlockSize * Instance->Media.BlockSize;
  NvStorageData = (UINT8 *) (UINTN) PcdGet32(PcdFlashNvStorageVariableBase);
  mMapNvStorageVariableBase = PcdGet32(PcdFlashNvStorageVariableBase);
  NvBlockDevicePath = &Instance->DevicePath;
  NvBlockDevicePath = ConvertTextToDevicePath ((CHAR16*)FixedPcdGetPtr (PcdNvStorageVariableBlockDevicePath));
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &NvBlockDevicePath,
                                  &Instance->Handle);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Warning: Couldn't locate NVM device (status: %r)\n", Status));
    return EFI_INVALID_PARAMETER;
  }
  Status = gBS->OpenProtocol (
		      Instance->Handle,
                      &gEfiBlockIoProtocolGuid,
		      (VOID **) &Instance->BlockIoProtocol,
                      gImageHandle,
                      NULL,
                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                      );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Warning: Couldn't open NVM device (status: %r)\n", Status));
    return EFI_DEVICE_ERROR;
  }
  WriteBackDataCacheRange (Instance, sizeof(BLOCK_VARIABLE_INSTANCE));
  NvStorageBlockIo = Instance->BlockIoProtocol;
  Status = NvStorageBlockIo->ReadBlocks (NvStorageBlockIo, NvStorageBlockIo->Media->MediaId, Lba, HeadersBlockLength, (VOID *)NvStorageData);

  if (EFI_ERROR (Status)) {
    return Status;
  }
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) NvStorageData;
  if ((FvHeader->Signature != EFI_FVH_SIGNATURE) || (!CompareGuid (&gEfiSystemNvDataFvGuid, &FvHeader->FileSystemGuid))) {
    // Check if the size of the area is at least one block size
    ASSERT((PcdGet32(PcdFlashNvStorageVariableSize) > 0) && (PcdGet32(PcdFlashNvStorageVariableSize) / NvStorageBlockIo->Media->BlockSize > 0));
    ASSERT((PcdGet32(PcdFlashNvStorageFtwWorkingSize) > 0) && (PcdGet32(PcdFlashNvStorageFtwWorkingSize) / NvStorageBlockIo->Media->BlockSize > 0));
    ASSERT((PcdGet32(PcdFlashNvStorageFtwSpareSize) > 0) && (PcdGet32(PcdFlashNvStorageFtwSpareSize) / NvStorageBlockIo->Media->BlockSize > 0));

    //
    // EFI_FIRMWARE_VOLUME_HEADER
    //
    CopyGuid (&FvHeader->FileSystemGuid, &gEfiSystemNvDataFvGuid);
    FvHeader->FvLength =
        PcdGet32(PcdFlashNvStorageVariableSize) +
        PcdGet32(PcdFlashNvStorageFtwWorkingSize) +
        PcdGet32(PcdFlashNvStorageFtwSpareSize);
    FvHeader->Signature = EFI_FVH_SIGNATURE;
    FvHeader->Attributes = (EFI_FVB_ATTRIBUTES_2) (
                                            EFI_FVB2_READ_ENABLED_CAP   | // Reads may be enabled
                                            EFI_FVB2_READ_STATUS        | // Reads are currently enabled
                                            EFI_FVB2_STICKY_WRITE       | // A block erase is required to flip bits into EFI_FVB2_ERASE_POLARITY
                                            EFI_FVB2_MEMORY_MAPPED      | // It is memory mapped
                                            EFI_FVB2_ERASE_POLARITY     | // After erasure all bits take this value (i.e. '1')
                                            EFI_FVB2_WRITE_STATUS       | // Writes are currently enabled
                                            EFI_FVB2_WRITE_ENABLED_CAP    // Writes may be enabled
                                        );
    FvHeader->HeaderLength          = sizeof(EFI_FIRMWARE_VOLUME_HEADER) + sizeof(EFI_FV_BLOCK_MAP_ENTRY);
    FvHeader->Revision              = EFI_FVH_REVISION;
    FvHeader->BlockMap[0].NumBlocks = Count;
    FvHeader->BlockMap[0].Length    = NvStorageBlockIo->Media->BlockSize;
    // BlockMap Terminator
    FvHeader->BlockMap[1].NumBlocks = 0;
    FvHeader->BlockMap[1].Length    = 0;
    FvHeader->Checksum = CalculateCheckSum16 ((UINT16*)FvHeader,FvHeader->HeaderLength);

    //
    // VARIABLE_STORE_HEADER
    //
    VariableStoreHeader = (VARIABLE_STORE_HEADER*)((UINTN)FvHeader + FvHeader->HeaderLength);
    CopyGuid (&VariableStoreHeader->Signature, &gEfiVariableGuid);
    VariableStoreHeader->Size = PcdGet32(PcdFlashNvStorageVariableSize) - FvHeader->HeaderLength;
    VariableStoreHeader->Format            = VARIABLE_STORE_FORMATTED;
    VariableStoreHeader->State             = VARIABLE_STORE_HEALTHY;

    Status = NvStorageBlockIo->WriteBlocks (NvStorageBlockIo, NvStorageBlockIo->Media->MediaId, Lba, HeadersBlockLength, (VOID *)NvStorageData);
  }
  return Status;
}

EFI_STATUS
BlockVariableDxeInitialize (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_HANDLE                      Handle;
  EFI_STATUS                      Status;
  BLOCK_VARIABLE_INSTANCE         *Instance = &mBlockVariableInstance;

  Instance->Signature = BLOCK_VARIABLE_SIGNATURE;
  Status = InitNonVolatileVariableStore (Instance);
  if (EFI_ERROR (Status)) {
    goto exit;
  }

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
		  &Handle,
		  &gEfiFirmwareVolumeBlockProtocolGuid, &Instance->FvbProtocol,
		  NULL
		  );
  if (EFI_ERROR (Status)) {
    goto exit;
  }

exit:
  return Status;
}
