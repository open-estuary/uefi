
#ifndef _SPI_FLASH_PROTOCOL_H_
#define _SPI_FLASH_PROTOCOL_H_

typedef struct _SPI_FLASH_PROTOCOL SPI_FLASH_PROTOCOL;

typedef 
EFI_STATUS
(EFIAPI *SPI_FLASH_ERASE_INTERFACE) (
    IN SPI_FLASH_PROTOCOL   *This,
    IN UINT32                  Offset,
    IN UINT32                  ulLength
    );

typedef 
EFI_STATUS
(EFIAPI *SPI_FLASH_WRITE_INTERFACE) (
    IN SPI_FLASH_PROTOCOL  *This,
    IN  UINT32                 Offset,
    IN  UINT8                 *Buffer,
    IN  UINT32                 ulLength
    );

typedef 
EFI_STATUS
(EFIAPI *SPI_FLASH_READ_INTERFACE) (
    IN SPI_FLASH_PROTOCOL   *This,
    IN UINT32                  Offset,
    IN OUT UINT8              *Buffer,
    IN UINT32                  ulLength
    );

typedef 
EFI_STATUS
(EFIAPI *SPI_FLASH_ERASE_WRITE_INTERFACE) (
    IN SPI_FLASH_PROTOCOL  *This,
    IN  UINT32                 Offset,
    IN  UINT8                 *Buffer,
    IN  UINT32                 ulLength
    );

struct _SPI_FLASH_PROTOCOL {
    SPI_FLASH_ERASE_INTERFACE             Erase;
    SPI_FLASH_WRITE_INTERFACE             Write;
    SPI_FLASH_READ_INTERFACE              Read;
    SPI_FLASH_ERASE_WRITE_INTERFACE       EraseWrite;
};

extern EFI_GUID gSpiFlashProtocolGuid;

#endif
