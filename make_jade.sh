#!/bin/bash
OPATH=$PATH
. edk2.sh
make -f $WORKSPACE/edk2-ampere-tools/Makefile \
    BOARD_SETTING=$WORKSPACE/AmpereAltra-ATF-SCP/board_settings/jade_board_setting_1.07.20210713.bin \
    VM_SHARED_DIR=$HOME/AmpereR \
    PACKAGES_PATH=$WORKSPACE/adlink-platforms:"${PACKAGES_PATH}" \
    ATF_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/atf/altra_atf_signed_1.07.20210713.slim \
    SCP_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/scp/altra_scp_signed_1.07.20210713.slim \
    LINUXBOOT_BIN=$WORKSPACE/flashkernel \
    VER=2.04 BUILD=100 \
    tianocore_capsule # tianocore_img # linuxboot_img # all # 
export PATH=$OPATH
