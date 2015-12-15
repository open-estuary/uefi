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
	// sysctl dsa
	Device(CTL0) {
		Name(_HID, "HISI0061")
		Name(_CRS, ResourceTemplate() {
			Memory32Fixed(ReadWrite, 0xC0000000, 0x10000)
		})
	}
	// sysctl pcie
	Device(CTL1) {
		Name(_HID, "HISI0061")
		Name(_CRS, ResourceTemplate() {
			Memory32Fixed(ReadWrite, 0xB0000000, 0x10000)
		})
	}
	// sysctl peri_c
	Device(CTL2) {
		Name(_HID, "HISI0061")
		Name(_CRS, ResourceTemplate() {
			Memory32Fixed(ReadWrite, 0x80000000, 0x10000)
		})
	}
}
