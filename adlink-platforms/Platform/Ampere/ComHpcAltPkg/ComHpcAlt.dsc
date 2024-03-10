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
  PLATFORM_GUID                  = A4365AA5-0696-4E90-BB5A-ABC1BF6BFAB0
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x0001001B
  OUTPUT_DIRECTORY               = Build/ComHpcAlt
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Platform/Ampere/ComHpcAltPkg/ComHpcAlt.fdf

  #
  # Defines for default states. These can be changed on the command line.
  # -D FLAG=VALUE
  #

  #  DEBUG_INIT      0x00000001  // Initialization
  #  DEBUG_WARN      0x00000002  // Warnings
  #  DEBUG_LOAD      0x00000004  // Load events
  #  DEBUG_FS        0x00000008  // EFI File system
  #  DEBUG_POOL      0x00000010  // Alloc & Free (pool)
  #  DEBUG_PAGE      0x00000020  // Alloc & Free (page)
  #  DEBUG_INFO      0x00000040  // Informational debug messages
  #  DEBUG_DISPATCH  0x00000080  // PEI/DXE/SMM Dispatchers
  #  DEBUG_VARIABLE  0x00000100  // Variable
  #  DEBUG_BM        0x00000400  // Boot Manager
  #  DEBUG_BLKIO     0x00001000  // BlkIo Driver
  #  DEBUG_NET       0x00004000  // SNP Driver
  #  DEBUG_UNDI      0x00010000  // UNDI Driver
  #  DEBUG_LOADFILE  0x00020000  // LoadFile
  #  DEBUG_EVENT     0x00080000  // Event messages
  #  DEBUG_GCD       0x00100000  // Global Coherency Database changes
  #  DEBUG_CACHE     0x00200000  // Memory range cachability changes
  #  DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may
  #                              // significantly impact boot performance
  #  DEBUG_ERROR     0x80000000  // Error
  DEFINE DEBUG_PRINT_ERROR_LEVEL = 0x8000004F
  DEFINE FIRMWARE_VER            = 2.04.100.07
  DEFINE SECURE_BOOT_ENABLE      = TRUE
  DEFINE TPM2_ENABLE             = TRUE
  DEFINE INCLUDE_TFTP_COMMAND    = TRUE
  DEFINE PLATFORM_CONFIG_UUID    = 0690C53C-01B5-40AD-A65B-5399AC0B1E9B
  #
  # Network definition
  #
  DEFINE NETWORK_ENABLE                      = TRUE
  DEFINE NETWORK_IP6_ENABLE                  = TRUE
  DEFINE NETWORK_HTTP_BOOT_ENABLE            = TRUE
  DEFINE NETWORK_ALLOW_HTTP_CONNECTIONS      = TRUE
  DEFINE NETWORK_TLS_ENABLE                  = TRUE
  DEFINE REDFISH_ENABLE                      = TRUE

  DEFINE DEFAULT_KEYS        = TRUE
  DEFINE PK_DEFAULT_FILE     = Platform/Ampere/JadePkg/TestKeys/PK.cer
  DEFINE KEK_DEFAULT_FILE1   = Platform/Ampere/JadePkg/TestKeys/MicCorKEKCA2011_2011-06-24.crt
  DEFINE DB_DEFAULT_FILE1    = Platform/Ampere/JadePkg/TestKeys/MicCorUEFCA2011_2011-06-27.crt
  DEFINE DB_DEFAULT_FILE2    = Platform/Ampere/JadePkg/TestKeys/MicWinProPCA2011_2011-10-19.crt
  DEFINE DB_DEFAULT_FILE3    = Platform/Ampere/JadePkg/TestKeys/canonical-uefi-ca.der
  DEFINE DB_DEFAULT_FILE4    = Platform/Ampere/JadePkg/TestKeys/SLES-UEFI-CA-Certificate.cer
  DEFINE DB_DEFAULT_FILE5    = Platform/Ampere/JadePkg/TestKeys/fedora-ca.cer

!include MdePkg/MdeLibs.dsc.inc

# Include default Ampere Platform DSC file
!include Silicon/Ampere/AmpereAltraPkg/AmpereAltraPkg.dsc.inc
!include AdlinkAmpereAltra.dsc.inc
!include AdlinkComHpc.dsc.inc

################################################################################
#
# Specific Platform Library
#
################################################################################
[LibraryClasses]
  #
  # Capsule Update requirements
  #
  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
  DisplayUpdateProgressLib|MdeModulePkg/Library/DisplayUpdateProgressLibGraphics/DisplayUpdateProgressLibGraphics.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeCapsuleLib.inf
  EdkiiSystemCapsuleLib|SignedCapsulePkg/Library/EdkiiSystemCapsuleLib/EdkiiSystemCapsuleLib.inf
  FmpAuthenticationLib|SecurityPkg/Library/FmpAuthenticationLibPkcs7/FmpAuthenticationLibPkcs7.inf
  IniParsingLib|SignedCapsulePkg/Library/IniParsingLib/IniParsingLib.inf
  PlatformFlashAccessLib|Silicon/Ampere/AmpereAltraPkg/Library/PlatformFlashAccessLib/PlatformFlashAccessLib.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf

  #
  # ACPI Libraries
  #
  AcpiLib|EmbeddedPkg/Library/AcpiLib/AcpiLib.inf
  AcpiHelperLib|Platform/Ampere/AmperePlatformPkg/Library/AcpiHelperLib/AcpiHelperLib.inf

  #
  # EFI Redfish drivers
  #
!if $(REDFISH_ENABLE) == TRUE
  RedfishContentCodingLib|RedfishPkg/Library/RedfishContentCodingLibNull/RedfishContentCodingLibNull.inf
  RedfishPlatformCredentialLib|Platform/Ampere/JadePkg/Library/RedfishPlatformCredentialLib/RedfishPlatformCredentialLib.inf
  RedfishPlatformHostInterfaceLib|RedfishPkg/Library/PlatformHostInterfaceLibNull/PlatformHostInterfaceLibNull.inf
!endif

  IOExpanderLib|Platform/Ampere/JadePkg/Library/IOExpanderLib/IOExpanderLib.inf

  PlatformBmcReadyLib|Platform/Ampere/JadePkg/Library/PlatformBmcReadyLib/PlatformBmcReadyLib.inf

[LibraryClasses.common.PEIM]
  SmbusLib|MdePkg/Library/PeiSmbusLibSmbus2Ppi/PeiSmbusLibSmbus2Ppi.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeRuntimeCapsuleLib.inf

[LibraryClasses.common.UEFI_DRIVER, LibraryClasses.common.UEFI_APPLICATION, LibraryClasses.common.DXE_RUNTIME_DRIVER, LibraryClasses.common.DXE_DRIVER]
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf

################################################################################
#
# Specific Platform Pcds
#
################################################################################
[PcdsFeatureFlag.common]
  #
  # Activate AcpiSdtProtocol
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE

[PcdsFixedAtBuild]
!ifdef $(FIRMWARE_VER)
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"$(FIRMWARE_VER)"
!endif

[PcdsFixedAtBuild.common]
  #
  # Platform config UUID
  #
  gAmpereTokenSpaceGuid.PcdPlatformConfigUuid|"$(PLATFORM_CONFIG_UUID)"

  gEfiMdeModulePkgTokenSpaceGuid.PcdSmbiosVersion|0x305

  # Clearing BIT0 in this PCD prevents installing a 32-bit SMBIOS entry point,
  # if the entry point version is >= 3.0. AARCH64 OSes cannot assume the
  # presence of the 32-bit entry point anyway (because many AARCH64 systems
  # don't have 32-bit addressable physical RAM), and the additional allocations
  # below 4 GB needlessly fragment the memory map. So expose the 64-bit entry
  # point only, for entry point versions >= 3.0.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSmbiosEntryPointProvideMethod|0x2

  #
  # Increasing the maximum size of capsule is to cover ARM Trusted Firmware binaries
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxSizeNonPopulateCapsule|0xE00000

!if $(SECURE_BOOT_ENABLE) == TRUE
  # Override the default values from SecurityPkg to ensure images
  # from all sources are verified in secure boot
  gEfiSecurityPkgTokenSpaceGuid.PcdOptionRomImageVerificationPolicy|0x04
  gEfiSecurityPkgTokenSpaceGuid.PcdFixedMediaImageVerificationPolicy|0x04
  gEfiSecurityPkgTokenSpaceGuid.PcdRemovableMediaImageVerificationPolicy|0x04
!endif

!if $(REDFISH_ENABLE) == TRUE
  gEfiRedfishPkgTokenSpaceGuid.PcdRedfishRestExServiceDevicePath.DevicePathMatchMode|DEVICE_PATH_MATCH_MAC_NODE
  gEfiRedfishPkgTokenSpaceGuid.PcdRedfishRestExServiceDevicePath.DevicePathNum|1
  #
  # Below is the MAC address of network adapter on EDK2 Emulator platform.
  # You can use ifconfig under EFI shell to get the MAC address of network adapter on EDK2 Emulator platform.
  #
  gEfiRedfishPkgTokenSpaceGuid.PcdRedfishRestExServiceDevicePath.DevicePath|{ DEVICE_PATH("MAC(001B21DC35B0,0x1)") }

  # Allow Redish Service while Secure boot is disabled
  gAmpereTokenSpaceGuid.PcdRedfishServiceStopIfSecureBootDisabled|FALSE
!endif

[PcdsDynamicDefault.common.DEFAULT]
  # SMBIOS Type 0 - BIOS Information
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareReleaseDateString|L"MM/DD/YYYY"

[PcdsDynamicExDefault.common.DEFAULT]
  gEfiSignedCapsulePkgTokenSpaceGuid.PcdEdkiiSystemFirmwareImageDescriptor|{0x0}|VOID*|0x100
  gEfiMdeModulePkgTokenSpaceGuid.PcdSystemFmpCapsuleImageTypeIdGuid|{0x31, 0xca, 0x8b, 0xf0, 0x2e, 0x54, 0xea, 0x4c, 0x8b, 0x48, 0x8e, 0x54, 0xf9, 0x42, 0x25, 0x94}
  gEfiSignedCapsulePkgTokenSpaceGuid.PcdEdkiiSystemFirmwareFileGuid|{0xed, 0x06, 0x1c, 0x43, 0xe2, 0x4f, 0x8f, 0x43, 0x98, 0xa3, 0xa9, 0xb1, 0xfd, 0x92, 0x30, 0x19}

[PcdsPatchableInModule]
  #
  # Console Resolution (HD mode)
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|1024
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|768

################################################################################
#
# Specific Platform Component
#
################################################################################
[Components.common]
  #
  # ACPI
  #
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf {
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2B
  }
  Platform/Ampere/JadePkg/Drivers/AcpiPlatformDxe/AcpiPlatformDxe.inf
  Silicon/Ampere/AmpereAltraPkg/AcpiCommonTables/AcpiCommonTables.inf
  Platform/Ampere/ComHpcAltPkg/AcpiTables/AcpiTables.inf
  Platform/Ampere/JadePkg/Ac02AcpiTables/Ac02AcpiTables.inf

  #
  # PCIe
  #
  Platform/Ampere/JadePkg/Drivers/PciPlatformDxe/PciPlatformDxe.inf

  #
  # Network PCIe I210
  #
  !if $(NETWORK_ENABLE) == TRUE
    Platform/Ampere/AmperePlatformPkg/Drivers/GigUndiDxe/GigUndiDxe.inf

    # For the Redfish USB CDC connection to the BMC
    MdeModulePkg/Bus/Usb/UsbNetwork/UsbCdcEcm/UsbCdcEcm.inf
  !endif

  #
  # VGA Aspeed
  #
  Platform/Ampere/AmperePlatformPkg/Drivers/ASpeedGopBinPkg/GopDxe.inf

  #
  # SMBIOS
  #
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  ArmPkg/Universal/Smbios/ProcessorSubClassDxe/ProcessorSubClassDxe.inf
  ArmPkg/Universal/Smbios/SmbiosMiscDxe/SmbiosMiscDxe.inf
  Platform/Ampere/ComHpcAltPkg/Drivers/SmbiosPlatformDxe/SmbiosPlatformDxe.inf
  Silicon/Ampere/AmpereSiliconPkg/Drivers/SmbiosBlobsTransferDxe/SmbiosBlobsTransferDxe.inf

  #
  # Firmware Capsule Update
  #
  Platform/Ampere/JadePkg/Capsule/SystemFirmwareDescriptor/SystemFirmwareDescriptor.inf
  MdeModulePkg/Universal/EsrtDxe/EsrtDxe.inf
  SignedCapsulePkg/Universal/SystemFirmwareUpdate/SystemFirmwareReportDxe.inf
  SignedCapsulePkg/Universal/SystemFirmwareUpdate/SystemFirmwareUpdateDxe.inf
  MdeModulePkg/Application/CapsuleApp/CapsuleApp.inf

  #
  # IPMI Utilities
  #
  Silicon/Ampere/AmpereSiliconPkg/Application/IpmiUtil/IpmiUtilDynamicCommand.inf

  #
  # EnrollAmpereSecureKey
  #
  Silicon/Ampere/AmpereAltraPkg/Application/EnrollAmpereSecureKey/EaskDynamicCommand.inf

  #
  # HII
  #
  Platform/Ampere/ComHpcAltPkg/Drivers/PlatformInfoDxe/PlatformInfoDxe.inf
  Silicon/Ampere/AmpereAltraPkg/Drivers/MemInfoDxe/MemInfoDxe.inf
  Silicon/Ampere/AmpereAltraPkg/Drivers/CpuConfigDxe/CpuConfigDxe.inf
  Silicon/Ampere/AmpereAltraPkg/Drivers/AcpiConfigDxe/AcpiConfigDxe.inf
  Silicon/Ampere/AmpereAltraPkg/Drivers/RasConfigDxe/RasConfigDxe.inf
  Silicon/Ampere/AmpereAltraPkg/Drivers/WatchdogConfigDxe/WatchdogConfigDxe.inf
  Silicon/Ampere/AmpereAltraPkg/Drivers/PcieDeviceConfigDxe/PcieDeviceConfigDxe.inf
  Silicon/Ampere/AmpereSiliconPkg/Drivers/BmcInfoScreenDxe/BmcInfoScreenDxe.inf
  Silicon/Ampere/AmpereAltraPkg/Drivers/RootComplexConfigDxe/RootComplexConfigDxe.inf

  #
  # Misc
  #
  Silicon/Ampere/AmpereAltraPkg/Drivers/IpmiBootDxe/IpmiBootDxe.inf

  #
  Application/BoardVersion/BoardVersion.inf
  
  # Redfish
  #
!include RedfishPkg/Redfish.dsc.inc
!if $(REDFISH_ENABLE) == TRUE
  Platform/Ampere/JadePkg/Drivers/SmbiosType42Dxe/SmbiosType42Dxe.inf
!endif

  #
  # Platform Boot Manager
  #
  Platform/Ampere/AmperePlatformPkg/Drivers/PlatformBootManagerDxe/PlatformBootManagerDxe.inf

  #
  # OpRom emulator
  #
!if $(TARGET) == RELEASE
  MultiArchUefiPkg/Drivers/EmulatorBin/EmulatorBinRelease.inf
!else
  MultiArchUefiPkg/Drivers/EmulatorBin/EmulatorBinDebug.inf
!endif
