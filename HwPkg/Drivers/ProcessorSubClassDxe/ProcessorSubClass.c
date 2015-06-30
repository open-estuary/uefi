/*-----------------------------------------------------------------------*/
/*!!Warning: Huawei key information asset. No spread without permission. */
/*CODEMARK:RHdz7ygZrZqPlkAy/+I2+ShIzoIqjpQfhsVejzUV8gT3FlKT+KQlBvPfenzCefQUjTM5Ry7W
lMlv1yXCGAx99LeFdvhJqcqoCvs5kRRqWP7Z7nFVWRnE+JluncMrEolmW9l5JzAqS7qgT1mL
ZT309QhYVnr0+E+vn2o4fVowQWmy2YDihUOpqrzdRghiAnxhf9dlFsqpRMnti2LUc027y2yE
R11cBKO0hZsmBFNSiy0YbwicajXoQJh06oRzEmtqdomIpS4dl7iVUDxLTiK8mw==#*/
/*--!!Warning: Deleting or modifying the preceding information is prohibited.--*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ProcessorSubClass.c
  版 本 号   : v1.0
  作    者   : 
  生成日期   : 2014年11月10日
  最近修改   :
  功能描述   : 
  修改历史   :
******************************************************************************/

/**@file
Module Name:

  ProcessorSubClass.c
  
Abstract: 

  This is the driver that locates the ProcessorConfigurationData Variable, if it
  exists, add the related SMBIOS tables by PI SMBIOS protocol.

**/

#include "ProcessorSubClass.h"

EFI_SMBIOS_PROTOCOL             *mSmbios;

UINT16
GetCpuFrequency (
  IN  UINT8 ProcessorNumber
)
{
    return (UINT16)(PlatformGetCpuFreq(ProcessorNumber)/1000/1000);
}

VOID
GetCacheInfo (
  IN  UINT8           ProcessorNumber,
  OUT CACHE_INFO      *CacheInfo
)
{
    CacheInfo[0].InstalledSize      = 48;       //48K
    CacheInfo[0].SystemCacheType    = CacheTypeInstruction;
    CacheInfo[0].Associativity      = CacheAssociativity8Way;

    CacheInfo[1].InstalledSize      = 32;       //32K
    CacheInfo[1].SystemCacheType    = CacheTypeData;
    CacheInfo[1].Associativity      = CacheAssociativity8Way;

    CacheInfo[2].InstalledSize      = 4096;     //4M
    CacheInfo[2].SystemCacheType    = CacheTypeUnified;
    CacheInfo[2].Associativity      = CacheAssociativity8Way;

    CacheInfo[3].InstalledSize      = 16384;    //16M
    CacheInfo[3].SystemCacheType    = CacheTypeUnified;
    CacheInfo[3].Associativity      = CacheAssociativity16Way;

    if (PACKAGE_16CORE != PlatformGetPackageType())  // 32 Core
    {
        CacheInfo[2].InstalledSize  = 8192;     //8M
        CacheInfo[3].InstalledSize  = 32768;    //32M
    }
}

/**
  Add Type 7 SMBIOS Record for Cache Information.
  
  @param[in]    ProcessorNumber     Processor number of specified processor.
  @param[out]   L1CacheHandle       Pointer to the handle of the L1 Cache SMBIOS record.
  @param[out]   L2CacheHandle       Pointer to the handle of the L2 Cache SMBIOS record.
  @param[out]   L3CacheHandle       Pointer to the handle of the L3 Cache SMBIOS record.

**/
EFI_STATUS
AddSmbiosCacheTypeTable (
  IN UINTN              	ProcessorNumber,
  OUT EFI_SMBIOS_HANDLE 	*L1CacheHandle,
  OUT EFI_SMBIOS_HANDLE 	*L2CacheHandle,
  OUT EFI_SMBIOS_HANDLE 	*L3CacheHandle
  )
{
	EFI_STATUS					Status;
	SMBIOS_TABLE_TYPE7			*Type7Record;
	EFI_SMBIOS_HANDLE			SmbiosHandle;
    UINTN                       TableSize;
	UINT8 						CacheLevel;
	CHAR8                       *OptionalStrStart;
	EFI_STRING					CacheSocketStr;
	UINTN				    	CacheSocketStrLen;
	UINTN						StringBufferSize;
	CACHE_SRAM_TYPE_DATA        CacheSramType;
	CACHE_CONFIGURATION			CacheConfig;
	CACHE_INFO					CacheInfo[MAX_CACHE_LEVEL];
	UINT16                      CoreCount;
	UINT32                      TotalSize;

    Status = EFI_SUCCESS;    

    CoreCount = PlatformGetCoreCount();
    TotalSize = 0;
	
	ZeroMem(CacheInfo, sizeof(CACHE_INFO) * MAX_CACHE_LEVEL);
	
	//
	// 获取Cache信息
	//
	GetCacheInfo(ProcessorNumber, CacheInfo);	
	
	for(CacheLevel = 0; CacheLevel < MAX_CACHE_LEVEL; CacheLevel++)
	{
        Type7Record = NULL;
        
        if(CacheInfo[CacheLevel].InstalledSize == 0)
        {
            continue;
        }

        StringBufferSize = sizeof(CHAR16) * SMBIOS_STRING_MAX_LENGTH;
        CacheSocketStr = AllocateZeroPool(StringBufferSize);
        if (CacheSocketStr == NULL) 
        {
            Status = EFI_OUT_OF_RESOURCES;
            goto Exit;
        }
        
        if(CacheLevel == CPU_CACHE_L1_Instruction)
        {
            CacheSocketStrLen = UnicodeSPrint (CacheSocketStr, StringBufferSize, L"L%x Instruction Cache", CacheLevel + 1);
        }
        else if(CacheLevel == CPU_CACHE_L1_Data)
        {
            CacheSocketStrLen = UnicodeSPrint (CacheSocketStr, StringBufferSize, L"L%x Data Cache", CacheLevel);
        }
        else
        {
            CacheSocketStrLen = UnicodeSPrint (CacheSocketStr, StringBufferSize, L"L%x Cache", CacheLevel);
        }
        
        if (CacheSocketStrLen > SMBIOS_STRING_MAX_LENGTH) 
        {
            Status = EFI_UNSUPPORTED;
            goto Exit;
        }

        TableSize = sizeof(SMBIOS_TABLE_TYPE7) + CacheSocketStrLen + 1 + 1;
		Type7Record = AllocatePool (TableSize);
        if (Type7Record == NULL) 
        {
            Status = EFI_OUT_OF_RESOURCES;
            goto Exit;
        }
        ZeroMem(Type7Record, TableSize);        
		
		Type7Record->Hdr.Type				= EFI_SMBIOS_TYPE_CACHE_INFORMATION;
		Type7Record->Hdr.Length				= (UINT8)sizeof(SMBIOS_TABLE_TYPE7);
		Type7Record->Hdr.Handle				= 0;
		Type7Record->SocketDesignation		= 1;
		Type7Record->CacheSpeed				= 0;
		Type7Record->SystemCacheType		= CacheInfo[CacheLevel].SystemCacheType;
		Type7Record->Associativity			= CacheInfo[CacheLevel].Associativity;        
		
		// 配置Cache Config信息
		CacheConfig.Socketed				= 0;	// Not Socketed
		CacheConfig.Reserved1				= 0;	// 
		CacheConfig.Location				= 0;	// Internal  
		CacheConfig.Enable					= 1;	// Enabled	
		CacheConfig.Reserved2				= 0;
        if(CacheLevel == CPU_CACHE_L1_Instruction || CacheLevel == CPU_CACHE_L1_Data)
        {
            CacheConfig.Level               = 0;
        	CacheConfig.OperationalMode		= 1;	// Write Back 
        }
        else
        {
            CacheConfig.Level               = CacheLevel - 1;
        	CacheConfig.OperationalMode		= 2;	// Varies with Memory Address
        }
        //CopyMem(&Type7Record->CacheConfiguration, &CacheConfig, sizeof(CACHE_CONFIGURATION));
        (VOID)memcpy_s(&Type7Record->CacheConfiguration, sizeof(CACHE_CONFIGURATION), &CacheConfig, sizeof(CACHE_CONFIGURATION));
		
		// 配置Cache Size信息
        if(CacheLevel <= 1)
        {
        	TotalSize = (CacheInfo[CacheLevel].InstalledSize) * CoreCount;
        }
        else
        {
        	TotalSize = CacheInfo[CacheLevel].InstalledSize;
        }
		
        if((TotalSize >> 15) == 0)	// 1K granularity 
        {
        	Type7Record->MaximumCacheSize	= (UINT16)TotalSize;
        	Type7Record->InstalledSize		= (UINT16)TotalSize;
        }
        else	// 64K granularity 
        {
        	Type7Record->MaximumCacheSize	= (UINT16)(TotalSize >> 6);
        	Type7Record->InstalledSize		= (UINT16)(TotalSize >> 6);
        	
        	Type7Record->MaximumCacheSize	|= BIT15;
        	Type7Record->InstalledSize		|= BIT15;
        }
        
        // 配置SRAM Type信息
       
        ZeroMem(&CacheSramType, sizeof(CACHE_SRAM_TYPE_DATA));
        CacheSramType.Synchronous = 1;
        //CopyMem(&Type7Record->SupportedSRAMType, &CacheSramType, sizeof(CACHE_SRAM_TYPE_DATA));
        //CopyMem(&Type7Record->CurrentSRAMType, &CacheSramType, sizeof(CACHE_SRAM_TYPE_DATA));
        (VOID)memcpy_s(&Type7Record->SupportedSRAMType, sizeof(CACHE_SRAM_TYPE_DATA), &CacheSramType, sizeof(CACHE_SRAM_TYPE_DATA));
        (VOID)memcpy_s(&Type7Record->CurrentSRAMType, sizeof(CACHE_SRAM_TYPE_DATA), &CacheSramType, sizeof(CACHE_SRAM_TYPE_DATA));
        
        // 配置Error Correction Type信息
        if(CacheLevel == CPU_CACHE_L1_Instruction)
        {
            Type7Record->ErrorCorrectionType	= CacheErrorParity;
        }
        else
        {
            Type7Record->ErrorCorrectionType	= CacheErrorSingleBit;
        }

        OptionalStrStart = (CHAR8 *) (Type7Record + 1);
        UnicodeStrToAsciiStr (CacheSocketStr, OptionalStrStart);

        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Status = mSmbios->Add (mSmbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) Type7Record);        
        if (EFI_ERROR (Status)) 
        {
            goto Exit;
        }
		
		// 配置L1/L2/L3 Cache Handle
        switch(CacheLevel)
        {
        	case CPU_CACHE_L1_Instruction:
            case CPU_CACHE_L1_Data:
        		*L1CacheHandle = SmbiosHandle;
        		break;
        	case CPU_CACHE_L2:
        		*L2CacheHandle = SmbiosHandle;
        		break;
        	case CPU_CACHE_L3:
        		*L3CacheHandle = SmbiosHandle;
        		break;
        	default :
        		break;
        }    
Exit:    
        if(Type7Record != NULL)
        {
            FreePool (Type7Record);
        }	
        if(CacheSocketStr != NULL)
        {
            FreePool (CacheSocketStr);
        }	
	}
    
    return Status;
}

/**
  Add Type 4 SMBIOS Record for Processor Information.
  
  @param[in]    ProcessorNumber     Processor number of specified processor.

**/
EFI_STATUS
AddSmbiosProcessorTypeTable (
  IN UINTN              	ProcessorNumber
  )
{
	EFI_STATUS					Status;
	SMBIOS_TABLE_TYPE4			*Type4Record;
	EFI_SMBIOS_HANDLE			SmbiosHandle;
    EFI_SMBIOS_HANDLE     		L1CacheHandle;
	EFI_SMBIOS_HANDLE     		L2CacheHandle;
	EFI_SMBIOS_HANDLE     		L3CacheHandle;
    
	CHAR8                       *OptionalStrStart; 
    EFI_STRING_ID            	ProcessorManu;
    EFI_STRING_ID            	ProcessorVersion;
    EFI_STRING_ID            	SerialNumber;
    EFI_STRING_ID            	AssetTag;
    EFI_STRING_ID            	PartNumber;
	EFI_STRING					ProcessorSocketStr;
	EFI_STRING					ProcessorManuStr;
	EFI_STRING					ProcessorVersionStr;
	EFI_STRING					SerialNumberStr;
	EFI_STRING					AssetTagStr;
	EFI_STRING					PartNumberStr;
	UINTN				    	ProcessorSocketStrLen;
	UINTN				    	ProcessorManuStrLen;
	UINTN				    	ProcessorVersionStrLen;
	UINTN				    	SerialNumberStrLen;
    UINTN				    	AssetTagStrLen;
	UINTN				    	PartNumberStrLen;
	UINTN						StringBufferSize;
	UINTN                       TotalSize;
    
	UINT8						Voltage;
    UINT8                       ProcessorUpgrade;
	UINT16						CoreCount;
	UINT16						CoreEnabled;
	UINT16						ThreadCount;
    UINT16						ExternalClock;
    UINT16						MaxSpeed;
    UINT16						CurrentSpeed;
    PROCESSOR_STATUS_DATA     	ProcessorStatus;
	PROCESSOR_CHARACTERISTICS_DATA	ProcessorCharacteristics;

    Type4Record         = NULL;
    ProcessorSocketStr  = NULL;
	ProcessorManuStr    = NULL;
	ProcessorVersionStr = NULL;
	SerialNumberStr     = NULL;
	AssetTagStr         = NULL;
	PartNumberStr       = NULL;

    if(OemIsSocketPresent(ProcessorNumber))  //CPU 在位
    {
        Voltage         = BIT7 | 9;          // 0.9V
        
        // 首先获取Cache信息
    	Status = AddSmbiosCacheTypeTable (ProcessorNumber, &L1CacheHandle, &L2CacheHandle, &L3CacheHandle);
        if(EFI_ERROR(Status))
        {
            return Status;
        }

        // 获取当前CPU的频率
        ExternalClock       = EXTERNAL_CLOCK;    // 50MHz
        MaxSpeed            = CPU_MAX_SPEED;     // 2.1GHz
        CurrentSpeed        = GetCpuFrequency(ProcessorNumber);  

        ProcessorUpgrade    = ProcessorUpgradeSocketFS1;

        CoreCount           = PlatformGetCoreCount();
        CoreEnabled         = CoreCount;
        ThreadCount         = CoreCount;

    	ProcessorManu       = STRING_TOKEN (STR_PROCESSOR_MANUFACTURE);    	
    	ProcessorVersion    = STRING_TOKEN (STR_PROCESSOR_VERSION);
    	SerialNumber        = STRING_TOKEN (STR_PROCESSOR_SERIAL_NUMBER);
    	AssetTag            = STRING_TOKEN (STR_PROCESSOR_ASSET_TAG);
    	PartNumber          = STRING_TOKEN (STR_PROCESSOR_PART_NUMBER);    	
    }
    else
    {        
        CoreCount           = 0;
    	CoreEnabled         = 0;
    	ThreadCount         = 0;
    	Voltage             = 0;
        ExternalClock       = 0;
        MaxSpeed            = 0;
        CurrentSpeed        = 0;
    	L1CacheHandle       = 0xFFFF;
    	L2CacheHandle       = 0xFFFF;
    	L3CacheHandle       = 0xFFFF;
        ProcessorUpgrade    = ProcessorUpgradeUnknown;

        ProcessorManu       = STRING_TOKEN (STR_PROCESSOR_UNKNOWN);
        ProcessorVersion    = STRING_TOKEN (STR_PROCESSOR_UNKNOWN);
        SerialNumber        = STRING_TOKEN (STR_PROCESSOR_UNKNOWN);
        AssetTag            = STRING_TOKEN (STR_PROCESSOR_UNKNOWN);
        PartNumber          = STRING_TOKEN (STR_PROCESSOR_UNKNOWN);
    }	
    
	// Processor Socket Designation
    StringBufferSize = sizeof(CHAR16) * SMBIOS_STRING_MAX_LENGTH;
    ProcessorSocketStr = AllocateZeroPool(StringBufferSize);
    if (ProcessorSocketStr == NULL) 
    {        
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
    }
    
    ProcessorSocketStrLen = UnicodeSPrint (ProcessorSocketStr, StringBufferSize, L"SOCKET %x", ProcessorNumber);
    if (ProcessorSocketStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }

    // Processor Manufacture
    ProcessorManuStr = HiiGetPackageString (&gEfiCallerIdGuid, ProcessorManu, NULL);
	ProcessorManuStrLen = StrLen (ProcessorManuStr);
    if (ProcessorManuStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }

    // Processor Version
    ProcessorVersionStr = HiiGetPackageString (&gEfiCallerIdGuid, ProcessorVersion, NULL);
	ProcessorVersionStrLen = StrLen (ProcessorVersionStr);
    if (ProcessorVersionStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }

    // Serial Number
    SerialNumberStr = HiiGetPackageString (&gEfiCallerIdGuid, SerialNumber, NULL);
	SerialNumberStrLen = StrLen (SerialNumberStr);
    if (SerialNumberStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }
	
	// Asset Tag
    AssetTagStr = HiiGetPackageString (&gEfiCallerIdGuid, AssetTag, NULL);
    PartNumberStrLen = StrLen (AssetTagStr);
    if (PartNumberStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }
	
	// Part Number
    PartNumberStr = HiiGetPackageString (&gEfiCallerIdGuid, PartNumber, NULL);
    AssetTagStrLen = StrLen (PartNumberStr);
    if (AssetTagStrLen > SMBIOS_STRING_MAX_LENGTH) 
    {
        Status = EFI_UNSUPPORTED;
        goto Exit;
    }    
	
    TotalSize = sizeof (SMBIOS_TABLE_TYPE4) + ProcessorSocketStrLen + 1 + ProcessorManuStrLen + 1 + ProcessorVersionStrLen + 1 + SerialNumberStrLen + 1 + AssetTagStrLen + 1 + PartNumberStrLen + 1 + 1;
    Type4Record = AllocatePool (TotalSize);
    if (Type4Record == NULL) 
    {        
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
    }
    ZeroMem (Type4Record, TotalSize);    

	Type4Record->Hdr.Type 					= EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
	Type4Record->Hdr.Length					= (UINT8) sizeof (SMBIOS_TABLE_TYPE4);
	Type4Record->Hdr.Handle 				= 0;
	Type4Record->Socket						= 1;
	Type4Record->ProcessorType				= CentralProcessor;
	Type4Record->ProcessorFamily			= ProcessorFamilyOther;
	Type4Record->ProcessorManufacture		= 2;
	//Type4Record->ProcessorId				= 0x0;  // Need update
	Type4Record->ProcessorVersion			= 3;
	*(UINT8 *) &Type4Record->Voltage        = Voltage;
	Type4Record->ExternalClock				= ExternalClock;    
    Type4Record->MaxSpeed                   = MaxSpeed;         
	Type4Record->CurrentSpeed				= CurrentSpeed;
	Type4Record->ProcessorUpgrade			= ProcessorUpgrade;
	Type4Record->L1CacheHandle				= L1CacheHandle; 
	Type4Record->L2CacheHandle				= L2CacheHandle;
	Type4Record->L3CacheHandle				= L3CacheHandle;
	Type4Record->SerialNumber				= 4;
	Type4Record->AssetTag					= 5;
	Type4Record->PartNumber					= 6;
    Type4Record->CoreCount				    = CoreCount;
    Type4Record->EnabledCoreCount		    = CoreEnabled;
    Type4Record->ThreadCount			    = ThreadCount;	
    Type4Record->ProcessorFamily2			= ProcessorFamilyOther;   
   
    if(OemIsSocketPresent(ProcessorNumber))
    {
        // Processor Status
    	ProcessorStatus.CpuStatus 				= 1;        // CPU Enabled
        ProcessorStatus.Reserved1 				= 0;
        ProcessorStatus.SocketPopulated 		= 1;  		// CPU Socket Populated
        ProcessorStatus.Reserved2 				= 0;
    	//CopyMem (&Type4Record->Status, &ProcessorStatus, sizeof(PROCESSOR_STATUS_DATA));
    	(VOID)memcpy_s(&Type4Record->Status, sizeof(PROCESSOR_STATUS_DATA), &ProcessorStatus, sizeof(PROCESSOR_STATUS_DATA));

        // Processor Characteristics
        ProcessorCharacteristics.Reserved 		= 0;
    	ProcessorCharacteristics.Capable64Bit 	= 1;        // 64-bit Capable
    	ProcessorCharacteristics.Unknown 		= 0;
    	ProcessorCharacteristics.EnhancedVirtualization 	= 1;
    	ProcessorCharacteristics.HardwareThread = 0;
    	ProcessorCharacteristics.MultiCore 		= 1;  
    	ProcessorCharacteristics.ExecuteProtection 			= 1;
    	ProcessorCharacteristics.PowerPerformanceControl 	= 1;
    	ProcessorCharacteristics.Reserved2 		= 0;
    	//CopyMem (&Type4Record->ProcessorCharacteristics, &ProcessorCharacteristics, sizeof(PROCESSOR_CHARACTERISTICS_DATA));
        (VOID)memcpy_s(&Type4Record->ProcessorCharacteristics, sizeof(PROCESSOR_CHARACTERISTICS_DATA), &ProcessorCharacteristics, sizeof(PROCESSOR_CHARACTERISTICS_DATA));
    }
	
    OptionalStrStart = (CHAR8 *) (Type4Record + 1);
    UnicodeStrToAsciiStr (ProcessorSocketStr, OptionalStrStart);
    UnicodeStrToAsciiStr (ProcessorManuStr, OptionalStrStart + ProcessorSocketStrLen + 1);
    UnicodeStrToAsciiStr (ProcessorVersionStr, OptionalStrStart + ProcessorSocketStrLen + 1 + ProcessorManuStrLen + 1);
    UnicodeStrToAsciiStr (SerialNumberStr, OptionalStrStart + ProcessorSocketStrLen + 1 + ProcessorManuStrLen + 1 + ProcessorVersionStrLen + 1);
    UnicodeStrToAsciiStr (AssetTagStr, OptionalStrStart + ProcessorSocketStrLen + 1 + ProcessorManuStrLen + 1 + ProcessorVersionStrLen + 1 + SerialNumberStrLen + 1);
    UnicodeStrToAsciiStr (PartNumberStr, OptionalStrStart + ProcessorSocketStrLen + 1 + ProcessorManuStrLen + 1 + ProcessorVersionStrLen + 1 + SerialNumberStrLen + 1 + AssetTagStrLen + 1);
    
	// 上报Type4 信息
	SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
	Status = mSmbios->Add (mSmbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) Type4Record);
    if (EFI_ERROR (Status)) 
    {
        goto Exit;
    }
    
Exit:    
    if(Type4Record != NULL)
    {
        FreePool (Type4Record);
    }
    if(ProcessorSocketStr != NULL)
    {
        FreePool (ProcessorSocketStr);
    }
    if(ProcessorManuStr != NULL)
    {
        FreePool (ProcessorManuStr);
    }
    if(ProcessorVersionStr != NULL)
    {
        FreePool (ProcessorVersionStr);
    }
    if(SerialNumberStr != NULL)
    {
        FreePool (SerialNumberStr);
    }
    if(AssetTagStr != NULL)
    {
        FreePool (AssetTagStr);
    }
    if(PartNumberStr != NULL)
    {
        FreePool (PartNumberStr);
    }

    return Status;	
}

/**
  Standard EFI driver point.  This driver locates the ProcessorConfigurationData Variable, 
  if it exists, add the related SMBIOS tables by PI SMBIOS protocol.

  @param  ImageHandle     Handle for the image of this driver
  @param  SystemTable     Pointer to the EFI System Table

  @retval  EFI_SUCCESS    The data was successfully stored.

**/                
EFI_STATUS 
EFIAPI
ProcessorSubClassEntryPoint(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
	EFI_STATUS						Status; 
    EFI_HII_HANDLE                  HiiHandle;
	UINT32 							SocketIndex;
		
	//
	// Locate dependent protocols
	//  
	Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&mSmbios);
	if (EFI_ERROR(Status)) 
	{
		DEBUG((EFI_D_ERROR, "Could not locate SMBIOS protocol.  %r\n", Status));
		return Status;
	} 

    //
    // Add our default strings to the HII database. They will be modified later.
    //
    HiiHandle = HiiAddPackages (
                &gEfiCallerIdGuid,
                NULL,
                ProcessorSubClassStrings,
                NULL,
                NULL
                );
    if (HiiHandle == NULL) 
    {        
        return EFI_OUT_OF_RESOURCES;
    }

	//
	// Add SMBIOS tables for populated sockets.
	//
    for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) 
    { 
        if((SocketIndex == 1) && !PcdGet32(PcdIsMPBoot))
        {
            break;
        }
        Status = AddSmbiosProcessorTypeTable (SocketIndex);	        
        if(EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "Add Processor Type Table Failed!  %r.\n", Status));
            return Status;
        }
    }
	  
    return Status;
}
