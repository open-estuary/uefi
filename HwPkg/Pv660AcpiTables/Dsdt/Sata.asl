/** @file
*
*  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
*  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/
*
**/

//
// SATA AHCI
//

Device (SATA0)
{
  Name(_HID, "HISI0600")	// HiSi AHCI
  Name (_CCA, 1)       		// Cache-coherent controller
  Name (_CLS, Package (3)	// How to identify direct attached AHCI to Windows
  {
    0x01,       // Base Class:  Mass Storage
    0x06,       // Sub-Class: Serial ATA
    0x01,       // Interface: AHCI
  })
  Name (_CRS, ResourceTemplate ()
  {
    Memory32Fixed (ReadWrite, %AHCI_ADDRESS%, 0x00010000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,) { %AHCI_GSIV% }
  })
}

