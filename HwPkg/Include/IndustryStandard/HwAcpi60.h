/** @file   
  ACPI 5.1 definitions from the ACPI Specification Revision 5.1 July, 2014.

  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             
**/

#ifndef _HW_ACPI_6_0_H_
#define _HW_ACPI_6_0_H_

#include <IndustryStandard/Acpi51.h>

//
// Multiple APIC Description Table APIC structure types
// All other values between 0x11 and 0x7F are reserved and
// will be ignored by OSPM. 0x80 ~ 0xFF are reserved for OEM.
//
#define EFI_ACPI_6_0_ITS                            0x0F
#define EFI_ACPI_HuaWei_MBI                         0x10

//
// Ensure proper structure formats
//
#pragma pack(1)

///
/// GIC MBI Structure
///
typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT16  Reserved;
  UINT32  Id;
  UINT32  ITSId;
  UINT64  BaseAddress;
  UINT32  Size;
} EFI_ACPI_HuaWei_GIC_MBI_STRUCTURE;

enum AcpiIortNodeType
{
    ACPI_IORT_NODE_TYPE_SMMU_V12     = 0,
    ACPI_IORT_NODE_TYPE_SMMU_V3      = 1,
    ACPI_IORT_NODE_TYPE_ITS_GROUP    = 2,
    ACPI_IORT_NODE_TYPE_NAMED_NODE   = 3,
    ACPI_IORT_NODE_TYPE_ROOT_COMPLEX = 4,
    ACPI_IORT_NODE_TYPE_RESERVED     = 5     /* 5 and greater are reserved */
};

typedef struct 
{
    UINT8                   Type;
    UINT16                  Length;
    UINT8                   Revision;
    UINT32                  Reserved;
    UINT32                  NumberOfIds;
    UINT32                  RefToIds;
    UINT32                  NumberOfChildrenNodes;
    UINT32                  RefToChildrenNodes;
} EFI_ACPI_6_0_IORT_NODE_HEADER;

///
/// IO Remapping Table definition.
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
} EFI_ACPI_6_0_IO_REMAPPING_TABLE;  

typedef struct
{
    UINT16                   Length;
    UINT16                   Flags;
    UINT32                   InputBase;
    UINT32                   OutputBase;
    UINT32                   OutputRef;
} EFI_ACPI_6_0_IORT_NODE_ID_STRUCTURE;


typedef struct 
{
    EFI_ACPI_6_0_IORT_NODE_HEADER   Header;
    UINT64                  BaseAddress;
    UINT64                  Span;
    UINT32                  Model;

    UINT32                  GlobalIrqNumber;
    UINT32                  ContextIrqNumber;
    UINT32                  PmuIrqNumber;
    EFI_ACPI_6_0_IORT_NODE_ID_STRUCTURE     NodeId[2];
} EFI_ACPI_6_0_IORT_NODE_TYPE_SMMUV12_STRUCTURE;

typedef struct 
{
    EFI_ACPI_6_0_IORT_NODE_HEADER   Header;
    UINT64                  BaseAddress;
    UINT64                  Span;
    UINT32                  Model;
    UINT32                  Event;
    UINT32                  EventFlags;
    UINT32                  Pri;
    UINT32                  PriFlags;
    UINT32                  Gerr;
    UINT32                  GerrFlags;
    UINT32                  Sync;
    UINT32                  SyncFlags;
} EFI_ACPI_6_0_IORT_NODE_TYPE_SMMUV3_STRUCTURE;

typedef struct 
{
    EFI_ACPI_6_0_IORT_NODE_HEADER   Header;
    UINT32                  NumberOfIts;
} EFI_ACPI_6_0_IORT_NODE_TYPE_ITS_GROUP_STRUCTURE;

typedef struct 
{
    EFI_ACPI_6_0_IORT_NODE_HEADER   Header;
    UINT32                  CoherencyAttribute;
    char                    DeviceName[1];
} EFI_ACPI_6_0_IORT_NODE_TYPE_NAMED_NODE_STRUCTURE;

typedef struct 
{
    EFI_ACPI_6_0_IORT_NODE_HEADER   Header;
    UINT32                  AtsAttribute;
    char                    DeviceName[1];
} EFI_ACPI_6_0_IORT_NODE_TYPE_ROOT_COMPLEX_STRUCTURE;

///
/// IORT Version
///
#define EFI_ACPI_6_0_IO_REMAPPING_TABLE_REVISION 0x01
///
///NODE Version
///
#define EFI_ACPI_6_0_NODE_REVISION  0x00
///
///Node ID Flags
///
#define EFI_ACPI_6_0_NODE_ID_FLAGS  0x0001
///
/// "IORT" IO Remapping Table
///
#define EFI_ACPI_6_0_IO_REMAPPING_TABLE_SIGNATURE  SIGNATURE_32('I', 'O', 'R', 'T')


#pragma pack()

#endif
