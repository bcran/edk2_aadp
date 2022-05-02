#!/bin/bash
WORKSPACE="`dirname $0`"
WORKSPACE="`readlink -f \"$WORKSPACE\"`"
if [ $WORKSPACE == /usr/bin ]; then
  WORKSPACE=$PWD
fi
export WORKSPACE
export PACKAGES_PATH=$WORKSPACE/edk2-platforms:$WORKSPACE/edk2-platforms/Features/Intel:$WORKSPACE/edk2:$WORKSPACE/edk2-platforms/Features/Intel/Debugging:
export PYTHON_COMMAND=/usr/bin/python3.6
. edk2/edksetup.sh
if ! [ -d "edk2/BaseTools/Source/C/bin" ]; then
    make -C edk2/BaseTools
fi
