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

Scope(_SB)
{
  // UART PL011
  Device(COM0) {
    Name(_HID, "APMC0D08") //Or AMD0020, trick to use dw8250 serial driver
    Name(_CID, "8250dw")
    Name(_UID, Zero)
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0x80300000, 0x1000) //0x7FF80000, 0x1000
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 349 } //115
    })
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"clock-frequency", 200000000},
      }
    })
  }
}
