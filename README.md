# uefi-dev
This is the source code of UEFI for Hisilicon D02. It is based on Linaro edk2 master branch.
HwPkg and HwProductsPkg are for D02 specifically, while Hw stands for Huawei. 

## How to build
1. Follow the instructions in BuildNotes2.txt in the root directory to setup build environment

2. Use below command to apply patch before building
(some changes have been upstream to Tiano edk2 and the patch will not be needed in future):

    git apply HwPkg/Patch/*.patch

3. Use below commands to build D02 UEFI:

    export LC_CTYPE=C
    make -C BaseTools
    source edksetup.sh
    build -a AARCH64 -b RELEASE -t ARMLINUXGCC -p HwProductsPkg/D02/Pv660D02.dsc
