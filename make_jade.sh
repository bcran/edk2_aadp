#!/bin/bash
OPATH=$PATH
. edk2.sh
make -f $WORKSPACE/edk2-ampere-tools/Makefile \
    BOARD_SETTING=$WORKSPACE/AmpereAltra-ATF-SCP/board_settings/jade_board_setting_2.06.20220308.bin \
    VM_SHARED_DIR=$HOME/AmpereR \
    ATF_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/atf/altra_atf_signed_2.06.20220308.slim \
    SCP_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/scp/altra_scp_signed_2.06.20220308.slim \
    LINUXBOOT_BIN=$WORKSPACE/flashkernel \
    VER=2.04 BUILD=100 \
    tianocore_capsule # tianocore_img # linuxboot_img # all # 
export PATH=$OPATH
