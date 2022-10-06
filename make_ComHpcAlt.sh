#!/bin/bash
export WORKSPACE=$PWD
DEVELMENT_MODE=0
#
# Firmware Version
#
VER=2.04
BUILD=100.07
ATF_SCP_VER=2.06.20220308
DEBUG=0

. edk2_adlink-ampere-altra/tools/make_adlink.sh ComHpcAlt A2
