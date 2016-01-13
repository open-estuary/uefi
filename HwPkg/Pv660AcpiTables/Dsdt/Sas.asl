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
    Device(SAS0) {
          Name(_HID, "HISI0161")
	  Name(_CCA, 1)
          Name(_CRS, ResourceTemplate() {
                  Memory32Fixed(ReadWrite, 0xc1000000, 0x10000)
                  Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                  {
			//phy irq(0~79)
			259,263,264,
			269,273,274,
			279,283,284,
			289,293,294,
			299,303,304,
			309,313,314,
			319,323,324,
			329,333,334,
		  }

                  Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )
                  {
			//cq irq (80~111)
			336,337,338,339,340,341,342,343,
			344,345,346,347,348,349,350,351,
			352,353,354,355,356,357,358,359,
			360,361,362,363,364,365,366,367,
		  }

                  Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                  {
			376, //chip fatal error irq(120)
			381, //chip fatal error irq(125)
                  }
          })

          Name (_DSD, Package () {
              ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
              Package () {
                  Package () {"interrupt-parent",Package() {\_SB.MBI1}},
		  Package (2) {"sas-addr", Package() {50, 01, 88, 20, 16, 00, 00, 0x0a}},
		  Package () {"queue-count", 32},
		  Package () {"phy-count", 8},
              }
          })

	  OperationRegion (CTL, SystemMemory, 0xC0000000, 0x10000)
	  Field (CTL, AnyAcc, NoLock, Preserve)
	  {
		  Offset (0x338),
		  CLK, 32,
		  CLKD, 32,
		  Offset (0xa60),
		  RST, 32,
		  DRST, 32,
		  Offset (0x5a30),
		  STS, 32,
	  }

	  Method (_RST, 0x0, Serialized)
	  {
		  Store(0x7ffff, RST)
		  Store(0x7ffff, CLKD)
		  Sleep(1)
		  Store(0x7ffff, DRST)
		  Store(0x7ffff, CLK)
		  Sleep(1)
	  }
    }

    Device(SAS1) {
          Name(_HID, "HISI0161")
	  Name(_CCA, 1)
          Name(_CRS, ResourceTemplate() {
                  Memory32Fixed(ReadWrite, 0xb1000000, 0x10000)
                  Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                  {
			//phy irq(0~79)
			259,263,264,
			269,273,274,
			279,283,284,
			289,293,294,
			299,303,304,
			309,313,314,
			319,323,324,
			329,333,334,
		  }

                  Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )
                  {
			//cq irq (80~111)
			336,337,338,339,340,341,342,343,
			344,345,346,347,348,349,350,351,
			352,353,354,355,356,357,358,359,
			360,361,362,363,364,365,366,367,
		  }

                  Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                  {
			376, //chip fatal error irq(120)
			381, //chip fatal error irq(125)
                  }
          })

          Name (_DSD, Package () {
              ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
              Package () {
                  Package () {"interrupt-parent",Package() {\_SB.MBI3}},
		  Package (2) {"sas-addr", Package() {50, 01, 88, 20, 16, 00, 00, 00}},
		  Package () {"queue-count", 32},
		  Package () {"phy-count", 8},
              }
          })

	  OperationRegion (CTL, SystemMemory, 0xB0000000, 0x10000)
	  Field (CTL, AnyAcc, NoLock, Preserve)
	  {
		  Offset (0x318),
		  CLK, 32,
		  CLKD, 32,
		  Offset (0xa18),
		  RST, 32,
		  DRST, 32,
		  Offset (0x5a0c),
		  STS, 32,
	  }

	  Method (_RST, 0x0, Serialized)
	  {
		  Store(0x7ffff, RST)
		  Store(0x7ffff, CLKD)
		  Sleep(1)
		  Store(0x7ffff, DRST)
		  Store(0x7ffff, CLK)
		  Sleep(1)
	  }
    }
}
