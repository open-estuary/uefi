/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Bds\CustomizedLoader.c


Description: 

*************************************************/

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Guid/Fdt.h>

#include <PlatformArch.h>
#include <Library/PlatformSysCtrlLib.h>
#include <Library/TimerLib.h>
#include <Library/OemAddressMapLib.h>
#include <Library/FdtUpdateLib.h>
// We move asm_read_reg from ArmLib to HwArmLib
#include <Library/HwArmLib.h>

#include <Library/LzmaCustomDecompressLib/LzmaDecompressLibInternal.h>

typedef VOID (*ESL_LINUX)(UINTN ParametersBase, UINTN Reserved0,
                          UINTN Reserved1, UINTN Reserved2);


EFI_STATUS
ShutdownUefiBootServices (
  VOID
  );

STATIC
EFI_STATUS
PreparePlatformHardware (
  VOID
  )
{
    //Note: Interrupts will be disabled by the GIC driver when ExitBootServices() will be called.

    // Clean before Disable else the Stack gets corrupted with old data.
    ArmCleanDataCache ();

    LlcCleanInvalidate ();

    ArmDisableDataCache ();
    
    // Invalidate all the entries that might have snuck in.
    ArmInvalidateDataCache ();

    // Disable and invalidate the instruction cache
    ArmDisableInstructionCache ();
    ArmInvalidateInstructionCache ();

    // Turn off MMU
    ArmDisableMmu();

    return EFI_SUCCESS;
}

VOID
ESL_Start_OS (
  )
{
    EFI_STATUS Status;
    UINTN     Reg_Value;
    ESL_LINUX LinuxKernel = (ESL_LINUX)(0x80000); 

    if(!PcdGet32(PcdIsMPBoot))
    {
        DEBUG((EFI_D_ERROR,"Update FDT\n"));
        Status = EFIFdtUpdate(0x06000000);
        if(EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR,"EFIFdtUpdate ERROR\n"));
            goto Exit;
        }
    }
    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Start to boot Linux\n", __FUNCTION__, __LINE__));

    SmmuConfigForLinux();

    ITSCONFIG();

    if(PcdGet32(PcdIsMPBoot))
    {
        *(volatile UINT32 *)(0x60016220)   = 0x7;
        *(volatile UINT32 *)(0x60016230)   = 0x40016260;
        *(volatile UINT32 *)(0x60016234)   = 0X0;
        *(volatile UINT32 *)(0x60016238)   = 0x60016260;
        *(volatile UINT32 *)(0x6001623C)   = 0x400;
        *(volatile UINT32 *)(0x60016240)   = 0x40016260;
        *(volatile UINT32 *)(0x60016244)   = 0x400;

        *(volatile UINT32 *)(0x40016220)   = 0x7;
        *(volatile UINT32 *)(0x40016230)   = 0x60016260;
        *(volatile UINT32 *)(0x40016234)   = 0X0;
        *(volatile UINT32 *)(0x40016238)   = 0x60016260;
        *(volatile UINT32 *)(0x4001623C)   = 0x400;
        *(volatile UINT32 *)(0x40016240)   = 0x40016260;
        *(volatile UINT32 *)(0x40016244)   = 0x400;

        *(volatile UINT32 *)(0x60016220 + S1_BASE)   = 0x7;
        *(volatile UINT32 *)(0x60016230 + S1_BASE)   = 0x40016260;
        *(volatile UINT32 *)(0x60016234 + S1_BASE)   = 0X0;
        *(volatile UINT32 *)(0x60016238 + S1_BASE)   = 0x60016260;
        *(volatile UINT32 *)(0x6001623C + S1_BASE)   = 0x0;
        *(volatile UINT32 *)(0x60016240 + S1_BASE)   = 0x40016260;
        *(volatile UINT32 *)(0x60016244 + S1_BASE)   = 0x400;

        *(volatile UINT32 *)(0x40016220 + S1_BASE)   = 0x7;
        *(volatile UINT32 *)(0x40016230 + S1_BASE)   = 0x60016260;
        *(volatile UINT32 *)(0x40016234 + S1_BASE)   = 0X0;
        *(volatile UINT32 *)(0x40016238 + S1_BASE)   = 0x60016260;
        *(volatile UINT32 *)(0x4001623C + S1_BASE)   = 0x400;
        *(volatile UINT32 *)(0x40016240 + S1_BASE)   = 0x40016260;
        *(volatile UINT32 *)(0x40016244 + S1_BASE)   = 0x0;
    }
    
    Status = ShutdownUefiBootServices ();
    if(EFI_ERROR(Status)) 
    {
        DEBUG((EFI_D_ERROR,"ERROR: Can not shutdown UEFI boot services. Status=0x%X\n", Status));
        goto Exit;
    }

    //
    // Switch off interrupts, caches, mmu, etc
    //
    Status = PreparePlatformHardware ();
    ASSERT_EFI_ERROR(Status);

    *(volatile UINT32*)0xFFF8 = 0x0;
    *(volatile UINT32*)0xFFFC = 0x0;
    asm("DSB SY");
    asm("ISB");

    if (!PcdGet64 (PcdTrustedFirmwareEnable))
    {
        StartupAp();
    }

    Reg_Value = asm_read_reg();

    DEBUG((EFI_D_ERROR,"CPUECTLR_EL1 = 0x%llx\n",Reg_Value));

    MN_CONFIG ();

    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Start to jump Linux kernel\n", __FUNCTION__, __LINE__));

    LinuxKernel (0x06000000,0,0,0);
    
    // Kernel should never exit
    // After Life services are not provided
    ASSERT(FALSE);

Exit:
    // Only be here if we fail to start Linux
    Print (L"ERROR  : Can not start the kernel. Status=0x%X\n", Status);

    // Free Runtimee Memory (kernel and FDT)
    return ;
}

EFI_STATUS 
LzmaDecompressKernel (
  IN  ESL_LINUX LinuxKernel
)
{  
	UINT32              OutputBufferSize;
	UINT32              ScratchBufferSize;
	VOID                *Buffer;
	VOID                *ScratchBuffer;
	INTN 			    Result;
    EFI_STATUS          Status = EFI_SUCCESS;
    
	(VOID)LzmaUefiDecompressGetInfo ((void *)0xA4B00000, 0x500000, &OutputBufferSize, &ScratchBufferSize);
	
	Buffer = AllocatePool(OutputBufferSize);
	if(NULL == Buffer)
	{
		DEBUG ((EFI_D_ERROR, "AllocatePool Fail\n"));  
        return EFI_OUT_OF_RESOURCES;
	}	
	ScratchBuffer = AllocatePool(ScratchBufferSize);
	if(NULL == ScratchBuffer)
	{
	    FreePool(Buffer);
		DEBUG ((EFI_D_ERROR, "Allocate ScratchBuffer Fail\n"));  
        return EFI_OUT_OF_RESOURCES;
	}
	 
	Result = LzmaUefiDecompress((void *)0xA4B00000, OutputBufferSize, Buffer, ScratchBuffer);
    if (0 != Result) 
    {
        DEBUG ((EFI_D_ERROR, "Decompress Failed. Result : %d \n", Result)); 
        Status = EFI_DEVICE_ERROR;
        goto Exit;
    }
	
    gBS->CopyMem((void *)(UINTN)LinuxKernel, Buffer, OutputBufferSize);

Exit:    
	FreePool(Buffer);
	FreePool(ScratchBuffer);

    return Status;
}

VOID
Flash_Start_OS (
  )
{
    UINT32              Index = 0;
    UINTN               FDTConfigTable = 0;
    EFI_STATUS Status;
    ESL_LINUX LinuxKernel = (ESL_LINUX)(0x80000); 

    if (!PcdGet32(PcdIsMPBoot))
    {
        for (Index = 0; Index < gST->NumberOfTableEntries; Index ++) 
        {
            if (CompareGuid (&gFdtTableGuid, &(gST->ConfigurationTable[Index].VendorGuid)))
            {
                FDTConfigTable = (UINTN)gST->ConfigurationTable[Index].VendorTable;
                DEBUG ((EFI_D_ERROR, "FDTConfigTable Address: 0x%lx\n",FDTConfigTable));     
                break;
            }
        }
        gBS->CopyMem((void *)0x6000000,(void *)FDTConfigTable,0x100000);
        MicroSecondDelay(20000);
        
        gBS->CopyMem((void *)LinuxKernel,(void *)0x90300000,0xA00000);
        MicroSecondDelay(200000);
        
        gBS->CopyMem((void *)0x7000000,(void *)0x91000000,0x2000000);
        MicroSecondDelay(200000);
    }
    else
    {
        Status = LzmaDecompressKernel (LinuxKernel);
        if(EFI_ERROR(Status))
        {
            goto Exit;
        }

        gBS->CopyMem((void *)0x6000000, (void *)0xA47C0000, 0x20000);
        MicroSecondDelay(20000);

        gBS->CopyMem((void *)0x7000000, (void *)0xA4000000, 0x7C0000);
        MicroSecondDelay(200000);
        if(!PcdGet32(PcdIsMPBoot))
        {
            DEBUG((EFI_D_ERROR,"Update FDT\n"));
            Status = EFIFdtUpdate(0x06000000);
            if(EFI_ERROR(Status))
            {
                DEBUG((EFI_D_ERROR,"EFIFdtUpdate ERROR\n"));
                goto Exit;
            }
        }
    }

    Status = ShutdownUefiBootServices ();
    if(EFI_ERROR(Status)) 
    {
        DEBUG((EFI_D_ERROR,"ERROR: Can not shutdown UEFI boot services. Status=0x%X\n", Status));
        goto Exit;
    }

    //
    // Switch off interrupts, caches, mmu, etc
    //
    Status = PreparePlatformHardware ();
    ASSERT_EFI_ERROR(Status);

    LinuxKernel (0x06000000,0,0,0);
    // Kernel should never exit
    // After Life services are not provided
    ASSERT(FALSE);

Exit:
    // Only be here if we fail to start Linux
    Print (L"ERROR  : Can not start the kernel. Status=0x%X\n", Status);
  
    // Free Runtimee Memory (kernel and FDT)
    return ;
}

VOID
FPGA_LOAD_SRE (
  )
{
    EFI_STATUS Status;
    //ESL_LINUX LinuxKernel = (ESL_LINUX)(0x80000); 

    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Start to boot SRE\n", __FUNCTION__, __LINE__));

    //asm ("b .");

    Status = ShutdownUefiBootServices ();
    if(EFI_ERROR(Status)) 
    {
        DEBUG((EFI_D_ERROR,"ERROR: Can not shutdown UEFI boot services. Status=0x%X\n", Status));
        goto Exit;
    }

    //
    // Switch off interrupts, caches, mmu, etc
    //
    Status = PreparePlatformHardware ();
    ASSERT_EFI_ERROR(Status);

    //asm ("b .");

    *(volatile UINT32 *)(0xC0040000 + 0)   = 0x00000101;

    DEBUG((EFI_D_ERROR, "[%a]:[%dL] Start to jump to SRE\n", __FUNCTION__, __LINE__));

    //asm ("b .");

    asm ("SMC #0");
    
    ASSERT(FALSE);

Exit:
    // Only be here if we fail to start Linux
    Print (L"ERROR  : Can not start the kernel. Status=0x%X\n", Status);

    // Free Runtimee Memory (kernel and FDT)
    return ;
}

