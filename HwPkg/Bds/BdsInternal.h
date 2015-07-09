/** @file
*
*  Copyright (c) 2011-2014, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#ifndef _BDSINTERNAL_H_
#define _BDSINTERNAL_H_

#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BdsLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/DevicePathFromText.h>
#include <Protocol/DevicePathToText.h>

#include <Guid/GlobalVariable.h>

#include "CustomizedLoader.h"

#define BOOT_DEVICE_DESCRIPTION_MAX   100
#define BOOT_DEVICE_FILEPATH_MAX      100
#define BOOT_DEVICE_OPTION_MAX        300
#define BOOT_DEVICE_ADDRESS_MAX       (sizeof(L"0x0000000000000000"))

#define ARM_BDS_OPTIONAL_DATA_SIGNATURE   SIGNATURE_32('a', 'b', 'o', 'd')

#define IS_ARM_BDS_BOOTENTRY(ptr)  \
  (((ptr)->OptionalData != NULL) && \
   (ReadUnaligned32 ((CONST UINT32*)&((ARM_BDS_LOADER_OPTIONAL_DATA*)((ptr)->OptionalData))->Header.Signature) \
      == ARM_BDS_OPTIONAL_DATA_SIGNATURE))

#define UPDATE_BOOT_ENTRY L"Update entry: "
#define DELETE_BOOT_ENTRY L"Delete entry: "
#define MOVE_BOOT_ENTRY   L"Move entry: "

typedef enum {
    BDS_LOADER_EFI_APPLICATION = 0,
    BDS_LOADER_KERNEL_LINUX_ATAG,
    BDS_LOADER_KERNEL_LINUX_FDT,
} ARM_BDS_LOADER_TYPE;

typedef struct {
  UINT16                     CmdLineSize;
  UINT16                     InitrdSize;

  // These following fields have variable length and are packed:
  //CHAR8                      *CmdLine;
  //EFI_DEVICE_PATH_PROTOCOL   *InitrdPathList;
} ARM_BDS_LINUX_ARGUMENTS;

typedef union {
  ARM_BDS_LINUX_ARGUMENTS    LinuxArguments;
} ARM_BDS_LOADER_ARGUMENTS;

typedef struct {
  UINT32                     Signature;
  ARM_BDS_LOADER_TYPE        LoaderType;
} ARM_BDS_LOADER_OPTIONAL_DATA_HEADER;

typedef struct {
  ARM_BDS_LOADER_OPTIONAL_DATA_HEADER Header;
  ARM_BDS_LOADER_ARGUMENTS            Arguments;
} ARM_BDS_LOADER_OPTIONAL_DATA;

typedef struct {
  LIST_ENTRY                  Link;
  BDS_LOAD_OPTION*            BdsLoadOption;
} BDS_LOAD_OPTION_ENTRY;

typedef enum {
  BDS_DEVICE_FILESYSTEM = 0,
  BDS_DEVICE_MEMMAP,
  BDS_DEVICE_PXE,
  BDS_DEVICE_TFTP,
  BDS_DEVICE_MAX
} BDS_SUPPORTED_DEVICE_TYPE;

typedef struct {
  LIST_ENTRY                          Link;
  CHAR16                              Description[BOOT_DEVICE_DESCRIPTION_MAX];
  EFI_DEVICE_PATH_PROTOCOL*           DevicePathProtocol;
  struct _BDS_LOAD_OPTION_SUPPORT*    Support;
} BDS_SUPPORTED_DEVICE;

#define SUPPORTED_BOOT_DEVICE_FROM_LINK(a)   BASE_CR(a, BDS_SUPPORTED_DEVICE, Link)

typedef struct _BDS_LOAD_OPTION_SUPPORT {
  BDS_SUPPORTED_DEVICE_TYPE   Type;
  EFI_STATUS    (*ListDevices)(IN OUT LIST_ENTRY* BdsLoadOptionList);
  BOOLEAN       (*IsSupported)(IN  EFI_DEVICE_PATH *DevicePath);
  EFI_STATUS    (*CreateDevicePathNode)(IN CHAR16* FileName, OUT EFI_DEVICE_PATH_PROTOCOL **DevicePathNodes);
  EFI_STATUS    (*UpdateDevicePathNode)(IN EFI_DEVICE_PATH *OldDevicePath, IN CHAR16* FileName, OUT EFI_DEVICE_PATH_PROTOCOL** NewDevicePath);

  /// Define if the boot menu should request if the file is a EFI binary or a Linux kernel
  /// Example: PXE boot always deliver a UEFI application.
  BOOLEAN       RequestBootType;
} BDS_LOAD_OPTION_SUPPORT;

#define LOAD_OPTION_ENTRY_FROM_LINK(a)  BASE_CR(a, BDS_LOAD_OPTION_ENTRY, Link)
#define LOAD_OPTION_FROM_LINK(a)        ((BDS_LOAD_OPTION_ENTRY*)BASE_CR(a, BDS_LOAD_OPTION_ENTRY, Link))->BdsLoadOption

//
// Cryptograhpic Key Information
//
typedef struct _CPL_KEY_INFO {
  UINT32        KeyLengthInBits;    // Key Length In Bits
  UINT32        BlockSize;          // Operation Block Size in Bytes
  UINT32        CipherBlockSize;    // Output Cipher Block Size in Bytes
  UINT32        KeyType;            // Key Type
  UINT32        CipherMode;         // Cipher Mode for Symmetric Algorithm
  UINT32        Flags;              // Additional Key Property Flags
} CPL_KEY_INFO;


typedef
EFI_STATUS
(EFIAPI *HASH_GET_CONTEXT_SIZE)(
  VOID
  );

typedef
BOOLEAN
(EFIAPI *HASH_INIT)(
  IN OUT  VOID  *HashContext
  );

typedef
BOOLEAN
(EFIAPI *HASH_UPDATE)(
  IN OUT  VOID        *HashContext,
  IN      CONST VOID  *Data,
  IN      UINTN       DataLength
  );

typedef
BOOLEAN
(EFIAPI *HASH_FINAL)(
  IN OUT  VOID   *HashContext,
  OUT     UINT8  *HashValue
  );

//
// Hash Algorithm Table
//
typedef struct {
  CHAR16                   *Name;           ///< Name for Hash Algorithm
  UINTN                    DigestLength;    ///< Digest Length
  UINT8                    *OidValue;       ///< Hash Algorithm OID ASN.1 Value 
  UINTN                    OidLength;       ///< Length of Hash OID Value
  HASH_GET_CONTEXT_SIZE    GetContextSize;  ///< Pointer to Hash GetContentSize function
  HASH_INIT                HashInit;        ///< Pointer to Hash Init function
  HASH_UPDATE              HashUpdate;      ///< Pointer to Hash Update function
  HASH_FINAL               HashFinal;       ///< Pointer to Hash Final function
} HASH_TABLE;

typedef struct {
  WIN_CERTIFICATE Hdr;
  UINT8           CertData[1];
} WIN_CERTIFICATE_EFI_PKCS;

typedef enum{
  ImageType_IA32,
  ImageType_X64
} IMAGE_TYPE;
#define MAX_CHAR              480
#define TWO_BYTE_ENCODE       0x82

#define SHA256_DIGEST_SIZE  32
#define MAX_DIGEST_SIZE    SHA256_DIGEST_SIZE
#define WIN_CERT_UEFI_RSA2048_SIZE               256

//
// Support hash types
//
#define HASHALG_SHA1                           0x00000000
#define HASHALG_SHA224                         0x00000001
#define HASHALG_SHA256                         0x00000002
#define HASHALG_SHA384                         0x00000003
#define HASHALG_SHA512                         0x00000004
#define HASHALG_MAX                            0x00000005
// Question ID 0x2000 ~ 0x2FFF is for KEK
//
#define OPTION_DEL_KEK_QUESTION_ID            0x2000
//
// Question ID 0x3000 ~ 0x3FFF is for DB
//
#define OPTION_DEL_DB_QUESTION_ID             0x3000
//
// Question ID 0x4000 ~ 0x4FFF is for DBX
//
#define OPTION_DEL_DBX_QUESTION_ID            0x4000 

typedef struct {
  UINT32               Offset;      // Offset of certificate
  UINT32               SizeOfCert;  // size of certificate appended
} EFI_IMAGE_SECURITY_DATA_DIRECTORY;

EFI_STATUS
BootDeviceListSupportedInit (
  IN OUT LIST_ENTRY *SupportedDeviceList
  );

EFI_STATUS
BootDeviceTypeListSupportedInit (
  IN OUT LIST_ENTRY *SupportedDeviceList,
  IN UINTN   DeviceType
  );


EFI_STATUS
ListAllSupportedBootDevice (VOID
//  IN LIST_ENTRY *BootOptionsList
  );

EFI_STATUS
BdsLoadOptionFileSystemCreatePcdDevicePath (
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePathNodes
  );

EFI_STATUS
BootDeviceListSupportedFree (
  IN LIST_ENTRY *SupportedDeviceList,
  IN BDS_SUPPORTED_DEVICE *Except
  );

EFI_STATUS
BootDeviceGetDeviceSupport (
  IN  EFI_DEVICE_PATH           *DevicePath,
  OUT BDS_LOAD_OPTION_SUPPORT   **DeviceSupport
  );

EFI_STATUS
GetHIInputStr (
  IN OUT CHAR16  *CmdLine,
  IN     UINTN   MaxCmdLine
  );

EFI_STATUS
EditHIInputStr (
  IN OUT CHAR16  *CmdLine,
  IN     UINTN   MaxCmdLine
  );

EFI_STATUS
GetHIInputAscii (
  IN OUT CHAR8   *CmdLine,
  IN     UINTN   MaxCmdLine
  );

EFI_STATUS
EditHIInputAscii (
  IN OUT CHAR8   *CmdLine,
  IN     UINTN   MaxCmdLine
  );

EFI_STATUS
GetHIInputInteger (
  IN OUT UINTN   *Integer
  );

EFI_STATUS
GetHIInputIP (
  OUT EFI_IP_ADDRESS   *Ip
  );

EFI_STATUS
EditHIInputIP (
  IN  EFI_IP_ADDRESS  *InIpAddr,
  OUT EFI_IP_ADDRESS  *OutIpAddr
  );

EFI_STATUS
GetHIInputBoolean (
  OUT BOOLEAN *Value
  );

BOOLEAN
HasFilePathEfiExtension (
  IN CHAR16* FilePath
  );

EFI_DEVICE_PATH*
GetLastDevicePathNode (
  IN EFI_DEVICE_PATH*  DevicePath
  );

EFI_STATUS
BdsStartBootOption (
  IN CHAR16* BootOption
  );

UINTN
GetUnalignedDevicePathSize (
  IN EFI_DEVICE_PATH* DevicePath
  );

EFI_DEVICE_PATH*
GetAlignedDevicePath (
  IN EFI_DEVICE_PATH* DevicePath
  );

EFI_STATUS
GenerateDeviceDescriptionName (
  IN  EFI_HANDLE  Handle,
  IN OUT CHAR16*  Description
  );

EFI_STATUS
BootOptionList (
  IN OUT LIST_ENTRY *BootOptionList
  );

EFI_STATUS
BootOptionOrderList (
  IN OUT LIST_ENTRY *BootOptionList
  );


EFI_STATUS
BootOptionParseLoadOption (
  IN  EFI_LOAD_OPTION EfiLoadOption,
  IN  UINTN           EfiLoadOptionSize,
  OUT BDS_LOAD_OPTION **BdsLoadOption
  );

VOID
ESL_Start_Kernel(
  VOID
  );

VOID
FPGA_Start_SRE(
  VOID
  );


VOID
Flash_Start_Kernel(
  VOID
  );



EFI_STATUS
BootOptionStart (
  IN BDS_LOAD_OPTION *BootOption
  );

EFI_STATUS
BootOptionCreate (
  IN  UINT32                    Attributes,
  IN  CHAR16*                   BootDescription,
  IN  EFI_DEVICE_PATH_PROTOCOL* DevicePath,
  IN  ARM_BDS_LOADER_TYPE       BootType,
  IN  UINT8*                    OptionalData,
  IN  UINTN                     OptionalDataSize,
  OUT BDS_LOAD_OPTION**         BdsLoadOption
  );

EFI_STATUS
BootOptionUpdate (
  IN  BDS_LOAD_OPTION*          BdsLoadOption,
  IN  UINT32                    Attributes,
  IN  CHAR16*                   BootDescription,
  IN  EFI_DEVICE_PATH_PROTOCOL* DevicePath,
  IN  ARM_BDS_LOADER_TYPE       BootType,
  IN UINT8*                     OptionalData,
  IN UINTN                      OptionalDataSize
  );

EFI_STATUS
BootOptionDelete (
  IN  BDS_LOAD_OPTION *BootOption
  );

EFI_STATUS
BootDeviceGetType (
  IN  EFI_DEVICE_PATH* DevicePath,
  OUT ARM_BDS_LOADER_TYPE *BootType,
  OUT UINT32 *Attributes
  );

EFI_STATUS
BootMenuMain (
  VOID
  );

BOOLEAN
IsUnicodeString (
  IN VOID* String
  );

/*
 * Try to detect if the given string is an ASCII or Unicode string
 *
 * There are actually few limitation to this function but it is mainly to give
 * a user friendly output.
 *
 * Some limitations:
 *   - it only supports unicode string that use ASCII character (< 0x100)
 *   - single character ASCII strings are interpreted as Unicode string
 *   - string cannot be longer than 2 x BOOT_DEVICE_OPTION_MAX (600 bytes)
 *
 * @param String    Buffer that might contain a Unicode or Ascii string
 * @param IsUnicode If not NULL this boolean value returns if the string is an
 *                  ASCII or Unicode string.
 */
BOOLEAN
IsPrintableString (
  IN  VOID*    String,
  OUT BOOLEAN *IsUnicode
  );

/**
  An empty function to pass error checking of CreateEventEx ().

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context,
                                which is implementation-dependent.

**/
VOID
EFIAPI
EmptyCallbackFunction (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  );


//start c00213799 secure boot
EFI_STATUS
EnrollPlatformKey (
    IN UINT16                 *FileName,
    IN UINTN                   DataSize,
    IN VOID                    *Data
  );                       


EFI_STATUS
EnrollKeyExchangeKey (
    IN UINT16                  *FileName,
    IN UINTN                   DataSize,
    IN VOID                    *Data
  ); 

EFI_STATUS
EnrollSignatureDatabase (
    IN UINT16                  *FileName,
    IN CHAR16                    *VarName,
    IN UINTN                   DataSize,
    IN VOID                    *Data
  ); 

EFI_STATUS
DeleteSignatureDatabase (
  IN CHAR16                           *VariableName,
  IN  UINTN                       DeleteIndex
  );

EFI_STATUS
DeleteKeyExchangeKey (
  IN  UINTN                       DeleteKekIndex
  );

EFI_STATUS
DeletePlatformKey (
  VOID
);

#endif /* _BDSINTERNAL_H_ */
