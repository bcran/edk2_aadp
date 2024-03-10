# Building the EDK2 UEFI firmware for ADLINK Ampere Altra Systems

## ADLINK COM-HPC-ALT code base & tools installation

To download the source code, run:
```
git clone --recursive https://github.com/bcran/edk2_aadp
```

The following tools are required to build the firmware:
  * Compiler: at the moment only gcc is supported. Install the native gcc, and if cross-building also install the aarch64 package
  * iasl: install the ACPI Component Architecture (ACPICA) tools
  * cert_create and fiptool. These may be found in packages in your distro (e.g. arm-trusted-firmware-tools) or you
    can build them from https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git
  * Bash
  * GNU make
  * Python 3.x

There are 2 files which are required to build the firmware, but which are only available from Ampere under NDA:
  * altra_atf_signed_\*.slim
  * altra_scp_signed_\*.slim

## Folders
* .vscode: Visual Studio Code settings for tasks which link to bash scripts
* edk2: a submodule from the TianoCore project
* edk2-ampere-tools: a submodule from edk2 fork of Ampere's edk2-ampere-tools
* edk2-platforms: a submodule from Ampere
* OpenPlatformPkg: a submodule from Linaro, required for the Renesas USB firmware
  
## Files
* bld.sh: Script to build the ADLINK COM-HPC-ALT UEFI firmware.
* IPMI scripts: define ${BMC\_IP}, ${BMC\_USER} and ${BMC\_PASS} to use them.
  * pon.sh: IPMI script to power on the system.
  * poff.sh: IPMI script to power off the system.
  * pstatus.sh: IPMI script to fetch the power status of the system.
* flash.sh: Script which uses the Linux Dediprog software to flash the firmware to a SPI-NOR EEPROM.

## Building EDK2 image

1. Enter into working directory.

2. Execute the below command to build EDK-II image for COM-HPC-ALT

   ```
   ./bld.sh
   ```

3. After successful compilation, the final EDK2 image **comhpcalt_tianocore_tfa_*.cap** will be found in below path:

   ```
   $PWD/Build/ComHpcAlt/comhpcalt_tianocore_tfa_{debug,release}_{date}-{build-number}.{cap,bin.img}
   ```

## Flashing SCP/EDK-II
* Following Capsule update steps works for v0.1-bexcran or later version, follow the instruction in release package to update BIOS otherwise.

1. Copy **CapsuleApp.efi** , **BoardVersion.efi**, **comhpcalt_scp_*.cap** , **comhpcalt_tianocore_tfa_*.cap** files from path *$PWD/Build/ComHpcAlt/* into FAT32 USB and connect to target board.

2. Power up the target and boot into UEFI shell. 

3. Run **map -r** command to identify the USB device name. Type the USB device name in the shell to enter into to the USB drive.

   For example: fs0 is USB device name in our case.

   ```
   Shell>fs0:
   fs0:\>
   ```

4. Run below command to flash SCP FW.

   ```
   CapsuleApp.efi comhpcalt_scp_*.cap
   ```

5. Run below command to flash EDK2 FW v* (this includes TF-A + UEFI + BoardSettings).

   ```
   CapsuleApp.efi comhpcalt_tianocore_tfa_*.cap
   ```

6. Power cycle the target board.

## POST code: Below list is utilized from Intel platform, could be refined if needed.
Progress Map:
```
  //
  // PEI
  //
  //Regular boot
  { PEI_CORE_STARTED, 0x10 },
  { PEI_CAR_CPU_INIT, 0x11 },
  { PEI_MEMORY_SPD_READ, 0x1D },
  { PEI_MEMORY_PRESENCE_DETECT, 0x1E },
  { PEI_MEMORY_TIMING, 0x1F},
  { PEI_MEMORY_CONFIGURING, 0x20 },
  { PEI_MEMORY_INIT, 0x21 },
  { PEI_MEMORY_INSTALLED, 0x31 },
  { PEI_CPU_INIT,  0x32 },
  { PEI_CPU_CACHE_INIT, 0x33 },
  { PEI_CPU_BSP_SELECT, 0x34 },
  { PEI_CPU_AP_INIT, 0x35 },
  { PEI_CPU_SMM_INIT, 0x36 },
  { PEI_MEM_NB_INIT, 0x37 },
  { PEI_MEM_SB_INIT, 0x3B },
  { PEI_DXE_IPL_STARTED, 0x4F },
  //
  // DXE
  //
  { DXE_CORE_STARTED, 0x60 },
  { DXE_SBRUN_INIT, 0x62 },
  { DXE_NB_HB_INIT, 0x68 },
  { DXE_NB_INIT, 0x69 },
  { DXE_NB_SMM_INIT, 0x6A },
  { DXE_SB_INIT, 0x70 },
  { DXE_SB_SMM_INIT, 0x71 },
  { DXE_SB_DEVICES_INIT, 0x72 },
  { DXE_BDS_STARTED, 0x90 },
  { DXE_PCI_BUS_BEGIN, 0x92 },
  { DXE_PCI_BUS_HPC_INIT, 0x93 },
  { DXE_PCI_BUS_ENUM, 0x94 },
  { DXE_PCI_BUS_REQUEST_RESOURCES, 0x95 },
  { DXE_PCI_BUS_ASSIGN_RESOURCES, 0x96 },
  { DXE_CON_OUT_CONNECT, 0x97 },
  { DXE_CON_IN_CONNECT, 0x98 },
  { DXE_SIO_INIT, 0x99 },
  { DXE_USB_BEGIN, 0x9A },
  { DXE_USB_RESET, 0x9B },
  { DXE_USB_DETECT, 0x9C },
  { DXE_USB_ENABLE, 0x9D },
  { DXE_IDE_BEGIN, 0xA1 },
  { DXE_IDE_RESET, 0xA2 },
  { DXE_IDE_DETECT, 0xA3 },
  { DXE_IDE_ENABLE, 0xA4 },
  { DXE_SCSI_BEGIN, 0xA5 },
  { DXE_SCSI_RESET, 0xA6 },
  { DXE_SCSI_DETECT, 0xA7 },
  { DXE_SCSI_ENABLE, 0xA8 },
  { DXE_SETUP_START, 0xAB },
  { DXE_SETUP_INPUT_WAIT, 0xAC },
  { DXE_READY_TO_BOOT, 0xAD },
  { DXE_LEGACY_BOOT, 0xAE },
  { DXE_EXIT_BOOT_SERVICES, 0xAF },
  { RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN, 0xB0 },
  { RT_SET_VIRTUAL_ADDRESS_MAP_END, 0xB1 },
  { DXE_LEGACY_OPROM_INIT, 0xB2 },
  { DXE_RESET_SYSTEM, 0xB3 },
  { DXE_USB_HOTPLUG, 0xB4 },
  { DXE_PCI_BUS_HOTPLUG, 0xB5 },
```

Error Map:
```
  { DXE_CPU_SELF_TEST_FAILED, 0x58 },
  { DXE_NB_ERROR, 0xD1 },
  { DXE_SB_ERROR, 0xD2 },
  { DXE_ARCH_PROTOCOL_NOT_AVAILABLE, 0xD3 },
  { DXE_PCI_BUS_OUT_OF_RESOURCES, 0xD4 },
  { DXE_LEGACY_OPROM_NO_SPACE, 0xD5 },
  { DXE_NO_CON_OUT, 0xD6 },
  { DXE_NO_CON_IN, 0xD7 },
  { DXE_INVALID_PASSWORD, 0xD8 },
  { DXE_BOOT_OPTION_LOAD_ERROR, 0xD9 },
  { DXE_BOOT_OPTION_FAILED, 0xDA },
  { DXE_FLASH_UPDATE_FAILED, 0xDB },
  { DXE_RESET_NOT_AVAILABLE, 0xDC },
```
