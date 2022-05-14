## @file
#
# Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = AvaAp1
  PLATFORM_GUID                  = 7787CE84-03ED-444E-9E0F-F8A99C865951
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x0001001B
  OUTPUT_DIRECTORY               = Build/AvaAp1
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Platform/Ampere/AvaAp1Pkg/AvaAp1Capsule.fdf

  #
  # Defines for default states.  These can be changed on the command line.
  # -D FLAG=VALUE
  #
  DEFINE UEFI_ATF_IMAGE          = Build/AvaAp1/AvaAp1_tianocore_atf.img
  DEFINE SCP_IMAGE               = Build/AvaAp1/AvaAp1_scp.slim
