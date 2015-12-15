/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(_SB) 
{
  // Mbi-gen totem
  Device(MBI0) {
    Name(_HID, "HISI0151")
    Name(_CID, "MBIGen")
    Name(_UID, 0)
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0x8c030000, 0x10000)
    })
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-msis", 256}
      }
    })
  }

  // mbi-gen dsa
  Device(MBI1) {
    Name(_HID, "HISI0151")
    Name(_CID, "MBIGen")
    Name(_UID, 1)
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xc6030000, 0x10000) 
    })
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-msis", 640}
      }
    })
  }

  // mbi-gen m3
  Device(MBI2) {
    Name(_HID, "HISI0151")
    Name(_CID, "MBIGen")
    Name(_UID, 2)
    Name(_CRS, ResourceTemplate() {
    Memory32Fixed(ReadWrite, 0xa3030000, 0x10000) 
    })

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-msis", 256}
      }
    })
  }

  // mbi-gen pcie
  Device(MBI3) {
    Name(_HID, "HISI0151")
    Name(_CID, "MBIGen")
    Name(_UID, 3)
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xb7030000, 0x10000) 
    })

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-msis", 640}
      }
    })
  }
}
