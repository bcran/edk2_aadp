#!/bin/sh

ipmitool -I lanplus -H ${BMC_IP} -U ${BMC_USER} -P ${BMC_PASS} -C 17 chassis power status
