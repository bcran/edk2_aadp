#!/bin/sh

ipmitool -I lanplus -H ${BMC_IP} -U ${BMC_USER} -P ${BMC_PASS} -C 17 chassis power off

#response=$(./pstatus.sh)
#while [ "$response" = "Chassis Power is on" ]; do
#	response=$(./pstatus.sh)
#done
