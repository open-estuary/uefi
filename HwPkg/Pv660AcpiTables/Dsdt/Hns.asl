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
        Device (MDIO) {
                Name(_HID, "HISI0141")
                Name (_CRS, ResourceTemplate (){
                        Memory32Fixed (ReadWrite, 0x803c0000 , 0x10000)
                })

		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"subctrl_vbase",Package() {\_SB.CTL2} }
			}
		})

                Device (PHY0) {
			Name (_ADR, Zero)
                        Name (_DSD, Package () {
                                ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                                Package () {
                                        Package () {"phy-addr", 0},
                                        Package () {"ethernet-phy", "ethernet-phy-ieee802.3-c22"},
                                }
                        })
                }
                Device (PHY1) {
			Name (_ADR, one)
                        Name (_DSD, Package () {
                                ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                                Package () {
                                        Package () {"phy-addr", 1},
                                        Package () {"ethernet-phy", "ethernet-phy-ieee802.3-c22"},
                                }
                        })
                }
        }
	Device (DSAF) {
                Name(_HID, "HISI00B1")
                Name (_CCA, 1) // Cache-coherent controller
                Name (_CRS, ResourceTemplate (){
                        Memory32Fixed (ReadWrite, 0xC2000000 , 0x300000)
                        Memory32Fixed (ReadWrite, 0xc5000000 , 0x890000)
                        Memory32Fixed (ReadWrite, 0xc7000000 , 0x60000)
                        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,)
				{
					149,150,151,152,153,154,26,27,155,156,157,158,159,160,	//[14] ge fifo err 8 / xge 6
					6,7,8,9,16,17,18,19,22,23,24,25,			//[12] rcb com 4*3
					0,1,2,3,4,5,12,13,					//[8] ppe tnl 0-7
					128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,	//[21] dsaf event int 3+18
				}
                        Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive,,,)
				{
					14, 15, 20, 21,	// [4] debug rcb 2*2
					384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399,  //[256] sevice rcb 2*128
					400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415,
					416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431,
					432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447,
					448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463,
					464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479,
					480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495,
					496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511,
					512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527,
					528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543,
					544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559,
					560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 574, 575,
				}
                })
                Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                        Package () {
                                Package () {"mode", "6port-16rss"},
                                Package () {"dsa_name", "dsaf0"},
                                Package () {"buf-size", 4096},
                                Package () {"desc-num", 1024},
				Package () {"interrupt-parent", Package() {\_SB.MBI1}},
                                Package () {"hisilicon,dsaf-syscon", Package() {\_SB.CTL0}},
                                Package () {"phy-handle4", Package (){\_SB.MDIO.PHY0}},
                                Package () {"phy-handle5", Package (){\_SB.MDIO.PHY1}},
                        }
                })
        }
	Device (ETH5) {
		Name(_HID, "HISI00C1")
		Name (_CCA, 1) // Cache-coherent controller
		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"local-mac-address", Package () { 0, 0, 0, 0, 0, 0 }}, // 6-bytes
				Package () {"ae-name", "dsaf0"},
				Package () {"port-id", 7},
			}
		})
	}
	Device (ETH4) {
		Name(_HID, "HISI00C1")
		Name (_CCA, 1) // Cache-coherent controller
		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"local-mac-address", Package () { 0, 0, 0, 0, 0, 0 }}, // 6-bytes
				Package () {"ae-name", "dsaf0"},
				Package () {"port-id", 6},
			}
		})
	}
	Device (ETH0) {
		Name(_HID, "HISI00C1")
		Name (_CCA, 1) // Cache-coherent controller
		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"local-mac-address", Package () { 0, 0, 0, 0, 0, 0 }}, // 6-bytes
				Package () {"ae-name", "dsaf0"},
				Package () {"port-id", 2},
			}
		})
	}
	Device (ETH1) {
		Name(_HID, "HISI00C1")
		Name (_CCA, 1) // Cache-coherent controller
		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"local-mac-address", Package () { 0, 0, 0, 0, 0, 0 }}, // 6-bytes
				Package () {"ae-name", "dsaf0"},
				Package () {"port-id", 3},
			}
		})
	}

}
