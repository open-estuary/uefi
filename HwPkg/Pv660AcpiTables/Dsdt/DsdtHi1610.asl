/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015, Hisilicon Limited. All rights reserved.<BR>
  Copyright (c) 2015, Linaro Limited. All rights reserved.<BR>
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/

**/

#include "Pv660Platform.h"

DefinitionBlock("DsdtTable.aml", "DSDT", 1, "HISI", "HISI-EVB", EFI_ACPI_ARM_OEM_REVISION) {
     include ("CPU.asl")
     include ("ComHi1610.asl")
     include ("I2c.asl")
     include ("Usb.asl")
  Scope(_SB) {
   
    
    //Device(I2C0)  { 
	//Name(_HID, "HISI0001")  
	//Name(_UID, 0)
	//Name (_CRS, ResourceTemplate ()  { 
	//	Memory32Fixed (ReadWrite, 0xA00E0000 , 0x10000) 
		//Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,)  {96} 
	//})
	 //Name (_DSD, Package () {
	//	ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), 
	//	Package () { 
	//		Package () {"bus-id",0}, 
	//		Package () {"clock-frequency", 175000000}, 
	//		Package () {"config-speed",1}, 
	//	} 
	//})
   //}

   //Device(I2C1)  { 
	//Name(_HID, "HISI0001")  
	//Name(_UID, 1)
	//Name (_CRS, ResourceTemplate ()  { 
	//	Memory32Fixed (ReadWrite, 0xA00F0000 , 0x10000) 
		//Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,)  {96} 
	//})
	// Name (_DSD, Package () {
	//	ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), 
	//	Package () { 
	//		Package () {"bus-id",1}, 
	//		Package () {"clock-frequency", 175000000}, 
	//		Package () {"config-speed",1}, 
	//	} 
	//})
    //}
    //Device(I2C2)  { 
	//Name(_HID, "HISI0001")  
	//Name(_UID, 2)
	//Name (_CRS, ResourceTemplate ()  { 
	//	Memory32Fixed (ReadWrite, 0x60320000 , 0x10000) 
		//Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,)  {96} 
	//})
	 //Name (_DSD, Package () {
	//	ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), 
	//	Package () { 
	//		Package () {"bus-id",3}, 
	//		Package () {"clock-frequency", 100000000}, 
	//		Package () {"config-speed",1}, 
	//	} 
	//})
    //}
    //Device(I2C3)  { 
	//Name(_HID, "HISI0001")  
	//Name(_UID, 3)
	//Name (_CRS, ResourceTemplate ()  { 
	//	Memory32Fixed (ReadWrite, 0x60330000 , 0x10000) 
		//Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,)  {96} 
	//})
	// Name (_DSD, Package () {
	//	ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), 
	//	Package () { 
	//		Package () {"bus-id",4}, 
	//		Package () {"clock-frequency", 100000000}, 
	//		Package () {"config-speed",1}, 
	//	} 
	//})
  //}
 } // Scope(_SB)
}
