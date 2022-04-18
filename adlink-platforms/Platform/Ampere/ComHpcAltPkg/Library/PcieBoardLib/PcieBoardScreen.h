/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef PCIE_SCREEN_H_
#define PCIE_SCREEN_H_

#include <Guid/MdeModuleHii.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigKeyword.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>

#include "NVDataStruc.h"

//
// This is the generated IFR binary data for each formset defined in VFR.
// This data array is ready to be used as input of HiiAddPackages() to
// create a packagelist (which contains Form packages, String packages, etc).
//
extern UINT8 VfrBin[];

//
// This is the generated String package data for all .UNI files.
// This data array is ready to be used as input of HiiAddPackages() to
// create a packagelist (which contains Form packages, String packages, etc).
//
extern UINT8 PcieDxeStrings[];

#define MAX_EDITABLE_ELEMENTS 3
#define PCIE_RC0_STATUS_OFFSET  \
  OFFSET_OF (PCIE_VARSTORE_DATA, RCStatus[0])
#define PCIE_RC0_BIFUR_LO_OFFSET  \
  OFFSET_OF (PCIE_VARSTORE_DATA, RCBifurLo[0])
#define PCIE_RC0_BIFUR_HI_OFFSET  \
  OFFSET_OF (PCIE_VARSTORE_DATA, RCBifurHi[0])
#define PCIE_SMMU_PMU_OFFSET  \
  OFFSET_OF (PCIE_VARSTORE_DATA, SmmuPmu)

#define PCIE_STRONG_ODERING_OFFSET  \
  OFFSET_OF (PCIE_NVPARAM_VARSTORE_DATA, PcieStrongOrdering)

#define PCIE_SCREEN_PRIVATE_DATA_SIGNATURE SIGNATURE_32 ('P', 'C', 'I', 'E')

#define MAX_STRING_SIZE                    32

#define PCIE_STRONG_ORDERING_DEFAULT_OPTION_VALUE  1
#define PCIE_STRONG_ORDERING_DEFAULT_NVPARAM_VALUE 0xFFFFFFFF

typedef struct {
  UINTN Signature;

  EFI_HANDLE         DriverHandle;
  EFI_HII_HANDLE     HiiHandle;
  PCIE_VARSTORE_DATA VarStoreConfig;
  PCIE_NVPARAM_VARSTORE_DATA          NVParamVarStoreConfig;

  //
  // Consumed protocol
  //
  EFI_HII_DATABASE_PROTOCOL           *HiiDatabase;
  EFI_HII_STRING_PROTOCOL             *HiiString;
  EFI_HII_CONFIG_ROUTING_PROTOCOL     *HiiConfigRouting;
  EFI_CONFIG_KEYWORD_HANDLER_PROTOCOL *HiiKeywordHandler;

  //
  // Produced protocol
  //
  EFI_HII_CONFIG_ACCESS_PROTOCOL ConfigAccess;
} PCIE_SCREEN_PRIVATE_DATA;

typedef union {
  UINT16 VAR_ID;
  struct _PCIE_VAR_ID {
    UINT16 PciDevIndex     : 12;
    UINT16 DataType        : 3;
    UINT16 Always1         : 1;
  } IdField;
} PCIE_VAR_ID;

typedef struct {
  UINTN         PciDevIdx;
  EFI_STRING_ID GotoStringId;
  EFI_STRING_ID GotoHelpStringId;
  UINT16        GotoKey;
  BOOLEAN       ShowItem;
} PCIE_SETUP_GOTO_DATA;

typedef struct {
  VOID               *StartOpCodeHandle;
  VOID               *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL *StartLabel;
  EFI_IFR_GUID_LABEL *EndLabel;

} PCIE_IFR_INFO;

#define PCIE_SCREEN_PRIVATE_FROM_THIS(a)  \
  CR (a, PCIE_SCREEN_PRIVATE_DATA, ConfigAccess, PCIE_SCREEN_PRIVATE_DATA_SIGNATURE)

#pragma pack(1)

///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH       VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL End;
} HII_VENDOR_DEVICE_PATH;

#pragma pack()

UINT8
PcieRCDevMapLoDefaultSetting (
  IN UINTN                    RCIndex,
  IN PCIE_SCREEN_PRIVATE_DATA *PrivateData
  );

UINT8
PcieRCDevMapHiDefaultSetting (
  IN UINTN                    RCIndex,
  IN PCIE_SCREEN_PRIVATE_DATA *PrivateData
  );

BOOLEAN
PcieRCActiveDefaultSetting (
  IN UINTN                    RCIndex,
  IN PCIE_SCREEN_PRIVATE_DATA *PrivateData
  );

#endif /* PCIE_SCREEN_H_ */
