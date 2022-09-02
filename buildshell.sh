#!/bin/bash
OPATH=$PATH
export WORKSPACE=$PWD
OEM_CHIP_DIR=$WORKSPACE/edk2_adlink-ampere-altra
OEM_CHIPTOOL_DIR=$OEM_CHIP_DIR/tools/
. $OEM_CHIPTOOL_DIR/edk2.sh
echo "==========================================================================="
echo "prepare toolchain"
echo "==========================================================================="
#
# ignore below attempting, just make any BIOS before run this script to build a AARCH64 UEFI Shell
#
if [ ! -d $WORKSPACE/edk2-ampere-tools/toolchain/ampere/bin ] ; then
    make -f $WORKSPACE/edk2-ampere-tools/Makefile _check_compiler
fi    
export PATH=$WORKSPACE/edk2-ampere-tools/toolchain/ampere/bin:"${PATH}"
export GCC5_AARCH64_PREFIX=aarch64-ampere-linux-gnu-
export GCC5_ARM_PREFIX=$GCC5_AARCH64_PREFIX
export CROSS_COMPILE=$GCC5_AARCH64_PREFIX
echo "==========================================================================="
echo "build shell with applications"
echo "==========================================================================="
build -a AARCH64 -t GCC5 -b RELEASE -p edk2/ShellPkg/ShellPkg.dsc
mkdir -p BUILDS/efi/boot
if ! [ -d "BUILDS" ] ; then
   mkdir -p BUILDS
fi   
cp -rf Build/Shell/RELEASE_GCC5/AARCH64/ShellPkg/Application/Shell/Shell/OUTPUT/Shell.efi BUILDS/efi/boot/bootaa64.efi
export PATH=$OPATH
