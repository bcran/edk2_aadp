/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef X16_CAP_PORT_CFG_H_
#define X16_CAP_PORT_CFG_H_


/* PCIe config space capabilities offset */
#define PM_CAP          0x40
#define MSI_CAP         0x50
#define PCIE_CAP        0x70
#define MSIX_CAP        0xB0
#define SLOT_CAP        0xC0
#define VPD_CAP         0xD0
#define SATA_CAP        0xE0
#define CFG_NEXT_CAP    0x40
#define PM_NEXT_CAP     0x70
#define MSI_NEXT_CAP    0x00
#define PCIE_NEXT_CAP   0x00
#define MSIX_NEXT_CAP   0x00
#define SLOT_NEXT_CAP   0x00
#define VPD_NEXT_CAP    0x00
#define SATA_NEXT_CAP   0x00
#define BASE_CAP        0x100
#define AER_CAP         0x100
#define VC_CAP          0x148
#define SN_CAP          0x178
#define PB_CAP          0x178
#define ARI_CAP         0x178
#define SPCIE_CAP_x8    0x148
#define SPCIE_CAP       0x178
#define PL16G_CAP       0x1A8
#define MARGIN_CAP      0x1D8
#define PL32G_CAP       0x220
#define SRIOV_CAP       0x220
#define TPH_CAP         0x220
#define ATS_CAP         0x220
#define ACS_CAP         0x230
#define PRS_CAP         0x238
#define LTR_CAP         0x248
#define L1SUB_CAP       0x248
#define PASID_CAP       0x248
#define DPA_CAP         0x248
#define DPC_CAP         0x248
#define MPCIE_CAP       0x248
#define FRSQ_CAP        0x248
#define RTR_CAP         0x248
#define LN_CAP          0x248
#define RAS_DES_CAP     0x248
#define VSECRAS_CAP     0x348
#define DLINK_CAP       0x380
#define PTM_CAP         0x38C
#define PTM_VSEC_CAP    0x38C
#define CCIX_TP_CAP     0x38C
#define CXS_CAP         0x3D0
#define RBAR_CAP        0x3E8
#define VF_RBAR_CAP     0x3E8

#endif /* X16_CAP_PORT_CFG_H_ */
