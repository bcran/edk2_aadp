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
* boardStepping.py: tool to switch A1/A2 board stepping setting in make script.
* buildshell.sh: build uefi shell for AA64.
* clean.sh: clean temporary files and tools.
* edk2.sh: Sample script to set environment variables and run edksetup.sh.
* flashkernel: tiny linux kernel for embedded.
* make_ComHpcAlt.sh: Sample script to make ADLINK COM-HPC-ALT.
* make_jade.sh: Sample script to make CRB Ampere Mt. Jade.
* make_A1A2.sh: make A1/A2+ BIOSes all at once.
* setup.sh: Sample script to install all source code and tools. Mind your own SSH connection setting if there is any.
    * *setup.sh [TARGET_FOLDER, default=Ampere_Altra]*

# Building EDK-II image

1. Enter into working directory.

2. Execute the below command to build EDK-II image for COM-HPC-ALT

   ```
   . make_ComHpcAlt.sh
   ```

3. After successful compilation, the final EDK-II image **ComHpcAlt_tianocore_atf_*.cap** will be found in below path

   ```
   $PWD/BUILDS/ComHpcAlt_tianocore_atf_*/
   ```

# Flashing SCP/EDK-II
* Following Capsule update steps works for v2.04.100.00 or later version, follow the instruction in release package to update BIOS otherwise.

1. Copy **CapsuleApp.efi** , **ComHpcAlt_scp_*.cap** , **ComHpcAlt_tianocore_atf_*.cap** files from path *$PWD/BUILDS/ComHpcAlt_tianocore_atf_*/* into FAT32 USB and connect to target board.

2. Power up the target and boot into UEFI shell. 

3. Run **map -r** command to identify the USB device name. Type the USB device name in the shell to enter into to the USB drive.

   For example: fs0 is USB device name in our case.

   ```
   Shell>fs0:
   fs0:\>
   ```

4. Run below command to flash SCP FW.

   ```
   CapsuleApp.efi ComHpcAlt_scp_*.cap
   ```

5. Run below command to flash EDK-II FW v* (this includes ATF + UEFI + BoardSettings).

   ```
   CapsuleApp.efi ComHpcAlt_tianocore_atf_*.cap
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
