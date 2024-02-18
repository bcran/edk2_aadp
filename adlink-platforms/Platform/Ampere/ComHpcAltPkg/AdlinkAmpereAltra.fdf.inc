## @file
#
# Copyright (c) 2022, ADLink. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##


  #
  # USB 3.0 Renesas μPD70220x
  #
  INF Drivers/Xhci/RenesasFirmwarePD720202/RenesasFirmwarePD720202.inf
  FILE FREEFORM = A059EBC4-D73D-4279-81BF-E4A89308B923 {
    SECTION RAW = Drivers/RenesasFirmwarePD720202/K2026090.mem
  }
  #
  # set MMC power off type
  #
  INF Drivers/MmcSetPowerOffType/Dxe/MmcSetPowerOffTypeDxe.inf
  INF Drivers/MmcSetPowerOffType/Pei/MmcSetPowerOffTypePei.inf
