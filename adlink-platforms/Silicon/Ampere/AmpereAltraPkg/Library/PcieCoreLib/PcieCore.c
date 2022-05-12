/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Guid/PlatformInfoHobGuid.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PcieBoardLib.h>
#include <Library/PciePhyLib.h>
#include <Library/SystemFirmwareInterfaceLib.h>
#include <PlatformInfoHob.h>

#include "Pcie.h"
#include "PcieCore.h"

#define PERICOM_B4_D1_CFG_ADDR	0x2FFFF0408000
#define PERICOM_B4_D2_CFG_ADDR	0x2FFFF0410000

STATIC INT32
Ac01PcieCsrOut32 (
  VOID   *Addr,
  UINT32 Val
  )
{
  MmioWrite32 ((UINT64)Addr, Val);
  PCIE_CSR_DEBUG (
    "PCIE CSR WR: 0x%p value: 0x%08X (0x%08X)\n",
    Addr,
    Val,
    MmioRead32 ((UINT64)Addr)
    );

  return 0;
}

STATIC INT32
Ac01PcieCsrOut32Serdes (
  VOID   *Addr,
  UINT32 Val
  )
{
  MmioWrite32 ((UINT64)Addr, Val);
  PCIE_CSR_DEBUG (
    "PCIE CSR WR: 0x%p value: 0x%08X (0x%08X)\n",
    Addr,
    Val,
    MmioRead32 ((UINT64)Addr)
    );

  return 0;
}

STATIC INT32
Ac01PcieCsrIn32 (
  VOID   *Addr,
  UINT32 *Val
  )
{
  *Val = MmioRead32 ((UINT64)Addr);
  PCIE_CSR_DEBUG ("PCIE CSR RD: 0x%p value: 0x%08X\n", Addr, *Val);

  return 0;
}

STATIC INT32
Ac01PcieCsrIn32Serdes (
  VOID   *Addr,
  UINT32 *Val
  )
{
  *Val = MmioRead32 ((UINT64)Addr);
  PCIE_CSR_DEBUG ("PCIE CSR RD: 0x%p value: 0x%08X\n", Addr, *Val);

  return 0;
}

VOID
Ac01PcieMmioRd (
  UINT64 Addr,
  UINT32 *Val
  )
{
  Ac01PcieCsrIn32Serdes ((VOID *)Addr, (UINT32 *)Val);
}

VOID
Ac01PcieMmioWr (
  UINT64 Addr,
  UINT32 Val
  )
{
  Ac01PcieCsrOut32Serdes ((VOID *)Addr, (UINT32)Val);
}

VOID
Ac01PciePuts (
  CONST CHAR8 *Msg
  )
{
  PCIE_PHY_DEBUG ("%a\n", __FUNCTION__);
}

VOID
Ac01PciePutInt (
  UINT32 val
  )
{
  PCIE_PHY_DEBUG ("%a\n", __FUNCTION__);
}

VOID
Ac01PciePutHex (
  UINT64 val
  )
{
  PCIE_PHY_DEBUG ("%a\n", __FUNCTION__);
}

INT32
Ac01PcieDebugPrint (
  CONST CHAR8 *fmt,
  ...
  )
{
  PCIE_PHY_DEBUG ("%a\n", __FUNCTION__);
  return 0;
}

VOID
Ac01PcieDelay (
  UINT32 Val
  )
{
  MicroSecondDelay (Val);
}

/**
   Write 32-bit value to config space address

   @param Addr          Address within config space
   @param Val           32-bit value to write
**/
INT32
Ac01PcieCfgOut32 (
  IN VOID   *Addr,
  IN UINT32 Val
  )
{
  MmioWrite32 ((UINT64)Addr, Val);
  PCIE_DEBUG_CFG (
    "PCIE CFG WR: 0x%p value: 0x%08X (0x%08X)\n",
    Addr,
    Val,
    MmioRead32 ((UINT64)Addr)
    );

  return 0;
}

/**
   Write 16-bit value to config space address

   @param Addr          Address within config space
   @param Val           16-bit value to write
**/
INT32
Ac01PcieCfgOut16 (
  IN VOID   *Addr,
  IN UINT16 Val
  )
{
  UINT64 AlignedAddr = (UINT64)Addr & ~0x3;
  UINT32 Val32  = MmioRead32 (AlignedAddr);

  switch ((UINT64)Addr & 0x3) {
  case 2:
    Val32 &= ~0xFFFF0000;
    Val32 |= (UINT32)Val << 16;
    break;

  case 0:
  default:
    Val32 &= ~0xFFFF;
    Val32 |= Val;
    break;
  }
  MmioWrite32 (AlignedAddr, Val32);
  PCIE_DEBUG_CFG (
    "PCIE CFG WR16: 0x%p value: 0x%04X (0x%08llX 0x%08X)\n",
    Addr,
    Val,
    AlignedAddr,
    MmioRead32 ((UINT64)AlignedAddr)
    );

  return 0;
}

/**
   Write 8-bit value to config space address

   @param Addr          Address within config space
   @param Val           8-bit value to write
**/
INT32
Ac01PcieCfgOut8 (
  IN VOID  *Addr,
  IN UINT8 Val
  )
{
  UINT64 AlignedAddr = (UINT64)Addr & ~0x3;
  UINT32 Val32  = MmioRead32 (AlignedAddr);

  switch ((UINT64)Addr & 0x3) {
  case 0:
    Val32 &= ~0xFF;
    Val32 |= Val;
    break;

  case 1:
    Val32 &= ~0xFF00;
    Val32 |= (UINT32)Val << 8;
    break;

  case 2:
    Val32 &= ~0xFF0000;
    Val32 |= (UINT32)Val << 16;
    break;

  case 3:
  default:
    Val32 &= ~0xFF000000;
    Val32 |= (UINT32)Val << 24;
    break;
  }
  MmioWrite32 (AlignedAddr, Val32);
  PCIE_DEBUG_CFG (
    "PCIE CFG WR8: 0x%p value: 0x%04X (0x%08llX 0x%08X)\n",
    Addr,
    Val,
    AlignedAddr,
    MmioRead32 ((UINT64)AlignedAddr)
    );

  return 0;
}

/**
   Read 32-bit value from config space address

   @param Addr          Address within config space
   @param Val           Point to address for read value
**/
INT32
Ac01PcieCfgIn32 (
  IN  VOID   *Addr,
  OUT UINT32 *Val
  )
{
  UINT32 RegC, Reg18;
  UINT8  MfHt, Ht, Primary = 0, Sec = 0, Sub = 0;

  if ((BUS_NUM (Addr) > 0) && (DEV_NUM (Addr) > 0) && (CFG_REG (Addr) == 0)) {
    *Val = MmioRead32 ((UINT64)Addr);
    PCIE_DEBUG_CFG (
      "PCIE CFG RD: B%X|D%X 0x%p value: 0x%08X\n",
      BUS_NUM (Addr),
      DEV_NUM (Addr),
      Addr,
      *Val
      );

    if (*Val != 0xffffffff) {
      RegC = MmioRead32 ((UINT64)Addr + 0xC);
      PCIE_DEBUG_CFG ("Peek PCIE MfHt RD32: 0x%p value: 0x%08X\n", Addr + 0xc, RegC);
      MfHt = RegC >> 16;
      PCIE_DEBUG_CFG ("  Peek RD8 MfHt=0x%02X\n", MfHt);

      Ht = MfHt & 0x7F;
      if (Ht != 0) { /* Type 1 header */
        Reg18 = MmioRead32 ((UINT64)Addr + 0x18);
        Primary = Reg18; Sec = Reg18 >> 8; Sub = Reg18 >> 16;
        PCIE_DEBUG_CFG (
          "  Bus Peek PCIE Sub:%01X Sec:%01X Primary:%01X  RD: 0x%p value: 0x%08X\n",
          Sub,
          Sec,
          Primary,
          Addr + 0x18,
          Reg18
          );
      }
      if ((Ht == 0) || (Primary != 0)) { /* Ampere Altra RPs Primary Bus is 0b */
	if (((UINT64)Addr != PERICOM_B4_D1_CFG_ADDR) && ((UINT64)Addr != PERICOM_B4_D2_CFG_ADDR)) { /* Allow AVA-AP1 Pericom switches */
		*Val = 0xffffffff;
		PCIE_DEBUG_CFG ("  Skip RD32 B%X|D%X PCIE CFG RD: 0x%p return 0xffffffff\n",
				BUS_NUM (Addr),
				DEV_NUM (Addr),
				Addr
			       );
	}
      }
    }
  } else {
    *Val = MmioRead32 ((UINT64)Addr);
  }
  PCIE_DEBUG_CFG ("PCIE CFG RD: 0x%p value: 0x%08X\n", Addr, *Val);

  return 0;
}

/**
   Read 16-bit value from config space address

   @param Addr          Address within config space
   @param Val           Point to address for read value
**/
INT32
Ac01PcieCfgIn16 (
  IN  VOID   *Addr,
  OUT UINT16 *Val
  )
{
  UINT64 AlignedAddr = (UINT64)Addr & ~0x3;
  UINT32 RegC, Reg18;
  UINT8  MfHt, Primary = 0, Sec = 0, Sub = 0;
  UINT32 Val32;

  if ((BUS_NUM (Addr) > 0) && (DEV_NUM (Addr) > 0) && (CFG_REG (Addr) == 0)) {
    *Val = MmioRead32 ((UINT64)Addr);
    PCIE_DEBUG_CFG (
      "PCIE CFG16 RD: B%X|D%X 0x%p value: 0x%08X\n",
      BUS_NUM (Addr),
      DEV_NUM (Addr),
      Addr,
      *Val
      );

    if (*Val != 0xffff) {
      RegC = MmioRead32 ((UINT64)Addr + 0xC);
      PCIE_DEBUG_CFG ("  Peek PCIE MfHt RD: 0x%p value: 0x%08X\n", Addr + 0xc, RegC);
      MfHt = RegC >> 16;
      PCIE_DEBUG_CFG ("  Peek RD8 MfHt=0x%02X\n", MfHt);


      if ((MfHt & 0x7F)!= 0) { /* Type 1 header */
        Reg18 = MmioRead32 ((UINT64)Addr + 0x18);
        Primary = Reg18; Sec = Reg18 >> 8; Sub = Reg18 >> 16;
        PCIE_DEBUG_CFG (
          "  Bus Peek PCIE Sub:%01X Sec:%01X Primary:%01X  RD: 0x%p value: 0x%08X\n",
          Sub,
          Sec,
          Primary,
          Addr + 0x18,
          Reg18
          );
      }
      if ((MfHt == 0) || (Primary != 0)) { /* QS RPs Primary Bus is 0b */
        *Val = 0xffff;
        PCIE_DEBUG_CFG (
          "  Skip RD16 B%X|D%X PCIE CFG RD: 0x%p return 0xffff\n",
          BUS_NUM (Addr),
          DEV_NUM (Addr),
          Addr
          );
        return 0;
      }
    }
  }

  Val32 = MmioRead32 (AlignedAddr);
  switch ((UINT64)Addr & 0x3) {
  case 2:
    *Val = Val32 >> 16;
    break;

  case 0:
  default:
    *Val = Val32;
    break;
  }
  PCIE_DEBUG_CFG (
    "PCIE CFG RD16: 0x%p value: 0x%04X (0x%08llX 0x%08X)\n",
    Addr,
    *Val,
    AlignedAddr,
    Val32
    );

  return 0;
}

/**
   Read 8-bit value from config space address

   @param Addr          Address within config space
   @param Val           Point to address for read value
**/
INT32
Ac01PcieCfgIn8 (
  IN  VOID  *Addr,
  OUT UINT8 *Val
  )
{
  UINT64 AlignedAddr = (UINT64)Addr & ~0x3;
  UINT32 Val32 = MmioRead32 (AlignedAddr);
  switch ((UINT64)Addr & 0x3) {
  case 3:
    *Val = Val32 >> 24;
    break;

  case 2:
    *Val = Val32 >> 16;
    break;

  case 1:
    *Val = Val32 >> 8;
    break;

  case 0:
  default:
    *Val = Val32;
    break;
  }
  PCIE_DEBUG_CFG (
    "PCIE CFG RD8: 0x%p value: 0x%02X (0x%08llX 0x%08X)\n",
    Addr,
    *Val,
    AlignedAddr,
    Val32
    );

  return 0;
}

/**
   Return the next extended capability address

   @param RC              Pointer to AC01_RC structure
   @param PcieIndex       PCIe index
   @param IsRC            0x1: Checking RC configuration space
                          0x0: Checking EP configuration space
   @param ExtendedCapId
**/
UINT64
PcieCheckCap (
  IN AC01_RC *RC,
  IN INTN    PcieIndex,
  IN BOOLEAN IsRC,
  IN UINT16  ExtendedCapId
  )
{
  VOID   *CfgAddr;
  UINT32 Val = 0, NextCap = 0, CapId = 0, ExCap = 0;

  if (IsRC) {
    CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));
  } else {
    CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 20));
  }

  Ac01PcieCsrIn32 (CfgAddr + TYPE1_CAP_PTR_REG, &Val);
  NextCap = Val & 0xFF;

  // Loop untill desired capability is found else return 0
  while (1) {
    if ((NextCap & 0x3) != 0) {
      /* Not alignment, just return */
      return 0;
    }
    Ac01PcieCsrIn32 (CfgAddr + NextCap, &Val);
    if (NextCap < EXT_CAP_OFFSET_START) {
      CapId = Val & 0xFF;
    } else {
      CapId = Val & 0xFFFF;
    }

    if (CapId == ExtendedCapId) {
      return (UINT64)(CfgAddr + NextCap);
    }

    if (NextCap < EXT_CAP_OFFSET_START) {
      NextCap = (Val & 0xFFFF) >> 8;
    } else {
      NextCap = (Val & 0xFFFF0000) >> 20;
    }

    if ((NextCap == 0) && (ExCap == 0)) {
      ExCap = 1;
      NextCap = EXT_CAP_OFFSET_START;
    }

    if ((NextCap == 0) && (ExCap == 1)) {
      return (UINT64)0;
    }
  }
}

/**
   Read 8-bit value from config space address

   @param RC          Pointer to AC01_RC strucutre
   @param RegBase     Base address of CSR, TCU, Hostbridge, Msg, Serdes, and MMCFG register
   @param MmioBase    Base address of 32-bit MMIO
   @param Mmio32Base  Base address of 64-bit MMIO
**/
VOID
Ac01PcieCoreBuildRCStruct (
  AC01_RC *RC,
  UINT64  RegBase,
  UINT64  MmioBase,
  UINT64  Mmio32Base
  )
{
  INTN PcieIndex;

  RC->BaseAddr = RegBase;
  RC->TcuAddr = RegBase + TCU_OFFSET;
  RC->HBAddr = RegBase + HB_CSR_OFFSET;
  RC->SerdesAddr = RegBase + SERDES_CSR_OFFSET;
  RC->MmcfgAddr = RegBase + MMCONFIG_OFFSET;
  RC->MmioAddr = MmioBase;
  RC->Mmio32Addr = Mmio32Base;
  RC->IoAddr = Mmio32Base + MMIO32_SPACE - IO_SPACE;

  RC->Type = (RC->ID < MAX_RCA) ? RCA : RCB;
  RC->MaxPcieController = (RC->Type == RCB) ? MAX_PCIE_B : MAX_PCIE_A;

  PcieBoardParseRCParams (RC);

  for (PcieIndex = 0; PcieIndex < RC->MaxPcieController; PcieIndex++) {
    RC->Pcie[PcieIndex].ID = PcieIndex;
    RC->Pcie[PcieIndex].CsrAddr = RC->BaseAddr + PCIE0_CSR_OFFSET + PcieIndex*0x10000;
    RC->Pcie[PcieIndex].SnpsRamAddr = RC->Pcie[PcieIndex].CsrAddr + SNPSRAM_OFFSET;
    RC->Pcie[PcieIndex].DevNum = PcieIndex + 1;
  }

  PCIE_DEBUG (
    " + S%d - RC%a%d, MMCfgAddr:0x%lx, MmioAddr:0x%lx, Mmio32Addr:0x%lx, Enabled:%a\n",
    RC->Socket,
    (RC->Type == RCA) ? "A" : "B",
    RC->ID,
    RC->MmcfgAddr,
    RC->MmioAddr,
    RC->Mmio32Addr,
    (RC->Active) ? "Y" : "N"
    );
  PCIE_DEBUG (" +   DevMapLo/Hi: 0x%x/0x%x\n", RC->DevMapLo, RC->DevMapHi);
  for (PcieIndex = 0; PcieIndex < RC->MaxPcieController; PcieIndex++) {
    PCIE_DEBUG (
      " +     PCIE%d:0x%lx - Enabled:%a - DevNum:0x%x\n",
      PcieIndex,
      RC->Pcie[PcieIndex].CsrAddr,
      (RC->Pcie[PcieIndex].Active) ? "Y" : "N",
      RC->Pcie[PcieIndex].DevNum
      );
  }
}

/**
   Configure equalization settings

   @param RC              Pointer to AC01_RC structure
   @param PcieIndex       PCIe index
**/
STATIC
VOID
Ac01PcieConfigureEqualization (
  IN AC01_RC *RC,
  IN INTN    PcieIndex
  )
{
  VOID   *CfgAddr;
  UINT32 Val;

  CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));

  // Select the FoM method, need double-write to convey settings
  Ac01PcieCfgIn32 (CfgAddr + GEN3_EQ_CONTROL_OFF, &Val);
  Val = GEN3_EQ_FB_MODE (Val, 0x1);
  Val = GEN3_EQ_PRESET_VEC (Val, 0x3FF);
  Val = GEN3_EQ_INIT_EVAL (Val, 0x1);
  Ac01PcieCfgOut32 (CfgAddr + GEN3_EQ_CONTROL_OFF, Val);
  Ac01PcieCfgOut32 (CfgAddr + GEN3_EQ_CONTROL_OFF, Val);
  Ac01PcieCfgIn32 (CfgAddr + GEN3_EQ_CONTROL_OFF, &Val);
}

/**
   Configure presets for GEN3 equalization

   @param RC              Pointer to AC01_RC structure
   @param PcieIndex       PCIe index
**/
STATIC
VOID
Ac01PcieConfigurePresetGen3 (
  IN AC01_RC *RC,
  IN INTN    PcieIndex
  )
{
  VOID   *CfgAddr, *SpcieBaseAddr;
  UINT32 Val, Idx;
  CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));

  // Bring to legacy mode
  Ac01PcieCfgIn32 (CfgAddr + GEN3_RELATED_OFF, &Val);
  Val = RATE_SHADOW_SEL_SET (Val, 0);
  Ac01PcieCfgOut32 (CfgAddr + GEN3_RELATED_OFF, Val);
  Val = EQ_PHASE_2_3_SET (Val, 0);
  Val = RXEQ_REGRDLESS_SET (Val, 1);
  Ac01PcieCfgOut32 (CfgAddr + GEN3_RELATED_OFF, Val);

  // Generate SPCIE capability address
  SpcieBaseAddr = (VOID *)PcieCheckCap (RC, PcieIndex, 0x1, SPCIE_CAP_ID);
  if (SpcieBaseAddr == 0) {
    PCIE_ERR (
      "PCIE%d.%d: Cannot get SPCIE capability address\n",
      RC->ID,
      PcieIndex
      );
    return;
  }

  for (Idx=0; Idx < RC->Pcie[PcieIndex].MaxWidth/2; Idx++) {
    // Program Preset to Gen3 EQ Lane Control
    Ac01PcieCfgIn32 (SpcieBaseAddr + CAP_OFF_0C + Idx*4, &Val);
    Val = DSP_TX_PRESET0_SET (Val, 0x7);
    Val = DSP_TX_PRESET1_SET (Val, 0x7);
    Ac01PcieCfgOut32 (SpcieBaseAddr + CAP_OFF_0C + Idx*4, Val);
  }
}

/**
   Configure presets for GEN4 equalization

   @param RC              Pointer to AC01_RC structure
   @param PcieIndex       PCIe index
**/
STATIC
VOID
Ac01PcieConfigurePresetGen4 (
  IN AC01_RC *RC,
  IN INTN    PcieIndex
  )
{
  UINT32 Val;
  VOID   *CfgAddr, *SpcieBaseAddr, *Pl16BaseAddr;
  UINT32 LinkWidth, i;
  UINT8  Preset;

  CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));

  // Bring to legacy mode
  Ac01PcieCfgIn32 (CfgAddr + GEN3_RELATED_OFF, &Val);
  Val = RATE_SHADOW_SEL_SET (Val, 1);
  Ac01PcieCfgOut32 (CfgAddr + GEN3_RELATED_OFF, Val);
  Val = EQ_PHASE_2_3_SET (Val, 0);
  Val = RXEQ_REGRDLESS_SET (Val, 1);
  Ac01PcieCfgOut32 (CfgAddr + GEN3_RELATED_OFF, Val);

  // Generate the PL16 capability address
  Pl16BaseAddr = (VOID *)PcieCheckCap (RC, PcieIndex, 0x1, PL16_CAP_ID);
  if (Pl16BaseAddr == 0) {
    PCIE_ERR (
      "PCIE%d.%d: Cannot get PL16 capability address\n",
      RC->ID,
      PcieIndex
      );
    return;
  }

  // Generate the SPCIE capability address
  SpcieBaseAddr = (VOID *)PcieCheckCap (RC, PcieIndex, 0x1, SPCIE_CAP_ID);
  if (SpcieBaseAddr == 0) {
    PCIE_ERR (
      "PCIE%d.%d: Cannot get SPICE capability address\n",
      RC->ID,
      PcieIndex
      );
    return;
  }

  // Configure downstream Gen4 Tx preset
  if (RC->PresetGen4[PcieIndex] == PRESET_INVALID) {
    Preset = 0x57; // Default Gen4 preset
  } else {
    Preset = RC->PresetGen4[PcieIndex];
  }

  LinkWidth = RC->Pcie[PcieIndex].MaxWidth;
  if (LinkWidth == 0x2) {
    Ac01PcieCfgIn32 (Pl16BaseAddr + PL16G_CAP_OFF_20H_REG_OFF, &Val);
    Val = DSP_16G_RXTX_PRESET0_SET (Val, Preset);
    Val = DSP_16G_RXTX_PRESET1_SET (Val, Preset);
    Ac01PcieCfgOut32 (Pl16BaseAddr + PL16G_CAP_OFF_20H_REG_OFF, Val);
  } else {
    for (i = 0; i < LinkWidth/4; i++) {
      Ac01PcieCfgIn32 (Pl16BaseAddr + PL16G_CAP_OFF_20H_REG_OFF + i*4, &Val);
      Val = DSP_16G_RXTX_PRESET0_SET (Val, Preset);
      Val = DSP_16G_RXTX_PRESET1_SET (Val, Preset);
      Val = DSP_16G_RXTX_PRESET2_SET (Val, Preset);
      Val = DSP_16G_RXTX_PRESET3_SET (Val, Preset);
      Ac01PcieCfgOut32 (Pl16BaseAddr + PL16G_CAP_OFF_20H_REG_OFF + i*4, Val);
    }
  }

  // Configure Gen3 preset
  for (i = 0; i < LinkWidth/2; i++) {
    Ac01PcieCfgIn32 (SpcieBaseAddr + CAP_OFF_0C + i*4, &Val);
    Val = DSP_TX_PRESET0_SET (Val, 0x7);
    Val = DSP_TX_PRESET1_SET (Val, 0x7);
    Ac01PcieCfgOut32 (SpcieBaseAddr + CAP_OFF_0C + i*4, Val);
  }
}

STATIC BOOLEAN
RasdpMitigationCheck (
  AC01_RC *RC,
  INTN    PcieIndex
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;
  VOID               *Hob;

  Hob = GetFirstGuidHob (&gPlatformHobGuid);
  PlatformHob = (PLATFORM_INFO_HOB *)GET_GUID_HOB_DATA (Hob);
  if ((PlatformHob->ScuProductId[0] & 0xff) == 0x01) {
    if (AsciiStrCmp ((CONST CHAR8 *)PlatformHob->CpuVer, "A0") == 0) {
      return ((RC->Type == RCB)||(PcieIndex > 0)) ? TRUE : FALSE;
    }
  }

  return FALSE;
}

/**
   Setup and initialize the AC01 PCIe Root Complex and underneath PCIe controllers

   @param RC                    Pointer to Root Complex structure
   @param ReInit                Re-init status
   @param ReInitPcieIndex       PCIe index
**/
INT32
Ac01PcieCoreSetupRC (
  IN AC01_RC *RC,
  IN UINT8   ReInit,
  IN UINT8   ReInitPcieIndex
  )
{
  VOID              *CsrAddr, *CfgAddr, *SnpsRamAddr, *DlinkBaseAddr;
  INTN              PcieIndex;
  INTN              TimeOut;
  UINT32            Val;
  PHY_CONTEXT       PhyCtx = { 0 };
  PHY_PLAT_RESOURCE PhyPlatResource = { 0 };
  INTN              Ret;
  UINT16            NextExtendedCapabilityOff;
  UINT32            VsecVal;

  PCIE_DEBUG ("Initializing Socket%d RC%d\n", RC->Socket, RC->ID);

  if (ReInit == 0) {
    // Initialize SERDES
    ZeroMem (&PhyCtx, sizeof (PhyCtx));
    PhyCtx.SdsAddr = RC->SerdesAddr;
    PhyCtx.PcieCtrlInfo |= ((RC->Socket & 0x1) << 2);
    PhyCtx.PcieCtrlInfo |= ((RC->ID & 0x7) << 4);
    PhyCtx.PcieCtrlInfo |= 0xF << 8;
    PhyPlatResource.MmioRd = Ac01PcieMmioRd;
    PhyPlatResource.MmioWr = Ac01PcieMmioWr;
    PhyPlatResource.UsDelay = Ac01PcieDelay;
    PhyPlatResource.Puts = Ac01PciePuts;
    PhyPlatResource.PutInt = Ac01PciePutInt;
    PhyPlatResource.PutHex = Ac01PciePutInt;
    PhyPlatResource.PutHex64 = Ac01PciePutHex;
    PhyPlatResource.DebugPrint = Ac01PcieDebugPrint;
    PhyCtx.PhyPlatResource = &PhyPlatResource;

    Ret = SerdesInitClkrst (&PhyCtx);
    if (Ret != PHY_INIT_PASS) {
      return -1;
    }
  }

  // Setup each controller
  for (PcieIndex = 0; PcieIndex < RC->MaxPcieController; PcieIndex++) {

    if (ReInit == 1) {
      PcieIndex = ReInitPcieIndex;
    }

    if (!RC->Pcie[PcieIndex].Active) {
      continue;
    }

    PCIE_DEBUG ("Initializing Controller %d\n", PcieIndex);

    CsrAddr = (VOID *)RC->Pcie[PcieIndex].CsrAddr;
    SnpsRamAddr = (VOID *)RC->Pcie[PcieIndex].SnpsRamAddr;
    CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));

    // Put Controller into reset if not in reset already
    Ac01PcieCsrIn32 (CsrAddr +  RESET, &Val);
    if (!(Val & RESET_MASK)) {
      Val = DWCPCIE_SET (Val, 1);
      Ac01PcieCsrOut32 (CsrAddr + RESET, Val);

      // Delay 50ms to ensure controller finish its reset
      // FIXME: Is this necessary?
      MicroSecondDelay (50000);
    }

    // Clear memory shutdown
    Ac01PcieCsrIn32 (CsrAddr + RAMSDR, &Val);
    Val = SD_SET (Val, 0);
    Ac01PcieCsrOut32 (CsrAddr + RAMSDR, Val);

    // Poll till mem is ready
    TimeOut = PCIE_MEMRDY_TIMEOUT;
    do {
      Ac01PcieCsrIn32 (CsrAddr + MEMRDYR, &Val);
      if (Val & 1) {
        break;
      }

      TimeOut--;
      MicroSecondDelay (1);
    } while (TimeOut > 0);

    if (TimeOut <= 0) {
      PCIE_ERR ("- Pcie[%d] - Mem not ready\n", PcieIndex);
      return -1;
    }

    // Hold link training
    Ac01PcieCsrIn32 (CsrAddr + LINKCTRL, &Val);
    Val = LTSSMENB_SET (Val, 0);
    Ac01PcieCsrOut32 (CsrAddr + LINKCTRL, Val);

    // Enable subsystem clock and release reset
    Ac01PcieCsrIn32 (CsrAddr + CLOCK, &Val);
    Val = AXIPIPE_SET (Val, 1);
    Ac01PcieCsrOut32 (CsrAddr + CLOCK, Val);
    Ac01PcieCsrIn32 (CsrAddr +  RESET, &Val);
    Val = DWCPCIE_SET (Val, 0);
    Ac01PcieCsrOut32 (CsrAddr + RESET, Val);

    //
    // Controller does not provide any indicator for reset released.
    // Must wait at least 1us as per EAS.
    //
    MicroSecondDelay (1);

    // Poll till PIPE clock is stable
    TimeOut = PCIE_PIPE_CLOCK_TIMEOUT;
    do {
      Ac01PcieCsrIn32 (CsrAddr + LINKSTAT, &Val);
      if (!(Val & PHY_STATUS_MASK)) {
        break;
      }

      TimeOut--;
      MicroSecondDelay (1);
    } while (TimeOut > 0);

    if (TimeOut <= 0) {
      PCIE_ERR ("- Pcie[%d] - PIPE clock is not stable\n", PcieIndex);
      return -1;
    }

    // Start PERST pulse
    PcieBoardAssertPerst (RC, PcieIndex, 0, TRUE);

    // Allow programming to config space
    Ac01PcieCsrIn32 (CfgAddr + MISC_CONTROL_1_OFF, &Val);
    Val = DBI_RO_WR_EN_SET (Val, 1);
    Ac01PcieCsrOut32 (CfgAddr + MISC_CONTROL_1_OFF, Val);

    // In order to detect the NVMe disk for booting without disk,
    // need to set Hot-Plug Slot Capable during port initialization.
    // It will help PCI Linux driver to initialize its slot iomem resource,
    // the one is used for detecting the disk when it is inserted.
    Ac01PcieCsrIn32 (CfgAddr + SLOT_CAPABILITIES_REG, &Val);
    Val = SLOT_HPC_SET (Val, 1);
    // Program the power limit
    Val = PCIE_SLOT_CAP_SLOT_POWER_LIMIT_VALUE_SET (Val, PCIE_SLOT_POWER_LIMIT);
    Ac01PcieCsrOut32 (CfgAddr + SLOT_CAPABILITIES_REG, Val);

    // Apply RASDP error mitigation for all x8, x4, and x2 controllers
    // This includes all RCB root ports, and every RCA root port controller
    // except for index 0 (i.e. x16 controllers are exempted from this WA)
    if (RasdpMitigationCheck (RC, PcieIndex)) {
      // Change the Read Margin dual ported RAMs
      Val = 0x10; // Margin to 0x0 (most conservative setting)
      Ac01PcieCsrOut32 (SnpsRamAddr + TPSRAM_RMR, Val);

      // Generate the DLINK capability address
      DlinkBaseAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));
      NextExtendedCapabilityOff = 0x100; // This is the 1st extended capability offset
      do {
        Ac01PcieCsrIn32 (DlinkBaseAddr + NextExtendedCapabilityOff, &Val);
        if (Val == 0xFFFFFFFF) {
          DlinkBaseAddr = 0x0;
          break;
        }
        if ((Val & 0xFFFF) == DLINK_VENDOR_CAP_ID) {
          Ac01PcieCsrIn32 (DlinkBaseAddr + NextExtendedCapabilityOff + 0x4, &VsecVal);
          if (VsecVal == DLINK_VSEC) {
            DlinkBaseAddr = DlinkBaseAddr + NextExtendedCapabilityOff;
            break;
          }
        }
        NextExtendedCapabilityOff = (Val >> 20);
      } while (NextExtendedCapabilityOff != 0);

      // Disable the scaled credit mode
      if (DlinkBaseAddr != 0x0) {
        Val = 1;
        Ac01PcieCsrOut32 (DlinkBaseAddr + DATA_LINK_FEATURE_CAP_OFF, Val);
        Ac01PcieCsrIn32 (DlinkBaseAddr + DATA_LINK_FEATURE_CAP_OFF, &Val);
        if (Val != 1) {
          PCIE_ERR ("- Pcie[%d] - Unable to disable scaled credit\n", PcieIndex);
          return -1;
        }
      } else {
        PCIE_ERR ("- Pcie[%d] - Unable to locate data link feature cap offset\n", PcieIndex);
        return -1;
      }

      // Reduce Posted Credits to 1 packet header and data credit for all
      // impacted controllers.  Also zero credit scale values for both
      // data and packet headers.
      Val=0x40201020;
      Ac01PcieCsrOut32 (CfgAddr + PORT_LOCIG_VC0_P_RX_Q_CTRL_OFF, Val);
    }

    // Program DTI for ATS support
    Ac01PcieCsrIn32 (CfgAddr + DTIM_CTRL0_OFF, &Val);
    Val = DTIM_CTRL0_ROOT_PORT_ID_SET (Val, 0);
    Ac01PcieCsrOut32 (CfgAddr + DTIM_CTRL0_OFF, Val);

    //
    // Program number of lanes used
    // - Reprogram LINK_CAPABLE of PORT_LINK_CTRL_OFF
    // - Reprogram NUM_OF_LANES of GEN2_CTRL_OFF
    // - Reprogram PCIE_CAP_MAX_LINK_WIDTH of LINK_CAPABILITIES_REG
    //
    Ac01PcieCsrIn32 (CfgAddr + PORT_LINK_CTRL_OFF, &Val);
    switch (RC->Pcie[PcieIndex].MaxWidth) {
    case LNKW_X2:
      Val = LINK_CAPABLE_SET (Val, LINK_CAPABLE_X2);
      break;

    case LNKW_X4:
      Val = LINK_CAPABLE_SET (Val, LINK_CAPABLE_X4);
      break;

    case LNKW_X8:
      Val = LINK_CAPABLE_SET (Val, LINK_CAPABLE_X8);
      break;

    case LNKW_X16:
    default:
      Val = LINK_CAPABLE_SET (Val, LINK_CAPABLE_X16);
      break;
    }
    Ac01PcieCsrOut32 (CfgAddr + PORT_LINK_CTRL_OFF, Val);
    Ac01PcieCsrIn32 (CfgAddr + GEN2_CTRL_OFF, &Val);
    switch (RC->Pcie[PcieIndex].MaxWidth) {
    case LNKW_X2:
      Val = NUM_OF_LANES_SET (Val, NUM_OF_LANES_X2);
      break;

    case LNKW_X4:
      Val = NUM_OF_LANES_SET (Val, NUM_OF_LANES_X4);
      break;

    case LNKW_X8:
      Val = NUM_OF_LANES_SET (Val, NUM_OF_LANES_X8);
      break;

    case LNKW_X16:
    default:
      Val = NUM_OF_LANES_SET (Val, NUM_OF_LANES_X16);
      break;
    }
    Ac01PcieCsrOut32 (CfgAddr + GEN2_CTRL_OFF, Val);
    Ac01PcieCsrIn32 (CfgAddr + LINK_CAPABILITIES_REG, &Val);
    switch (RC->Pcie[PcieIndex].MaxWidth) {
    case LNKW_X2:
      Val = PCIE_CAP_MAX_LINK_WIDTH_SET (Val, PCIE_CAP_MAX_LINK_WIDTH_X2);
      break;

    case LNKW_X4:
      Val = PCIE_CAP_MAX_LINK_WIDTH_SET (Val, PCIE_CAP_MAX_LINK_WIDTH_X4);
      break;

    case LNKW_X8:
      Val = PCIE_CAP_MAX_LINK_WIDTH_SET (Val, PCIE_CAP_MAX_LINK_WIDTH_X8);
      break;

    case LNKW_X16:
    default:
      Val = PCIE_CAP_MAX_LINK_WIDTH_SET (Val, PCIE_CAP_MAX_LINK_WIDTH_X16);
      break;
    }

    switch (RC->Pcie[PcieIndex].MaxGen) {
    case SPEED_GEN1:
      Val = PCIE_CAP_MAX_LINK_SPEED_SET (Val, MAX_LINK_SPEED_25);
      break;

    case SPEED_GEN2:
      Val = PCIE_CAP_MAX_LINK_SPEED_SET (Val, MAX_LINK_SPEED_50);
      break;

    case SPEED_GEN3:
      Val = PCIE_CAP_MAX_LINK_SPEED_SET (Val, MAX_LINK_SPEED_80);
      break;

    default:
      Val = PCIE_CAP_MAX_LINK_SPEED_SET (Val, MAX_LINK_SPEED_160);
      break;
    }
    /* Enable ASPM Capability */
    Val = PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_SET (Val, L0S_L1_SUPPORTED);
    Ac01PcieCsrOut32 (CfgAddr + LINK_CAPABILITIES_REG, Val);

    Ac01PcieCsrIn32 (CfgAddr + LINK_CONTROL2_LINK_STATUS2_REG, &Val);
    switch (RC->Pcie[PcieIndex].MaxGen) {
    case SPEED_GEN1:
      Val = PCIE_CAP_TARGET_LINK_SPEED_SET (Val, MAX_LINK_SPEED_25);
      break;

    case SPEED_GEN2:
      Val = PCIE_CAP_TARGET_LINK_SPEED_SET (Val, MAX_LINK_SPEED_50);
      break;

    case SPEED_GEN3:
      Val = PCIE_CAP_TARGET_LINK_SPEED_SET (Val, MAX_LINK_SPEED_80);
      break;

    default:
      Val = PCIE_CAP_TARGET_LINK_SPEED_SET (Val, MAX_LINK_SPEED_160);
      break;
    }
    Ac01PcieCsrOut32 (CfgAddr + LINK_CONTROL2_LINK_STATUS2_REG, Val);

    // Set Zero byte request handling
    Ac01PcieCsrIn32 (CfgAddr + FILTER_MASK_2_OFF, &Val);
    Val = CX_FLT_MASK_VENMSG0_DROP_SET (Val, 0);
    Val = CX_FLT_MASK_VENMSG1_DROP_SET (Val, 0);
    Val = CX_FLT_MASK_DABORT_4UCPL_SET (Val, 0);
    Ac01PcieCsrOut32 (CfgAddr + FILTER_MASK_2_OFF, Val);
    Ac01PcieCsrIn32 (CfgAddr + AMBA_ORDERING_CTRL_OFF, &Val);
    Val = AX_MSTR_ZEROLREAD_FW_SET (Val, 0);
    Ac01PcieCsrOut32 (CfgAddr + AMBA_ORDERING_CTRL_OFF, Val);

    //
    // Set Completion with CRS handling for CFG Request
    // Set Completion with CA/UR handling non-CFG Request
    //
    Ac01PcieCsrIn32 (CfgAddr + AMBA_ERROR_RESPONSE_DEFAULT_OFF, &Val);
    Val = AMBA_ERROR_RESPONSE_CRS_SET (Val, 0x2);
    Ac01PcieCsrOut32 (CfgAddr + AMBA_ERROR_RESPONSE_DEFAULT_OFF, Val);

    // Set Legacy PCIE interrupt map to INTA
    Ac01PcieCsrIn32 (CfgAddr + BRIDGE_CTRL_INT_PIN_INT_LINE_REG, &Val);
    Val = INT_PIN_SET (Val, 1);
    Ac01PcieCsrOut32 (CfgAddr + BRIDGE_CTRL_INT_PIN_INT_LINE_REG, Val);
    Ac01PcieCsrIn32 (CsrAddr + IRQSEL, &Val);
    Val = INTPIN_SET (Val, 1);
    Ac01PcieCsrOut32 (CsrAddr + IRQSEL, Val);

    if (RC->Pcie[PcieIndex].MaxGen != SPEED_GEN1) {
      Ac01PcieConfigureEqualization (RC, PcieIndex);
      if (RC->Pcie[PcieIndex].MaxGen == SPEED_GEN3) {
        Ac01PcieConfigurePresetGen3 (RC, PcieIndex);
      } else if (RC->Pcie[PcieIndex].MaxGen == SPEED_GEN4) {
        Ac01PcieConfigurePresetGen4 (RC, PcieIndex);
      }
    }

    // Mask Completion Timeout
    Ac01PcieCsrIn32 (CfgAddr + AMBA_LINK_TIMEOUT_OFF, &Val);
    Val = LINK_TIMEOUT_PERIOD_DEFAULT_SET (Val, 1);
    Ac01PcieCsrOut32 (CfgAddr + AMBA_LINK_TIMEOUT_OFF, Val);
    Ac01PcieCsrIn32 (CfgAddr + UNCORR_ERR_MASK_OFF, &Val);
    Val = CMPLT_TIMEOUT_ERR_MASK_SET (Val, 1);
    // AER surprise link down error should be masked due to hotplug is enabled
    // This event must be handled by Hotplug handler, instead of error handler
    Val = SDES_ERR_MASK_SET (Val, 1);
    Ac01PcieCsrOut32 (CfgAddr + UNCORR_ERR_MASK_OFF, Val);

    // Program Class Code
    Ac01PcieCsrIn32 (CfgAddr + TYPE1_CLASS_CODE_REV_ID_REG, &Val);
    Val = REVISION_ID_SET (Val, 4);
    Val = SUBCLASS_CODE_SET (Val, 4);
    Val = BASE_CLASS_CODE_SET (Val, 6);
    Ac01PcieCsrOut32 (CfgAddr + TYPE1_CLASS_CODE_REV_ID_REG, Val);

    // Program VendorID and DeviceID
    Ac01PcieCsrIn32 (CfgAddr + TYPE1_DEV_ID_VEND_ID_REG, &Val);
    Val = VENDOR_ID_SET (Val, AMPERE_PCIE_VENDORID);
    if (RCA == RC->Type) {
      Val = DEVICE_ID_SET (Val, AC01_PCIE_BRIDGE_DEVICEID_RCA + PcieIndex);
    } else {
      Val = DEVICE_ID_SET (Val, AC01_PCIE_BRIDGE_DEVICEID_RCB + PcieIndex);
    }
    Ac01PcieCsrOut32 (CfgAddr + TYPE1_DEV_ID_VEND_ID_REG, Val);

    // Enable common clock for downstream
    Ac01PcieCsrIn32 (CfgAddr + LINK_CONTROL_LINK_STATUS_REG, &Val);
    Val = PCIE_CAP_SLOT_CLK_CONFIG_SET (Val, 1);
    Val = PCIE_CAP_COMMON_CLK_SET (Val, 1);
    Ac01PcieCsrOut32 (CfgAddr + LINK_CONTROL_LINK_STATUS_REG, Val);

    // Match aux_clk to system
    Ac01PcieCsrIn32 (CfgAddr + AUX_CLK_FREQ_OFF, &Val);
    Val = AUX_CLK_FREQ_SET (Val, AUX_CLK_500MHZ);
    Ac01PcieCsrOut32 (CfgAddr + AUX_CLK_FREQ_OFF, Val);

    // Assert PERST low to reset endpoint
    PcieBoardAssertPerst (RC, PcieIndex, 0, FALSE);

    // Start link training
    Ac01PcieCsrIn32 (CsrAddr + LINKCTRL, &Val);
    Val = LTSSMENB_SET (Val, 1);
    Ac01PcieCsrOut32 (CsrAddr + LINKCTRL, Val);

    // Complete the PERST pulse
    PcieBoardAssertPerst (RC, PcieIndex, 0, TRUE);

    // Lock programming of config space
    Ac01PcieCsrIn32 (CfgAddr + MISC_CONTROL_1_OFF, &Val);
    Val = DBI_RO_WR_EN_SET (Val, 0);
    Ac01PcieCsrOut32 (CfgAddr + MISC_CONTROL_1_OFF, Val);

    if (ReInit == 1) {
      return 0;
    }
  }

  // Program VendorID and DeviceId
  if (!EFI_ERROR (MailboxMsgRegisterRead (RC->Socket, RC->HBAddr  + HBPDVIDR, &Val))) {
    Val = PCIVENDID_SET (Val, AMPERE_PCIE_VENDORID);
    if (RCA == RC->Type) {
      Val = PCIDEVID_SET (Val, AC01_HOST_BRIDGE_DEVICEID_RCA);
    } else {
      Val = PCIDEVID_SET (Val, AC01_HOST_BRIDGE_DEVICEID_RCB);
    }
    MailboxMsgRegisterWrite (RC->Socket, RC->HBAddr  + HBPDVIDR, Val);
  }

  return 0;
}

BOOLEAN
PcieLinkUpCheck (
  IN  AC01_PCIE *Pcie
  )
{
  VOID   *CsrAddr;
  UINT32 BlockEvent, LinkStat;

  CsrAddr = (VOID *)Pcie->CsrAddr;

  // Check if card present
  // smlh_ltssm_state[13:8] = 0
  // phy_status[2] = 0
  // smlh_link_up[1] = 0
  // rdlh_link_up[0] = 0
  Ac01PcieCsrIn32 (CsrAddr + LINKSTAT, &LinkStat);
  LinkStat = LinkStat & (SMLH_LTSSM_STATE_MASK | PHY_STATUS_MASK_BIT |
                         SMLH_LINK_UP_MASK_BIT | RDLH_LINK_UP_MASK_BIT);
  if (LinkStat == 0x0000) {
    return FALSE;
  }

  Ac01PcieCsrIn32 (CsrAddr +  BLOCKEVENTSTAT, &BlockEvent);
  Ac01PcieCsrIn32 (CsrAddr +  LINKSTAT, &LinkStat);

  if (((BlockEvent & LINKUP_MASK) != 0)
      && (SMLH_LTSSM_STATE_GET(LinkStat) == S_L0))
  {
    PCIE_DEBUG ("%a Linkup\n", __FUNCTION__);
    return TRUE;
  }

  return FALSE;
}

VOID
Ac01PcieCoreEndEnumeration (
  AC01_RC *RC
  )
{
  VOID            *Reg;
  UINTN           Index;
  UINT32          Val;
  VOID            *CfgAddr;

  if (RC == NULL || !RC->Active) {
    return;
  }

  // Clear uncorrectable error during enumuration phase. Mainly completion timeout.
  for (Index = 0; Index < RC->MaxPcieController; Index++) {
    if (!RC->Pcie[Index].Active) {
      continue;
    }
    if (!PcieLinkUpCheck(&RC->Pcie[Index])) {
      // If link down/disabled after enumeration, disable completed time out
      CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[Index].DevNum << 15));
      Ac01PcieCsrIn32 (CfgAddr + UNCORR_ERR_MASK_OFF, &Val);
      Val = CMPLT_TIMEOUT_ERR_MASK_SET (Val, 1);
      Ac01PcieCsrOut32 (CfgAddr + UNCORR_ERR_MASK_OFF, Val);
    }
    // Clear all errors
    Reg = (VOID *)((UINT64)RC->MmcfgAddr + ((Index + 1) << 15) + UNCORR_ERR_STATUS_OFF);
    Ac01PcieCfgIn32 (Reg, &Val);
    if (Val != 0) {
      // Clear error by writting
      Ac01PcieCfgOut32 (Reg, Val);
    }
  }
}

/**
   Comparing current link status with the max capabilities of the link

   @param RC            Pointer to AC01_RC structure
   @param PcieIndex     PCIe index
   @param EpMaxWidth    EP max link width
   @param EpMaxGen      EP max link speed
   @retval              -1: Link status do not match with link max capabilities
                         1: Link capabilites are invalid
                         0: Link status are correct
**/
INT32
Ac01PcieCoreLinkCheck (
  IN AC01_RC  *RC,
  IN INTN      PcieIndex,
  IN UINT8     EpMaxWidth,
  IN UINT8     EpMaxGen
  )
{
  VOID         *CsrAddr, *CfgAddr;
  UINT32        Val, LinkStat;
  UINT32        MaxWidth, MaxGen;

  CsrAddr = (VOID *)RC->Pcie[PcieIndex].CsrAddr;
  CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));

  Ac01PcieCsrIn32 (CfgAddr + LINK_CAPABILITIES_REG, &Val);
  if ((PCIE_CAP_MAX_LINK_WIDTH_GET (Val) == 0) ||
      (PCIE_CAP_MAX_LINK_SPEED_GET (Val) == 0)) {
    PCIE_DEBUG ("\tPCIE%d.%d: Wrong RC capabilities\n", RC->ID, PcieIndex);
    return LINK_CHECK_WRONG_PARAMETER;
  }

  if ((EpMaxWidth == 0) || (EpMaxGen == 0)) {
    PCIE_DEBUG ("\tPCIE%d.%d: Wrong EP capabilities\n", RC->ID, PcieIndex);
    return LINK_CHECK_FAILED;
  }

  // Compare RC and EP capabilities
  if (PCIE_CAP_MAX_LINK_WIDTH_GET (Val) > EpMaxWidth) {
    MaxWidth = EpMaxWidth;
  } else {
    MaxWidth = PCIE_CAP_MAX_LINK_WIDTH_GET (Val);
  }

  // Compare RC and EP capabilities
  if (PCIE_CAP_MAX_LINK_SPEED_GET (Val) > EpMaxGen) {
    MaxGen = EpMaxGen;
  } else {
    MaxGen = PCIE_CAP_MAX_LINK_SPEED_GET (Val);
  }

  Ac01PcieCsrIn32 (CsrAddr + LINKSTAT, &LinkStat);
  Ac01PcieCsrIn32 (CfgAddr + LINK_CONTROL_LINK_STATUS_REG, &Val);
  PCIE_DEBUG (
    "PCIE%d.%d: Link MaxWidth %d MaxGen %d, LINKSTAT 0x%x",
    RC->ID,
    PcieIndex,
    MaxWidth,
    MaxGen,
    LinkStat
    );

  // Checking all conditions of the link
  // If one of them is not sastified, return link up fail
  if ((PCIE_CAP_NEGO_LINK_WIDTH_GET (Val) != MaxWidth) ||
      (PCIE_CAP_LINK_SPEED_GET (Val) != MaxGen) ||
      (RDLH_SMLH_LINKUP_STATUS_GET (LinkStat) != (SMLH_LINK_UP_MASK_BIT | RDLH_LINK_UP_MASK_BIT)))
  {
    PCIE_DEBUG ("\tLinkCheck FAILED\n");
    return LINK_CHECK_FAILED;
  }

  PCIE_DEBUG ("\tLinkCheck SUCCESS\n");
  return LINK_CHECK_SUCCESS;
}

INT32
Ac01PFAEnableAll (
  IN AC01_RC   *RC,
  IN INTN      PcieIndex,
  IN INTN      PFAMode
  )
{
  VOID   *RasDesVSecBase;
  VOID   *CfgAddr;
  UINT8  ErrCode, ErrGrpNum;
  UINT32 Val;
  INT32  Ret = LINK_CHECK_SUCCESS;

  UINT32 ErrCtrlCfg[MAX_NUM_ERROR_CODE] = {
    0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00A, // Per Lane
    0x105, 0x106, 0x107, 0x108, 0x109, 0x10A,
    0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207,
    0x300, 0x301, 0x302, 0x303, 0x304, 0x305,
    0x400, 0x401,                                                                // Per Lane
    0x500, 0x501, 0x502, 0x503, 0x504, 0x505, 0x506, 0x507, 0x508, 0x509, 0x50A, 0x50B, 0x50C, 0x50D
  };

  CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));

  // Allow programming to config space
  Ac01PcieCsrIn32 (CfgAddr + MISC_CONTROL_1_OFF, &Val);
  Val = DBI_RO_WR_EN_SET (Val, 1);
  Ac01PcieCsrOut32 (CfgAddr + MISC_CONTROL_1_OFF, Val);

  // Generate the RAS DES capability address
  // RAS_DES_CAP_ID = 0xB
  RasDesVSecBase = (VOID *)PcieCheckCap (RC, PcieIndex, 0x1, RAS_DES_CAP_ID);
  if (RasDesVSecBase == 0) {
    PCIE_DEBUG ("PCIE%d.%d: Cannot get RAS DES capability address\n", RC->ID, PcieIndex);
    return LINK_CHECK_WRONG_PARAMETER;
  }

  if (PFAMode == PFA_REG_ENABLE) {
    // PFA Counters Enable
    Ac01PcieCsrIn32 (RasDesVSecBase + EVENT_COUNTER_CONTROL_REG_OFF, &Val);
    Val = ECCR_EVENT_COUNTER_ENABLE_SET (Val, 0x7);
    Val = ECCR_EVENT_COUNTER_CLEAR_SET (Val, 0);
    Ac01PcieCsrOut32 (RasDesVSecBase + EVENT_COUNTER_CONTROL_REG_OFF, Val);
  } else if (PFAMode == PFA_REG_CLEAR) {
    // PFA Counters Clear
    Ac01PcieCsrIn32 (RasDesVSecBase + EVENT_COUNTER_CONTROL_REG_OFF, &Val);
    Val = ECCR_EVENT_COUNTER_ENABLE_SET (Val, 0);
    Val = ECCR_EVENT_COUNTER_CLEAR_SET (Val, 0x3);
    Ac01PcieCsrOut32 (RasDesVSecBase + EVENT_COUNTER_CONTROL_REG_OFF, Val);
  } else {
    // PFA Counters Read
    for (ErrCode = 0; ErrCode < MAX_NUM_ERROR_CODE; ErrCode++) {
      ErrGrpNum = (ErrCtrlCfg[ErrCode] & 0xF00) >> 8;
      // Skipping per lane group
      // Checking common lane group because AER error are included in common group only
      if ((ErrGrpNum != 0) && (ErrGrpNum != 4)) {
        Ac01PcieCsrIn32 (RasDesVSecBase + EVENT_COUNTER_CONTROL_REG_OFF, &Val);
        if (RC->Type == RCA) { // RCA - 4 PCIe controller per port, 1 controller in charge of 4 lanes
          Val = ECCR_LANE_SEL_SET (Val, PcieIndex*4);
        } else { // RCB - 8 PCIe controller per port, 1 controller in charge of 2 lanes
          Val = ECCR_LANE_SEL_SET (Val, PcieIndex*2);
        }
        Val = ECCR_GROUP_EVENT_SEL_SET (Val, ErrCtrlCfg[ErrCode]);
        Ac01PcieCsrOut32 (RasDesVSecBase + EVENT_COUNTER_CONTROL_REG_OFF, Val);

        // After setting Counter Control reg
        // This delay just to make sure Counter Data reg is update with new value
        MicroSecondDelay (1);
        Ac01PcieCsrIn32 (RasDesVSecBase + EVENT_COUNTER_DATA_REG_OFF, &Val);
        if (Val != 0) {
          Ret = LINK_CHECK_FAILED;
          PCIE_DEBUG (
            "\tS%d RC%d RP%d \t%s: %d \tGROUP:%d-EVENT:%d\n",
            RC->Socket,
            RC->ID,
            PcieIndex,
            Val,
            ((ErrCtrlCfg[ErrCode] & 0xF00) >> 8),  // Group
            (ErrCtrlCfg[ErrCode] & 0x0FF)          // Event
            );
        }
      }
    }
  }

  // Disable programming to config space
  Ac01PcieCsrIn32 (CfgAddr + MISC_CONTROL_1_OFF, &Val);
  Val = DBI_RO_WR_EN_SET (Val, 0);
  Ac01PcieCsrOut32 (CfgAddr + MISC_CONTROL_1_OFF, Val);

  return Ret;
}

/**
   Get link capabilities link width and speed of endpoint

   @param RC[in]           Pointer to AC01_RC structure
   @param PcieIndex[in]    PCIe controller index
   @param EpMaxWidth[out]  EP max link width
   @param EpMaxGen[out]    EP max link speed
**/
VOID
Ac01PcieCoreGetEndpointInfo (
  IN  AC01_RC  *RC,
  IN  INTN     PcieIndex,
  OUT UINT8    *EpMaxWidth,
  OUT UINT8    *EpMaxGen
  )
{
  VOID         *PcieCapBaseAddr, *EpCfgAddr;
  VOID         *RcCfgAddr;
  UINT32       Val, RestoreVal;
  UINTN        TimeOut;

  RcCfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));

  // Allow programming to config space
  Ac01PcieCsrIn32 (RcCfgAddr + MISC_CONTROL_1_OFF, &Val);
  Val = DBI_RO_WR_EN_SET (Val, 1);
  Ac01PcieCsrOut32 (RcCfgAddr + MISC_CONTROL_1_OFF, Val);

  Ac01PcieCsrIn32 (RcCfgAddr + SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG, &Val);
  RestoreVal = Val;
  Val = SUB_BUS_SET (Val, DEFAULT_SUB_BUS);             /* Set DEFAULT_SUB_BUS to Subordinate bus */
  Val = SEC_BUS_SET (Val, RC->Pcie[PcieIndex].DevNum);  /* Set RC->Pcie[PcieIndex].DevNum to Secondary bus */
  Val = PRIM_BUS_SET (Val, 0x0);                        /* Set 0x0 to Primary bus */
  Ac01PcieCsrOut32 (RcCfgAddr + SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG, Val);
  EpCfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 20)); /* Bus 1, dev 0, func 0 */

  // Loop read EpCfgAddr value until got valid value or
  // reach to timeout PCIE_EP_LINKUP_TIMEOUT (or more depend on card)
  TimeOut = PCIE_EP_LINKUP_TIMEOUT;
  do {
    Ac01PcieCsrIn32 (EpCfgAddr, &Val);
    if (Val != 0xFFFF0001 && Val != 0xFFFFFFFF) {
      break;
    }
    MicroSecondDelay (PCIE_LINK_WAIT_INTERVAL_US);
    TimeOut -= PCIE_LINK_WAIT_INTERVAL_US;
  } while (TimeOut > 0);

  Ac01PcieCsrIn32 (EpCfgAddr, &Val);

  // Check whether EP config space is accessible or not
  if (Val == 0xFFFFFFFF) {
    *EpMaxWidth = 0;   // Invalid Width
    *EpMaxGen = 0;     // Invalid Speed
    PCIE_DEBUG ("PCIE%d.%d Cannot access EP config space!\n", RC->ID, PcieIndex);
  } else {
    PcieCapBaseAddr = (VOID *)PcieCheckCap (RC, PcieIndex, 0x0, PCIE_CAP_ID);
    if (PcieCapBaseAddr == 0) {
      *EpMaxWidth = 0;   // Invalid Width
      *EpMaxGen = 0;     // Invalid Speed
      PCIE_DEBUG (
        "PCIE%d.%d Cannot get PCIe capability extended address!\n",
        RC->ID,
        PcieIndex
        );
    } else {
      Ac01PcieCsrIn32 (PcieCapBaseAddr + LINK_CAPABILITIES_REG_OFF, &Val);
      *EpMaxWidth = (Val >> 4) & 0x3F;
      *EpMaxGen = Val & 0xF;
      PCIE_DEBUG (
        "PCIE%d.%d EP MaxWidth %d EP MaxGen %d \n", RC->ID,
        PcieIndex,
        *EpMaxWidth,
        *EpMaxGen
        );

      // From EP, enabling common clock for upstream
      Ac01PcieCsrIn32 (PcieCapBaseAddr + LINK_CONTROL_LINK_STATUS_OFF, &Val);
      Val = PCIE_CAP_SLOT_CLK_CONFIG_SET (Val, 1);
      Val = PCIE_CAP_COMMON_CLK_SET (Val, 1);
      Ac01PcieCsrOut32 (PcieCapBaseAddr + LINK_CONTROL_LINK_STATUS_OFF, Val);
    }
  }

  // Restore value in order to not affect enumeration process
  Ac01PcieCsrOut32 (RcCfgAddr + SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG, RestoreVal);

  // Disable programming to config space
  Ac01PcieCsrIn32 (RcCfgAddr + MISC_CONTROL_1_OFF, &Val);
  Val = DBI_RO_WR_EN_SET (Val, 0);
  Ac01PcieCsrOut32 (RcCfgAddr + MISC_CONTROL_1_OFF, Val);
}

/**
  Check active PCIe controllers of RC, retrain or soft reset if needed

  @param RC[in]         Pointer to AC01_RC structure
  @param PcieIndex[in]  PCIe controller index

  @retval               -1: Link recovery had failed
                        1: Link width and speed are not correct
                        0: Link recovery succeed
**/
INT32
Ac01PcieCoreQoSLinkCheckRecovery (
  IN AC01_RC   *RC,
  IN INTN      PcieIndex
  )
{
  VOID        *CsrAddr;
  INTN        TimeOut;
  INT32       NumberOfReset = MAX_REINIT; // Number of soft reset retry
  UINT8       EpMaxWidth, EpMaxGen;
  INT32       LinkStatusCheck, RasdesChecking;

  // PCIe controller is not active or Link is not up
  // Nothing to be done
  if ((!RC->Pcie[PcieIndex].Active) || (!RC->Pcie[PcieIndex].LinkUp)) {
    return LINK_CHECK_WRONG_PARAMETER;
  }

  do {
    if (RC->Pcie[PcieIndex].LinkUp) {
      // Enable all of RASDES register to detect any training error
      Ac01PFAEnableAll (RC, PcieIndex, PFA_REG_ENABLE);

      // Accessing Endpoint and checking current link capabilities
      Ac01PcieCoreGetEndpointInfo (RC, PcieIndex, &EpMaxWidth, &EpMaxGen);
      LinkStatusCheck = Ac01PcieCoreLinkCheck (RC, PcieIndex, EpMaxWidth, EpMaxGen);

      // Delay to allow the link to perform internal operation and generate
      // any error status update. This allows detection of any error observed
      // during initial link training. Possible evaluation time can be
      // between 100ms to 200ms.
      MicroSecondDelay (100000);

      // Check for error
      RasdesChecking = Ac01PFAEnableAll (RC, PcieIndex, PFA_REG_READ);

      // Clear error counter
      Ac01PFAEnableAll (RC, PcieIndex, PFA_REG_CLEAR);

      // If link check functions return passed, then breaking out
      // else go to soft reset
      if (LinkStatusCheck != LINK_CHECK_FAILED &&
          RasdesChecking != LINK_CHECK_FAILED &&
          PcieLinkUpCheck (&RC->Pcie[PcieIndex]))
      {
        return LINK_CHECK_SUCCESS;
      }

      RC->Pcie[PcieIndex].LinkUp = FALSE;
    }

    // Trigger controller soft reset
    PCIE_DEBUG ("PCIE%d.%d Start link re-initialization..\n", RC->ID, PcieIndex);
    Ac01PcieCoreSetupRC (RC, 1, PcieIndex);

    // Poll until link up
    // This checking for linkup status and
    // give LTSSM state the time to transit from DECTECT state to L0 state
    // Total delay are 100ms, smaller number of delay cannot always make sure
    // the state transition is completed
    TimeOut = PCIE_LTSSM_TRANSITION_TIMEOUT;
    CsrAddr = (VOID *)RC->Pcie[PcieIndex].CsrAddr;
    do {
      if (PcieLinkUpCheck (&RC->Pcie[PcieIndex])) {
        PCIE_DEBUG (
          "\tPCIE%d.%d LinkStat is correct after soft reset, transition time: %d\n",
          RC->ID,
          PcieIndex,
          TimeOut
          );
        RC->Pcie[PcieIndex].LinkUp = TRUE;
        break;
      }

      MicroSecondDelay (100);
      TimeOut -= 100;
    } while (TimeOut > 0);

    if (TimeOut <= 0) {
      PCIE_DEBUG ("\tPCIE%d.%d LinkStat TIMEOUT after re-init\n", RC->ID, PcieIndex);
    } else {
      PCIE_DEBUG ("PCIE%d.%d Link re-initialization passed!\n", RC->ID, PcieIndex);
    }

    NumberOfReset--;
  } while (NumberOfReset > 0);

  return LINK_CHECK_SUCCESS;
}

VOID
Ac01PcieCoreUpdateLink (
  IN  AC01_RC *RC,
  OUT BOOLEAN *IsNextRoundNeeded,
  OUT INT8    *FailedPciePtr,
  OUT INT8    *FailedPcieCount
  )
{
  INTN      PcieIndex;
  AC01_PCIE *Pcie;
  UINT32    i;
  UINT32    Val;
  VOID      *CfgAddr;

  *IsNextRoundNeeded = FALSE;
  *FailedPcieCount   = 0;
  for (i = 0; i < MAX_PCIE_B; i++) {
    FailedPciePtr[i] = -1;
  }

  if (!RC->Active) {
    return;
  }

  // Loop for all controllers
  for (PcieIndex = 0; PcieIndex < RC->MaxPcieController; PcieIndex++) {
    Pcie = &RC->Pcie[PcieIndex];
    CfgAddr = (VOID *)(RC->MmcfgAddr + (RC->Pcie[PcieIndex].DevNum << 15));

    if (Pcie->Active && !Pcie->LinkUp) {
      if (PcieLinkUpCheck (Pcie)) {
        Pcie->LinkUp = TRUE;
        Ac01PcieCsrIn32 (CfgAddr + LINK_CONTROL_LINK_STATUS_REG, &Val);
        PCIE_DEBUG (
          "%a S%d RC%d RP%d NEGO_LINK_WIDTH: 0x%x LINK_SPEED: 0x%x\n",
          __FUNCTION__,
          RC->Socket,
          RC->ID,
          PcieIndex,
          PCIE_CAP_NEGO_LINK_WIDTH_GET (Val),
          PCIE_CAP_LINK_SPEED_GET (Val)
          );

        // Doing link checking and recovery if needed
        Ac01PcieCoreQoSLinkCheckRecovery (RC, PcieIndex);

        // Un-mask Completion Timeout
        Ac01PcieCsrIn32 (CfgAddr + AMBA_LINK_TIMEOUT_OFF, &Val);
        Val = LINK_TIMEOUT_PERIOD_DEFAULT_SET (Val, 32);
        Ac01PcieCsrOut32 (CfgAddr + AMBA_LINK_TIMEOUT_OFF, Val);
        Ac01PcieCsrIn32 (CfgAddr + UNCORR_ERR_MASK_OFF, &Val);
        Val = CMPLT_TIMEOUT_ERR_MASK_SET (Val, 0);
        Ac01PcieCsrOut32 (CfgAddr + UNCORR_ERR_MASK_OFF, Val);
      } else {
        *IsNextRoundNeeded = FALSE;
        FailedPciePtr[*FailedPcieCount] = PcieIndex;
        *FailedPcieCount += 1;
      }
    }
  }
}
