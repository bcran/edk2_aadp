# Initialize Building Environment

Ampere Mountain Jade code base & tools installation.
* Copy 'setup.sh' to local machine and run. OS used for build environment is Ubuntu 20.04. 
* Follow the instructions to install tools and download source code.
* **3 files in AmpereAltra-ATF-SCP submodule you may not be able to access, if so, please get them from Ampere and place them as below:**
    * *board_settings/jade_board_setting_\*.bin*
    * *atf/altra_atf_signed_\*.slim*
    * *scp/altra_scp_signed_\*.slim*

# Folders
* .vscode: Visual Studio Code settings for tasks which link to bash scripts.
* adlink-platforms: source of ADLINK.
* **AmpereAltra-ATF-SCP**: Ampere ATF/SCP firmware, need authorization to get the content of https://github.com/ADLINK/AmpereAltra-ATF-SCP.git, or get them from Ampere.
* edk2: a submodule from Ampere's edk2 branch.
* edk2-ampere-tools: a submodule from edk2 fork of Ampere's edk2-ampere-tools.
* edk2-platforms: a submodule from Ampere.
* OpenPlatformPkg: a submodule from Linaro.
  
# Files
* buildshell.sh: build uefi shell for AA64.
* clean.sh: clean temporary files and tools.
* edk2.sh: Sample script to set environment variables and run edksetup.sh.
* flashkernel: tiny linux kernel for embedded.
* make_AvaAp1.sh: Sample script to make ADLINK AVA-AP1.
* make_ComHpcAlt.sh: Sample script to make ADLINK COM-HPC-ALT.
* make_jade.sh: Sample script to make CRB Ampere Mt. Jade.
* setup.sh: Sample script to install all source code and tools. Mind your own SSH connection setting if there is any.
    * *setup.sh [TARGET_FOLDER, default=Ampere_Altra]*

# Building EDK-II image

1. Enter into working directory.

2. Execute the below command to build EDK-II image for COM-HPC-ALT

   ```
   . make_[Board].sh
   ```

3. After successful compilation, the final EDK-II image **[Board]_tianocore_atf_*.cap** will be found in below path

   ```
   $PWD/BUILDS/[Board]_tianocore_atf_*/
   ```

# Flashing SCP/EDK-II
* Following Capsule update steps works for v2.04.100.00 or later version, follow the instruction in release package to update BIOS otherwise.

1. Copy **CapsuleApp.efi** , **[Board]_scp_*.cap** , **[Board]_tianocore_atf_*.cap** files from path *$PWD/BUILDS/[Board]_tianocore_atf_*/* into FAT32 USB and connect to target board.

2. Power up the target and boot into UEFI shell. 

3. Run **map -r** command to identify the USB device name. Type the USB device name in the shell to enter into to the USB drive.

   For example: fs0 is USB device name in our case.

   ```
   Shell>fs0:
   fs0:\>
   ```

4. Run below command to flash SCP FW.

   ```
   CapsuleApp.efi [Board]_scp_*.cap
   ```

5. Run below command to flash EDK-II FW v* (this includes ATF + UEFI + BoardSettings).

   ```
   CapsuleApp.efi [Board]_tianocore_atf_*.cap
   ```

6. Power cycle the target board.

NOTE:

- The target board by default should have EDK-II firmware version greater than or equal to v2.04.100.00 (which will support capsule update).

### Flashing Capsule Supported EDK-II Firmware

1. Go to [Ask a Expert page](https://www.adlinktech.com/en/Askanexpert) or [AVA Developer Platform Forum](https://www.ipi.wiki/community/forum/ava-developer-platform) where you can request us and then we will provide the download link
2. Download & Unzip into FAT32 USB device.
3. Boot into UEFI shell
4. Run fwu.nsh to flash EDK-II to v2.04.100.00 which will support capsule update.
5. Power cycle the target board.

NOTE: 

- The above steps required only when the target is having EDK-II firmware version lesser than v2.04.100.00.
# POST code:
* Progress Map 
  * DXE_CORE_STARTED, 0x60 },
  * DXE_SBRUN_INIT, 0x62 },
  * DXE_NB_HB_INIT, 0x68 },
  * DXE_NB_INIT, 0x69 },
  * DXE_NB_SMM_INIT, 0x6A },
  * DXE_SB_INIT, 0x70 },
  * DXE_SB_SMM_INIT, 0x71 },
  * DXE_SB_DEVICES_INIT, 0x72 },
  * DXE_BDS_STARTED, 0x90 },
  * DXE_PCI_BUS_BEGIN, 0x92 },
  * DXE_PCI_BUS_HPC_INIT, 0x93 },
  * DXE_PCI_BUS_ENUM, 0x94 },
  * DXE_PCI_BUS_REQUEST_RESOURCES, 0x95 },
  * DXE_PCI_BUS_ASSIGN_RESOURCES, 0x96 },
  * DXE_CON_OUT_CONNECT, 0x97 },
  * DXE_CON_IN_CONNECT, 0x98 },
  * DXE_SIO_INIT, 0x99 },
  * DXE_USB_BEGIN, 0x9A },
  * DXE_USB_RESET, 0x9B },
  * DXE_USB_DETECT, 0x9C },
  * DXE_USB_ENABLE, 0x9D },
  * DXE_IDE_BEGIN, 0xA1 },
  * DXE_IDE_RESET, 0xA2 },
  * DXE_IDE_DETECT, 0xA3 },
  * DXE_IDE_ENABLE, 0xA4 },
  * DXE_SCSI_BEGIN, 0xA5 },
  * DXE_SCSI_RESET, 0xA6 },
  * DXE_SCSI_DETECT, 0xA7 },
  * DXE_SCSI_ENABLE, 0xA8 },
  * DXE_SETUP_START, 0xAB },
  * DXE_SETUP_INPUT_WAIT, 0xAC },
  * DXE_READY_TO_BOOT, 0xAD },
  * DXE_LEGACY_BOOT, 0xAE },
  * DXE_EXIT_BOOT_SERVICES, 0xAF },
  * RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN, 0xB0 },
  * RT_SET_VIRTUAL_ADDRESS_MAP_END, 0xB1 },
  * DXE_LEGACY_OPROM_INIT, 0xB2 },
  * DXE_RESET_SYSTEM, 0xB3 },
  * DXE_USB_HOTPLUG, 0xB4 },
  * DXE_PCI_BUS_HOTPLUG, 0xB5 },

* Error Map
  * DXE_CPU_SELF_TEST_FAILED, 0x58 },
  * DXE_NB_ERROR, 0xD1 },
  * DXE_SB_ERROR, 0xD2 },
  * DXE_ARCH_PROTOCOL_NOT_AVAILABLE, 0xD3 },
  * DXE_PCI_BUS_OUT_OF_RESOURCES, 0xD4 },
  * DXE_LEGACY_OPROM_NO_SPACE, 0xD5 },
  * DXE_NO_CON_OUT, 0xD6 },
  * DXE_NO_CON_IN, 0xD7 },
  * DXE_INVALID_PASSWORD, 0xD8 },
  * DXE_BOOT_OPTION_LOAD_ERROR, 0xD9 },
  * DXE_BOOT_OPTION_FAILED, 0xDA },
  * DXE_FLASH_UPDATE_FAILED, 0xDB },
  * DXE_RESET_NOT_AVAILABLE, 0xDC },
