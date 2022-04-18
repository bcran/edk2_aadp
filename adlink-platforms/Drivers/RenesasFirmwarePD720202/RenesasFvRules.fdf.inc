## @file
#
# Copyright (c) 2021, ADLink Technology Inc. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

############################################################################
# This is a Enhancement to FvRules.fdf.inc to supoort ADLink projects      #
############################################################################

[Rule.Common.USER_DEFINED]
  FILE FREEFORM = $(NAMED_GUID) {
    RAW BIN               |.mem
  }

[Rule.Common.USER_DEFINED.BINARY]
  FILE FREEFORM = $(NAMED_GUID) {
    RAW BIN               |.mem
  }
