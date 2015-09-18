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

    // USB Host Controller

    //

    Device(USB0){

        Name(_HID, "ARMH0D20")

        Name(_CID, "PNP0D20")

        Name(_UID, 1)

 

        Method(_CRS, 0x0, Serialized){

            Name(RBUF, ResourceTemplate(){

                Memory32Fixed(ReadWrite, 0xa1000000, 0x10000)

                Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {13588}  // 0x3500 + 0x14,level trigger, for USB EHCI Controller

            })

            Return(RBUF)

        }

 

        //

        // Root Hub

        //

        Device(RHUB){

            Name(_ADR, 0x00000000)  // Address of Root Hub should be 0 as per ACPI 5.0 spec

 

            //

            // Ports connected to Root Hub

            //

            Device(HUB1){

                Name(_ADR, 0x00000001)

                Name(_UPC, Package(){

                    0x00,       // Port is NOT connectable

                    0xFF,       // Don't care

                    0x00000000, // Reserved 0 must be zero

                    0x00000000  // Reserved 1 must be zero

                })

 

                Device(PRT1){

                    Name(_ADR, 0x00000001)

                    Name(_UPC, Package(){

                        0xFF,        // Port is connectable

                        0x00,        // Port connector is A

                        0x00000000,

                        0x00000000

                    })

                    Name(_PLD, Package(){

                        Buffer(0x10){

                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

                        }

                    })

                } // USB0_RHUB_HUB1_PRT1

                Device(PRT2){

                    Name(_ADR, 0x00000002)

                    Name(_UPC, Package(){

                        0xFF,        // Port is connectable

                        0x00,        // Port connector is A

                        0x00000000,

                        0x00000000

                    })

                    Name(_PLD, Package(){

                        Buffer(0x10){

                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

                        }

                    })

                } // USB0_RHUB_HUB1_PRT2

 

                Device(PRT3){

                    Name(_ADR, 0x00000003)

                    Name(_UPC, Package(){

                        0xFF,        // Port is connectable

                        0x00,        // Port connector is A

                        0x00000000,

                        0x00000000

                    })

                    Name(_PLD, Package(){

                        Buffer(0x10){

                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

                        }

                    })

                } // USB0_RHUB_HUB1_PRT3

 

                Device(PRT4){

                    Name(_ADR, 0x00000004)

                    Name(_UPC, Package(){

                        0xFF,        // Port is connectable

                        0x00,        // Port connector is A

                        0x00000000,

                        0x00000000

                    })

                    Name(_PLD, Package(){

                        Buffer(0x10){

                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

                        }

                    })

                } // USB0_RHUB_HUB1_PRT4

            } // USB0_RHUB_HUB1

        } // USB0_RHUB

    } // USB0
}
