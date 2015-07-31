/*-----------------------------------------------------------------------*/
/*!!Warning: Huawei key information asset. No spread without permission. */
/*CODEMARK:EG4uRhTwMmgcVFBsBnYHCDadN5jJKSuVyxmmaCmKFU6eJEbB2fyHF9weu4/jer/hxLHb+S1e
E0zVg4C3NiZh4WIPvj4+Pmfbh+YGahSH5JdS/1MeXWIN/Su+lyHPU8Ju8/XPgqOK/1CKOV13
RWYRb5Ee+3GEtRz6aB8O6FmfEz2rZSQTYYqV6zkp7JA23HAqjx5FHCTB4bJ35zOtwanDulV6
3GiooklnefLZbUwz+FAwxl7hKIyVseoJjRwQQ5aAmdGpqW07slMMZAJHgPekeA==#*/
/*--!!Warning: Deleting or modifying the preceding information is prohibited.--*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <libfdt.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FdtUpdateLib.h>
#include <Protocol/OemNicProtocol.h>
#include <Library/MemoryAllocationLib.h>


CHAR8  *EthName[8]=
{
 "ethernet@0","ethernet@1",
 "ethernet@2","ethernet@3",
 "ethernet@4","ethernet@5",
 "ethernet@6","ethernet@7"
};

CHAR8  *MacName[4]=
{
 "ethernet-mac@c7040000",
 "ethernet-mac@c7044000",
 "ethernet-mac@c7048000",
 "ethernet-mac@c704c000"
};

STATIC
BOOLEAN
IsMemMapRegion (
  IN EFI_MEMORY_TYPE MemoryType
  )
{
  switch(MemoryType)
    {
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
        case EfiConventionalMemory:
        case EfiACPIReclaimMemory:
        case EfiACPIMemoryNVS:
        case EfiLoaderCode:
        case EfiLoaderData: 
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiPalCode:
                return TRUE;
        default:
                return FALSE;
  }
}

EFI_STATUS
GetMacAddress (UINT32 Port)
{
    EFI_MAC_ADDRESS Mac;
    EFI_STATUS Status;
    OEM_NIC_PROTOCOL *OemNic = NULL;

    Status = gBS->LocateProtocol(&gOemNicProtocolGuid, NULL, (VOID **)&OemNic);
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] LocateProtocol failed %r\n", __FUNCTION__, __LINE__, Status));
        return Status;
    }

    Status = OemNic->GetMac(&Mac, Port);
    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "[%a]:[%dL] GetMac failed %r\n", __FUNCTION__, __LINE__, Status));
        return Status;
    }
  
    gMacAddress[0].data0=Mac.Addr[0];
    gMacAddress[0].data1=Mac.Addr[1];
    gMacAddress[0].data2=Mac.Addr[2];
    gMacAddress[0].data3=Mac.Addr[3];
    gMacAddress[0].data4=Mac.Addr[4];
    gMacAddress[0].data5=Mac.Addr[5];
    DEBUG((EFI_D_ERROR, "Port%d:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
        Port,gMacAddress[0].data0,gMacAddress[0].data1,gMacAddress[0].data2,
        gMacAddress[0].data3,gMacAddress[0].data4,gMacAddress[0].data5));
   
    return EFI_SUCCESS;
}

STATIC
EFI_STATUS
DelPhyhandleUpdateMacAddress(IN VOID* Fdt)
{
    UINT8               port;
    INTN                ethernetnode;
    INTN			    node;
    INTN                Error;
    struct              fdt_property *m_prop;
    int                 m_oldlen;
    EFI_STATUS          Status = EFI_SUCCESS;
    
    node = fdt_subnode_offset(Fdt, 0, "soc");
    if (node < 0) 
    {
        DEBUG ((EFI_D_ERROR, "can not find soc root node\n"));
        return EFI_INVALID_PARAMETER;
    }
    else
    {
        for( port=0; port<8; port++ )
        {
            (VOID) GetMacAddress(port);
            ethernetnode=fdt_subnode_offset(Fdt, node,EthName[port]);
            if (node < 0) 
            {
                DEBUG ((EFI_D_ERROR, "can not find ethernet@1 node\n"));
            }
            m_prop = fdt_get_property_w(Fdt, ethernetnode, "local-mac-address", &m_oldlen);
            if(m_prop)
            {
                Error = fdt_delprop(Fdt, ethernetnode, "local-mac-address");
                if (Error) 
                {
                    DEBUG ((EFI_D_ERROR, "ERROR:fdt_delprop() Local-mac-address: %a\n", fdt_strerror (Error)));
                    Status = EFI_INVALID_PARAMETER;
                } 
                Error = fdt_setprop(Fdt, ethernetnode, "local-mac-address",gMacAddress,sizeof(MAC_ADDRESS));
                if (Error) 
                {
                    DEBUG ((EFI_D_ERROR, "ERROR:fdt_setprop():local-mac-address %a\n", fdt_strerror (Error)));
                    Status = EFI_INVALID_PARAMETER;
                } 
            }
        }
    }
    return Status;
}

EFI_STATUS UpdateMemoryNode(VOID* Fdt)
{
    INTN                Error = 0;
    EFI_STATUS          Status = EFI_SUCCESS;
    UINT32              Index = 0;
    UINT32              MemIndex;
    INTN			    node;
    struct              fdt_property *m_prop;
    int                 m_oldlen;
    EFI_MEMORY_DESCRIPTOR *MemoryMap;
    EFI_MEMORY_DESCRIPTOR *MemoryMapPtr;
    EFI_MEMORY_DESCRIPTOR *MemoryMapPtrCurrent;
    UINTN                 MemoryMapSize;
    UINTN                 Pages0 = 0;
    UINTN                 Pages1 = 0;
    UINTN                 MapKey;
    UINTN                 DescriptorSize;
    UINT32                DescriptorVersion;
    PHY_MEM_REGION        *mRegion;
    UINTN                 MemoryMapLastEndAddress ;
    UINTN                 MemoryMapcontinuousStartAddress ;
    UINTN                 MemoryMapCurrentStartAddress;
    BOOLEAN               FindMemoryRegionFlag = FALSE;
    node = fdt_subnode_offset(Fdt, 0, "memory");
    if (node < 0) 
    {
        // Create the memory node
        node = fdt_add_subnode(Fdt, 0, "memory");
        if(node < 0)
        {
          DEBUG((EFI_D_INFO, "[%a]:[%dL] fdt add subnode error\n", __FUNCTION__, __LINE__));
        }
    }
    //find the memory node property  
    m_prop = fdt_get_property_w(Fdt, node, "memory", &m_oldlen);
    if(m_prop)
        Error=fdt_delprop(Fdt, node, "reg");
    if (Error) 
    {
        DEBUG ((EFI_D_ERROR, "ERROR:fdt_delprop(): %a\n", fdt_strerror (Error)));
        Status = EFI_INVALID_PARAMETER;
        return Status;
    } 

    MemoryMap = NULL;
    MemoryMapSize = 0;
    MemIndex = 0;
 
    Status = gBS->GetMemoryMap (&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        // The UEFI specification advises to allocate more memory for the MemoryMap buffer between successive
        // calls to GetMemoryMap(), since allocation of the new buffer may potentially increase memory map size.
        DEBUG ((EFI_D_ERROR, "MemoryMapsize: 0x%lx\n",MemoryMapSize));
        Pages0 = EFI_SIZE_TO_PAGES (MemoryMapSize) + 1;
        MemoryMap = AllocatePages (Pages0);
        if (MemoryMap == NULL)
        {
            Status = EFI_OUT_OF_RESOURCES;
            return Status;
        }
        Status = gBS->GetMemoryMap (&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    }

    if(MemoryMap == NULL)
    {
        Status = EFI_OUT_OF_RESOURCES;
        //goto EXIT;
        return Status;
    }

    mRegion = NULL;
    Pages1 = EFI_SIZE_TO_PAGES (sizeof(PHY_MEM_REGION) *( MemoryMapSize / DescriptorSize));
    mRegion = (PHY_MEM_REGION*)AllocatePages(Pages1);
    if (mRegion == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      return Status;
    }
	
    if (!EFI_ERROR(Status))
    {
        MemoryMapPtr = MemoryMap;
        MemoryMapPtrCurrent = MemoryMapPtr;
        MemoryMapLastEndAddress = 0;
        MemoryMapcontinuousStartAddress = 0;
        MemoryMapCurrentStartAddress = 0;
        for ( Index = 0; Index < (MemoryMapSize / DescriptorSize); Index++)
        {
            MemoryMapPtrCurrent = (EFI_MEMORY_DESCRIPTOR*)((UINTN)MemoryMapPtr + Index*DescriptorSize);
            MemoryMapCurrentStartAddress = (UINTN)MemoryMapPtrCurrent->PhysicalStart;
       
            if (!IsMemMapRegion ((EFI_MEMORY_TYPE)MemoryMapPtrCurrent->Type))
            {
                continue;
            }
            else 
            {
                FindMemoryRegionFlag = TRUE;
                if(MemoryMapCurrentStartAddress != MemoryMapLastEndAddress)
                {
                    mRegion[MemIndex].BaseHigh= cpu_to_fdt32(MemoryMapcontinuousStartAddress>>32);
	                mRegion[MemIndex].BaseLow=cpu_to_fdt32(MemoryMapcontinuousStartAddress);
                    mRegion[MemIndex].LengthHigh= cpu_to_fdt32((MemoryMapLastEndAddress-MemoryMapcontinuousStartAddress)>>32);
	                mRegion[MemIndex].LengthLow=cpu_to_fdt32(MemoryMapLastEndAddress-MemoryMapcontinuousStartAddress);
                    MemIndex+=1;
                    MemoryMapcontinuousStartAddress=MemoryMapCurrentStartAddress;
                }
            }
            MemoryMapLastEndAddress = (UINTN)(MemoryMapPtrCurrent->PhysicalStart + MemoryMapPtrCurrent->NumberOfPages * EFI_PAGE_SIZE);
        }
        if (FindMemoryRegionFlag)
        {
            mRegion[MemIndex].BaseHigh = cpu_to_fdt32(MemoryMapcontinuousStartAddress>>32);
            mRegion[MemIndex].BaseLow = cpu_to_fdt32(MemoryMapcontinuousStartAddress);
            mRegion[MemIndex].LengthHigh = cpu_to_fdt32((MemoryMapLastEndAddress-MemoryMapcontinuousStartAddress)>>32);
	        mRegion[MemIndex].LengthLow = cpu_to_fdt32(MemoryMapLastEndAddress-MemoryMapcontinuousStartAddress);
        }
    } 
    Error = fdt_setprop(Fdt, node, "reg",mRegion,sizeof(PHY_MEM_REGION) *(MemIndex+1));
    FreePages (mRegion, Pages1);
    FreePages (MemoryMap, Pages0);
    if (Error)
    {
        DEBUG ((EFI_D_ERROR, "ERROR:fdt_setprop(): %a\n", fdt_strerror (Error)));
        Status = EFI_INVALID_PARAMETER;
        return Status;
    }
  return Status;
}

/*
 * Entry point for fdtupdate lib.
 */

EFI_STATUS EFIFdtUpdate(UINTN FdtFileAddr)
{
    INTN                Error;
    VOID*               Fdt;      
    UINT32              Size;     
    UINTN               NewFdtBlobSize;   
    UINTN               NewFdtBlobBase; 
    EFI_STATUS          Status = EFI_SUCCESS;
    UINT32              Index = 0;

    Error = fdt_check_header ((VOID*)(UINTN)(FdtFileAddr));
    if (Error != 0) 
    {
        DEBUG ((EFI_D_ERROR,"ERROR: Device Tree header not valid (%a)\n", fdt_strerror(Error)));
        return EFI_INVALID_PARAMETER;
    }   

    Size = (UINTN)fdt_totalsize ((VOID*)(UINTN)(FdtFileAddr));
    NewFdtBlobSize = Size + ADD_FILE_LENGTH;
    Fdt = (VOID*)(UINTN)FdtFileAddr;

    Status = gBS->AllocatePages (AllocateAnyPages, EfiBootServicesData, EFI_SIZE_TO_PAGES(NewFdtBlobSize), &NewFdtBlobBase);
    if (EFI_ERROR (Status)) 
    {
        return EFI_OUT_OF_RESOURCES;
    }


    Error = fdt_open_into(Fdt,(VOID*)(UINTN)(NewFdtBlobBase), (NewFdtBlobSize));
    if (Error) {
        DEBUG ((EFI_D_ERROR, "ERROR:fdt_open_into(): %a\n", fdt_strerror (Error)));
        Status = EFI_INVALID_PARAMETER;
        goto EXIT;
    }

    Fdt = (VOID*)(UINTN)NewFdtBlobBase;   

    for(Index = 0; gExtMemRegion[Index].LengthHigh || gExtMemRegion[Index].LengLow; Index++)
    {
        gExtMemRegion[Index].BaseHigh = cpu_to_fdt32 (gExtMemRegion[Index].BaseHigh);        
        gExtMemRegion[Index].BaseLow = cpu_to_fdt32 (gExtMemRegion[Index].BaseLow);
        gExtMemRegion[Index].LengthHigh= cpu_to_fdt32 (gExtMemRegion[Index].LengthHigh);
        gExtMemRegion[Index].LengLow = cpu_to_fdt32 (gExtMemRegion[Index].LengLow);
        gExtMemRegion[Index].Type= cpu_to_fdt32 (gExtMemRegion[Index].Type);
    }

    Error = fdt_setprop(Fdt, 0, "mem-extends", &gExtMemRegion, Index * sizeof(gExtMemRegion[0]));
    if (Error) 
    {
        DEBUG ((EFI_D_ERROR, "ERROR:fdt_setprop(): %a\n", fdt_strerror (Error)));
        Status = EFI_INVALID_PARAMETER;
        goto EXIT;
    }     

    Status = DelPhyhandleUpdateMacAddress(Fdt);
    if (EFI_ERROR (Status))
    {
        DEBUG ((EFI_D_ERROR, "DelPhyhandleUpdateMacAddress fail:\n"));
        Status = EFI_SUCCESS;
    }
    
    Status = UpdateMemoryNode(Fdt);
    if (EFI_ERROR (Status)) 
    {
        goto EXIT;
    }    
    
    gBS->CopyMem(((VOID*)(UINTN)(FdtFileAddr)),((VOID*)(UINTN)(NewFdtBlobBase)),NewFdtBlobSize);
    
EXIT:    
    gBS->FreePages(NewFdtBlobBase,EFI_SIZE_TO_PAGES(NewFdtBlobSize));

    return Status;

    
    
}
