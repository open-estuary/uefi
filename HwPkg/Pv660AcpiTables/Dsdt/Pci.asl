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
// PCIE root complex
//

Device (PCI0) {

  //
  // Hardware ID must be PNP0A08, which maps to a PCIe root complex.
  // Section 6.1.5
  //

  Name(_HID, EISAID("PNP0A08"))

  //
  // Optionally, include a compatible ID of PNP0A03, which maps to a PCI
  // root complex for use with pre-PCIe operating systems.
  // Section 6.1.2
  //

  Name(_CID, EISAID("PNP0A03"))

  //
  // Declare the segment number of this root complex. Most systems only
  // have one segment, which is numbered 0.
  // Section 6.5.6
  //

  Name(_SEG, 0)

  //
  // Declare the base bus number, which is the bus number of the root
  // bus in this root complex. This is usually 0, but need not be.
  // For root complexes supporting multiple root busses, this should
  // be the lowest numbered root bus.
  // Section 6.5.5
  //

  Name(_BBN, 0)

  //
  // The _UID value provides a way of uniquely identifying a device
  // in the case where more than one instance of a specific device
  // is implemented with the same _HID/_CID. For systems with a
  // single root complex, this is usually just 0. For systems with
  // multiple root complexes, this should be different for each
  // root complex.
  // Section 6.1.12
  //

  Name(_UID, 0)

  //
  // Declare the PCI Routing Table.
  // This defines SPI mappings of the four line-based interrupts
  // associated with the root complex and hierarchy below it.
  // Section 6.2.12
  //

  Name(_PRT, Package() {

    //
    // Routing for device 0, all functions.
    // Note: ARM doesn't support LNK nodes, so the third param
    // is 0 and the fourth param is the SPI number of the interrupt
    // line. In this example, the A/B/C/D interrupts are wired to
    // SPI lines 100/101/102/103 respectively.
    //

    Package() {0x0000FFFF, 0, 0, 100},
    Package() {0x0000FFFF, 1, 0, 101},
    Package() {0x0000FFFF, 2, 0, 102},
    Package() {0x0000FFFF, 3, 0, 103}
  })

  //
  // Declare the resources assigned to this root complex.
  // Section 6.2.2
  //

  Method (_CRS) {

    //
    // Declare a ResourceTemplate buffer to return the resource
    // requirements from _CRS.
    // Section 19.5.109
    //

    Name (RBUF, ResourceTemplate () {

      //
      // Declare the range of bus numbers assigned to this root
      // complex. In this example, the minimum bus number will be
      // 0, the maximum bus number will be 0xFF, supporting
      // 256 busses total.
      // Section 19.5.141
      //

      WordBusNumber (
        ResourceProducer,
        MinFixed,   // IsMinFixed
        MaxFixed,   // IsMaxFixed
        PosDecode,  // Decode
        0,      // AddressGranularity
        0,      // AddressMinimum - Minimum Bus Number
        255,    // AddressMaximum - Maximum Bus Number
        0,      // AddressTranslation - Set to 0
        256)    // RangeLength - Number of Busses

      //
      // Declare physical address ranges available to be assigned
      // to PCIe BARs and Bridge/Root Port memory windows.
      //
      // To support 32-bit OSes and/or PCIe devices which only
      // implement 32-bit BARs, firmware should supply at least
      // one physical address ranged below 4GB with a size of at
      // least 16MB (ideally larger).
      //
      // On systems supporting 64-bit addressing, to better
      // support 64-bit OSes and PCIe devices which
      // could benefit from availability of a very large address
      // range, firmware should also supply at least one
      // physical address range above 4GB with a size of at least
      // 4GB.
      //

      //
      // Declare the memory range <4GB to be used for BAR memory
      // windows. This example declares a 64MB region starting at
      // 0xE0000000.
      // Section 19.5.80
      //

      Memory32Fixed (ReadWrite, 0xE0000000, 0x04000000)

      //
      // Declare the memory range >4GB to be used for BAR memory
      // windows. This example declares a 16GB region starting at
      // 0x8000000000.
      // Section 19.5.102
      //

      QWordMemory(
        ResourceConsumer,
        PosDecode,
        MinFixed,
        MaxFixed,
        Cacheable,
        ReadWrite,
        0x0000000000000000, // Granularity
        0x0000008000000000, // Min Base Address
        0x0000008000000000, // Max Base Address, same as Min Base for a fixed memory range.
        0x0000000000000000, // Translate
        0x0000000400000000  // Length
        )

    })

    Return (RBUF)
  }

  //
  // Declare an _OSC (OS Control Handoff) method which takes 4 arguments.
  //
  // Argments:
  //   Arg0 - A Buffer containing a UUID
  //   Arg1 - An Integer containing a Revision ID of the buffer format
  //   Arg2 - An Integer containing a count of entries in Arg3
  //   Arg3 - A Buffer containing a list of DWORD capabilities
  // Return Value:
  //   A Buffer containing a list of capabilities
  // See the APCI spec, Section 6.2.10,
  // and the PCI FW spec, Section 4.5.
  //
  // The following is an example, and may need modification for
  // specific implementations.
  //

  Name(SUPP,0) // PCI _OSC Support Field value
  Name(CTRL,0) // PCI _OSC Control Field value
  Method(_OSC, 4) {
    //
    // Look for the PCI Host Bridge Interface UUID.
    // Section 6.2.10.3
    //

    //
    // Create DWord-adressable fields from the Capabilities Buffer
    // Create CDW1 outside the test as it's used in the else clause.
    //

    CreateDWordField(Arg3,0,CDW1)
    If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))) {

      CreateDWordField(Arg3,4,CDW2)
      CreateDWordField(Arg3,8,CDW3)

      //
      // Save Capabilities DWord 2 & 3
      //

      Store(CDW2,SUPP)
      Store(CDW3,CTRL)

      //
      // Only allow native hot plug control if OS supports:
      //  ASPM
      //  Clock PM
      //  MSI/MSI-X
      //

      If(LNotEqual(And(SUPP, 0x16), 0x16)) {

        //
        // Mask bit 0 (and undefined bits)
        //

        And(CTRL,0x1E,CTRL)
      }

      //
      // Always allow native PME, AER (no dependencies).
      // Never allow SHPC (no SHPC controller in this system).
      //

      And(CTRL,0x1D,CTRL)

      //
      // Check for unknown revision.
      //

      If(LNotEqual(Arg1,One)) {
        Or(CDW1,0x08,CDW1)
      }

      //
      // Check if capabilities bits were masked.
      //

      If(LNotEqual(CDW3,CTRL)) {
        Or(CDW1,0x10,CDW1)
      }

      //
      // Update DWORD3 in the buffer.
      //

      Store(CTRL,CDW3)
      Return(Arg3)

    } Else {

      //
      // Unrecognized UUID
      //

      Or(CDW1,4,CDW1)
      Return(Arg3)
    }
  } // End _OSC

  //
  // Declare a _DSM method for various functions called by the OS.
  // See the APCI spec, Section 9.14.1,
  // and the PCI FW spec, Section 4.6.
  // See also:
  // http://download.microsoft.com/download/9/c/5/9c5b2167-8017-4bae-9fde-d599bac8184a/PCI-rsc.doc
  //

  Method(_DSM, 0x4, NotSerialized) {

    //
    // Match against the _DSM PCI GUID.
    //

    If(LEqual(Arg0,ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))) {

      switch(ToInteger(Arg2))
      {
        //
        // Function 0: Return supported functions as a bitfield
        // with one bit for each supported function.
        // Bit 0 must always be set, as that represents
        // function 0 (which is what is being called here).
        // Support for different functions may depend on
        // the revision ID of the interface, passed as Arg1.
        //

        case(0) {

          //
          // Functions 0-7 are supported.
          //

          return (Buffer() {0xFF})
        }

        //
        // Function 1: Return PCIe Slot Information.
        //

        case(1) {

          Return (Package(2) {

            //
            // Success
            //

            One,

            //
            // Returned information.
            //

            Package(3) {
              0x1,  // x1 PCIe link
              0x1,  // PCI express card slot
              0x1   // WAKE# signal supported
            }
          })
        }

        //
        // Function 2: Return PCIe Slot Number.
        //

        case(2) {

          Return (Package(1) {

            //
            // Depending on the first parameter (Source ID),
            // the PCI slot number may be determined by:
            //
            // 0: The _SUN method in the ACPI namespace.
            //
            // 1: The data in the Chassis ID register of the
            //  PCI-to-PCI bridge Slot Numbering Capability.
            //
            // 2: The data in the Physical Slot Number field
            //  of the Slot Capabilities register in the
            //  PCI Express Capability.
            //
            // This example uses the physical slot number,
            // which is in bits 31:19 of the Slot Capabilities
            // register of the PCI Express Capability.
            // This example uses the lowest 8 bits of that
            // field.
            //

            Package(4) {
              2,  // Source ID
              4,  // Token ID: ID refers to a slot
              0,  // Start bit of the field to use.
              7   // End bit of the field to use.
            }
          })
        }

        //
        // Function 3: Return Vendor-specific Token ID Strings.
        //

        case(3) {

          //
          // Not implemented here.
          //

          Return (Package(0) {})
        }

        //
        // Function 4: Return PCI Bus Capabilities
        //

        case(4) {

          Return (Package(2) {

            //
            // Success
            //

            One,

            //
            // Buffer
            //

            Buffer() {
              1,0,      // Version
              0,0,      // Status, 0:Success
              24,0,0,0,     // Length
              1,0,      // PCI
              16,0,       // Length
              0,        // Attributes
              0x0D,       // Current Speed/Mode
              0x3F,0,     // Supported Speeds/Modes
              0,        // Voltage
              0,0,0,0,0,0,0   // Reserved
            }
          })
        }

        //
        // Function 5: Return Ignore PCI Boot Configuration
        //

        case(5) {

          Return (Package(1) {1})
        }

        //
        // Function 6: Return LTR Maximum Latency
        //

        case(6) {

          Return (Package(4) {
            Package(1){0}, // Maximum Snoop Latency Scale
            Package(1){0}, // Maximum Snoop Latency Value
            Package(1){0}, // Maximum No-Snoop Latency Scale
            Package(1){0}  // Maximum No-Snoop Latency Value
          })
        }

        //
        // Function 7: Return PCI Express Naming
        //

        case(7) {

          Return (Package(2) {
              Package(1) {0},         // Instance number
              Package(1) {Unicode("PCI0")}  // Name

          })
        }

        // Functions 2+: not supported
        default {
        }
      }
    }

    //
    // If not one of the function identifiers we recognize, then return a buffer
    // with bit 0 set to 0 indicating no functions supported.
    //

    return(Buffer(){0})
  }

  //
  // Root Port 0 Device within the Root Complex.
  //

  Device(RP0) {

    //
    // Device 0, Function 0.
    //

    Name(_ADR, 0x00000000)

    //
    // Power resources required for D0.
    //

    Name(_PR0, Package() {\_SB.PVR0})

    //
    // Power resources required for D3Hot.
    //

    Name(_PR3, Package(){\_SB.PVR0})

    //
    // Indicate support for wake from S0.
    //

    Name(_S0W, 4)

    //
    // Enable wake events, needed to support D3Cold.
    // One of the following:
    //

    // GPIO wake
    // Name(_CRS, ...)

    // GPE bit (SCI)
    // Name(_PRW, ...)

    // Firmware notification
    // Method(_DSW, 3) {...) }
  }
}

