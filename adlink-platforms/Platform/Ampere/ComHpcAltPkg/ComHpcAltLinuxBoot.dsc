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
  PLATFORM_NAME                  = ComHpcAlt
  PLATFORM_GUID                  = BDFD62FB-6372-4475-92D0-7DA694395449
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x0001001B
  OUTPUT_DIRECTORY               = Build/ComHpcAlt
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Platform/Ampere/ComHpcAltPkg/ComHpcAltLinuxBoot.fdf

  #
  # Defines for default states.  These can be changed on the command line.
  # -D FLAG=VALUE
  #
  DEFINE DEBUG_PRINT_ERROR_LEVEL = 0x8000000F
  DEFINE FIRMWARE_VER            = 2.04.100
  DEFINE EDK2_SKIP_PEICORE       = TRUE

!include MdePkg/MdeLibs.dsc.inc

# Include default Ampere Platform DSC file
!include Silicon/Ampere/AmpereAltraPkg/AmpereAltraLinuxBootPkg.dsc.inc

#
# Specific Platform Library
#
[LibraryClasses.common]
  #
  # ACPI Libraries
  #
  AcpiLib|EmbeddedPkg/Library/AcpiLib/AcpiLib.inf
  AcpiHelperLib|Platform/Ampere/AmperePlatformPkg/Library/AcpiHelperLib/AcpiHelperLib.inf

  #
  # Pcie Board
  #
  BoardPcieLib|Platform/Ampere/ComHpcAltPkg/Library/BoardPcieLib/BoardPcieLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  #
  # RTC Library: Common RTC
  #
  RealTimeClockLib|Platform/Ampere/ComHpcAltPkg/Library/PCF8563RealTimeClockLib/PCF8563RealTimeClockLib.inf

[LibraryClasses.common.UEFI_DRIVER, LibraryClasses.common.UEFI_APPLICATION, LibraryClasses.common.DXE_RUNTIME_DRIVER, LibraryClasses.common.DXE_DRIVER]
  SmbusLib|Platform/Ampere/JadePkg/Library/DxePlatformSmbusLib/DxePlatformSmbusLib.inf

[PcdsDynamicDefault.common.DEFAULT]
  # SMBIOS Type 0 - BIOS Information
  gAmpereTokenSpaceGuid.PcdSmbiosTables0BiosReleaseDate|"MM/DD/YYYY"

[PcdsFixedAtBuild.common]
  gAmpereTokenSpaceGuid.PcdSmbiosTables1MajorVersion|$(MAJOR_VER)
  gAmpereTokenSpaceGuid.PcdSmbiosTables1MinorVersion|$(MINOR_VER)

  # Clearing BIT0 in this PCD prevents installing a 32-bit SMBIOS entry point,
  # if the entry point version is >= 3.0. AARCH64 OSes cannot assume the
  # presence of the 32-bit entry point anyway (because many AARCH64 systems
  # don't have 32-bit addressable physical RAM), and the additional allocations
  # below 4 GB needlessly fragment the memory map. So expose the 64-bit entry
  # point only, for entry point versions >= 3.0.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSmbiosEntryPointProvideMethod|0x2

#
# Specific Platform Component
#
[Components.common]

  #
  # ACPI
  #
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  Platform/Ampere/JadePkg/Drivers/AcpiPlatformDxe/AcpiPlatformDxe.inf
  Silicon/Ampere/AmpereAltraPkg/AcpiCommonTables/AcpiCommonTables.inf
  Platform/Ampere/ComHpcAltPkg/AcpiTables/AcpiTables.inf
  Platform/Ampere/JadePkg/Ac02AcpiTables/Ac02AcpiTables.inf

  #
  # BDS
  #
  Platform/Ampere/ComHpcAltPkg/Universal/BdsDxe/BdsDxe.inf

  #
  # SMBIOS
  #
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  Platform/Ampere/ComHpcAltPkg/Drivers/SmbiosPlatformDxe/SmbiosPlatformDxe.inf
  Platform/Ampere/JadePkg/Drivers/SmbiosCpuDxe/SmbiosCpuDxe.inf
  Platform/Ampere/JadePkg/Drivers/SmbiosMemInfoDxe/SmbiosMemInfoDxe.inf

  #
  # FailSafeDxe added to prevent watchdog from resetting the system
  #
  Silicon/Ampere/AmpereAltraPkg/Drivers/FailSafeDxe/FailSafeDxe.inf

  #
  # USB 3.0 Renesas μPD70220x
  #
  Drivers/Xhci/RenesasFirmwarePD720202/RenesasFirmwarePD720202.inf {
    <LibraryClasses>
      DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  } 
