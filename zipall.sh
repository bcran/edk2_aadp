#!/bin/bash
basedir="$(basename "$PWD")"
zfname="fullsource"
cd ..
#
# make zip, to extract: unzip $zfname..zip [-d target_folder]
# disable to save space.
#
# if [ -f "$zfname.zip" ]; then
#     rm "$zfname.zip"
# fi
# zip -r "$zfname.zip" $basedir -9 -x "*Build*" -x "*BUILDS*" -x "*.git*" -x "edk2-ampere-tools/toolchain"

#
# make gz, to extract: tar zxvf $zfname.tar.gz [-C target_folder] 
#
if [ -f "$zfname.tar.gz" ]; then
    rm "$zfname.tar.gz"
fi    
tar -zcvpf "$zfname.tar.gz" --exclude={"Build","BUILDS",".git","edk2-ampere-tools/toolchain"} $basedir
# tar -zcvpf "$zfname.tar.gz" --exclude={"Build","BUILDS","edk2-ampere-tools/toolchain"} $basedir

cd $basedir
