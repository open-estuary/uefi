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

//#include "ArmPlatform.h"
Scope(_SB) 
{
    //
    // A57x2-A53x4 Processor declaration
    //
    Device(CPU0) { // A57-0: Cluster 0, Cpu 0
      Name(_HID, "ACPI0007")
      Name(_UID, 0)
    }
    Device(CPU1) { // A57-1: Cluster 0, Cpu 1
      Name(_HID, "ACPI0007")
      Name(_UID, 1)
    }
    Device(CPU2) { // A53-0: Cluster 1, Cpu 0
      Name(_HID, "ACPI0007")
      Name(_UID, 2)
    }
    Device(CPU3) { // A53-1: Cluster 1, Cpu 1
      Name(_HID, "ACPI0007")
      Name(_UID, 3)
    }
    Device(CPU4) { // A53-2: Cluster 1, Cpu 2
      Name(_HID, "ACPI0007")
      Name(_UID, 4)
    }
    Device(CPU5) { // A53-3: Cluster 1, Cpu 3
      Name(_HID, "ACPI0007")
      Name(_UID, 5)
    }
    Device(CPU6) { // A57-0: Cluster 0, Cpu 0
      Name(_HID, "ACPI0007")
      Name(_UID, 6)
    }
    Device(CPU7) { // A57-1: Cluster 0, Cpu 1
      Name(_HID, "ACPI0007")
      Name(_UID, 7)
    }
    Device(CPU8) { // A53-0: Cluster 1, Cpu 0
      Name(_HID, "ACPI0007")
      Name(_UID, 8)
    }
    Device(CPU9) { // A53-1: Cluster 1, Cpu 1
      Name(_HID, "ACPI0007")
      Name(_UID, 9)
    }
    Device(CP10) { // A53-2: Cluster 1, Cpu 2
      Name(_HID, "ACPI0007")
      Name(_UID, 10)
    }
    Device(CP11) { // A53-3: Cluster 1, Cpu 3
      Name(_HID, "ACPI0007")
      Name(_UID, 11)
    }
    Device(CP12) { // A57-0: Cluster 0, Cpu 0
      Name(_HID, "ACPI0007")
      Name(_UID, 12)
    }
    Device(CP13) { // A57-1: Cluster 0, Cpu 1
      Name(_HID, "ACPI0007")
      Name(_UID, 13)
    }
    Device(CP14) { // A53-0: Cluster 1, Cpu 0
      Name(_HID, "ACPI0007")
      Name(_UID, 14)
    }
    Device(CP15) { // A53-1: Cluster 1, Cpu 1
      Name(_HID, "ACPI0007")
      Name(_UID, 15)
    }
}