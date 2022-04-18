#!/bin/bash
OPATH=$PATH
. edk2.sh
make -f $WORKSPACE/edk2-ampere-tools/Makefile clean
export PATH=$OPATH
