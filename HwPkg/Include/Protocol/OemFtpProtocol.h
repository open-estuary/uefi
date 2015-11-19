

#ifndef _OEM_FTP_PROTOCOL_H_
#define _OEM_FTP_PROTOCOL_H_

#define OEM_FTP_PROTOCOL_GUID \
    { \
        0x21655A5D, 0x1233, 0x4CE8, 0xB4, 0xD8, 0x87, 0xFD, 0xC1, 0x6C, 0x39, 0x68 \
    }

#define MAX_STR_SIZE 40

typedef struct
{
    IN EFI_HANDLE       nicInfo; 
    IN EFI_IPv4_ADDRESS serverIp;
    IN UINT16           port;
    IN CHAR8            userName[MAX_STR_SIZE]; 
    IN CHAR8            BMM[MAX_STR_SIZE]; 
} OEM_FTP_TOKEN;


typedef
EFI_STATUS
(EFIAPI * FTP_GET_TO_FILESYS)(
    IN OEM_FTP_TOKEN *ftpToken,
    IN CHAR8   *      fileName 
);

typedef
EFI_STATUS
(EFIAPI * FTP_GET_TO_MEM)(
    IN OEM_FTP_TOKEN *ftpToken,
    IN CHAR8    *     fileName,
    OUT UINTN   *     fileSize, 
    OUT UINT8   **    data 
);


typedef
EFI_STATUS
(EFIAPI * FTP_PUT_FROM_FILESYS)(
    IN OEM_FTP_TOKEN *ftpToken,
    IN CHAR8    *     fileName 
);

typedef
EFI_STATUS
(EFIAPI * FTP_PUT_FROM_MEM)(
    IN OEM_FTP_TOKEN *     ftpToken,
    IN UINT8              *data, 
    IN UINTN               size, 
    IN CHAR8              *fileName 
);

typedef struct _OEM_FTP_PROTOCOL
{
    FTP_GET_TO_FILESYS   FtpRead;
    FTP_GET_TO_MEM       FtpReadToMem;
    FTP_PUT_FROM_FILESYS FtpWrite;
    FTP_PUT_FROM_MEM     FtpWriteFromMem;
} OEM_FTP_PROTOCOL;

extern EFI_GUID gOemFtpProtocolGuid;

#endif
