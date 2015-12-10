/*
 * Intel ACPI Component Architecture
 * iASL Compiler/Disassembler version 20151124-64
 * Copyright (c) 2000 - 2015 Intel Corporation
 * 
 * Template for [IORT] ACPI Table (static data table)
 * Format: [ByteLength]  FieldName : HexFieldValue
 */
[0004]                          Signature : "IORT"    [IO Remapping Table]
[0004]                       Table Length : 0000010C
[0001]                           Revision : 00
[0001]                           Checksum : BC
[0006]                             Oem ID : "INTEL "
[0008]                       Oem Table ID : "TEMPLATE"
[0004]                       Oem Revision : 00000000
[0004]                    Asl Compiler ID : "INTL"
[0004]              Asl Compiler Revision : 20150410

[0004]                         Node Count : 0000000A
[0004]                        Node Offset : 00000034
[0004]                           Reserved : 00000000
[0004]                   Optional Padding : 00 00 00 00 

/* ITS 0, for totem */
[0001]                               Type : 00
[0002]                             Length : 0018
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000000
[0004]                     Mapping Offset : 00000000

[0004]                           ItsCount : 00000001
[0004]                        Identifiers : 00000000

/* ITS 1, for dsa */
[0001]                               Type : 00
[0002]                             Length : 0018
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000000
[0004]                     Mapping Offset : 00000000

[0004]                           ItsCount : 00000001
[0004]                        Identifiers : 00000001

/* ITS 2, m3 */
[0001]                               Type : 00
[0002]                             Length : 0018
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000000
[0004]                     Mapping Offset : 00000000

[0004]                           ItsCount : 00000001
[0004]                        Identifiers : 00000002

/* ITS 3, pcie */                        
[0001]                               Type : 00
[0002]                             Length : 0018
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000000
[0004]                     Mapping Offset : 00000000

[0004]                           ItsCount : 00000001
[0004]                        Identifiers : 00000003

/* mbi-gen pc, named component */
[0001]                               Type : 01
[0002]                             Length : 003B
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000001
[0004]                     Mapping Offset : 00000027

[0004]                         Node Flags : 00000000
[0008]                  Memory Properties : [IORT Memory Access Properties]
[0004]                    Cache Coherency : 00000000
[0001]              Hints (decoded below) : 00
                                Transient : 0
                           Write Allocate : 0
                            Read Allocate : 0
                                 Override : 0
[0002]                           Reserved : 0000
[0001]       Memory Flags (decoded below) : 00
                                Coherency : 0
                         Device Attribute : 0
[0001]                  Memory Size Limit : 00
[0012]                        Device Name : "\_SB_.MBI0"
[0004]                            Padding : 00 00 00 00

[0004]                         Input base : 00000000
[0004]                           ID Count : 00000001
[0004]                        Output Base : 0000FFCC
[0004]                   Output Reference : 00000034  // point to its totem
[0004]              Flags (decoded below) : 00000000
                           Single Mapping : 1

/* mbi-gen dsa, named component */
[0001]                               Type : 01
[0002]                             Length : 003B
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000001
[0004]                     Mapping Offset : 00000027

[0004]                         Node Flags : 00000000
[0008]                  Memory Properties : [IORT Memory Access Properties]
[0004]                    Cache Coherency : 00000000
[0001]              Hints (decoded below) : 00
                                Transient : 0
                           Write Allocate : 0
                            Read Allocate : 0
                                 Override : 0
[0002]                           Reserved : 0000
[0001]       Memory Flags (decoded below) : 00
                                Coherency : 0
                         Device Attribute : 0
[0001]                  Memory Size Limit : 00
[0011]                        Device Name : "\_SB_.MBI1"
[0004]                            Padding : 00 00 00 00

[0004]                         Input base : 00000000
[0004]                           ID Count : 00000001
[0004]                        Output Base : 0000FFCC
[0004]                   Output Reference : 0000004C
[0004]              Flags (decoded below) : 00000000
                           Single Mapping : 1

/* mbi-gen m3, named component */
[0001]                               Type : 01
[0002]                             Length : 003B
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000001
[0004]                     Mapping Offset : 00000027

[0004]                         Node Flags : 00000000
[0008]                  Memory Properties : [IORT Memory Access Properties]
[0004]                    Cache Coherency : 00000000
[0001]              Hints (decoded below) : 00
                                Transient : 0
                           Write Allocate : 0
                            Read Allocate : 0
                                 Override : 0
[0002]                           Reserved : 0000
[0001]       Memory Flags (decoded below) : 00
                                Coherency : 0
                         Device Attribute : 0
[0001]                  Memory Size Limit : 00
[0011]                        Device Name : "\_SB_.MBI2"
[0004]                            Padding : 00 00 00 00

[0004]                         Input base : 00000000
[0004]                           ID Count : 00000001
[0004]                        Output Base : 0000FFCC
[0004]                   Output Reference : 00000064
[0004]              Flags (decoded below) : 00000000
                           Single Mapping : 1

/* mbi-gen pcie, named component */
[0001]                               Type : 01
[0002]                             Length : 003B
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000001
[0004]                     Mapping Offset : 00000027

[0004]                         Node Flags : 00000000
[0008]                  Memory Properties : [IORT Memory Access Properties]
[0004]                    Cache Coherency : 00000000
[0001]              Hints (decoded below) : 00
                                Transient : 0
                           Write Allocate : 0
                            Read Allocate : 0
                                 Override : 0
[0002]                           Reserved : 0000
[0001]       Memory Flags (decoded below) : 00
                                Coherency : 0
                         Device Attribute : 0
[0001]                  Memory Size Limit : 00
[0011]                        Device Name : "\_SB_.MBI3"
[0004]                            Padding : 00 00 00 00

[0004]                         Input base : 00000000
[0004]                           ID Count : 00000001
[0004]                        Output Base : 0000FFCC
[0004]                   Output Reference : 0000007C
[0004]              Flags (decoded below) : 00000000
                           Single Mapping : 1

						   /* RC 0 */
[0001]                               Type : 02
[0002]                             Length : 0034
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000000
[0004]                     Mapping Offset : 00000020

[0008]                  Memory Properties : [IORT Memory Access Properties]
[0004]                    Cache Coherency : 00000000
[0001]              Hints (decoded below) : 00
                                Transient : 0
                           Write Allocate : 0
                            Read Allocate : 0
                                 Override : 0
[0002]                           Reserved : 0000
[0001]       Memory Flags (decoded below) : 00
                                Coherency : 0
                         Device Attribute : 0
[0004]                      ATS Attribute : 00000000
[0004]                 PCI Segment Number : 00000000

[0004]                         Input base : 00000000
[0004]                           ID Count : 00000040
[0004]                        Output Base : 00000000
[0004]                   Output Reference : 0000007C
[0004]              Flags (decoded below) : 00000000
                           Single Mapping : 0

/* RC 1 */
[0001]                               Type : 02
[0002]                             Length : 0034
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000000
[0004]                     Mapping Offset : 00000020

[0008]                  Memory Properties : [IORT Memory Access Properties]
[0004]                    Cache Coherency : 00000000
[0001]              Hints (decoded below) : 00
                                Transient : 0
                           Write Allocate : 0
                            Read Allocate : 0
                                 Override : 0
[0002]                           Reserved : 0000
[0001]       Memory Flags (decoded below) : 00
                                Coherency : 0
                         Device Attribute : 0
[0004]                      ATS Attribute : 00000000
[0004]                 PCI Segment Number : 00000000

[0004]                         Input base : 00000000
[0004]                           ID Count : 00000040
[0004]                        Output Base : 00000000
[0004]                   Output Reference : 0000007C
[0004]              Flags (decoded below) : 00000000
                           Single Mapping : 0
						   
/*
[0001]                               Type : 03
[0002]                             Length : 005C
[0001]                           Revision : 00
[0004]                           Reserved : 00000000
[0004]                      Mapping Count : 00000000
[0004]                     Mapping Offset : 0000005C

[0008]                       Base Address : 0000000000000000
[0008]                               Span : 0000000000000000
[0004]                              Model : 00000000
[0004]              Flags (decoded below) : 00000000
                            DVM Supported : 0
                            Coherent Walk : 0
[0004]            Global Interrupt Offset : 0000003C
[0004]            Context Interrupt Count : 00000001
[0004]           Context Interrupt Offset : 0000004C
[0004]                PMU Interrupt Count : 00000001
[0004]               PMU Interrupt Offset : 00000054

[0008]             SMMU_NSgIrpt Interrupt : 0000000000000000
[0008]          SMMU_NSgCfgIrpt Interrupt : 0000000000000000
[0008]                  Context Interrupt : 0000000000000000
[0008]                      PMU Interrupt : 0000000000000000
*/
