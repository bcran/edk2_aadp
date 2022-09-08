#!/bin/bash
export WORKSPACE=$PWD
DEVELMENT_MODE=0
#
# Firmware Version
#
VER=2.04
BUILD=100.07
DEBUG=0
ATF_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/atf/altra_atf_signed_2.06.20220308.slim
SCP_SLIM=$WORKSPACE/AmpereAltra-ATF-SCP/scp/altra_scp_signed_2.06.20220308.slim

. edk2_adlink-ampere-altra/tools/make_adlink.sh ComHpcAlt A2
