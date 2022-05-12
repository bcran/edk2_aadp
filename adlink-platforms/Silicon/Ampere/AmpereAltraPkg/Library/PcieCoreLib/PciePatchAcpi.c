/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <AcpiHeader.h>
#include <Guid/PlatformInfoHobGuid.h>
#include <IndustryStandard/Acpi30.h>
#include <IndustryStandard/IoRemappingTable.h>
#include <Library/AcpiHelperLib.h>
#include <Library/AmpereCpuLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PcieBoardLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Platform/Ac01.h>
#include <PlatformInfoHob.h>
#include <Protocol/AcpiTable.h>

#include "Pcie.h"
#include "PcieCore.h"

#define ACPI_RESOURCE_NAME_ADDRESS16            0x88
#define ACPI_RESOURCE_NAME_ADDRESS64            0x8A

#define RCA_NUM_TBU_PMU         6
#define RCB_NUM_TBU_PMU         10

// Required to be 1 to match the kernel quirk for ECAM
#define EFI_ACPI_MCFG_OEM_REVISION 1

STATIC UINT32 gTbuPmuIrqArray[] = { SMMU_TBU_PMU_IRQ_START_ARRAY };
STATIC UINT32 gTcuPmuIrqArray[] = { SMMU_TCU_PMU_IRQ_START_ARRAY };

#pragma pack(1)
typedef struct
{
  UINT64 ullBaseAddress;
  UINT16 usSegGroupNum;
  UINT8  ucStartBusNum;
  UINT8  ucEndBusNum;
  UINT32 Reserved2;
} EFI_MCFG_CONFIG_STRUCTURE;

typedef struct
{
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT64                      Reserved1;
} EFI_MCFG_TABLE_CONFIG;

typedef struct {
  UINT64 AddressGranularity;
  UINT64 AddressMin;
  UINT64 AddressMax;
  UINT64 AddressTranslation;
  UINT64 RangeLength;
} QWordMemory;

typedef struct ResourceEntry {
  UINT8  ResourceType;
  UINT16 ResourceSize;
  UINT8  Attribute;
  UINT8  Byte0;
  UINT8  Byte1;
  VOID   *ResourcePtr;
} RESOURCE;

STATIC QWordMemory Qmem[] = {
  { AC01_PCIE_RCA2_QMEM },
  { AC01_PCIE_RCA3_QMEM },
  { AC01_PCIE_RCB0_QMEM },
  { AC01_PCIE_RCB1_QMEM },
  { AC01_PCIE_RCB2_QMEM },
  { AC01_PCIE_RCB3_QMEM }
};

typedef struct {
  EFI_ACPI_6_0_IO_REMAPPING_NODE Node;
  UINT64                         Base;
  UINT32                         Flags;
  UINT32                         Reserved;
  UINT64                         VatosAddress;
  UINT32                         Model;
  UINT32                         Event;
  UINT32                         Pri;
  UINT32                         Gerr;
  UINT32                         Sync;
  UINT32                         ProximityDomain;
  UINT32                         DeviceIdMapping;
} EFI_ACPI_6_2_IO_REMAPPING_SMMU3_NODE;

typedef struct {
  EFI_ACPI_6_0_IO_REMAPPING_ITS_NODE Node;
  UINT32                             ItsIdentifier;
} AC01_ITS_NODE;


typedef struct {
  EFI_ACPI_6_0_IO_REMAPPING_RC_NODE  Node;
  EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE RcIdMapping;
} AC01_RC_NODE;

typedef struct {
  EFI_ACPI_6_2_IO_REMAPPING_SMMU3_NODE Node;
  EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE   InterruptMsiMapping;
  EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE   InterruptMsiMappingSingle;
} AC01_SMMU_NODE;

typedef struct {
  EFI_ACPI_6_0_IO_REMAPPING_TABLE Iort;
  AC01_ITS_NODE                   ItsNode[2];
  AC01_RC_NODE                    RcNode[2];
  AC01_SMMU_NODE                  SmmuNode[2];
} AC01_IO_REMAPPING_STRUCTURE;

#define FIELD_OFFSET(type, name)            __builtin_offsetof (type, name)
#define __AC01_ID_MAPPING(In, Num, Out, Ref, Flags)    \
  {                                                    \
    In,                                                \
    Num,                                               \
    Out,                                               \
    FIELD_OFFSET (AC01_IO_REMAPPING_STRUCTURE, Ref),   \
    Flags                                              \
  }

EFI_STATUS
EFIAPI
AcpiPatchPciMem32 (
  INT8 *PciSegEnabled
  )
{
  EFI_ACPI_SDT_PROTOCOL *AcpiTableProtocol;
  EFI_STATUS            Status;
  UINTN                 Idx, Ix;
  EFI_ACPI_HANDLE       TableHandle, SegHandle;
  CHAR8                 Buffer[MAX_ACPI_NODE_PATH];
  CHAR8                 *KB, *B;
  EFI_ACPI_DATA_TYPE    DataType;
  UINTN                 DataSize, Mem32;
  RESOURCE              *Rs;
  QWordMemory           *Qm;
  UINT8                 Segment;

  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **)&AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    PCIE_ERR ("Unable to locate ACPI table protocol Guid\n");
    return Status;
  }

  /* Open DSDT Table */
  Status = AcpiOpenDSDT (AcpiTableProtocol, &TableHandle);
  if (EFI_ERROR (Status)) {
    PCIE_ERR ("Unable to open DSDT table\n");
    return Status;
  }

  for (Idx = 0; PciSegEnabled[Idx] != -1; Idx++) {
    if (PciSegEnabled[Idx] > SOCKET0_LAST_RC) { /* Physical segment */
      break;
    }
    if (PciSegEnabled[Idx] < SOCKET0_FIRST_RC) {
      continue;
    }

    /* DSDT PCI devices to use Physical segment */
    AsciiSPrint (Buffer, sizeof (Buffer), "\\_SB.PCI%x.RBUF", PciSegEnabled[Idx]);
    Status = AcpiTableProtocol->FindPath (TableHandle, (VOID *)Buffer, &SegHandle);
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (Ix = 0; Ix < 3; Ix++) {
      Status = AcpiTableProtocol->GetOption (
                                    SegHandle,
                                    Ix,
                                    &DataType,
                                    (VOID *)&B,
                                    &DataSize
                                    );
      KB = B;
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (Ix == 0) { /* B[0] == AML_NAME_OP */
        if (!((DataSize == 1) && (DataType == EFI_ACPI_DATA_TYPE_OPCODE))) {
          break;
        }
      } else if (Ix == 1) { /* *B == "RBUF"  */
        if (!((DataSize == 4) && (DataType == EFI_ACPI_DATA_TYPE_NAME_STRING))) {
          break;
        }
      } else { /* Ix:2 11 42 07 0A 6E 88 ... */
        if (DataType != EFI_ACPI_DATA_TYPE_CHILD) {
          break;
        }

        KB += 5; /* Point to Resource type */
        Rs = (RESOURCE *)KB;
        Mem32 = 0;
        while ((Mem32 == 0) && ((KB - B) < DataSize)) {
          if (Rs->ResourceType == ACPI_RESOURCE_NAME_ADDRESS16) {
            KB += (Rs->ResourceSize + 3); /* Type + Size */
            Rs = (RESOURCE *)KB;
          } else if (Rs->ResourceType == ACPI_RESOURCE_NAME_ADDRESS64) {

            if (Rs->Attribute == 0x00) { /* The first QWordMemory */
              Mem32 = 1;
              Segment = PciSegEnabled[Idx] - 2;
              Qm = (QWordMemory *)&(Rs->ResourcePtr);
              *Qm = Qmem[Segment]; /* Physical segment */
            }
            KB += (Rs->ResourceSize + 3); /* Type + Size */
            Rs = (RESOURCE *)KB;
          }
        }
        if (Mem32 != 0) {
          Status = AcpiTableProtocol->SetOption (
                                        SegHandle,
                                        Ix,
                                        (VOID *)B,
                                        DataSize
                                        );
        }
      }
    }
  }
  AcpiTableProtocol->Close (TableHandle);
  /* Update DSDT Checksum */
  AcpiDSDTUpdateChecksum (AcpiTableProtocol);

  return Status;
}

VOID
ConstructMcfg (
  VOID   *McfgPtr,
  INT8   *PciSegEnabled,
  UINT32 McfgCount
  )
{
  EFI_MCFG_TABLE_CONFIG     McfgHeader = {
    {
      EFI_ACPI_6_1_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE,
      McfgCount,
      1,
      0x00,                        // Checksum will be updated at runtime
      EFI_ACPI_OEM_ID,
      EFI_ACPI_OEM_TABLE_ID,
      EFI_ACPI_MCFG_OEM_REVISION,
      EFI_ACPI_CREATOR_ID,
      EFI_ACPI_CREATOR_REVISION
    },
    0x0000000000000000,            // Reserved
  };
  EFI_MCFG_CONFIG_STRUCTURE TMcfg = {
    .ullBaseAddress = 0,
    .usSegGroupNum = 0,
    .ucStartBusNum = 0,
    .ucEndBusNum = 255,
    .Reserved2 = 0,
  };
  UINT32                    Idx;
  VOID                      *TMcfgPtr = McfgPtr;
  AC01_RC                   *Rc;

  CopyMem (TMcfgPtr, &McfgHeader, sizeof (EFI_MCFG_TABLE_CONFIG));
  TMcfgPtr += sizeof (EFI_MCFG_TABLE_CONFIG);
  for (Idx = 0; PciSegEnabled[Idx] != -1; Idx++) {
    Rc = GetRCList (PciSegEnabled[Idx]); /* Logical */
    TMcfg.ullBaseAddress = Rc->MmcfgAddr;
    TMcfg.usSegGroupNum = Rc->Logical;
    CopyMem (TMcfgPtr, &TMcfg, sizeof (EFI_MCFG_CONFIG_STRUCTURE));
    TMcfgPtr += sizeof (EFI_MCFG_CONFIG_STRUCTURE);
  }
}

EFI_STATUS
EFIAPI
AcpiInstallMcfg (
  INT8 *PciSegEnabled
  )
{
  UINT32                  RcCount, McfgCount;
  EFI_ACPI_TABLE_PROTOCOL *AcpiTableProtocol;
  UINTN                   TableKey;
  EFI_STATUS              Status;
  VOID                    *McfgPtr;

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID **)&AcpiTableProtocol
                  );
  if (EFI_ERROR (Status)) {
    PCIE_ERR ("MCFG: Unable to locate ACPI table entry\n");
    return Status;
  }
  for (RcCount = 0; PciSegEnabled[RcCount] != -1; RcCount++) {
  }
  McfgCount = sizeof (EFI_MCFG_TABLE_CONFIG) + sizeof (EFI_MCFG_CONFIG_STRUCTURE) * RcCount;
  McfgPtr = AllocateZeroPool (McfgCount);
  if (McfgPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ConstructMcfg (McfgPtr, PciSegEnabled, McfgCount);
  Status = AcpiTableProtocol->InstallAcpiTable (
                                AcpiTableProtocol,
                                McfgPtr,
                                McfgCount,
                                &TableKey
                                );
  if (EFI_ERROR (Status)) {
    PCIE_ERR ("MCFG: Unable to install MCFG table entry\n");
  }
  FreePool (McfgPtr);
  return Status;
}

STATIC
VOID
ConstructIort (
  VOID   *IortPtr,
  UINT32 RcCount,
  UINT32 SmmuPmuAgentCount,
  UINT32 HeaderCount,
  INT8   *PciSegEnabled
  )
{
  EFI_ACPI_6_0_IO_REMAPPING_TABLE TIort = {
    .Header = __ACPI_HEADER (
                EFI_ACPI_6_0_IO_REMAPPING_TABLE_SIGNATURE,
                AC01_IO_REMAPPING_STRUCTURE,
                EFI_ACPI_IO_REMAPPING_TABLE_REVISION
                ),
    .NumNodes = (3 * RcCount) + SmmuPmuAgentCount,
    .NodeOffset = sizeof (EFI_ACPI_6_0_IO_REMAPPING_TABLE),
    0
  };

  AC01_ITS_NODE TItsNode = {
    .Node = {
      EFI_ACPI_IORT_TYPE_ITS_GROUP,
      sizeof (EFI_ACPI_6_0_IO_REMAPPING_ITS_NODE) + 4,
      0x0,
      0x0,
      0x0,
      0x0,
      .NumItsIdentifiers = 1,
    },
    .ItsIdentifier = 1,
  };

  AC01_RC_NODE TRcNode = {
    {
      {
        EFI_ACPI_IORT_TYPE_ROOT_COMPLEX,
        sizeof (AC01_RC_NODE),
        0x1,
        0x0,
        0x1,
        FIELD_OFFSET (AC01_RC_NODE, RcIdMapping),
      },
      EFI_ACPI_IORT_MEM_ACCESS_PROP_CCA,
      0x0,
      0x0,
      EFI_ACPI_IORT_MEM_ACCESS_FLAGS_CPM |
      EFI_ACPI_IORT_MEM_ACCESS_FLAGS_DACS,
      EFI_ACPI_IORT_ROOT_COMPLEX_ATS_UNSUPPORTED,
      .PciSegmentNumber = 0,
      .MemoryAddressSize = 64,
    },
    __AC01_ID_MAPPING (0x0, 0xffff, 0x0, SmmuNode, 0),
  };

  AC01_SMMU_NODE TSmmuNode = {
    {
      {
        EFI_ACPI_IORT_TYPE_SMMUv3,
        sizeof (AC01_SMMU_NODE),
        0x2,  /* Revision */
        0x0,
        0x2,  /* Mapping Count */
        FIELD_OFFSET (AC01_SMMU_NODE, InterruptMsiMapping),
      },
      .Base = 0,
      EFI_ACPI_IORT_SMMUv3_FLAG_COHAC_OVERRIDE,
      0,
      0,
      0,
      0,
      0,
      0x0,
      0x0,
      0,  // Proximity domain - need fill in
      .DeviceIdMapping = 1,
    },
    __AC01_ID_MAPPING (0x0, 0xffff, 0, SmmuNode, 0),
    __AC01_ID_MAPPING (0x0, 0x1, 0, SmmuNode, 1),
  };

  EFI_ACPI_6_0_IO_REMAPPING_PMCG_NODE TPmcgNode = {
    {
      EFI_ACPI_IORT_TYPE_PMCG,
      sizeof (EFI_ACPI_6_0_IO_REMAPPING_PMCG_NODE),
      0x1,
      0x0,
      0x0,
      0x0,
    },
    0, /* Page 0 Base. Need to be filled */
    0, /* GSIV. Need to be filled */
    0, /* Node reference. Need to be filled */
    0, /* Page 1 Base. Need to be filled. */
  };

  AC01_RC             *Rc;
  PLATFORM_INFO_HOB   *PlatformHob;
  UINT32              Idx, Idx1, SmmuNodeOffset[MAX_AC01_PCIE_ROOT_COMPLEX];
  UINT32              ItsOffset[MAX_AC01_PCIE_ROOT_COMPLEX];
  UINTN               NumTbuPmu;
  VOID                *Hob;
  VOID                *TIortPtr = IortPtr, *SmmuPtr, *PmcgPtr;

  /* Get the Platform HOB */
  Hob = GetFirstGuidHob (&gPlatformHobGuid);
  if (Hob == NULL) {
    return;
  }

  PlatformHob = (PLATFORM_INFO_HOB *)GET_GUID_HOB_DATA (Hob);

  TIort.Header.Length = HeaderCount;
  CopyMem (TIortPtr, &TIort, sizeof (EFI_ACPI_6_0_IO_REMAPPING_TABLE));
  TIortPtr += sizeof (EFI_ACPI_6_0_IO_REMAPPING_TABLE);
  for (Idx = 0; Idx < RcCount; Idx++) {
    ItsOffset[Idx] = TIortPtr - IortPtr;
    TItsNode.ItsIdentifier = PciSegEnabled[Idx]; /* Physical */
    CopyMem (TIortPtr, &TItsNode, sizeof (AC01_ITS_NODE));
    TIortPtr += sizeof (AC01_ITS_NODE);
  }

  SmmuPtr = TIortPtr + RcCount * sizeof (AC01_RC_NODE);
  PmcgPtr = SmmuPtr + RcCount * sizeof (AC01_SMMU_NODE);
  for (Idx = 0; Idx < RcCount; Idx++) {
    SmmuNodeOffset[Idx] = SmmuPtr - IortPtr;
    Rc = GetRCList (PciSegEnabled[Idx]); /* Physical RC */
    TSmmuNode.Node.Base = Rc->TcuAddr;
    TSmmuNode.InterruptMsiMapping.OutputBase = PciSegEnabled[Idx] << 16;
    TSmmuNode.InterruptMsiMapping.OutputReference = ItsOffset[Idx];
    TSmmuNode.InterruptMsiMappingSingle.OutputBase = PciSegEnabled[Idx] << 16;
    TSmmuNode.InterruptMsiMappingSingle.OutputReference = ItsOffset[Idx];
    /* All RCs on master be assigned to node 0, while remote RCs be assigned to first remote node */
    TSmmuNode.Node.Flags = EFI_ACPI_IORT_SMMUv3_FLAG_PROXIMITY_DOMAIN;
    TSmmuNode.Node.ProximityDomain = 0;
    if ((Rc->TcuAddr & SLAVE_SOCKET_BASE_ADDRESS_OFFSET) != 0) {
      /* RC on remote socket */
      TSmmuNode.Node.Flags = EFI_ACPI_IORT_SMMUv3_FLAG_PROXIMITY_DOMAIN;
      switch (PlatformHob->SubNumaMode[0]) {
      case SUBNUMA_MODE_MONOLITHIC:
        TSmmuNode.Node.ProximityDomain += MONOLITIC_NUM_OF_REGION;
        break;
      case SUBNUMA_MODE_HEMISPHERE:
        TSmmuNode.Node.ProximityDomain += HEMISPHERE_NUM_OF_REGION;
        break;
      case SUBNUMA_MODE_QUADRANT:
        TSmmuNode.Node.ProximityDomain += QUADRANT_NUM_OF_REGION;
        break;
      }
    }
    CopyMem (SmmuPtr, &TSmmuNode, sizeof (AC01_SMMU_NODE));
    SmmuPtr += sizeof (AC01_SMMU_NODE);

    if (!SmmuPmuAgentCount) {
      continue;
    }

    /* Add PMCG nodes */
    if (Rc->Type == RCA) {
      NumTbuPmu = RCA_NUM_TBU_PMU;
    } else {
      NumTbuPmu = RCB_NUM_TBU_PMU;
    }
    for (Idx1 = 0; Idx1 < NumTbuPmu; Idx1++) {
      TPmcgNode.Base = Rc->TcuAddr;
      if (NumTbuPmu == RCA_NUM_TBU_PMU) {
        switch (Idx1) {
        case 0:
          TPmcgNode.Base += 0x40000;
          break;

        case 1:
          TPmcgNode.Base += 0x60000;
          break;

        case 2:
          TPmcgNode.Base += 0xA0000;
          break;

        case 3:
          TPmcgNode.Base += 0xE0000;
          break;

        case 4:
          TPmcgNode.Base += 0x100000;
          break;

        case 5:
          TPmcgNode.Base += 0x140000;
          break;
        }
      } else {
       switch (Idx1) {
        case 0:
          TPmcgNode.Base += 0x40000;
          break;

        case 1:
          TPmcgNode.Base += 0x60000;
          break;

        case 2:
          TPmcgNode.Base += 0xA0000;
          break;

        case 3:
          TPmcgNode.Base += 0xE0000;
          break;

        case 4:
          TPmcgNode.Base += 0x120000;
          break;

        case 5:
          TPmcgNode.Base += 0x160000;
          break;

        case 6:
          TPmcgNode.Base += 0x180000;
          break;

        case 7:
          TPmcgNode.Base += 0x1C0000;
          break;

        case 8:
          TPmcgNode.Base += 0x200000;
          break;

        case 9:
          TPmcgNode.Base += 0x240000;
          break;
        }
      }
      TPmcgNode.Page1Base = TPmcgNode.Base + 0x12000;
      TPmcgNode.Base += 0x2000;
      TPmcgNode.NodeReference = SmmuNodeOffset[Idx];
      TPmcgNode.OverflowInterruptGsiv = gTbuPmuIrqArray[PciSegEnabled[Idx]] + Idx1;
      CopyMem (PmcgPtr, &TPmcgNode, sizeof (TPmcgNode));
      PmcgPtr += sizeof (TPmcgNode);
    }

    /* TCU PMCG */
    TPmcgNode.Base = Rc->TcuAddr;
    TPmcgNode.Base += 0x2000;
    TPmcgNode.Page1Base = Rc->TcuAddr + 0x12000;
    TPmcgNode.NodeReference = SmmuNodeOffset[Idx];
    TPmcgNode.OverflowInterruptGsiv = gTcuPmuIrqArray[PciSegEnabled[Idx]];
    CopyMem (PmcgPtr, &TPmcgNode, sizeof (TPmcgNode));
    PmcgPtr += sizeof (TPmcgNode);
  }

  for (Idx = 0; Idx < RcCount; Idx++) {
    TRcNode.Node.PciSegmentNumber = GetRCList (PciSegEnabled[Idx])->Logical; /* Logical */
    TRcNode.RcIdMapping.OutputReference = SmmuNodeOffset[Idx];
    CopyMem (TIortPtr, &TRcNode, sizeof (AC01_RC_NODE));
    TIortPtr += sizeof (AC01_RC_NODE);
  }
}

EFI_STATUS
EFIAPI
AcpiInstallIort (
  INT8 *PciSegEnabled
  )
{
  UINT32                  RcCount, SmmuPmuAgentCount, TotalCount;
  VOID                    *IortPtr;
  UINTN                   TableKey;
  EFI_STATUS              Status;
  EFI_ACPI_TABLE_PROTOCOL *AcpiTableProtocol;

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID **)&AcpiTableProtocol
                  );
  if (EFI_ERROR (Status)) {
    PCIE_ERR ("IORT: Unable to locate ACPI table entry\n");
    return Status;
  }

  for (RcCount = 0, SmmuPmuAgentCount = 0; PciSegEnabled[RcCount] != -1; RcCount++) {
    if ((GetRCList (PciSegEnabled[RcCount]))->Type == RCA) {
      SmmuPmuAgentCount += RCA_NUM_TBU_PMU;
    } else {
      SmmuPmuAgentCount += RCB_NUM_TBU_PMU;
    }
    SmmuPmuAgentCount += 1; /* Only 1 TCU */
  }

  if (!PcieBoardCheckSmmuPmuEnabled ()) {
    SmmuPmuAgentCount = 0;
  }

  TotalCount = sizeof (EFI_ACPI_6_0_IO_REMAPPING_TABLE) +
               RcCount * (sizeof (AC01_ITS_NODE) + sizeof (AC01_RC_NODE) + sizeof (AC01_SMMU_NODE)) +
               SmmuPmuAgentCount * sizeof (EFI_ACPI_6_0_IO_REMAPPING_PMCG_NODE);

  IortPtr = AllocateZeroPool (TotalCount);
  if (IortPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ConstructIort (IortPtr, RcCount, SmmuPmuAgentCount, TotalCount, PciSegEnabled);

  Status = AcpiTableProtocol->InstallAcpiTable (
                                AcpiTableProtocol,
                                IortPtr,
                                TotalCount,
                                &TableKey
                                );
  if (EFI_ERROR (Status)) {
    PCIE_ERR ("IORT: Unable to install IORT table entry\n");
  }
  FreePool (IortPtr);
  return Status;
}
