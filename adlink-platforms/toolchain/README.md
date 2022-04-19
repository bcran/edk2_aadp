# ADLink tools
* checksum: provides tradtional 8 digits checksum of a file, source: https://github.com/adlinktech-philxing/checksum_gcc.git
* Dediprog SPI Programmer
  * dpcmd: Dediprog Command Line, make from https://github.com/DediProgSW/SF100Linux, this will works with VM if the Dediprog device is shared with the VM by the host system.
  * ChipInfoDb.dedicfg: Dediprog Command Line Chip info. database, made and work with "dpcmd".
  * sudo cp dediprog.rules /etc/udev/rules.d
  * reboot
* USB Relay CH340: https://github.com/adlinktech-philxing/UsbRelayCh340
  * sudo cp 70-ttyusb.rules /etc/udev/rules.d & sudo udevadm control --reload-rules && udevadm trigger
  * reboot
  * Usage: UsbRelayCh340 /dev/ttyUSB0 [NC|NO]
* USB Relay HID: https://github.com/darrylb123/usbrelay
  * sudo apt install -y usbrelay
  * sudo cp 50-usbrelay.rules /etc/udev/rules.d & sudo udevadm control --reload-rules && udevadm trigger
  * reboot
  * get relay ID by running usbrelay, get it via the last line of the output with its current state.
  * Usage: usbrelay HW348_1=[0|1]; 0=NC, 1=NO
  