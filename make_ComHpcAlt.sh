#!/bin/bash
OPATH=$PATH
. edk2.sh
OEM_SRC_DIR=$WORKSPACE/adlink-platforms
BOARD_NAME=ComHpcAlt
EDK2_PLATFORMS_PKG_DIR=$OEM_SRC_DIR/Platform/Ampere/"$BOARD_NAME"Pkg
#
# specify the BOARD stepping here
#
BOARD_STEPPING=A2
#
# specify the ROM programing tool here
#
PROGRAMMER_TOOL=#$OEM_SRC_DIR/toolchain/dpcmd
#
if  [ "${BOARD_STEPPING}" == "A1" ]; then
    BUILD_NUMBER_EXT=.A1
    FAILSAFE_WORKAROUND=1
    BOARD_SETTING=$OEM_SRC_DIR/Platform/Ampere/ComHpcAltPkg/ComHpcAltBoardSettingVRWA.cfg
else   
    BUILD_NUMBER_EXT=
    FAILSAFE_WORKAROUND=0
    BOARD_SETTING=$OEM_SRC_DIR/Platform/Ampere/ComHpcAltPkg/ComHpcAltBoardSetting.cfg
fi    

VER=2.04
BUILD=100.02$BUILD_NUMBER_EXT

make -f $WORKSPACE/edk2-ampere-tools/Makefile \
    PROGRAMMER_TOOL=$PROGRAMMER_TOOL \
    POWER_SCRIPT=$OEM_SRC_DIR/toolchain/target_power.sh \
    EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
    BOARD_NAME=$BOARD_NAME \
    VM_SHARED_DIR=$HOME/AmpereR \
    CHECKSUM_TOOL=$OEM_SRC_DIR/toolchain/checksum \
    PACKAGES_PATH=$OEM_SRC_DIR:$WORKSPACE/OpenPlatformPkg:"${PACKAGES_PATH}" \
    ATF_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/atf/altra_atf_signed_2.06.20220308.slim \
    SCP_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/scp/altra_scp_signed_2.06.20220308.slim \
    FAILSAFE_WORKAROUND=$FAILSAFE_WORKAROUND \
    BOARD_SETTING=$BOARD_SETTING \
    LINUXBOOT_BIN=$WORKSPACE/flashkernel \
    SPI_SIZE_MB=32 \
    DEBUG=0 \
    VER=$VER BUILD=$BUILD \
    tianocore_capsule # tianocore_img # linuxboot_img # all # 

make -f $WORKSPACE/edk2-ampere-tools/Makefile \
    EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
    BOARD_NAME=$BOARD_NAME \
    VM_SHARED_DIR=$HOME/AmpereR \
    CHECKSUM_TOOL=$OEM_SRC_DIR/toolchain/checksum \
    SPI_SIZE_MB=32 \
    VER=$VER BUILD=$BUILD \
    history
export PATH=$OPATH
