# This sample script is for target power control via USB HID Relay & USB CH340 Relay
# check detailed setting in adlink-platforms/toolchain/README.md

# uncomment and modify to adapt environment for USB Relay HID
#
# if command -v usbrelay &> /dev/null; then
#     if [ $1 == 'ON' ]; then
#         usbrelay HW348_1=0
#     else    
#         usbrelay HW348_1=1
#         sleep 3
#     fi    
# fi    

# uncomment and modify to adapt environment for USB Relay CH340

# if command -v adlink-platforms/toolchain/UsbRelayCh340 &> /dev/null; then
#     if [ $1 == 'ON' ]; then
#         adlink-platforms/toolchain/UsbRelayCh340 /dev/ttyUSB0 NC
#     else    
#         adlink-platforms/toolchain/UsbRelayCh340 /dev/ttyUSB0 NO
#         sleep 3
#     fi    
# fi    
