/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpat0TH65jPMlUENBvGXufQCTRBL0qDbp15usAFL4sRUNaa9PQnehq7qerrn2
LiJ41nIlwfFKj89OIPNlT8p4BEwC9MTzZ2aX+ZRoyaL5PELQhOJQErCF57+D+SnuiIpPempS
tpwbHf5yS9ytax5N6EwO4mZ8BvkQEMlYLeWwyAuT/beusl+6kVhtixIa4qgXkg==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*************************************************

  Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.


  Author: h00132467		Version:        Date: 2011.4.18  16:59:39

  Description:

  History:

    1. Date:       Author:

       Modification:

*************************************************/
//dts2012071600238,d00214344,2012-7-16,fortify整改BMM

#ifndef _OEM_FTP_PROTOCOL_H_
#define _OEM_FTP_PROTOCOL_H_

#define OEM_FTP_PROTOCOL_GUID \
    { \
        0x21655A5D, 0x1233, 0x4CE8, 0xB4, 0xD8, 0x87, 0xFD, 0xC1, 0x6C, 0x39, 0x68 \
    }

#define MAX_STR_SIZE 40

typedef struct
{
    IN EFI_HANDLE       nicInfo; // 网口信息
    IN EFI_IPv4_ADDRESS serverIp; // ftp服务器IP
    IN UINT16           port; // 端口
    IN CHAR8            userName[MAX_STR_SIZE]; // ftp用户名, NULL表示匿名
    IN CHAR8            BMM[MAX_STR_SIZE]; // ftp密码, NULL表示匿名
} OEM_FTP_TOKEN;

/*下载文件协议接口定义，默认下载到RamDisk的 根目录，如果本地文件系统存在同名的文件，则会覆盖同名文件*/
typedef
EFI_STATUS
(EFIAPI * FTP_GET_TO_FILESYS)(
    IN OEM_FTP_TOKEN *ftpToken,
    IN CHAR8   *      fileName //文件名
);

typedef
EFI_STATUS
(EFIAPI * FTP_GET_TO_MEM)(
    IN OEM_FTP_TOKEN *ftpToken,
    IN CHAR8    *     fileName, //文件名
    OUT UINTN   *     fileSize, // 输出的文件大小
    OUT UINT8   **    data // Data在函数里面分配内存，在下载成功时调用者需要释放Data
);

// 从Ramdisk的根目录上传文件，如果服务器的目录下存在同名文件，则由服务器的设定决定是否覆盖文件
typedef
EFI_STATUS
(EFIAPI * FTP_PUT_FROM_FILESYS)(
    IN OEM_FTP_TOKEN *ftpToken,
    IN CHAR8    *     fileName //文件名
);

typedef
EFI_STATUS
(EFIAPI * FTP_PUT_FROM_MEM)(
    IN OEM_FTP_TOKEN *     ftpToken,
    IN UINT8              *data, //需要上传的数据
    IN UINTN               size, //上传数据的大小
    IN CHAR8              *fileName // 上传到服务器的文件名称
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
