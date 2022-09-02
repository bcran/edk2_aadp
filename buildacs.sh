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
echo "build SBSA-ACS"
echo "==========================================================================="
export GCC49_AARCH64_PREFIX=$GCC5_AARCH64_PREFIX
PACKAGES_PATH=$WORKSPACE/edk2/edk2-libc:"${PACKAGES_PATH}"
# #
# # ont time routine: To create branch SBSA-ACE
# #
# echo "==========================================================================="
# echo "build Create branch SBSA-ACS following: https://github.com/ARM-software/sbsa-acs"
# echo "==========================================================================="
# git checkout edk2-stable202008
# git checkout -b SBSA-ACS
# #
# # one time 
# #
# git submodule add https://github.com/ADLINK/edk2-libc.git
# git submodule update --init --recursive
# git submodule add https://github.com/ADLINK/sbsa-acs.git ShellPkg/Application/sbsa-acs
#
# make ACS after modify ShellPkg.dsc
#
cd $WORKSPACE/edk2
echo "==========================================================================="
echo "Make SBSA-ACS"
echo "==========================================================================="
git checkout SBSA-ACS
git submodule update --init --recursive

if [ "eval $(ssh -T git@github.com-adlink | grep -q "authenticated")" != "" ] ; then
  echo "==========================================================================="
  echo "replace HTTPS access with SSH access if authenticated"
  echo "==========================================================================="
  if [ -d "ShellPkg/Application/sbsa-acs" ] ; then
    cd ShellPkg/Application/sbsa-acs
    git remote set-url origin git@github.com-adlink:ADLINK/sbsa-acs.git
    git checkout adlink
    cd $WORKSPACE/edk2
  fi  
fi

source ShellPkg/Application/sbsa-acs/tools/scripts/avsbuild.sh

git reset --hard HEAD
git checkout - # switch back to previous branch
git submodule update --init --recursive

cd $WORKSPACE
if ! [ -d "BUILDS" ] ; then
   mkdir -p BUILDS
fi   
cp -rf ./Build/Shell/DEBUG_GCC49/AARCH64/Sbsa.efi ./BUILDS/Sbsa.efi

export PATH=$OPATH
