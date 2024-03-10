#!/usr/bin/env bash

set -e

tfa_usage () {
  echo -n "cert_create or fiptool could not be found. If running on a "
  echo -n "Debian-based distro (e.g. Ubuntu) you can get them by "
  echo    "installing the arm-trusted-firmware-tools package."
  echo -n "Otherwise, you can build them and add them to the \$PATH by "
  echo    "running the following commands:"
  echo
  echo "  git clone --depth 1 https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git"
  echo "  pushd trusted-firmware-a"
  echo "  ${MAKE_COMMAND} CC=cc fiptool"
  echo "  ${MAKE_COMMAND} CC=cc certtool"
  echo "  export PATH=\$PWD/tools/cert_create:\$PWD/tools/fiptool:\$PATH"
  echo "  popd"
  exit 1
}

ATF_SLIM=$PWD/altra_atf_signed_2.10.20230517.slim
SCP_SLIM=$PWD/altra_scp_signed_2.10.20230517.slim
SPI_SIZE_MB=32

BOARD_NAME=ComHpcAlt
OUTPUT_BIN_DIR=$PWD/Build/${BOARD_NAME}
BOARD_SETTINGS_CFG=edk2-platforms/Platform/Ampere/${BOARD_NAME}Pkg/${BOARD_NAME}BoardSetting.cfg
SCRIPTS_DIR=$PWD/edk2-ampere-tools/
EDK_PLATFORMS_PKG_DIR=$PWD/edk2-platforms/Platform/Ampere/${BOARD_NAME}Pkg
OUTPUT_BOARD_SETTINGS_BIN=${OUTPUT_BIN_DIR}/$(basename ${BOARD_SETTINGS_CFG}).bin

TOOLCHAIN=GCC
BLDTYPE=RELEASE
BUILD_THREADS=$(getconf _NPROCESSORS_ONLN)
export PYTHON_COMMAND=python3
export WORKSPACE=$PWD
export PACKAGES_PATH=$PWD/edk2:$PWD:$PWD/edk2-platforms:$PWD/edk2-platforms/Platform/Ampere/ComHpcAltPkg:$PWD/OpenPlatformPkg:$PWD/edk2-platforms/Features/Intel/Debugging:$PWD/edk2-platforms/Features:$PWD/edk2-platforms/Features/Intel

if [ `uname -o` = "FreeBSD" ]; then
  MAKE_COMMAND=gmake
  GETOPT_COMMAND=/usr/local/bin/getopt
  if ! command -v ${MAKE_COMMAND} >/dev/null 2>&1; then
    echo "GNU make is required. Please install the gmake package."
    exit 1
  fi
  mkdir bin || true
  ln -sfv /usr/local/bin/make bin/${MAKE_COMMAND}
  export PATH=$PWD/bin:$PATH
else
  MAKE_COMMAND=make
  GETOPT_COMMAND=getopt
fi

if [ ! -e ${ATF_SLIM} ]; then
  echo "The TF-A (Trusted Firmware) binary ${ATF_SLIM} doesn't exist."
  echo "Please download it from Ampere Customer Connect (https://amperecomputing.com/customer-connect)."
  exit 1
fi

if [ ! -e ${SCP_SLIM} ]; then
  echo "The SCP (System Control Processor) binary ${SCP_SLIM} doesn't exist."
  echo "Please download it from Ampere Customer Connect (https://amperecomputing.com/customer-connect)."
  exit 1
fi

if ! command -v cert_create >/dev/null 2>&1; then
  tfa_usage
fi

if ! command -v fiptool >/dev/null 2>&1; then
  tfa_usage
fi

if ! command -v python3 >/dev/null 2>&1; then
  echo "Could not find python3. Please install the python3 package."
  exit 1
fi

OPTIONS=`${GETOPT_COMMAND} -o t:b: --long toolchain:,build: -- "$@"`
eval set -- "$OPTIONS"

while true; do
  case "$1" in
    -t|--toolchain)
      TOOLCHAIN=$2; shift 2;;
    -b|--build)
      BLDTYPE=$2; shift 2;;
    --) shift; break;;
    *) echo "Internal error!"; exit 1;;
  esac
done

eval set -- ""

case `uname -m` in
  "x86_64")
    if [ "$TOOLCHAIN" = "GCC" -a -z ${GCC_AARCH64_PREFIX} ]; then
      echo "Error: need to define \$GCC_AARCH64_PREFIX since the native compiler won't work"
      exit 1
    fi
    ;;
esac

if [ ${TOOLCHAIN} = "CLANG" ]; then
  TOOLCHAIN=CLANGDWARF
fi

mkdir -p ${OUTPUT_BIN_DIR}
cp -v ${BOARD_SETTINGS_CFG} ${OUTPUT_BIN_DIR}/$(basename ${BOARD_SETTINGS_CFG}).txt
python3 ${SCRIPTS_DIR}/nvparam.py -f ${BOARD_SETTINGS_CFG} -o ${OUTPUT_BOARD_SETTINGS_BIN}
rm -fv ${OUTPUT_BOARD_SETTINGS_BIN}.padded

. ./fw_ver UPDATE
. edk2/edksetup.sh

build -a AARCH64 -t ${TOOLCHAIN} -p MultiArchUefiPkg/Emulator.dsc -b ${BLDTYPE}

build -a AARCH64 -t ${TOOLCHAIN} -b ${BLDTYPE} -n ${BUILD_THREADS} \
        -D FIRMWARE_VER="${VER}-${BUILD} TF-A 2.10"                \
        -D MAJOR_VER=${MAJOR_VER} -D MINOR_VER=${MINOR_VER}        \
        -D SECURE_BOOT_ENABLE=TRUE               \
        -D NETWORK_ENABLE=TRUE                   \
        -D INCLUDE_TFTP_COMMAND=TRUE             \
        -D NETWORK_IP6_ENABLE=TRUE               \
        -D NETWORK_ALLOW_HTTP_CONNECTIONS=TRUE   \
        -D NETWORK_TLS_ENABLE=TRUE               \
        -D REDFISH_ENABLE=TRUE                   \
        -p Platform/Ampere/${BOARD_NAME}Pkg/${BOARD_NAME}.dsc

OUTPUT_BASENAME=${OUTPUT_BIN_DIR}/${BOARD_NAME,,}_tianocore_tfa_${BLDTYPE,,}_${VER}-${BUILD}
OUTPUT_RAW_IMAGE=${OUTPUT_BASENAME}.raw

# Create a 2MB file with 0xff
dd bs=1024 count=2048 if=/dev/zero | tr "\000" "\377" > ${OUTPUT_RAW_IMAGE}
dd bs=1024 seek=0 conv=notrunc if=${ATF_SLIM} of=${OUTPUT_RAW_IMAGE}
dd bs=1 seek=2031616 conv=notrunc if=${OUTPUT_BOARD_SETTINGS_BIN} of=${OUTPUT_RAW_IMAGE}

OUTPUT_FD_IMAGE=${OUTPUT_BASENAME}.fd

cp -v Build/${BOARD_NAME}/${BLDTYPE}_${TOOLCHAIN}/FV/BL33_${BOARD_NAME^^}_UEFI.fd ${OUTPUT_FD_IMAGE}

# Sign FD
DBB_KEY=${EDK_PLATFORMS_PKG_DIR}/TestKeys/Dbb_AmpereTest.priv.pem
cert_create -n --ntfw-nvctr 0 --key-alg rsa --nt-fw-key ${DBB_KEY} --nt-fw-cert ${OUTPUT_FD_IMAGE}.crt --nt-fw ${OUTPUT_FD_IMAGE}
fiptool create --nt-fw-cert ${OUTPUT_FD_IMAGE}.crt --nt-fw ${OUTPUT_FD_IMAGE} ${OUTPUT_FD_IMAGE}.signed
rm -fv ${OUTPUT_FD_IMAGE}.crt

dd bs=1024 seek=2048 if=${OUTPUT_FD_IMAGE}.signed of=${OUTPUT_RAW_IMAGE}
rm -fv ${OUTPUT_FD_IMAGE}.signed
cp -fv ${OUTPUT_RAW_IMAGE} ${OUTPUT_BASENAME}.img

rm -fv ${OUTPUT_RAW_IMAGE} ${OUTPUT_FD_IMAGE} ${OUTPUT_BOARD_SETTINGS_BIN}

dd bs=1M count=${SPI_SIZE_MB} if=/dev/zero | tr "\000" "\377" > ${OUTPUT_BASENAME}.bin
dd conv=notrunc bs=1M seek=4 if=${OUTPUT_BASENAME}.img of=${OUTPUT_BASENAME}.bin

mkdir -p Build/${BOARD_NAME} || true
cp -vf ${OUTPUT_BASENAME}.img Build/${BOARD_NAME}/${BOARD_NAME}_tianocore_tfa.img

# Build the capsule (for upgrading from the UEFI Shell or Linux)
build -a AARCH64 -t ${TOOLCHAIN} -b ${BLDTYPE} -n 8 \
        -D FIRMWARE_VER="${VER}-${BUILD} TF-A 2.10" \
        -D MAJOR_VER=${MAJOR_VER} -D MINOR_VER=${MINOR_VER} -D SECURE_BOOT_ENABLE -D EMU_X64_RAZ_WI_PIO=YES \
        -p Platform/Ampere/${BOARD_NAME}Pkg/${BOARD_NAME}Capsule.dsc

cp -vf Build/${BOARD_NAME}/${BLDTYPE}_${TOOLCHAIN}/FV/${BOARD_NAME^^}UEFIATFFIRMWAREUPDATECAPSULEFMPPKCS7.Cap ${OUTPUT_BASENAME}.cap
cp -vf Build/${BOARD_NAME}/${BLDTYPE}_${TOOLCHAIN}/FV/JADESCPFIRMWAREUPDATECAPSULEFMPPKCS7.Cap ${OUTPUT_BIN_DIR}/${BOARD_NAME,,}_scp_upgrade.cap

echo "Done. Firmware is in ${OUTPUT_BASENAME}.{bin,img,cap}"

# ./flash.sh ${OUTPUT_BASENAME}.img

# .img - SPI NOR image starting at 4MB
# .bin - SPI NOR image starting at 0MB

