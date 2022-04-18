/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/NVParamLib.h>
#include <Library/SystemFirmwareInterfaceLib.h>
#include <NVParamDef.h>

#include "Pcie.h"

/* Host bridge registers */
#define HBRCAPDMR    0x0
#define HBRCBPDMR    0x4

/* HBRCAPDMR */
#define RCAPCIDEVMAP_SET(dst, src) \
  (((dst) & ~0x7) | (((UINT32)(src)) & 0x7))

/* HBRCBPDMR */
#define RCBPCIDEVMAPLO_SET(dst, src) \
  (((dst) & ~0x7) | (((UINT32)(src)) & 0x7))
#define RCBPCIDEVMAPHI_SET(dst, src) \
  (((dst) & ~0x70) | (((UINT32)(src) << 4) & 0x70))

#define PCIE_GET_MAX_WIDTH(Pcie, Max) \
  !((Pcie).MaxWidth) ? (Max) : MIN((Pcie).MaxWidth, (Max))

BOOLEAN
IsEmptyRC (
  IN AC01_RC *RC
  )
{
  INTN Idx;

  for (Idx = PCIE_0; Idx < MAX_PCIE; Idx++) {
    if (RC->Pcie[Idx].Active) {
      return FALSE;
    }
  }

  return TRUE;
}

VOID
PcieBoardSetRCBifur (
  IN AC01_RC *RC,
  IN UINT8   RPStart,
  IN UINT8   DevMap
  )
{
  UINT8 MaxWidth;

  if (RPStart != PCIE_0 && RPStart != PCIE_4) {
    return;
  }

  if (RC->Type != RCB && RPStart == PCIE_4) {
    return;
  }

  if (RC->Type == RCA && RC->Pcie[RPStart].MaxWidth == LNKW_X16) {
    RC->Pcie[RPStart + 1].MaxWidth = LNKW_X4;
    RC->Pcie[RPStart + 2].MaxWidth = LNKW_X8;
    RC->Pcie[RPStart + 3].MaxWidth = LNKW_X4;
  }

  if (RC->Type == RCB && RC->Pcie[RPStart].MaxWidth == LNKW_X8) {
    RC->Pcie[RPStart + 1].MaxWidth = LNKW_X2;
    RC->Pcie[RPStart + 2].MaxWidth = LNKW_X4;
    RC->Pcie[RPStart + 3].MaxWidth = LNKW_X2;
  }

  switch (DevMap) {
  case 1:
    MaxWidth = (RC->Type == RCA) ? LNKW_X8 : LNKW_X4;
    RC->Pcie[RPStart].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart], MaxWidth);
    RC->Pcie[RPStart + 1].Active = 0;
    RC->Pcie[RPStart + 2].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart + 2], MaxWidth);
    RC->Pcie[RPStart + 2].Active = 1;
    RC->Pcie[RPStart + 3].Active = 0;
    break;

  case 2:
    MaxWidth = (RC->Type == RCA) ? LNKW_X8 : LNKW_X4;
    RC->Pcie[RPStart].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart], MaxWidth);
    RC->Pcie[RPStart + 1].Active = 0;
    MaxWidth = (RC->Type == RCA) ? LNKW_X4 : LNKW_X2;
    RC->Pcie[RPStart + 2].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart + 2], MaxWidth);
    RC->Pcie[RPStart + 2].Active = 1;
    RC->Pcie[RPStart + 3].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart + 3], MaxWidth);
    RC->Pcie[RPStart + 3].Active = 1;
    break;

  case 3:
    MaxWidth = (RC->Type == RCA) ? LNKW_X4 : LNKW_X2;
    RC->Pcie[RPStart].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart], MaxWidth);
    RC->Pcie[RPStart + 1].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart + 1], MaxWidth);
    RC->Pcie[RPStart + 1].Active = 1;
    RC->Pcie[RPStart + 2].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart + 2], MaxWidth);
    RC->Pcie[RPStart + 2].Active = 1;
    RC->Pcie[RPStart + 3].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart + 3], MaxWidth);
    RC->Pcie[RPStart + 3].Active = 1;
    break;

  case 0:
  default:
    MaxWidth = (RC->Type == RCA) ? LNKW_X16 : LNKW_X8;
    RC->Pcie[RPStart].MaxWidth = PCIE_GET_MAX_WIDTH (RC->Pcie[RPStart], MaxWidth);
    RC->Pcie[RPStart + 1].Active = 0;
    RC->Pcie[RPStart + 2].Active = 0;
    RC->Pcie[RPStart + 3].Active = 0;
    break;
  }
}

VOID
PcieBoardGetLaneAllocation (
  IN AC01_RC *RC
  )
{
  INTN    RPIndex, Ret;
  UINT32  Nv, Width;
  NVPARAM NvParam;

  // Retrieve lane allocation and capabilities for each controller
  if (RC->Type == RCA) {
    NvParam = NV_SI_RO_BOARD_S0_RCA0_CFG + RC->Socket * 96 +
                RC->ID * 8;
  } else {
    NvParam = NV_SI_RO_BOARD_S0_RCB0_LO_CFG + RC->Socket * 96 +
              (RC->ID - MAX_RCA) * 16;
  }

  Ret = NVParamGet (NvParam, NV_PERM_ALL, &Nv);
  Nv = (Ret != EFI_SUCCESS) ? 0 : Nv;

  for (RPIndex = 0; RPIndex < MAX_PCIE_A; RPIndex++) {
    Width = (Nv >> (RPIndex * 8)) & 0xF;
    switch (Width) {
    case 1:
    case 2:
    case 3:
    case 4:
      RC->Pcie[RPIndex].MaxWidth = 1 << Width;
      RC->Pcie[RPIndex].MaxGen = SPEED_GEN3;
      RC->Pcie[RPIndex].Active = TRUE;
      break;

    case 0:
    default:
      RC->Pcie[RPIndex].MaxWidth = LNKW_NONE;
      RC->Pcie[RPIndex].MaxGen = 0;
      RC->Pcie[RPIndex].Active = FALSE;
      break;
    }
  }

  if (RC->Type == RCB) {
    // Processing Hi
    NvParam += 8;
    Ret = NVParamGet (NvParam, NV_PERM_ALL, &Nv);
    Nv = (Ret != EFI_SUCCESS) ? 0 : Nv;

    for (RPIndex = MAX_PCIE_A; RPIndex < MAX_PCIE; RPIndex++) {
      Width = (Nv >> ((RPIndex - MAX_PCIE_A) * 8)) & 0xF;
      switch (Width) {
      case 1:
      case 2:
      case 3:
      case 4:
        RC->Pcie[RPIndex].MaxWidth = 1 << Width;
        RC->Pcie[RPIndex].MaxGen = SPEED_GEN3;
        RC->Pcie[RPIndex].Active = TRUE;
        break;

      case 0:
      default:
        RC->Pcie[RPIndex].MaxWidth = LNKW_NONE;
        RC->Pcie[RPIndex].MaxGen = 0;
        RC->Pcie[RPIndex].Active = FALSE;
        break;
      }
    }
  }

  // Do not proceed if no Root Port enabled
  if (IsEmptyRC (RC)) {
    RC->Active = FALSE;
  }
}

VOID
PcieBoardSetupDevmap (
  IN AC01_RC *RC
  )
{
  UINT32 Val;

  if (RC->Pcie[PCIE_0].Active
      && RC->Pcie[PCIE_1].Active
      && RC->Pcie[PCIE_2].Active
      && RC->Pcie[PCIE_3].Active)
  {
    RC->DefaultDevMapLo = 3;
  } else if (RC->Pcie[PCIE_0].Active
             && RC->Pcie[PCIE_2].Active
             && RC->Pcie[PCIE_3].Active)
  {
    RC->DefaultDevMapLo = 2;
  } else if (RC->Pcie[PCIE_0].Active
             && RC->Pcie[PCIE_2].Active)
  {
    RC->DefaultDevMapLo = 1;
  } else {
    RC->DefaultDevMapLo = 0;
  }

  if (RC->Pcie[PCIE_4].Active
      && RC->Pcie[PCIE_5].Active
      && RC->Pcie[PCIE_6].Active
      && RC->Pcie[PCIE_7].Active)
  {
    RC->DefaultDevMapHi = 3;
  } else if (RC->Pcie[PCIE_4].Active
             && RC->Pcie[PCIE_6].Active
             && RC->Pcie[PCIE_7].Active)
  {
    RC->DefaultDevMapHi = 2;
  } else if (RC->Pcie[PCIE_4].Active
             && RC->Pcie[PCIE_6].Active)
  {
    RC->DefaultDevMapHi = 1;
  } else {
    RC->DefaultDevMapHi = 0;
  }

  if (RC->DevMapLo == 0) {
    RC->DevMapLo = RC->DefaultDevMapLo;
  }

  if (RC->Type == RCB && RC->DevMapHi == 0) {
    RC->DevMapHi = RC->DefaultDevMapHi;
  }

  PcieBoardSetRCBifur (RC, PCIE_0, RC->DevMapLo);
  if (RC->Type == RCB) {
    PcieBoardSetRCBifur (RC, PCIE_4, RC->DevMapHi);
  }

  if (RC->Active) {
    if (RC->Type == RCA) {
      if (!EFI_ERROR (MailboxMsgRegisterRead (RC->Socket, RC->HBAddr + HBRCAPDMR, &Val))) {
        Val = RCAPCIDEVMAP_SET (Val, RC->DevMapLo & 0x7);
        MailboxMsgRegisterWrite (RC->Socket, RC->HBAddr + HBRCAPDMR, Val);
      }
    } else {
      if (!EFI_ERROR (MailboxMsgRegisterRead (RC->Socket, RC->HBAddr + HBRCBPDMR, &Val))) {
        Val = RCBPCIDEVMAPLO_SET (Val, RC->DevMapLo & 0x7);
        Val = RCBPCIDEVMAPHI_SET (Val, RC->DevMapHi & 0x7);
        MailboxMsgRegisterWrite (RC->Socket, RC->HBAddr + HBRCBPDMR, Val);
      }
    }
  }
}

VOID
PcieBoardGetSpeed (
  IN AC01_RC *RC
  )
{
  UINT8 MaxGenTbl[MAX_PCIE_A] = { SPEED_GEN4, SPEED_GEN4, SPEED_GEN4, SPEED_GEN4 };         // Bifurcation 0: RCA x16 / RCB x8
  UINT8 MaxGenTblX8X4X4[MAX_PCIE_A] = { SPEED_GEN4, SPEED_GEN4, SPEED_GEN1, SPEED_GEN1 };   // Bifurcation 2: x8 x4 x4 (PCIE_ERRATA_SPEED1)
  UINT8 MaxGenTblX4X4X4X4[MAX_PCIE_A] = { SPEED_GEN1, SPEED_GEN1, SPEED_GEN1, SPEED_GEN1 }; // Bifurcation 3: x4 x4 x4 x4 (PCIE_ERRATA_SPEED1)
  UINT8 MaxGenTblRCB[MAX_PCIE_A] = { SPEED_GEN1, SPEED_GEN1, SPEED_GEN1, SPEED_GEN1 };      // RCB PCIE_ERRATA_SPEED1
  INTN  RPIdx;
  UINT8 *MaxGen;

  ASSERT (MAX_PCIE_A == 4);
  ASSERT (MAX_PCIE == 8);

  //
  // Due to hardware errata, for A0/A1*
  //  RCB is limited to Gen1 speed.
  //  RCA x16, x8 port supports up to Gen4,
  //  RCA x4 port only supports Gen1.
  //
  MaxGen = MaxGenTbl;
  if (RC->Type == RCB) {
    if (RC->Flags & PCIE_ERRATA_SPEED1) {
      MaxGen = MaxGenTblRCB;
    }
  } else {
    switch (RC->DevMapLo) {
    case 2: /* x8 x4 x4 */
      if (RC->Flags & PCIE_ERRATA_SPEED1) {
        MaxGen = MaxGenTblX8X4X4;
      }
      break;

    case 3: /* X4 x4 x4 x4 */
      if (RC->Flags & PCIE_ERRATA_SPEED1) {
        MaxGen = MaxGenTblX4X4X4X4;
      }
      break;

    case 1: /* x8 x8 */
    default:
      break;
    }
  }

  for (RPIdx = 0; RPIdx < MAX_PCIE_A; RPIdx++) {
    RC->Pcie[RPIdx].MaxGen = RC->Pcie[RPIdx].Active ? MaxGen[RPIdx] : 0;
  }

  if (RC->Type == RCB) {
    for (RPIdx = MAX_PCIE_A; RPIdx < MAX_PCIE; RPIdx++) {
      RC->Pcie[RPIdx].MaxGen = RC->Pcie[RPIdx].Active ?
                               MaxGen[RPIdx - MAX_PCIE_A] : 0;
    }
  }
}
