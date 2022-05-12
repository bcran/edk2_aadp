#!/bin/bash
OPATH=$PATH
. edk2.sh
echo "==========================================================================="
echo "prepare toolchain"
echo "==========================================================================="
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
export PATH=$OPATH
