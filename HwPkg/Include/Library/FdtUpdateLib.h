

#ifndef _FDTUPDATELIB_H_
#define _FDTUPDATELIB_H_

#define E820_OSP_NVRAM 6     
#define E820_OSP_MEM 7       
#define E820_PCIE_OPP 8       /* pcie opp cache virtual address */
#define E820_BIOS_MEM 9      
#define E820_PBS 0x0a        
#define E820_BIOS_FLASH 0x0b 
#define E820_PCIE_MEM 0x0c    
#define E820_MEM_VOS 0X0d    

#define ADD_FILE_LENGTH  0x400

typedef struct
{
  UINT32 BaseHigh;    
  UINT32 BaseLow;
  UINT32 LengthHigh;    
  UINT32 LengthLow;
}PHY_MEM_REGION;

typedef struct 
{
  UINT8 data0;    
  UINT8 data1;
  UINT8 data2;    
  UINT8 data3;
  UINT8 data4;
  UINT8 data5;
}MAC_ADDRESS;

MAC_ADDRESS gMacAddress[1];

typedef struct region
{
    UINT32 BaseHigh;    
    UINT32 BaseLow;
    UINT32 LengthHigh;    
    UINT32 LengLow;
    UINT32 Type;
}RESERVED_MEM_REGION;

//Attention: This is not produced, but consumed by this lib!!
extern  RESERVED_MEM_REGION gExtMemRegion[];

extern  EFI_STATUS EFIFdtUpdate(UINTN FdtFileAddr);

#endif


