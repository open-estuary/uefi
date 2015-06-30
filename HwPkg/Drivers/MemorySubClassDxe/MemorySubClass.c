/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpZ4X5IrQg0R6dsKjrHb0BePcxREXVZ5dUR67fNFdcsGWwP1ecJzNoC5Nk+8B
iJvImN1S+tx+2aUFF3UpnqtcKos7TaqjNZUlquSbqWJ1CbZiMt61kQ5MPvDnuF6TvahDeYLJ
7AvJamas+cGYBZYsCdgXsffc3Co3tIFYL6hf0xKgNUN3pawllmLU2WsGw5H5Ww==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MemorySubClass.c
  版 本 号   : v1.0
  作    者   : 
  生成日期   : 2014年10月20日
  最近修改   :
  功能描述   : 
  修改历史   :
******************************************************************************/

/**@file
Module Name:

  MemorySubClass.c
  
Abstract: 

  This is the driver that locates the MemoryConfigurationData Variable, if it
  exists, add the related SMBIOS tables by PI SMBIOS protocol.

**/

#include "MemorySubClass.h"

EFI_SMBIOS_PROTOCOL             *mSmbios = NULL;
EFI_HII_HANDLE                	mHiiHandle;

UINT8                           mMaxSkt;
UINT8                           mMaxCh;
UINT8                           mMaxDimm;

EFI_STATUS
SmbiosGetManufacturer (
  IN  UINT8           MfgIdLSB,
  IN  UINT8           MfgIdMSB,
  OUT CHAR16          *Manufacturer
)
{
	UINT32                  Index = 0;

	while (JEP106[Index].MfgIdLSB != 0xFF && JEP106[Index].MfgIdMSB != 0xFF ) 
	{
		if (JEP106[Index].MfgIdLSB == MfgIdLSB && JEP106[Index].MfgIdMSB == MfgIdMSB) 
		{
			StrnCpy (Manufacturer, JEP106[Index].Name, SMBIOS_STRING_MAX_LENGTH);
			return EFI_SUCCESS;
		}
		Index++;
	}

	return EFI_NOT_FOUND;
}

VOID
SmbiosGetPartNumber (
  IN pGBL_DATA          pGblData,
  IN UINT8       		Skt, 
  IN UINT8       		Ch,
  IN UINT8       		Dimm,
  OUT CHAR16           *PartNumber
  )
{
    CHAR16                        	StringBuffer2[SMBIOS_STRING_MAX_LENGTH];
    UINT32                        	Index2;

    StrnCpy(PartNumber, L"", 1);
    if (pGblData->Channel[Skt][Ch].Dimm[Dimm].DramType == SPD_TYPE_DDR3) 
    {
        for (Index2 = 0; Index2 < SPD_MODULE_PART; Index2++) 
        {
            UnicodeSPrint (StringBuffer2, 4, L"%c", pGblData->Channel[Skt][Ch].Dimm[Dimm].SpdModPart[Index2]);
            StrnCat(PartNumber, StringBuffer2, 4);
        }
    } 
    else 
    {
        for (Index2 = 0; Index2 < SPD_MODULE_PART_DDR4; Index2++) 
        {
            UnicodeSPrint (StringBuffer2, 4, L"%c", pGblData->Channel[Skt][Ch].Dimm[Dimm].SpdModPartDDR4[Index2]);
            StrnCat(PartNumber, StringBuffer2, 4);
        }            
    }
    return;
}

VOID
SmbiosGetSerialNumber (
  IN pGBL_DATA          pGblData,
  IN UINT8       		Skt, 
  IN UINT8       		Ch,
  IN UINT8       		Dimm,
  OUT CHAR16           *SerialNumber
  )
{
    CHAR16                        	StringBuffer2[SMBIOS_STRING_MAX_LENGTH];
    UINT32                        	Index2;

    StrnCpy(SerialNumber, L"0x", 3);
    for (Index2 = 0; Index2 < 4; Index2++) 
    {
        UnicodeValueToString (
            				  StringBuffer2,
            				  PREFIX_ZERO | RADIX_HEX,
            				  ((UINT8 *)&(pGblData->Channel[Skt][Ch].Dimm[Dimm].SpdSerialNum))[Index2],
            				  2
            				  );
        StrnCat(SerialNumber, StringBuffer2, 2);
    }
    
    return;
}


EFI_STATUS
SmbiosAddType16Table (
  IN  pGBL_DATA          pGblData,
  OUT EFI_SMBIOS_HANDLE  *MemArraySmbiosHandle
  )
{
    EFI_STATUS						Status;
    UINT64                          MemoryCapacity;	
    SMBIOS_TABLE_TYPE16           	*Type16Record;
    
    //MemoryCapacity = (UINT64) LShiftU64 (MAX_SOCKET * MAX_CHANNEL * MAX_DIMM * MAX_DIMM_SIZE / 2, 20); // GB to KB.
    MemoryCapacity = (UINT64) LShiftU64 (mMaxSkt * mMaxCh * mMaxDimm * MAX_DIMM_SIZE, 20); // GB to KB.    
    
    //
    // 上报Type 16 SMBIOS Record 
    //
    Type16Record = AllocatePool(sizeof(SMBIOS_TABLE_TYPE16) + 1 + 1);
    if (NULL == Type16Record)
    {
        return EFI_OUT_OF_RESOURCES;
    }
    ZeroMem(Type16Record, sizeof(SMBIOS_TABLE_TYPE16) + 1 + 1);
    Type16Record->Hdr.Type							= EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY;
    Type16Record->Hdr.Length 						= sizeof (SMBIOS_TABLE_TYPE16);
    Type16Record->Hdr.Handle 						= 0x0;
    Type16Record->Location 							= MemoryArrayLocationSystemBoard;
    Type16Record->Use 								= MemoryArrayUseSystemMemory;
    Type16Record->MemoryErrorInformationHandle 		= 0xFFFE;
    //Type16Record->NumberOfMemoryDevices 			= (UINT16)(MAX_SOCKET * MAX_CHANNEL * MAX_DIMM);
    Type16Record->NumberOfMemoryDevices 			= (UINT16)(mMaxSkt * mMaxCh * mMaxDimm);
    
    if(pGblData->EccEn)
    {
        Type16Record->MemoryErrorCorrection 			= MemoryErrorCorrectionMultiBitEcc;
    }
    else
    {
        Type16Record->MemoryErrorCorrection 			= MemoryErrorCorrectionNone;
    }

    if (MemoryCapacity >= 0x80000000) 
    {
        Type16Record->MaximumCapacity = 0x80000000;   // in KB;
        Type16Record->ExtendedMaximumCapacity = MemoryCapacity << 10;  // Extended Max capacity should be stored in bytes.
    } 
    else 
    {
        Type16Record->MaximumCapacity = (UINT32)MemoryCapacity;  // Max capacity should be stored in kilo bytes.
        Type16Record->ExtendedMaximumCapacity = 0;
    }

    *MemArraySmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = mSmbios->Add (mSmbios, NULL, MemArraySmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *)Type16Record);

    FreePool(Type16Record);    
    return Status;
}

EFI_STATUS
SmbiosAddType19Table (
  IN pGBL_DATA          pGblData,
  IN EFI_SMBIOS_HANDLE  MemArraySmbiosHandle
  )
{
    EFI_STATUS						Status;
    UINT32                        	MemInfoTotalMem;
    UINT64							TotalMemorySize;
    EFI_SMBIOS_HANDLE             	MemArrayMappedAddrSmbiosHandle;
    SMBIOS_TABLE_TYPE19           	*Type19Record;

    MemInfoTotalMem = pGblData->MemSize;  // In MB

    if (MemInfoTotalMem == 0) 
    {
    	return EFI_NOT_FOUND;
    }

    TotalMemorySize = (UINT64) LShiftU64 (MemInfoTotalMem, 10);  // MB to KB.

    //
    // 上报Type 19 SMBIOS Record 
    //
    Type19Record = AllocatePool(sizeof(SMBIOS_TABLE_TYPE19) + 1 + 1);
    ZeroMem(Type19Record, sizeof(SMBIOS_TABLE_TYPE19) + 1 + 1);
    Type19Record->Hdr.Type 							= EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS;
    Type19Record->Hdr.Length 						= sizeof(SMBIOS_TABLE_TYPE19);
    Type19Record->Hdr.Handle 						= 0x0;
    Type19Record->StartingAddress					= 0x0;
    Type19Record->EndingAddress						= (UINT32) (TotalMemorySize - 1); // in KB;
    Type19Record->MemoryArrayHandle	 				= MemArraySmbiosHandle;
    //Type19Record->PartitionWidth	 				= (UINT8)(MAX_SOCKET * MAX_CHANNEL * MAX_DIMM);
    Type19Record->PartitionWidth	 				= (UINT8)(mMaxSkt * mMaxCh * mMaxDimm);
    Type19Record->ExtendedStartingAddress 			= 0x0;
    Type19Record->ExtendedEndingAddress 			= 0x0;

    MemArrayMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = mSmbios->Add (mSmbios, NULL, &MemArrayMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *)Type19Record );
    
    FreePool(Type19Record);
    return Status;
}


EFI_STATUS
SmbiosAddType17Table (
  IN pGBL_DATA          pGblData,
  IN UINT8       		Skt, 
  IN UINT8       		Ch,
  IN UINT8       		Dimm,
  IN EFI_SMBIOS_HANDLE  MemArraySmbiosHandle
  )
{
    EFI_STATUS						Status;
    SMBIOS_TABLE_TYPE17           	*Type17Record;
    EFI_SMBIOS_HANDLE             	MemDevSmbiosHandle;
    BOOLEAN                       	DimmPresent;
    UINTN                         	TableSize;

    UINTN                         	StringBufferSize;
    EFI_STRING                    	StringBuffer;
    UINT16                        	MemInfoMemFreq;	
    UINT16                        	MemoryTotalWidth;
    UINT16                        	MemoryDataWidth;
    UINT16                        	MemoryDeviceSize;
    UINT16                        	MemorySpeed;
    UINT8                     		Attributes;
    UINT32                        	MemoryDeviceExtendSize;
    UINT16                        	CfgMemorySpeed;

    CHAR8                         	*OptionalStrStart;
    UINTN                         	DeviceLocatorStrLen;
    UINTN                         	BankLocatorStrLen;
    UINTN                         	ManufactureStrLen;
    UINTN                         	SerialNumberStrLen;
    UINTN                         	AssertTagStrLen;
    UINTN                         	PartNumberStrLen;
    EFI_STRING                    	DeviceLocatorStr;
    EFI_STRING                    	BankLocatorStr;
    EFI_STRING                    	ManufactureStr;
    EFI_STRING                    	SerialNumberStr;
    EFI_STRING                    	AssertTagStr;
    EFI_STRING                    	PartNumberStr;
    EFI_STRING_ID                   Manufacturer;
    EFI_STRING_ID                   SerialNumber;
    EFI_STRING_ID                   AssetTag;
    EFI_STRING_ID                   PartNumber;

    Status              = EFI_SUCCESS;
    Type17Record        = NULL;
    DeviceLocatorStr    = NULL;
    BankLocatorStr      = NULL;
    ManufactureStr      = NULL;
    SerialNumberStr     = NULL;
    AssertTagStr        = NULL;
    PartNumberStr       = NULL;

    MemoryTotalWidth    = 0;
    MemoryDataWidth     = 0;
    MemoryDeviceSize    = 0;
    MemoryDeviceExtendSize = 0;				
    MemorySpeed         = 0;
    Attributes          = 0;
    CfgMemorySpeed      = 0;

    //
    // Allocate Buffers
    //
    StringBufferSize = (sizeof (CHAR16)) * SMBIOS_STRING_MAX_LENGTH;
    StringBuffer = AllocateZeroPool (StringBufferSize);
    if(NULL == StringBuffer)
    {
        return EFI_OUT_OF_RESOURCES;
    }

    StrnCpy(StringBuffer, L"NO DIMM", 8); 
    Manufacturer = HiiSetString (mHiiHandle, 0, StringBuffer, NULL);
    SerialNumber = HiiSetString (mHiiHandle, 0, StringBuffer, NULL);
    AssetTag = HiiSetString (mHiiHandle, 0, StringBuffer, NULL);
    PartNumber = HiiSetString (mHiiHandle, 0, StringBuffer, NULL); 
    

    DimmPresent = TRUE;

    if (pGblData->Channel[Skt][Ch].Status == FALSE || 
          pGblData->Channel[Skt][Ch].Dimm[Dimm].Status == FALSE) 
	{
	    DimmPresent = FALSE;
	}
	
    if(DimmPresent)
	{										
		MemoryDataWidth = pGblData->Channel[Skt][Ch].Dimm[Dimm].PrimaryBusWidth;
        MemoryTotalWidth = MemoryDataWidth + pGblData->Channel[Skt][Ch].Dimm[Dimm].ExtensionBusWidth;
		
		MemoryDeviceSize = pGblData->Channel[Skt][Ch].Dimm[Dimm].DimmSize;  //in MB
		MemoryDeviceExtendSize = 0;
		
		if (MemoryDeviceSize >= 0x7fff) 
		{						
			MemoryDeviceExtendSize = MemoryDeviceSize << 10;  // in MB
			MemoryDeviceSize = 0x7fff;                    // max value
		} 
		
        MemInfoMemFreq = pGblData->Freq;
		MemorySpeed = pGblData->Channel[Skt][Ch].Dimm[Dimm].DimmSpeed;
		Attributes = pGblData->Channel[Skt][Ch].Dimm[Dimm].RankNum;
		CfgMemorySpeed = MemInfoMemFreq;
		
		//
		// Manufacturer
		//
        Status = SmbiosGetManufacturer (pGblData->Channel[Skt][Ch].Dimm[Dimm].SpdMMfgId & 0xFF,
					pGblData->Channel[Skt][Ch].Dimm[Dimm].SpdMMfgId >> 8,
					StringBuffer
					);
        if(EFI_ERROR(Status))
        {
            StrnCpy(StringBuffer, L"Unknown", 8); 
        }
        Manufacturer = HiiSetString (mHiiHandle, 0, StringBuffer, NULL);	

		//
		// SerialNumber
		//
        SmbiosGetSerialNumber(pGblData, Skt, Ch, Dimm, StringBuffer);
        SerialNumber = HiiSetString (mHiiHandle, 0, StringBuffer, NULL);				
		
		//
		// AssetTag
		//
		AssetTag = STRING_TOKEN (STR_MEMORY_SUBCLASS_UNKNOWN);
		
		//
		// PartNumber
		//            
        SmbiosGetPartNumber(pGblData, Skt, Ch, Dimm, StringBuffer);
        PartNumber = HiiSetString (mHiiHandle, 0, StringBuffer, NULL);
	}
	
	//
	// DeviceLocator
	//
    DeviceLocatorStr = AllocateZeroPool (SMBIOS_STRING_MAX_LENGTH);
    if(NULL == DeviceLocatorStr)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto EXIT;
    }
    UnicodeSPrint(DeviceLocatorStr, SMBIOS_STRING_MAX_LENGTH, L"DIMM%x%x%x", Skt, Ch, Dimm);
    DeviceLocatorStrLen = StrLen (DeviceLocatorStr);
    if (DeviceLocatorStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto EXIT;
    }
	
	//
	// BankLocator
	//
    BankLocatorStr = AllocateZeroPool (SMBIOS_STRING_MAX_LENGTH);
    if(NULL == BankLocatorStr)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto EXIT;
    }
    UnicodeSPrint(BankLocatorStr, SMBIOS_STRING_MAX_LENGTH, L"SOCKET %x CHANNEL %x DIMM %x", Skt, Ch, Dimm);
    BankLocatorStrLen = StrLen (BankLocatorStr);
    if (BankLocatorStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto EXIT;
    }
	
	//
	// Manufacture
	//				
    ManufactureStr = HiiGetPackageString (&gEfiCallerIdGuid, Manufacturer, NULL);					
    ManufactureStrLen = StrLen (ManufactureStr);
    if (ManufactureStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto EXIT;
    }
	
	//
	// SerialNumber
	//				
    SerialNumberStr = HiiGetPackageString (&gEfiCallerIdGuid, SerialNumber, NULL);
    SerialNumberStrLen = StrLen (SerialNumberStr);
    if (SerialNumberStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto EXIT;
    }
	
	//
	// AssetTag
	//				
    AssertTagStr = HiiGetPackageString (&gEfiCallerIdGuid, AssetTag, NULL);
    AssertTagStrLen = StrLen (AssertTagStr);
    if (AssertTagStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto EXIT;
    }
	
	//
	// PartNumber
	//				      
    PartNumberStr = HiiGetPackageString (&gEfiCallerIdGuid, PartNumber, NULL);
    PartNumberStrLen = StrLen (PartNumberStr);
    if (PartNumberStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto EXIT;
    }
  
	//
	// 上报Type 17 SMBIOS Record 
	//
    TableSize = sizeof(SMBIOS_TABLE_TYPE17) + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1 + ManufactureStrLen + 1 + SerialNumberStrLen + 1 + AssertTagStrLen + 1 + PartNumberStrLen + 1 + 1;				
    Type17Record = AllocatePool (TableSize);
    if(NULL == Type17Record)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto EXIT;
    }
    ZeroMem (Type17Record, TableSize);
    Type17Record->Hdr.Type 						= EFI_SMBIOS_TYPE_MEMORY_DEVICE;
    Type17Record->Hdr.Length 					= sizeof (SMBIOS_TABLE_TYPE17);
    Type17Record->Hdr.Handle 					= 0;
    Type17Record->MemoryArrayHandle 			= MemArraySmbiosHandle;
    Type17Record->MemoryErrorInformationHandle 	= 0xFFFE;
    Type17Record->TotalWidth 					= MemoryTotalWidth;
    Type17Record->DataWidth 					= MemoryDataWidth;
    Type17Record->Size 							= MemoryDeviceSize;           // in MB
    Type17Record->FormFactor 					= MemoryFormFactorDimm;
    Type17Record->DeviceLocator 				= 1;
    Type17Record->BankLocator 					= 2;
    Type17Record->MemoryType = MemoryTypeDram;    //暂时以0x03替代,没有DDR4 标准
    if (pGblData->Channel[Skt][Ch].Dimm[Dimm].DramType == SPD_TYPE_DDR3) 
    {
        Type17Record->MemoryType = MemoryTypeDdr3;
    }

    Type17Record->TypeDetail.Synchronous 		= 1;
	
    switch (pGblData->Channel[Skt][Ch].Dimm[Dimm].ModuleType) 
	{
		case SPD_UDIMM:
        case SPD_LRDIMM:
		  Type17Record->TypeDetail.Unbuffered = 1;
		  break;
		  
		case SPD_RDIMM:
		  Type17Record->TypeDetail.Registered = 1;
		  break;
		  
		default:
		  Type17Record->TypeDetail.Unknown = 1;
		  break;
	}
	Type17Record->Speed 						= MemorySpeed; // in MHZ
	Type17Record->Manufacturer 					= 3;
	Type17Record->SerialNumber 					= 4;
	Type17Record->AssetTag 						= 5;
	Type17Record->PartNumber 					= 6;
	Type17Record->Attributes 					= Attributes;
	Type17Record->ExtendedSize					= MemoryDeviceExtendSize;
	Type17Record->ConfiguredMemoryClockSpeed	= CfgMemorySpeed;		

	OptionalStrStart = (CHAR8 *) (Type17Record + 1);
	UnicodeStrToAsciiStr (DeviceLocatorStr, OptionalStrStart);
	UnicodeStrToAsciiStr (BankLocatorStr, OptionalStrStart + DeviceLocatorStrLen + 1);
	UnicodeStrToAsciiStr (ManufactureStr, OptionalStrStart + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1);
	UnicodeStrToAsciiStr (SerialNumberStr, OptionalStrStart + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1 + ManufactureStrLen + 1);
	UnicodeStrToAsciiStr (AssertTagStr, OptionalStrStart + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1 + ManufactureStrLen + 1 + SerialNumberStrLen + 1);
	UnicodeStrToAsciiStr (PartNumberStr, OptionalStrStart + DeviceLocatorStrLen + 1 + BankLocatorStrLen + 1 + ManufactureStrLen + 1 + SerialNumberStrLen + 1 + AssertTagStrLen + 1);			
	
	MemDevSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
	Status = mSmbios->Add (mSmbios, NULL, &MemDevSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type17Record);
    
EXIT:
    if(NULL != StringBuffer)
    {
        FreePool (StringBuffer);
    }
    if(NULL != DeviceLocatorStr)
    {
        FreePool (DeviceLocatorStr);
    }
    if(NULL != BankLocatorStr)
    {
        FreePool (BankLocatorStr);
    }
    if(NULL != ManufactureStr)
    {
        FreePool (ManufactureStr);
    }
    if(NULL != SerialNumberStr)
    {
        FreePool (SerialNumberStr);
    }
    if(NULL != AssertTagStr)
    {
        FreePool (AssertTagStr);
    }
    if(NULL != PartNumberStr)
    {
        FreePool (PartNumberStr);
    }
    if(NULL != Type17Record)
    {
        FreePool (Type17Record);
    }

    return Status;
}


/**
  Standard EFI driver point.  This driver locates the MemoryConfigurationData Variable, 
  if it exists, add the related SMBIOS tables by PI SMBIOS protocol.

  @param  ImageHandle     Handle for the image of this driver
  @param  SystemTable     Pointer to the EFI System Table

  @retval  EFI_SUCCESS    The data was successfully stored.

**/                
EFI_STATUS 
EFIAPI
MemorySubClassEntryPoint(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
    EFI_STATUS						Status;
    EFI_SMBIOS_PROTOCOL             *Smbios;
    EFI_HOB_GUID_TYPE             	*GuidHob;
    pGBL_DATA                       pGblData;
    EFI_SMBIOS_HANDLE             	MemArraySmbiosHandle;
    UINT8       				  	Skt, Ch, Dimm;
    
    GuidHob = GetFirstGuidHob(&gEfiMemoryMapGuid);
    if(NULL == GuidHob)
    {
        DEBUG((EFI_D_ERROR, "Could not get MemoryMap Guid hob.  %r\n"));
    	return EFI_NOT_FOUND;
    }
    pGblData = (pGBL_DATA) GET_GUID_HOB_DATA(GuidHob);

    //
    // Locate dependent protocols
    //  
    Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
    if (EFI_ERROR(Status)) 
    {
    	DEBUG((EFI_D_ERROR, "Could not locate SMBIOS protocol.  %r\n", Status));
    	return Status;
    } 
    mSmbios = Smbios;

    //
    // Add our default strings to the HII database. They will be modified later.
    //
    mHiiHandle = HiiAddPackages (
    			&gEfiCallerIdGuid,
    			NULL,
    			MemorySubClassStrings,
    			NULL,
    			NULL
    			);
    if(NULL == mHiiHandle)
    {
        return EFI_OUT_OF_RESOURCES;
    }

    mMaxSkt  = OemGetSocketNumber();
    mMaxCh   = PlatformGetDdrChannel();
    // Get DIMM slot number on Socket 0 Channel 0
    // TODO: Assume all channels have same slot number
    mMaxDimm = OemGetDimmSlot(0, 0);
    
    Status = SmbiosAddType16Table (pGblData, &MemArraySmbiosHandle);
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "Smbios Add Type16 Table Failed.  %r\n", Status));
        return Status;
    }

    Status = SmbiosAddType19Table (pGblData, MemArraySmbiosHandle);
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "Smbios Add Type19 Table Failed.  %r\n", Status));
        return Status;
    } 

    for(Skt = 0; Skt < mMaxSkt; Skt++)
    {        
        for(Ch = 0; Ch < mMaxCh; Ch++)        
    	{
            for(Dimm = 0; Dimm < mMaxDimm; Dimm++)
    		{
                Status = SmbiosAddType17Table (pGblData, Skt, Ch, Dimm, MemArraySmbiosHandle);
                if(EFI_ERROR(Status))
                {
                    DEBUG((EFI_D_ERROR, "Smbios Add Type17 Table Failed.  %r\n", Status));
                }
    		}
        }	
    }     
    
    return Status;
}
