
#include "BdsInternal.h"
//c00213799 secure boot
#include <Library/BaseCryptLib.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/ImageAuthentication.h>
//
// OID ASN.1 Value for Hash Algorithms
//
UINT8 mHashOidValue[] = {
  0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05,         // OBJ_md5
  0x2B, 0x0E, 0x03, 0x02, 0x1A,                           // OBJ_sha1
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04,   // OBJ_sha224
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,   // OBJ_sha256
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02,   // OBJ_sha384
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03,   // OBJ_sha512
  };

HASH_TABLE mHash[] = {
  { L"SHA1",   20, &mHashOidValue[8],  5, Sha1GetContextSize,  Sha1Init,   Sha1Update,    Sha1Final  },
  { L"SHA224", 28, &mHashOidValue[13], 9, NULL,                NULL,       NULL,          NULL       },
  { L"SHA256", 32, &mHashOidValue[22], 9, Sha256GetContextSize,Sha256Init, Sha256Update,  Sha256Final},
  { L"SHA384", 48, &mHashOidValue[31], 9, NULL,                NULL,       NULL,          NULL       },
  { L"SHA512", 64, &mHashOidValue[40], 9, NULL,                NULL,       NULL,          NULL       }
};

//
// Variable Definitions 
//                                          
UINT32            mPeCoffHeaderOffset = 0;
WIN_CERTIFICATE   *mCertificate = NULL;
IMAGE_TYPE        mImageType;
UINT8             *mImageBase = NULL;
UINTN             mImageSize = 0;
UINT8             mImageDigest[MAX_DIGEST_SIZE];
UINTN             mImageDigestSize;
EFI_GUID          mCertType;
EFI_IMAGE_SECURITY_DATA_DIRECTORY    *mSecDataDir = NULL;
EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION  mNtHeader;

//
// Possible DER-encoded certificate file suffixes, end with NULL pointer.
//
CHAR16* mDerEncodedSuffix[] = {
  L".cer",
  L".der",
  L".crt",
  NULL
};
CHAR16* mSupportX509Suffix = L"*.cer/der/crt";
EFI_GUID gSignatureOwnerGuid      = { 0x79736E49, 0x6564, 0xBBAA, 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 };

/**
  This code checks if the FileSuffix is one of the possible DER-encoded certificate suffix.

  @param[in] FileSuffix            The suffix of the input certificate file

  @retval    TRUE           It's a DER-encoded certificate.
  @retval    FALSE          It's NOT a DER-encoded certificate.

**/
BOOLEAN
IsDerEncodeCertificate (
  IN CONST CHAR16         *FileSuffix
)
{
  UINTN     Index; 
  for (Index = 0; mDerEncodedSuffix[Index] != NULL; Index++) {
    
    if (StrCmp (FileSuffix, mDerEncodedSuffix[Index]) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Create a time based data payload by concatenating the EFI_VARIABLE_AUTHENTICATION_2
  descriptor with the input data. NO authentication is required in this function.
  
  @param[in, out]   DataSize       On input, the size of Data buffer in bytes.
                                   On output, the size of data returned in Data
                                   buffer in bytes.
  @param[in, out]   Data           On input, Pointer to data buffer to be wrapped or 
                                   pointer to NULL to wrap an empty payload.
                                   On output, Pointer to the new payload date buffer allocated from pool,
                                   it's caller's responsibility to free the memory when finish using it. 

  @retval EFI_SUCCESS              Create time based payload successfully.
  @retval EFI_OUT_OF_RESOURCES     There are not enough memory resourses to create time based payload.
  @retval EFI_INVALID_PARAMETER    The parameter is invalid.
  @retval Others                   Unexpected error happens.

**/
EFI_STATUS
CreateTimeBasedPayload (
  IN OUT UINTN            *DataSize,
  IN OUT UINT8            **Data
  )
{
 // EFI_STATUS                       Status;
  UINT8                            *NewData;
  UINT8                            *Payload;
  UINTN                            PayloadSize;
  EFI_VARIABLE_AUTHENTICATION_2    *DescriptorData;
  UINTN                            DescriptorSize;
  EFI_TIME                         Time;
  
  if (Data == NULL || DataSize == NULL) {
    DEBUG ((EFI_D_ERROR, "Data == NULL || DataSize == NULL"));
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // In Setup mode or Custom mode, the variable does not need to be signed but the 
  // parameters to the SetVariable() call still need to be prepared as authenticated
  // variable. So we create EFI_VARIABLE_AUTHENTICATED_2 descriptor without certificate
  // data in it.
  //
  Payload     = *Data;
  PayloadSize = *DataSize;
  //计算descriptor2结构体大小
  DescriptorSize    = OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo) + OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  
  NewData = (UINT8*) AllocateZeroPool (DescriptorSize + PayloadSize);
  if (NewData == NULL) {
    DEBUG ((EFI_D_ERROR, "NewData == NULL"));
    return EFI_OUT_OF_RESOURCES;
  }

  if ((Payload != NULL) && (PayloadSize != 0)) {
    DEBUG ((EFI_D_ERROR, "(Payload != NULL) && (PayloadSize != 0)"));
    CopyMem (NewData + DescriptorSize, Payload, PayloadSize);
  }

  DescriptorData = (EFI_VARIABLE_AUTHENTICATION_2 *) (NewData);

  ZeroMem (&Time, sizeof (EFI_TIME));
  //由于时间获取失败，采用直接赋值的方式
  //Status = gRT->GetTime (&Time, NULL);
  //DEBUG ((EFI_D_ERROR, "[CreateTimeBasedPayload]gRT->GetTime: %r\n", Status));
  //if (EFI_ERROR (Status)) {
  //  FreePool(NewData);
  //  return Status;
  //}
  //DEBUG ((EFI_D_ERROR, "[CreateTimeBasedPayload]Time.Day 0x%x,Time.Month: 0x%x",Time.Day,Time.Month));
  Time.Day=0;
  Time.Hour=0;
  Time.Minute=0;
  Time.Month=0;
  Time.Second=0;
  Time.Year=0;
  
  Time.Pad1       = 0;
  Time.Nanosecond = 0;
  Time.TimeZone   = 0;
  Time.Daylight   = 0;
  Time.Pad2       = 0;
  CopyMem (&DescriptorData->TimeStamp, &Time, sizeof (EFI_TIME));
 
  DescriptorData->AuthInfo.Hdr.dwLength         = OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  DescriptorData->AuthInfo.Hdr.wRevision        = 0x0200;
  DescriptorData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
  CopyGuid (&DescriptorData->AuthInfo.CertType, &gEfiCertPkcs7Guid);
  
  if (Payload != NULL) {
    FreePool(Payload);
  }
  
  *DataSize = DescriptorSize + PayloadSize;
  *Data     = NewData;
  return EFI_SUCCESS;
}

/**
  Internal helper function to delete a Variable given its name and GUID, NO authentication
  required.

  @param[in]      VariableName            Name of the Variable.
  @param[in]      VendorGuid              GUID of the Variable.

  @retval EFI_SUCCESS              Variable deleted successfully.
  @retval Others                   The driver failed to start the device.

**/
EFI_STATUS
DeleteVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
{
  EFI_STATUS              Status;
  VOID*                   Variable;
  UINT8                   *Data;
  UINTN                   DataSize;
  UINT32                  Attr;

  GetVariable2 (VariableName, VendorGuid, &Variable, NULL);
  if (Variable == NULL) {
    DEBUG ((EFI_D_ERROR, "Variable == NULL"));
    return EFI_SUCCESS;
  }
  FreePool (Variable);

  Data     = NULL;
  DataSize = 0;
  Attr     = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS
             | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

  Status = CreateTimeBasedPayload (&DataSize, &Data);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    return Status;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attr,
                  DataSize,
                  Data
                  );
  if (Data != NULL) {
    FreePool (Data);
  }
  return Status;
}

/**

  Set the platform secure boot mode into "Custom" or "Standard" mode.

  @param[in]   SecureBootMode        New secure boot mode: STANDARD_SECURE_BOOT_MODE or
                                     CUSTOM_SECURE_BOOT_MODE.
  
  @return EFI_SUCCESS                The platform has switched to the special mode successfully.
  @return other                      Fail to operate the secure boot mode.
  
**/
EFI_STATUS
SetSecureBootMode (
  IN     UINT8         SecureBootMode
  )
{
  return gRT->SetVariable (                          
                EFI_CUSTOM_MODE_NAME,
                &gEfiCustomModeEnableGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof (UINT8),
                &SecureBootMode
                );
}

/**
  Generate the PK signature list from the X509 Certificate storing file (.cer)

  @param[in]   X509File              FileHandle of X509 Certificate storing file.
  @param[out]  PkCert                Point to the data buffer to store the signature list.
  
  @return EFI_UNSUPPORTED            Unsupported Key Length.
  @return EFI_OUT_OF_RESOURCES       There are not enough memory resourses to form the signature list.
  
**/
EFI_STATUS
CreatePkX509SignatureList (
    IN UINTN                   X509DataSize,
    IN VOID                    *X509Data,
    OUT   EFI_SIGNATURE_LIST    **PkCert 
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;  
  EFI_SIGNATURE_DATA      *PkCertData;

  PkCertData = NULL;
  
  //
  // Allocate space for PK certificate list and initialize it.
  // Create PK database entry with SignatureHeaderSize equals 0.
  //
  *PkCert = (EFI_SIGNATURE_LIST*) AllocateZeroPool (
              sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1
              + X509DataSize
              );
  if (*PkCert == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  (*PkCert)->SignatureListSize   = (UINT32) (sizeof(EFI_SIGNATURE_LIST) 
                                    + sizeof(EFI_SIGNATURE_DATA) - 1
                                    + X509DataSize);
  (*PkCert)->SignatureSize       = (UINT32) (sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize);
    (*PkCert)->SignatureHeaderSize = 0;
  CopyGuid (&(*PkCert)->SignatureType, &gEfiCertX509Guid);
  PkCertData                     = (EFI_SIGNATURE_DATA*) ((UINTN)(*PkCert) 
                                                          + sizeof(EFI_SIGNATURE_LIST)
                                                          + (*PkCert)->SignatureHeaderSize);
  CopyGuid (&PkCertData->SignatureOwner, &gEfiGlobalVariableGuid);   
  //
  // Fill the PK database with PKpub data from X509 certificate file.
  //  
  CopyMem (&(PkCertData->SignatureData[0]), X509Data, X509DataSize);
  
ON_EXIT:
  
  if (EFI_ERROR(Status) && *PkCert != NULL) {
    FreePool (*PkCert);
    *PkCert = NULL;
  }
  
  return Status;
}

/**
  Enroll new PK into the System without original PK's authentication.

  The SignatureOwner GUID will be the same with PK's vendorguid.

  @param[in] PrivateData     The module's private data.

  @retval   EFI_SUCCESS            New PK enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.
  
**/
EFI_STATUS
EnrollPlatformKey (
  IN UINT16                 *FileName,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  )                       
{
  EFI_STATUS                      Status;
  UINT32                          Attr;
  UINTN                           DataSize1;
  EFI_SIGNATURE_LIST              *PkCert;
  UINT16*                         FilePostFix;
  
  PkCert = NULL;

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Parse the file's postfix. Only support DER encoded X.509 certificate files.
  //
  FilePostFix = FileName + StrLen (FileName) - 4;
  if (!IsDerEncodeCertificate(FilePostFix)) {
    DEBUG ((EFI_D_ERROR, "Unsupported file type, only DER encoded certificate (%s) is supported.", mSupportX509Suffix));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Prase the selected PK file and generature PK certificate list.
  //
  
  Status = CreatePkX509SignatureList (DataSize, Data, &PkCert);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (PkCert != NULL);
                         
  //
  // Set Platform Key variable.
  // 

  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS 
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  DataSize1 = PkCert->SignatureListSize;
  Status = CreateTimeBasedPayload (&DataSize1, (UINT8**) &PkCert);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->SetVariable(
                  EFI_PLATFORM_KEY_NAME, 
                  &gEfiGlobalVariableGuid, 
                  Attr, 
                  DataSize1, 
                  PkCert
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_OUT_OF_RESOURCES) {
      DEBUG ((EFI_D_ERROR, "Enroll PK failed with out of resource.\n"));
    }
    goto ON_EXIT;
  }
  
ON_EXIT:

  if (PkCert != NULL) {
    FreePool(PkCert);
  }

  return Status;
}

/**
  Remove the PK variable.

  @retval EFI_SUCCESS    Delete PK successfully.
  @retval Others         Could not allow to delete PK.
  
**/
EFI_STATUS
DeletePlatformKey (
  VOID
)
{
  EFI_STATUS Status;

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DeleteVariable (
             EFI_PLATFORM_KEY_NAME,
             &gEfiGlobalVariableGuid
             );
  return Status;
}

/**
  Convert a nonnegative integer to an octet string of a specified length.

  @param[in]   Integer          Pointer to the nonnegative integer to be converted
  @param[in]   IntSizeInWords   Length of integer buffer in words
  @param[out]  OctetString      Converted octet string of the specified length 
  @param[in]   OSSizeInBytes    Intended length of resulting octet string in bytes

Returns:

  @retval   EFI_SUCCESS            Data conversion successfully
  @retval   EFI_BUFFER_TOOL_SMALL  Buffer is too small for output string

**/
EFI_STATUS
EFIAPI
Int2OctStr (
  IN     CONST UINTN                *Integer,
  IN     UINTN                      IntSizeInWords,
     OUT UINT8                      *OctetString,
  IN     UINTN                      OSSizeInBytes
  )
{
  CONST UINT8  *Ptr1;
  UINT8        *Ptr2;

  for (Ptr1 = (CONST UINT8 *)Integer, Ptr2 = OctetString + OSSizeInBytes - 1;
       Ptr1 < (UINT8 *)(Integer + IntSizeInWords) && Ptr2 >= OctetString;
       Ptr1++, Ptr2--) {
    *Ptr2 = *Ptr1;
  }
       
  for (; Ptr1 < (CONST UINT8 *)(Integer + IntSizeInWords) && *Ptr1 == 0; Ptr1++);
  
  if (Ptr1 < (CONST UINT8 *)(Integer + IntSizeInWords)) {
    return EFI_BUFFER_TOO_SMALL;
  }
  
  if (Ptr2 >= OctetString) {
    ZeroMem (OctetString, Ptr2 - OctetString + 1);
  }
  
  return EFI_SUCCESS;
}

/**
  Enroll a new KEK item from public key storing file (*.pbk).

  @param[in] PrivateData           The module's private data.

  @retval   EFI_SUCCESS            New KEK enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_UNSUPPORTED        Unsupported command.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollRsa2048ToKek (
    IN UINTN                   DataSize,
    IN VOID                    *Data
  )
{
  EFI_STATUS                      Status;
  UINT32                          Attr;
  UINTN                           DataSize1;
  EFI_SIGNATURE_LIST              *KekSigList;
  CPL_KEY_INFO                    *KeyInfo;
  EFI_SIGNATURE_DATA              *KEKSigData;
  UINTN                           KekSigListSize;
  UINT8                           *KeyBuffer;  
  UINTN                           KeyLenInBytes;

  Attr        = 0;
  DataSize1    = 0;
  KeyBuffer   = NULL;
  KeyInfo     = NULL;
  KEKSigData  = NULL;
  KekSigList  = NULL;
  KekSigListSize = 0;
  
  KeyInfo = (CPL_KEY_INFO *) Data;
  if (KeyInfo->KeyLengthInBits / 8 != WIN_CERT_UEFI_RSA2048_SIZE) {
    DEBUG ((DEBUG_ERROR, "Unsupported key length, Only RSA2048 is supported.\n"));
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }
  
  //
  // Convert the Public key to fix octet string format represented in RSA PKCS#1.
  // 
  KeyLenInBytes = KeyInfo->KeyLengthInBits / 8;
  KeyBuffer = AllocateZeroPool (KeyLenInBytes);
  if (KeyBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  Int2OctStr (
    (UINTN*) (Data + sizeof (CPL_KEY_INFO)), 
    KeyLenInBytes / sizeof (UINTN), 
    KeyBuffer, 
    KeyLenInBytes
    );
  CopyMem(Data + sizeof(CPL_KEY_INFO), KeyBuffer, KeyLenInBytes);
  
  //
  // Form an new EFI_SIGNATURE_LIST.
  //
  KekSigListSize = sizeof(EFI_SIGNATURE_LIST)
                     + sizeof(EFI_SIGNATURE_DATA) - 1
                     + WIN_CERT_UEFI_RSA2048_SIZE;

  KekSigList = (EFI_SIGNATURE_LIST*) AllocateZeroPool (KekSigListSize);
  if (KekSigList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  KekSigList->SignatureListSize   = sizeof(EFI_SIGNATURE_LIST)
                                  + sizeof(EFI_SIGNATURE_DATA) - 1
                                  + WIN_CERT_UEFI_RSA2048_SIZE;
  KekSigList->SignatureHeaderSize = 0;
  KekSigList->SignatureSize = sizeof(EFI_SIGNATURE_DATA) - 1 + WIN_CERT_UEFI_RSA2048_SIZE;
  CopyGuid (&KekSigList->SignatureType, &gEfiCertRsa2048Guid);
  
  KEKSigData = (EFI_SIGNATURE_DATA*)((UINT8*)KekSigList + sizeof(EFI_SIGNATURE_LIST));
  CopyGuid (&KEKSigData->SignatureOwner, &gSignatureOwnerGuid);
  CopyMem (
    KEKSigData->SignatureData,
    Data + sizeof(CPL_KEY_INFO),
    WIN_CERT_UEFI_RSA2048_SIZE
    );
  
  //
  // Check if KEK entry has been already existed. 
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the 
  // new KEK to original variable.
  //            
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS 
         | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&KekSigListSize, (UINT8**) &KekSigList);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->GetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME, 
                  &gEfiGlobalVariableGuid, 
                  NULL, 
                  &DataSize1, 
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }
  
  //
  // Done. Now we have formed the correct KEKpub database item, just set it into variable storage,
  //             
  Status = gRT->SetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME, 
                  &gEfiGlobalVariableGuid, 
                  Attr, 
                  KekSigListSize, 
                  KekSigList
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  
ON_EXIT:

  if (KeyBuffer != NULL) {
    FreePool (KeyBuffer);
  }
  if (KekSigList != NULL) {
    FreePool (KekSigList);
  }
  
  return Status;
}

/**
  Enroll a new KEK item from X509 certificate file.

  @param[in] PrivateData           The module's private data.

  @retval   EFI_SUCCESS            New X509 is enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_UNSUPPORTED        Unsupported command.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollX509ToKek (
    IN UINTN                   DataSize,
    IN VOID                    *Data
  ) 
{
  EFI_STATUS                        Status;
  EFI_SIGNATURE_DATA                *KEKSigData;
  EFI_SIGNATURE_LIST                *KekSigList;
  UINTN                             DataSize1;
  UINTN                             KekSigListSize;
  UINT32                            Attr;
  //EFI_GUID              SignatureGuid;
  
  KekSigList     = NULL;
  KekSigListSize = 0;
  DataSize1       = 0;
  KEKSigData     = NULL;

  KekSigListSize = sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1 + DataSize;
  KekSigList = (EFI_SIGNATURE_LIST*) AllocateZeroPool (KekSigListSize);
  if (KekSigList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Fill Certificate Database parameters.
  // 
  KekSigList->SignatureListSize   = (UINT32) KekSigListSize;
  KekSigList->SignatureHeaderSize = 0;
  KekSigList->SignatureSize = (UINT32) (sizeof(EFI_SIGNATURE_DATA) - 1 + DataSize);
  CopyGuid (&KekSigList->SignatureType, &gEfiCertX509Guid);

  KEKSigData = (EFI_SIGNATURE_DATA*) ((UINT8*) KekSigList + sizeof (EFI_SIGNATURE_LIST));
  CopyGuid (&KEKSigData->SignatureOwner, &gSignatureOwnerGuid);//signatureGUID 直接赋值了
  CopyMem (KEKSigData->SignatureData, Data, DataSize);

  //
  // Check if KEK been already existed. 
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the 
  // new kek to original variable
  //    
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS 
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&KekSigListSize, (UINT8**) &KekSigList);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }
  
  Status = gRT->GetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME, 
                  &gEfiGlobalVariableGuid, 
                  NULL, 
                  &DataSize1, 
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }  

  Status = gRT->SetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME, 
                  &gEfiGlobalVariableGuid, 
                  Attr, 
                  KekSigListSize,
                  KekSigList
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  if (KekSigList != NULL) {
    FreePool (KekSigList);
  }

  return Status;
}

/**
  Enroll new KEK into the System without PK's authentication.
  The SignatureOwner GUID will be Private->SignatureGUID.
  
  @param[in] PrivateData     The module's private data.
  
  @retval   EFI_SUCCESS            New KEK enrolled successful.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   others                 Fail to enroll KEK data.
  
**/
EFI_STATUS
EnrollKeyExchangeKey (
    IN UINT16                  *FileName,
    IN UINTN                   DataSize,
    IN VOID                    *Data
  ) 
{
  UINT16*     FilePostFix;
  EFI_STATUS  Status;

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FilePostFix = FileName + StrLen (FileName) - 4;
  if (!IsDerEncodeCertificate(FilePostFix)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Parse the file's postfix. Supports DER-encoded X509 certificate, 
  // and .pbk as RSA public key file.
  //
  if (IsDerEncodeCertificate(FilePostFix)) {
    return EnrollX509ToKek (DataSize,Data);
  } else if (CompareMem (FilePostFix, L".pbk",4) == 0) {
    return EnrollRsa2048ToKek (DataSize,Data);
  } else {
    DEBUG ((EFI_D_ERROR, "Unsupported file type, only DER encoded certificate (%s) is supported.", mSupportX509Suffix));
    return EFI_INVALID_PARAMETER;
  }
}

/**
  Enroll a new X509 certificate into Signature Database (DB or DBX) without
  KEK's authentication.

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database, must be 
                             EFI_IMAGE_SECURITY_DATABASE or EFI_IMAGE_SECURITY_DATABASE1.
  
  @retval   EFI_SUCCESS            New X509 is enrolled successfully.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollX509toSigDB (
    IN CHAR16                    *VarName,
    IN UINTN                   X509DataSize,
    IN VOID                    *X509Data
  ) 
{
  EFI_STATUS                        Status;
  EFI_SIGNATURE_LIST                *SigDBCert;
  EFI_SIGNATURE_DATA                *SigDBCertData;
  VOID                              *TempData;
  UINTN                             DataSize1;
  UINTN                             SigDBSize;
  UINT32                            Attr;
  //EFI_GUID              SignatureGuid;

  SigDBSize     = 0;
  DataSize1      = 0;
  SigDBCert     = NULL;
  SigDBCertData = NULL;
  TempData          = NULL;

  //构造签名数据库格式
  SigDBSize = sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize;
  TempData = AllocateZeroPool (SigDBSize);
  if (TempData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Fill Certificate Database parameters.
  // 
  SigDBCert = (EFI_SIGNATURE_LIST*) TempData;
  SigDBCert->SignatureListSize   = (UINT32) SigDBSize;
  SigDBCert->SignatureHeaderSize = 0;
  SigDBCert->SignatureSize = (UINT32) (sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize);
  
  CopyGuid (&SigDBCert->SignatureType, &gEfiCertX509Guid);

  SigDBCertData = (EFI_SIGNATURE_DATA*) ((UINT8* ) SigDBCert + sizeof (EFI_SIGNATURE_LIST));
  CopyGuid (&SigDBCertData->SignatureOwner, &gSignatureOwnerGuid);
  CopyMem ((UINT8* ) (SigDBCertData->SignatureData), X509Data, X509DataSize);
  //
  // Check if signature database entry has been already existed. 
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the 
  // new signature data to original variable
  //
  //构造time-base属性的证书格式
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS 
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&SigDBSize, (UINT8**) &TempData);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r\n", Status));
    goto ON_EXIT;
  }
  //获取现有的DB证书
  Status = gRT->GetVariable(
                  VarName, 
                  &gEfiImageSecurityDatabaseGuid, 
                  NULL, 
                  &DataSize1, 
                  NULL
                  );
  
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }  
  //设置DB证书
  Status = gRT->SetVariable(
                  VarName, 
                  &gEfiImageSecurityDatabaseGuid, 
                  Attr, 
                  SigDBSize,
                  TempData
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:


  if (TempData != NULL) {
    FreePool (TempData);
  }

  return Status;
}

/**
  Load PE/COFF image information into internal buffer and check its validity.

  @retval   EFI_SUCCESS         Successful
  @retval   EFI_UNSUPPORTED     Invalid PE/COFF file
  @retval   EFI_ABORTED         Serious error occurs, like file I/O error etc.

**/
EFI_STATUS
LoadPeImage (
  VOID 
  )   
{
  EFI_IMAGE_DOS_HEADER                  *DosHdr;
  EFI_IMAGE_NT_HEADERS32                *NtHeader32;
  EFI_IMAGE_NT_HEADERS64                *NtHeader64;

  NtHeader32 = NULL;
  NtHeader64 = NULL;
  //
  // Read the Dos header
  //
  DosHdr = (EFI_IMAGE_DOS_HEADER*)(mImageBase);
  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE)
  {
    //
    // DOS image header is present, 
    // So read the PE header after the DOS image header
    //
    mPeCoffHeaderOffset = DosHdr->e_lfanew;
  }
  else
  {
    mPeCoffHeaderOffset = 0;
  }

  //
  // Read PE header and check the signature validity and machine compatibility
  //
  NtHeader32 = (EFI_IMAGE_NT_HEADERS32*) (mImageBase + mPeCoffHeaderOffset);
  if (NtHeader32->Signature != EFI_IMAGE_NT_SIGNATURE)
  {
    return EFI_UNSUPPORTED;
  }

  mNtHeader.Pe32 = NtHeader32;

  //
  // Check the architecture field of PE header and get the Certificate Data Directory data
  // Note the size of FileHeader field is constant for both IA32 and X64 arch
  //
  if ((NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA32) 
      || (NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_EBC)) {
    //
    // IA-32 Architecture
    //
    mImageType = ImageType_IA32;
    mSecDataDir = (EFI_IMAGE_SECURITY_DATA_DIRECTORY*) &(NtHeader32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]);
  }
  else if ((NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64)
          || (NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_X64)) {
    //
    // 64-bits Architecture
    //
    mImageType = ImageType_X64;
    NtHeader64 = (EFI_IMAGE_NT_HEADERS64 *) (mImageBase + mPeCoffHeaderOffset);
    mSecDataDir = (EFI_IMAGE_SECURITY_DATA_DIRECTORY*) &(NtHeader64->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]);
  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Calculate hash of Pe/Coff image based on the authenticode image hashing in
  PE/COFF Specification 8.0 Appendix A

  @param[in]    HashAlg   Hash algorithm type.
 
  @retval TRUE            Successfully hash image.
  @retval FALSE           Fail in hash image.

**/
BOOLEAN 
HashPeImage (
  IN  UINT32                HashAlg
  )
{
  BOOLEAN                   Status;
  UINT16                    Magic;
  EFI_IMAGE_SECTION_HEADER  *Section;
  VOID                      *HashCtx;
  UINTN                     CtxSize;
  UINT8                     *HashBase;
  UINTN                     HashSize;
  UINTN                     SumOfBytesHashed;
  EFI_IMAGE_SECTION_HEADER  *SectionHeader;
  UINTN                     Index;
  UINTN                     Pos;

  HashCtx       = NULL;
  SectionHeader = NULL;
  Status        = FALSE;

  if ((HashAlg != HASHALG_SHA1) && (HashAlg != HASHALG_SHA256)) {
    return FALSE;
  }
  
  //
  // Initialize context of hash.
  //
  ZeroMem (mImageDigest, MAX_DIGEST_SIZE);

  if (HashAlg == HASHALG_SHA1) {
    mImageDigestSize  = SHA1_DIGEST_SIZE;
    mCertType         = gEfiCertSha1Guid;    
  } else if (HashAlg == HASHALG_SHA256) {
    mImageDigestSize  = SHA256_DIGEST_SIZE;
    mCertType         = gEfiCertSha256Guid;
  }

  CtxSize   = mHash[HashAlg].GetContextSize();
  
  HashCtx = AllocatePool (CtxSize);
  ASSERT (HashCtx != NULL);

  // 1.  Load the image header into memory.

  // 2.  Initialize a SHA hash context.
  Status = mHash[HashAlg].HashInit(HashCtx);
  if (!Status) {
    goto Done;
  }
  //
  // Measuring PE/COFF Image Header;
  // But CheckSum field and SECURITY data directory (certificate) are excluded
  //
  if (mNtHeader.Pe32->FileHeader.Machine == IMAGE_FILE_MACHINE_IA64 && mNtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // NOTE: Some versions of Linux ELILO for Itanium have an incorrect magic value 
    //       in the PE/COFF Header. If the MachineType is Itanium(IA64) and the 
    //       Magic value in the OptionalHeader is EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC
    //       then override the magic value to EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC
    //
    Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  } else {
    //
    // Get the magic value from the PE/COFF Optional Header
    //
    Magic = mNtHeader.Pe32->OptionalHeader.Magic;
  }
  
  //
  // 3.  Calculate the distance from the base of the image header to the image checksum address.
  // 4.  Hash the image header from its base to beginning of the image checksum.
  //
  HashBase = mImageBase;
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    HashSize = (UINTN) ((UINT8 *) (&mNtHeader.Pe32->OptionalHeader.CheckSum) - HashBase);
  } else {
    //
    // Use PE32+ offset.
    //
    HashSize = (UINTN) ((UINT8 *) (&mNtHeader.Pe32Plus->OptionalHeader.CheckSum) - HashBase);
  }

  Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
  if (!Status) {
    goto Done;
  }
  //
  // 5.  Skip over the image checksum (it occupies a single ULONG).
  // 6.  Get the address of the beginning of the Cert Directory.
  // 7.  Hash everything from the end of the checksum to the start of the Cert Directory.
  //
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    HashBase = (UINT8 *) &mNtHeader.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
    HashSize = (UINTN) ((UINT8 *) (&mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
  } else {
    //
    // Use PE32+ offset.
    //    
    HashBase = (UINT8 *) &mNtHeader.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
    HashSize = (UINTN) ((UINT8 *) (&mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
  }

  Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
  if (!Status) {
    goto Done;
  }
  //
  // 8.  Skip over the Cert Directory. (It is sizeof(IMAGE_DATA_DIRECTORY) bytes.)
  // 9.  Hash everything from the end of the Cert Directory to the end of image header.
  //
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset
    //
    HashBase = (UINT8 *) &mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
    HashSize = mNtHeader.Pe32->OptionalHeader.SizeOfHeaders - (UINTN) ((UINT8 *) (&mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - mImageBase);
  } else {
    //
    // Use PE32+ offset.
    //
    HashBase = (UINT8 *) &mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
    HashSize = mNtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders - (UINTN) ((UINT8 *) (&mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - mImageBase);
  }

  Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
  if (!Status) {
    goto Done;
  }
  //
  // 10. Set the SUM_OF_BYTES_HASHED to the size of the header.
  //
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    SumOfBytesHashed = mNtHeader.Pe32->OptionalHeader.SizeOfHeaders;
  } else {
    //
    // Use PE32+ offset
    //
    SumOfBytesHashed = mNtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders;
  }

  //
  // 11. Build a temporary table of pointers to all the IMAGE_SECTION_HEADER
  //     structures in the image. The 'NumberOfSections' field of the image
  //     header indicates how big the table should be. Do not include any
  //     IMAGE_SECTION_HEADERs in the table whose 'SizeOfRawData' field is zero.
  //
  SectionHeader = (EFI_IMAGE_SECTION_HEADER *) AllocateZeroPool (sizeof (EFI_IMAGE_SECTION_HEADER) * mNtHeader.Pe32->FileHeader.NumberOfSections);
  ASSERT (SectionHeader != NULL);
  //
  // 12.  Using the 'PointerToRawData' in the referenced section headers as
  //      a key, arrange the elements in the table in ascending order. In other
  //      words, sort the section headers according to the disk-file offset of
  //      the section.
  //
  Section = (EFI_IMAGE_SECTION_HEADER *) (
               mImageBase +
               mPeCoffHeaderOffset +
               sizeof (UINT32) +
               sizeof (EFI_IMAGE_FILE_HEADER) +
               mNtHeader.Pe32->FileHeader.SizeOfOptionalHeader
               );
  for (Index = 0; Index < mNtHeader.Pe32->FileHeader.NumberOfSections; Index++) {
    Pos = Index;
    while ((Pos > 0) && (Section->PointerToRawData < SectionHeader[Pos - 1].PointerToRawData)) {
      CopyMem (&SectionHeader[Pos], &SectionHeader[Pos - 1], sizeof (EFI_IMAGE_SECTION_HEADER));
      Pos--;
    }
    CopyMem (&SectionHeader[Pos], Section, sizeof (EFI_IMAGE_SECTION_HEADER));
    Section += 1;
  }

  //
  // 13.  Walk through the sorted table, bring the corresponding section
  //      into memory, and hash the entire section (using the 'SizeOfRawData'
  //      field in the section header to determine the amount of data to hash).
  // 14.  Add the section's 'SizeOfRawData' to SUM_OF_BYTES_HASHED .
  // 15.  Repeat steps 13 and 14 for all the sections in the sorted table.
  //
  for (Index = 0; Index < mNtHeader.Pe32->FileHeader.NumberOfSections; Index++) {
    Section = &SectionHeader[Index];
    if (Section->SizeOfRawData == 0) {
      continue;
    }
    HashBase  = mImageBase + Section->PointerToRawData;
    HashSize  = (UINTN) Section->SizeOfRawData;

    Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
    if (!Status) {
      goto Done;
    }

    SumOfBytesHashed += HashSize;
  }

  //
  // 16.  If the file size is greater than SUM_OF_BYTES_HASHED, there is extra
  //      data in the file that needs to be added to the hash. This data begins
  //      at file offset SUM_OF_BYTES_HASHED and its length is:
  //             FileSize  -  (CertDirectory->Size)
  //
  if (mImageSize > SumOfBytesHashed) {
    HashBase = mImageBase + SumOfBytesHashed;
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset.
      //
      HashSize = (UINTN)(
                 mImageSize -
                 mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size -
                 SumOfBytesHashed);
    } else {
      //
      // Use PE32+ offset.
      //
      HashSize = (UINTN)(
                 mImageSize -
                 mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size -
                 SumOfBytesHashed);      
    }

    Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
    if (!Status) {
      goto Done;
    }
  }

  Status  = mHash[HashAlg].HashFinal(HashCtx, mImageDigest);

Done:
  if (HashCtx != NULL) {
    FreePool (HashCtx);
  }
  if (SectionHeader != NULL) {
    FreePool (SectionHeader);
  }
  return Status;
}

/**
  Recognize the Hash algorithm in PE/COFF Authenticode and caculate hash of 
  Pe/Coff image based on the authenticated image hashing in PE/COFF Specification 
  8.0 Appendix A

  @retval EFI_UNSUPPORTED             Hash algorithm is not supported.
  @retval EFI_SUCCESS                 Hash successfully.

**/
EFI_STATUS 
HashPeImageByType (
  VOID
  )
{
  UINT8                     Index;
  WIN_CERTIFICATE_EFI_PKCS  *PkcsCertData;

  PkcsCertData = (WIN_CERTIFICATE_EFI_PKCS *) (mImageBase + mSecDataDir->Offset);

  for (Index = 0; Index < HASHALG_MAX; Index++) {  
    //
    // Check the Hash algorithm in PE/COFF Authenticode.
    //    According to PKCS#7 Definition: 
    //        SignedData ::= SEQUENCE {
    //            version Version,
    //            digestAlgorithms DigestAlgorithmIdentifiers,
    //            contentInfo ContentInfo,
    //            .... }
    //    The DigestAlgorithmIdentifiers can be used to determine the hash algorithm in PE/COFF hashing
    //    This field has the fixed offset (+32) in final Authenticode ASN.1 data.
    //    Fixed offset (+32) is calculated based on two bytes of length encoding.
     //
    if ((*(PkcsCertData->CertData + 1) & TWO_BYTE_ENCODE) != TWO_BYTE_ENCODE) {
      //
      // Only support two bytes of Long Form of Length Encoding.
      //
      continue;
    }

    //    
    if (CompareMem (PkcsCertData->CertData + 32, mHash[Index].OidValue, mHash[Index].OidLength) == 0) {
      break;
    }
  }

  if (Index == HASHALG_MAX) {
    return EFI_UNSUPPORTED;
  }

  //
  // HASH PE Image based on Hash algorithm in PE/COFF Authenticode.
  //
  if (!HashPeImage(Index)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EnrollRSA2048ToSigDB (
    IN CHAR16                    *VarName,
    IN UINTN                   DataSize,
    IN VOID                    *Data
  )
{
  
    EFI_STATUS                        Status;
    EFI_SIGNATURE_LIST                *SigDBCert;
    EFI_SIGNATURE_DATA                *SigDBCertData;
    UINTN                             DataSize1;
    UINTN                             SigDBSize;
    UINT32                            Attr;
  
    SigDBSize     = 0;
    DataSize1      = 0;
    SigDBCert     = NULL;
    SigDBCertData = NULL;

    //
    // Form an new EFI_SIGNATURE_LIST.
    //
    SigDBSize = sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1 + WIN_CERT_UEFI_RSA2048_SIZE;
    SigDBCert =  (EFI_SIGNATURE_LIST*) AllocateZeroPool (SigDBSize);
    if (SigDBCert == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }

    //
    // Fill Certificate Database parameters.
    // 
    SigDBCert->SignatureListSize   = (UINT32) sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1 + WIN_CERT_UEFI_RSA2048_SIZE;
    SigDBCert->SignatureHeaderSize = 0;
    SigDBCert->SignatureSize = (sizeof(EFI_SIGNATURE_DATA) - 1 + WIN_CERT_UEFI_RSA2048_SIZE);
    CopyGuid (&SigDBCert->SignatureType, &gEfiCertRsa2048Sha256Guid);
  
    SigDBCertData = (EFI_SIGNATURE_DATA*) ((UINT8* ) SigDBCert + sizeof (EFI_SIGNATURE_LIST));
    CopyGuid (&SigDBCertData->SignatureOwner, &gSignatureOwnerGuid);
    CopyMem ((UINT8* ) (SigDBCertData->SignatureData), Data, WIN_CERT_UEFI_RSA2048_SIZE);
    //
    // Check if signature database entry has been already existed. 
    // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the 
    // new signature data to original variable
    //
    //构造time-base属性的证书格式
    Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS 
            | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
    Status = CreateTimeBasedPayload (&SigDBSize, (UINT8**) &SigDBCert);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r\n", Status));
      goto ON_EXIT;
    }
    //获取现有的DB证书
    Status = gRT->GetVariable(
                    VarName, 
                    &gEfiImageSecurityDatabaseGuid, 
                    NULL, 
                    &DataSize1, 
                    NULL
                    );
    
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Attr |= EFI_VARIABLE_APPEND_WRITE;
    } else if (Status != EFI_NOT_FOUND) {
      goto ON_EXIT;
    }  
    //设置DB证书
    Status = gRT->SetVariable(
                    VarName, 
                    &gEfiImageSecurityDatabaseGuid, 
                    Attr, 
                    SigDBSize,
                    SigDBCert
                    );
    
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }
  
  ON_EXIT:
  
  
    if (SigDBCert != NULL) {
      FreePool (SigDBCert);
    }
  
    return Status;
}

/**
  Enroll a new executable's signature into Signature Database. 

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database, must be 
                             EFI_IMAGE_SECURITY_DATABASE or EFI_IMAGE_SECURITY_DATABASE1.

  @retval   EFI_SUCCESS            New signature is enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_UNSUPPORTED        Unsupported command.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollImageSignatureToSigDB (
    IN CHAR16                    *VarName,
    IN UINTN                   DataSize,
    IN VOID                    *Data
  )
{
  EFI_STATUS                        Status;
  EFI_SIGNATURE_LIST                *SigDBCert;
  EFI_SIGNATURE_DATA                *SigDBCertData;
  VOID                              *Data1;
  UINTN                             DataSize1;
  UINTN                             SigDBSize;
  UINT32                            Attr;
  WIN_CERTIFICATE_UEFI_GUID         *GuidCertData;
  /*EFI_GUID              SignatureGuid;

  SignatureGuid.Data1 = 0x1065aaac;
  SignatureGuid.Data2 = 0xad4f;
  SignatureGuid.Data3 = 0x4627;
  SignatureGuid.Data4[0] = 0xa5;
  SignatureGuid.Data4[1] = 0xc2;
  SignatureGuid.Data4[2] = 0xe2;
  SignatureGuid.Data4[3] = 0xb0;
  SignatureGuid.Data4[4] = 0x05;
  SignatureGuid.Data4[5] = 0x26;
  SignatureGuid.Data4[6] = 0x14;
  SignatureGuid.Data4[7] = 0xc5;*/

  Data1 = NULL;
  GuidCertData = NULL;

  //
  // Form the SigDB certificate list.
  // Format the data item into EFI_SIGNATURE_LIST type.
  //
  // We need to parse executable's signature data from specified signed executable file.
  // In current implementation, we simply trust the pass-in signed executable file.
  // In reality, it's OS's responsibility to verify the signed executable file.
  //

  //
  // Read the whole file content
  //
 mImageBase =  Data;
 mImageSize =  DataSize;

  Status = LoadPeImage ();
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  if (mSecDataDir->SizeOfCert == 0) {
    if (!HashPeImage (HASHALG_SHA256)) {
      Status =  EFI_SECURITY_VIOLATION;
      goto ON_EXIT;
    }
  } else {
  
    //
    // Read the certificate data
    //
    mCertificate = (WIN_CERTIFICATE *)(mImageBase + mSecDataDir->Offset);

    if (mCertificate->wCertificateType == WIN_CERT_TYPE_EFI_GUID) {
      GuidCertData = (WIN_CERTIFICATE_UEFI_GUID*) mCertificate;
      if (CompareMem (&GuidCertData->CertType, &gEfiCertTypeRsa2048Sha256Guid, sizeof(EFI_GUID)) != 0) {
        Status = EFI_ABORTED;
        goto ON_EXIT;
      }

      if (!HashPeImage (HASHALG_SHA256)) {
        Status = EFI_ABORTED;
        goto ON_EXIT;;
      }
    
    } else if (mCertificate->wCertificateType == WIN_CERT_TYPE_PKCS_SIGNED_DATA) {

      Status = HashPeImageByType ();
      if (EFI_ERROR (Status)) {
        goto ON_EXIT;;
      }
    } else {
      Status = EFI_ABORTED;
      goto ON_EXIT;
    }
  }

  //
  // Create a new SigDB entry.
  //
  SigDBSize = sizeof(EFI_SIGNATURE_LIST) 
              + sizeof(EFI_SIGNATURE_DATA) - 1
              + (UINT32) mImageDigestSize;

  Data1 = (UINT8*) AllocateZeroPool (SigDBSize);
  if (Data1 == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  
  //
  // Adjust the Certificate Database parameters.
  // 
  SigDBCert = (EFI_SIGNATURE_LIST*) Data1;
  SigDBCert->SignatureListSize   = (UINT32) SigDBSize;
  SigDBCert->SignatureHeaderSize = 0;
  SigDBCert->SignatureSize       = sizeof(EFI_SIGNATURE_DATA) - 1 + (UINT32) mImageDigestSize;
  CopyGuid (&SigDBCert->SignatureType, &mCertType);

  SigDBCertData = (EFI_SIGNATURE_DATA*)((UINT8*)SigDBCert + sizeof(EFI_SIGNATURE_LIST));
  CopyGuid (&SigDBCertData->SignatureOwner, &gSignatureOwnerGuid);
  CopyMem (SigDBCertData->SignatureData, mImageDigest, mImageDigestSize);

  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS 
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&SigDBSize, (UINT8**) &Data1);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }
  
  //
  // Check if SigDB variable has been already existed. 
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the 
  // new signature data to original variable
  //    
  DataSize1 = 0;
  Status = gRT->GetVariable(
                  VarName, 
                  &gEfiImageSecurityDatabaseGuid, 
                  NULL, 
                  &DataSize1, 
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }  

  //
  // Enroll the variable.
  //
  Status = gRT->SetVariable(
                  VarName, 
                  &gEfiImageSecurityDatabaseGuid, 
                  Attr, 
                  SigDBSize, 
                  Data1
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  if (Data1 != NULL) {
    FreePool (Data1);
  }

  if (mImageBase != NULL) {
    FreePool (mImageBase);
    mImageBase = NULL;
  }

  return Status;
}

/**
  Enroll signature into DB/DBX without KEK's authentication.
  The SignatureOwner GUID will be Private->SignatureGUID.
  
  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database, must be 
                             EFI_IMAGE_SECURITY_DATABASE or EFI_IMAGE_SECURITY_DATABASE1.
  
  @retval   EFI_SUCCESS            New signature enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   others                 Fail to enroll signature data.
  
**/
EFI_STATUS
EnrollSignatureDatabase (
    IN UINT16                  *FileName,
    IN CHAR16                    *VarName,
    IN UINTN                   DataSize,
    IN VOID                    *Data
  ) 
{
//  EFI_STATUS   Status;
  UINT16*      FilePostFix;
  //
  // Parse the file's postfix. 
  //
  FilePostFix = FileName + StrLen (FileName) - 4;
  
  if (IsDerEncodeCertificate(FilePostFix)) {
    //
    // Supports DER-encoded X509 certificate.
    //
    DEBUG ((EFI_D_ERROR, "Supports DER-encoded X509 certificate\n"));
    return EnrollX509toSigDB (VarName,DataSize,Data);
  }

  return EnrollRSA2048ToSigDB (VarName,DataSize,Data);
  
}

/**
  Delete a KEK entry from KEK database. 

  @param[in]    PrivateData         Module's private data.
  @param[in]    QuestionId          Question id of the KEK item to delete.

  @retval   EFI_SUCCESS            Delete kek item successfully.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.
  
**/
EFI_STATUS
DeleteKeyExchangeKey (
  IN  UINTN                       DeleteKekIndex
  )
{
  EFI_STATUS                  Status;
  UINTN                       DataSize;
  UINT8                       *Data;
  UINT8                       *OldData;
  UINT32                      Attr;
  UINT32                      Index;
  EFI_SIGNATURE_LIST          *CertList;
  EFI_SIGNATURE_LIST          *NewCertList;
  EFI_SIGNATURE_DATA          *Cert;
  UINTN                       CertCount;
  UINT32                      Offset;
  BOOLEAN                     IsKEKItemFound;
  UINT32                      KekDataSize;
  UINTN                       GuidIndex;

  Data            = NULL;
  OldData         = NULL;
  CertList        = NULL;
  Cert            = NULL;
  Attr            = 0;   

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // Get original KEK variable.
  //                           
  DataSize = 0;  
  Status = gRT->GetVariable (EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, NULL);
  if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
    goto ON_EXIT;
  }

  OldData = (UINT8*)AllocateZeroPool(DataSize);
  if (OldData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;  
    goto ON_EXIT;
  }

  Status = gRT->GetVariable (EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid, &Attr, &DataSize, OldData);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  //
  // Allocate space for new variable.  
  //
  Data = (UINT8*) AllocateZeroPool (DataSize);
  if (Data == NULL) {
    Status  =  EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Enumerate all KEK pub data and erasing the target item.
  //
  IsKEKItemFound = FALSE;
  KekDataSize = (UINT32) DataSize;
  CertList = (EFI_SIGNATURE_LIST *) OldData;
  Offset = 0;
  GuidIndex = 0;
  while ((KekDataSize > 0) && (KekDataSize >= CertList->SignatureListSize)) {
    if (CompareGuid (&CertList->SignatureType, &gEfiCertRsa2048Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
      CopyMem (Data + Offset, CertList, (sizeof(EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize));
      NewCertList = (EFI_SIGNATURE_LIST *)(Data + Offset);
      Offset += (sizeof(EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      Cert      = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
      for (Index = 0; Index < CertCount; Index++) {
        if (GuidIndex == DeleteKekIndex ) {
          //
          // Find it! Skip it!
          //
          NewCertList->SignatureListSize -= CertList->SignatureSize;
          IsKEKItemFound = TRUE;          
        } else {
          //
          // This item doesn't match. Copy it to the Data buffer.
          //
          CopyMem (Data + Offset, Cert, CertList->SignatureSize);
          Offset += CertList->SignatureSize;
        }
        GuidIndex++;
        Cert = (EFI_SIGNATURE_DATA *) ((UINT8*) Cert + CertList->SignatureSize);
      }
    } else {
      //
      // This List doesn't match. Copy it to the Data buffer.
      //
      CopyMem (Data + Offset, CertList, CertList->SignatureListSize);
      Offset += CertList->SignatureListSize;
    }
      
    KekDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST*) ((UINT8*) CertList + CertList->SignatureListSize);
  }

  if (!IsKEKItemFound) {
    //
    // Doesn't find the Kek Item!
    //
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  //
  // Delete the Signature header if there is no signature in the list.
  //
  KekDataSize = Offset;
  CertList = (EFI_SIGNATURE_LIST*) Data;
  Offset = 0;
  ZeroMem (OldData, KekDataSize);
  while ((KekDataSize > 0) && (KekDataSize >= CertList->SignatureListSize)) {
    CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    DEBUG ((DEBUG_ERROR, "       CertCount = %x\n", CertCount));
    if (CertCount != 0) {
      CopyMem (OldData + Offset, CertList, CertList->SignatureListSize);
      Offset += CertList->SignatureListSize;
    }    
    KekDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

  DataSize = Offset;
  if ((Attr & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
    Status = CreateTimeBasedPayload (&DataSize, &OldData);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
      goto ON_EXIT;
    }
  }

  Status = gRT->SetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME, 
                  &gEfiGlobalVariableGuid, 
                  Attr, 
                  DataSize, 
                  OldData
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set variable, Status = %r\n", Status));
    goto ON_EXIT;
  }
 
ON_EXIT:
  if (Data != NULL) {
    FreePool(Data);
  }

  if (OldData != NULL) {
    FreePool(OldData);
  }

  return Status;
}

/**
  Delete a signature entry from siganture database.

  @param[in]    PrivateData         Module's private data.
  @param[in]    VariableName        The variable name of the vendor's signature database.
  @param[in]    VendorGuid          A unique identifier for the vendor.
  @param[in]    LabelNumber         Label number to insert opcodes.
  @param[in]    FormId              Form ID of current page.
  @param[in]    QuestionIdBase      Base question id of the signature list.
  @param[in]    DeleteIndex         Signature index to delete.
  
  @retval   EFI_SUCCESS             Delete siganture successfully.
  @retval   EFI_NOT_FOUND           Can't find the signature item,
  @retval   EFI_OUT_OF_RESOURCES    Could not allocate needed resources.
**/
EFI_STATUS
DeleteSignatureDatabase (
  IN CHAR16                           *VariableName,
  IN  UINTN                       DeleteIndex
  )
{
  EFI_STATUS                  Status;
  UINTN                       DataSize;
  UINT8                       *Data;
  UINT8                       *OldData;
  UINT32                      Attr;
  UINT32                      Index;
  EFI_SIGNATURE_LIST          *CertList;
  EFI_SIGNATURE_LIST          *NewCertList;
  EFI_SIGNATURE_DATA          *Cert;
  UINTN                       CertCount;
  UINT32                      Offset;
  BOOLEAN                     IsItemFound;
  UINT32                      ItemDataSize;
  UINTN                       GuidIndex;

  Data            = NULL;
  OldData         = NULL;
  CertList        = NULL;
  Cert            = NULL;
  Attr            = 0; 

  //进入custom模式，删除变量
  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get original signature list data.
  //                           
  DataSize = 0;
  Status = gRT->GetVariable (VariableName, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, NULL);
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    goto ON_EXIT;
  }

  OldData = (UINT8 *) AllocateZeroPool (DataSize);
  if (OldData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;  
    goto ON_EXIT;
  }

  Status = gRT->GetVariable (VariableName, &gEfiImageSecurityDatabaseGuid, &Attr, &DataSize, OldData);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  } 

  //
  // Allocate space for new variable.  
  //
  Data = (UINT8*) AllocateZeroPool (DataSize);
  if (Data == NULL) {
    Status  =  EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Enumerate all signature data and erasing the target item.
  //
  IsItemFound = FALSE;
  ItemDataSize = (UINT32) DataSize;
  CertList = (EFI_SIGNATURE_LIST *) OldData;
  Offset = 0;
  GuidIndex = 0;
  while ((ItemDataSize > 0) && (ItemDataSize >= CertList->SignatureListSize)) {
    if (CompareGuid (&CertList->SignatureType, &gEfiCertRsa2048Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertSha1Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertSha256Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertRsa2048Sha256Guid)
        ) {
      //
      // Copy EFI_SIGNATURE_LIST header then calculate the signature count in this list.
      //
      CopyMem (Data + Offset, CertList, (sizeof(EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize));
      NewCertList = (EFI_SIGNATURE_LIST*) (Data + Offset);
      Offset += (sizeof(EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      Cert      = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
      for (Index = 0; Index < CertCount; Index++) {
        if (GuidIndex == DeleteIndex) {
          //
          // Find it! Skip it!
          //
          NewCertList->SignatureListSize -= CertList->SignatureSize;
          IsItemFound = TRUE;          
        } else {
          //
          // This item doesn't match. Copy it to the Data buffer.
          //
          CopyMem (Data + Offset, (UINT8*)(Cert), CertList->SignatureSize);
          Offset += CertList->SignatureSize;
        }
        GuidIndex++;
        Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) Cert + CertList->SignatureSize);
      }
    } else {
      //
      // This List doesn't match. Just copy it to the Data buffer.
      //
      CopyMem (Data + Offset, (UINT8*)(CertList), CertList->SignatureListSize);
      Offset += CertList->SignatureListSize;
    }
      
    ItemDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

  if (!IsItemFound) {
    //
    // Doesn't find the signature Item!
    //
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  //
  // Delete the EFI_SIGNATURE_LIST header if there is no signature in the list.
  //
  ItemDataSize = Offset;
  CertList = (EFI_SIGNATURE_LIST *) Data;
  Offset = 0;
  ZeroMem (OldData, ItemDataSize);
  while ((ItemDataSize > 0) && (ItemDataSize >= CertList->SignatureListSize)) {
    CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    DEBUG ((DEBUG_ERROR, "       CertCount = %x\n", CertCount));
    if (CertCount != 0) {
      CopyMem (OldData + Offset, (UINT8*)(CertList), CertList->SignatureListSize);
      Offset += CertList->SignatureListSize;
    }    
    ItemDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

  DataSize = Offset;
  if ((Attr & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
    Status = CreateTimeBasedPayload (&DataSize, &OldData);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
      goto ON_EXIT;
    }
  }

  Status = gRT->SetVariable(
                  VariableName, 
                  &gEfiImageSecurityDatabaseGuid, 
                  Attr, 
                  DataSize, 
                  OldData
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set variable, Status = %r\n", Status));
    goto ON_EXIT;
  }
 
ON_EXIT:
  if (Data != NULL) {
    FreePool(Data);
  }

  if (OldData != NULL) {
    FreePool(OldData);
  }

  return Status;
}
