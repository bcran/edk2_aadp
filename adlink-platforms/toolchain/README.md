# ADLink tools
* checksum: provides tradtional 8 digits checksum of a file, source: https://github.com/adlinktech-philxing/checksum_gcc.git
* dpcmd: Dediprog Command Line, make from https://github.com/DediProgSW/SF100Linux, this will works with VM if the Dediprog device is shared with the VM by the host system.
* ChipInfoDb.dedicfg: Dediprog Command Line Chip info. database, made and work with "dpcmd".
* myVendor.rules: copy to /etc/udev/rules.d/ directory and reboot the system to skip sudo password for "dpcmd" running via enabling Dediprog USB device to all users.
  