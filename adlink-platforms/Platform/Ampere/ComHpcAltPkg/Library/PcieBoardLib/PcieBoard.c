/** @file

  Pcie board specific driver to handle asserting PERST signal to Endpoint
  card and parsing NVPARAM board settings for bifurcation programming.

  PERST asserting is via group of GPIO pins to CPLD as Platform Specification.

  NVPARAM board settings is spec-ed within Firmware Interface Requirement.
  Bifuration devmap is programmed before at SCP following the rule

  Root Complex Type-A devmap settings (RP == Root Port)
  -----------------------------------------
  | RP0   | RP1  | RP2  | RP3  | Devmap   |
  | (x16) | (x4) | (x8) | (x4) | (output) |
  -------------------------------------------
  |  Y    |  N   |  N   |  N   | 0        |
  |  Y    |  N   |  Y   |  N   | 1        |
  |  Y    |  N   |  Y   |  Y   | 2        |
  |  Y    |  Y   |  Y   |  Y   | 3        |
  -----------------------------------------

  Root Complex Type-B LO (aka RCBxA) devmap settings (RP == Root Port)
  ----------------------------------------
  | RP0  | RP1  | RP2  | RP3  | Devmap   |
  | (x8) | (x2) | (x4) | (x3) | (output) |
  ----------------------------------------
  |  Y   |  N   |  N   |  N   | 0        |
  |  Y   |  N   |  Y   |  N   | 1        |
  |  Y   |  N   |  Y   |  Y   | 2        |
  |  Y   |  Y   |  Y   |  Y   | 3        |
  ----------------------------------------

  Root Complex Type-B LO (aka RCBxB) devmap settings (RP == Root Port)
  ----------------------------------------
  | RP4  | RP5  | RP6  | RP7  | Devmap   |
  | (x8) | (x2) | (x4) | (x3) | (output) |
  ----------------------------------------
  |  Y   |  N   |  N   |  N   | 0        |
  |  Y   |  N   |  Y   |  N   | 1        |
  |  Y   |  N   |  Y   |  Y   | 2        |
  |  Y   |  Y   |  Y   |  Y   | 3        |
  ----------------------------------------

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Guid/PlatformInfoHobGuid.h>
#include <Library/AmpereCpuLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/NVParamLib.h>
#include <Library/PcieBoardLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <NVDataStruc.h>
#include <NVParamDef.h>
#include <Pcie.h>
#include <PlatformInfoHob.h>

#ifndef BIT
#define BIT(nr)                         (1 << (nr))
#endif
#define NOPERST

extern CHAR16   gPcieVarstoreName[];
extern EFI_GUID gPcieFormSetGuid;

VOID
EFIAPI
PcieBoardLoadPreset (
  IN AC01_RC *RC
  )
{
  UINT32 Nv;
  INTN   NvParam;
  INTN   Ret;
  INTN   i;

  // Load default value
  for (i = 0; i < MAX_PCIE_B; i++) {
    RC->PresetGen3[i] = PRESET_INVALID;
    RC->PresetGen4[i] = PRESET_INVALID;
  }

  // Load override value
  if (RC->Socket == 0) {
    if (RC->Type == RCA) {
      if (RC->ID < 4) {
        NvParam = NV_SI_RO_BOARD_S0_RCA0_TXRX_G3PRESET + RC->ID * NVPARAM_SIZE;
      } else {
        NvParam = NV_SI_RO_BOARD_S0_RCA4_TXRX_G3PRESET + (RC->ID - 4) * NVPARAM_SIZE;
      }
    } else {
      NvParam = NV_SI_RO_BOARD_S0_RCB0A_TXRX_G3PRESET + (RC->ID - 4) * (2 * NVPARAM_SIZE);
    }
  } else if (RC->Type == RCA) {
    if (RC->ID < 4) {
      NvParam = NV_SI_RO_BOARD_S1_RCA2_TXRX_G3PRESET + (RC->ID - 2) * NVPARAM_SIZE;
    } else {
      NvParam = NV_SI_RO_BOARD_S1_RCA4_TXRX_G3PRESET + (RC->ID - 4) * NVPARAM_SIZE;
    }
  } else {
    NvParam = NV_SI_RO_BOARD_S1_RCB0A_TXRX_G3PRESET + (RC->ID - 4) * (2 * NVPARAM_SIZE);
  }

  Ret = NVParamGet ((NVPARAM)NvParam, NV_PERM_ALL, &Nv);
  if (Ret == EFI_SUCCESS) {
    for (i = 0; i < 4; i++) {
      RC->PresetGen3[i] = (Nv >> (NVPARAM_SIZE * i)) & 0xFF;
    }
  }

  if (RC->Type == RCB) {
    NvParam += NVPARAM_SIZE;
    Ret = NVParamGet ((NVPARAM)NvParam, NV_PERM_ALL, &Nv);
    if (Ret == EFI_SUCCESS) {
      for (i = 0; i < 4; i++) {
        RC->PresetGen3[i] = (Nv >> (NVPARAM_SIZE * i)) & 0xFF;
      }
    }
  }

  if (RC->Socket == 0) {
    if (RC->Type == RCA) {
      if (RC->ID < 4) {
        NvParam = NV_SI_RO_BOARD_S0_RCA0_TXRX_G4PRESET + RC->ID * NVPARAM_SIZE;
      } else {
        NvParam = NV_SI_RO_BOARD_S0_RCA4_TXRX_G4PRESET + (RC->ID - 4) * NVPARAM_SIZE;
      }
    } else {
      NvParam = NV_SI_RO_BOARD_S0_RCB0A_TXRX_G4PRESET + (RC->ID - 4) * (2 * NVPARAM_SIZE);
    }
  } else if (RC->Type == RCA) {
    if (RC->ID < 4) {
      NvParam = NV_SI_RO_BOARD_S1_RCA2_TXRX_G4PRESET + (RC->ID - 2) * NVPARAM_SIZE;
    } else {
      NvParam = NV_SI_RO_BOARD_S1_RCA4_TXRX_G4PRESET + (RC->ID - 4) * NVPARAM_SIZE;
    }
  } else {
    NvParam = NV_SI_RO_BOARD_S1_RCB0A_TXRX_G4PRESET + (RC->ID - 4) * (2 * NVPARAM_SIZE);
  }

  Ret = NVParamGet ((NVPARAM)NvParam, NV_PERM_ALL, &Nv);
  if (Ret == EFI_SUCCESS) {
    for (i = 0; i < 4; i++) {
      RC->PresetGen4[i] = (Nv >> (8 * i)) & 0xFF;
    }
  }

  if (RC->Type == RCB) {
    NvParam += NVPARAM_SIZE;
    Ret = NVParamGet ((NVPARAM)NvParam, NV_PERM_ALL, &Nv);
    if (Ret == EFI_SUCCESS) {
      for (i = 0; i < 4; i++) {
        RC->PresetGen4[i + 4] = (Nv >> (8 * i)) & 0xFF;
      }
    }
  }
}

VOID
EFIAPI
PcieBoardParseRCParams (
  IN AC01_RC *RC
  )
{
  UINT32             Efuse;
  PLATFORM_INFO_HOB  *PlatformHob;
  UINT8              PlatRCId;
  EFI_STATUS         Status;
  VOID               *Hob;
  UINTN              BufferSize;
  PCIE_VARSTORE_DATA VarStoreConfig = {
    .RCStatus = {TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
                 TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE},
    .RCBifurLo = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .RCBifurHi = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .SmmuPmu = 0
  };

  PCIE_DEBUG ("%a - Socket%d RC%d\n", __FUNCTION__, RC->Socket, RC->ID);

  PlatRCId = RC->Socket * RCS_PER_SOCKET + RC->ID;
  // Get RC activation status
  BufferSize = sizeof (PCIE_VARSTORE_DATA);
  Status = gRT->GetVariable (
                  gPcieVarstoreName,
                  &gPcieFormSetGuid,
                  NULL,
                  &BufferSize,
                  &VarStoreConfig
                  );
  if (EFI_ERROR (Status)) {
    PCIE_DEBUG ("%a - Failed to read PCIE variable data from config store.\n", __FUNCTION__);
  }

  RC->Active = VarStoreConfig.RCStatus[PlatRCId];
  RC->DevMapLo = VarStoreConfig.RCBifurLo[PlatRCId];
  RC->DevMapHi = VarStoreConfig.RCBifurHi[PlatRCId];

  PCIE_DEBUG (
    "%a - Socket%d RC%d is %s\n",
    __FUNCTION__,
    RC->Socket,
    RC->ID,
    (RC->Active) ? "ACTIVE" : "INACTIVE"
    );

  if (!IsSlaveSocketActive () && RC->Socket == 1) {
    RC->Active = FALSE;
  }

  if (RC->Active) {
    //
    // Consolidate with E-fuse
    //
    Efuse = 0;
    Hob = GetFirstGuidHob (&gPlatformHobGuid);
    if (Hob != NULL) {
      PlatformHob = (PLATFORM_INFO_HOB *)GET_GUID_HOB_DATA (Hob);
      Efuse = PlatformHob->RcDisableMask[0] | (PlatformHob->RcDisableMask[1] << RCS_PER_SOCKET);
      PCIE_DEBUG (
        "RcDisableMask[0]: 0x%x [1]: 0x%x\n",
        PlatformHob->RcDisableMask[0],
        PlatformHob->RcDisableMask[1]
        );

      // Update errata flags for Ampere Altra
      if ((PlatformHob->ScuProductId[0] & 0xff) == 0x01) {
        if (PlatformHob->AHBCId[0] == 0x20100
            || PlatformHob->AHBCId[0] == 0x21100
            || (IsSlaveSocketActive ()
                && (PlatformHob->AHBCId[1] == 0x20100
                    || PlatformHob->AHBCId[1] == 0x21100)))
        {
          RC->Flags |= PCIE_ERRATA_SPEED1;
          PCIE_DEBUG ("RC[%d]: Flags 0x%x\n", RC->ID, RC->Flags);
        }
      }
    }

    RC->Active = (RC->Active && !(Efuse & BIT (PlatRCId))) ? TRUE : FALSE;
  }

  /* Load Gen3/Gen4 preset */
  PcieBoardLoadPreset (RC);
  PcieBoardGetLaneAllocation (RC);
  PcieBoardSetupDevmap (RC);
  PcieBoardGetSpeed (RC);
}

VOID
EFIAPI
PcieBoardReleaseAllPerst (
  IN UINT8 SocketId
  )
{
#ifndef NOPERST
  UINT32 GpioIndex, GpioPin;

  // Write 1 to all GPIO[16..21] to release all PERST
  GpioPin = GPIO_DWAPB_PINS_PER_SOCKET * SocketId + 16;
  for (GpioIndex = 0; GpioIndex < 6; GpioIndex++) {
    GpioModeConfig (GpioPin + GpioIndex, GPIO_CONFIG_OUT_HI);
  }
#endif
}

VOID
EFIAPI
PcieBoardAssertPerst (
  AC01_RC *RC,
  UINT32  PcieIndex,
  UINT8   Bifurcation,
  BOOLEAN isPullToHigh
  )
{
#ifndef NOPERST
  UINT32 GpioGroupVal, Val, GpioIndex, GpioPin;

  /*
   * For Post-silicon, Fansipan board is using GPIO combination (GPIO[16..21])
   * to control CPLD.
   * It should be follow PCIE RESET TABLE in Fansipan schematic.
   *
   * Depend on Bifurcation settings, will active corresponding PERST pin or not
   */

  if (RC->Type == RCA) {
    switch (Bifurcation) {
    case 0:                 // RCA_BIFURCATION_ONE_X16:
      if (PcieIndex != 0) { // 1,2,3
      }
      break;

    case 1:                                       // RCA_BIFURCATION_TWO_X8:
      if ((PcieIndex == 1) || (PcieIndex == 3)) { // 1,3
      }
      break;

    case 2:                 // RCA_BIFURCATION_ONE_X8_TWO_X4:
      if (PcieIndex == 1) { // 1
      }
      break;

    case 3: // RCA_BIFURCATION_FOUR_X4:
      break;

    default:
      PCIE_DEBUG ("Invalid Bifurcation setting\n");
      break;
    }
  } else { // RCB
    switch (Bifurcation) {
    case 0:                                       // RCB_BIFURCATION_ONE_X8:
      if ((PcieIndex != 0) && (PcieIndex != 4)) { // 1,2,3,5,6,7
      }
      break;

    case 1:                       // RCB_BIFURCATION_TWO_X4:
      if ((PcieIndex % 2) != 0) { // 1,3,5,7
      }
      break;

    case 2: // RCB_BIFURCATION_ONE_X4_TWO_X2:
      if ((PcieIndex == 1) || (PcieIndex == 5)) {
      }
      break;

    case 3: // RCB_BIFURCATION_FOUR_X2:
      break;

    default:
      PCIE_DEBUG ("Invalid Bifurcation setting\n");
      break;
    }
  }

  if (!isPullToHigh) { // Pull PERST to Low
    if (RC->Type == RCA) { // RCA: RC->ID: 0->3 ; PcieIndex: 0->3
      GpioGroupVal = 62 - RC->ID*4 - PcieIndex;
    }
    else { // RCB: RC->ID: 4->7 ; PcieIndex: 0->7
      GpioGroupVal = 46 - (RC->ID - 4)*8 - PcieIndex;
    }

    GpioPin = GPIO_DWAPB_PINS_PER_SOCKET * RC->Socket + 16;
    for (GpioIndex = 0; GpioIndex < 6; GpioIndex++) {
      // 6: Number of GPIO pins to control via CPLD
      Val = (GpioGroupVal & 0x3F) & (1 << GpioIndex);
      if (Val == 0) {
        GpioModeConfig (GpioPin + GpioIndex, GPIO_CONFIG_OUT_LOW);
      } else {
        GpioModeConfig (GpioPin + GpioIndex, GPIO_CONFIG_OUT_HI);
      }
    }

    // Keep reset as low as 100 ms as specification
    MicroSecondDelay (100 * 1000);
  } else { // Pull PERST to High
    PcieBoardReleaseAllPerst (RC->Socket);
  }
#endif
}

/**
 * Overrride the segment number for a root complex with
 * a board specific number.
 **/
VOID
EFIAPI
PcieBoardGetRCSegmentNumber (
  IN  AC01_RC *RC,
  OUT UINTN   *SegmentNumber
  )
{
  if (RC->Socket == 0) {
    if (RC->Type == RCA) {
      switch (RC->ID) {
      case 0:
        *SegmentNumber = 12;
        break;

      case 1:
        *SegmentNumber = 13;
        break;

      case 2:
        *SegmentNumber = 1;
        break;

      case 3:
        *SegmentNumber = 0;
        break;

        default:
          *SegmentNumber = 16;
      }
    } else { /* Socket0, CCIX: RCA0 and RCA1 */
      *SegmentNumber = RC->ID - 2;
    }
  } else { /* Socket1, CCIX: RCA0 and RCA1 */
    if (RC->ID == 0 || RC->ID == 1) {
      *SegmentNumber = 16;
    } else {
      *SegmentNumber = 4 + RC->ID;
    }
  }
}

BOOLEAN
EFIAPI
PcieBoardCheckSmmuPmuEnabled (
  VOID
  )
{
  PCIE_VARSTORE_DATA VarStoreConfig;
  UINTN              BufferSize;
  EFI_STATUS         Status;

  // Get Buffer Storage data from EFI variable
  BufferSize = sizeof (PCIE_VARSTORE_DATA);
  Status = gRT->GetVariable (
                  gPcieVarstoreName,
                  &gPcieFormSetGuid,
                  NULL,
                  &BufferSize,
                  &VarStoreConfig
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  return VarStoreConfig.SmmuPmu;
}
