#!/bin/bash
export WORKSPACE=$PWD
#
# DEVEL_MODE 
# 1. disable PXE/HTTP boot
# 2. no BIOS capsule generated
# 3. generate .img only.
#
DEVEL_MODE=0
#
# Firmware Version
#
  VER=2.09
# BUILD=100.00
# DEBUG=0

. edk2_adlink-ampere-altra/tools/make_adlink.sh ComHpcAlt A2
