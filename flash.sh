#!/usr/bin/env bash

set -e

if [ $# -eq 0 ]; then
	echo "Error: No filename specified."
	echo "usage: $0 <filename>"
	exit 1
elif [ $# -ne 1 ]; then
	echo "Error: usage: $0 <filename>"
	exit 1
fi

FILENAME=$1

# For a Dediprog SF600G2 Plus, a SPI clock of 24 MHz works well.
# For other SPI programmers, a slower clock might be required.
#
# Options for --spi-clk are:
#
# 2, 12 MHz(Default)
# 0, 24 MHz
# 1, 8 MHz
# 3, 3 MHz
# 4, 2.18 MHz
# 5, 1.5 MHz
# 6, 750 KHz
# 7, 375 KHz

./edk2_adlink-ampere-altra/tools/dpcmd \
	--spi-clk 0                    \
	-v                             \
	-a 0x400000                    \
	-u ${FILENAME}

# Add -a 0x400000 for the .img files

# Note. The .img is a smaller file with just the code.
# The .bin is the full 32MB image.

# Add -e to the dpcmd commandline to run a full erase before programming.
