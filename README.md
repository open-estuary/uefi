# uefi-dev
This is the source code of UEFI for Hisilicon D02. It is based on Linaro edk2 master branch.
HwPkg and HwProductsPkg are for D02 specifically, while Hw stands for Huawei. 

## How to build
1. Follow the instructions in BuildNotes2.txt in the root directory to setup build environment

2. Use below commands to build D02 UEFI:

    uefi-tools/uefi-build.sh d02<br>

3. UEFI image will be "Build/Pv660D02/RELEASE_GCC49/FV/PV660D02.fd" (Assuming you are using gcc 4.9, or else replace 49 in the path with your gcc version).

[NOTE] As we need to build ACPI ASL file and some tables are only valid until ACPI specification v6.0, we recommend to use acpica tools of version 20150930 or later.
