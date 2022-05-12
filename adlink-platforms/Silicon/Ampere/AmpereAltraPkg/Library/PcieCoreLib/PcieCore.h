/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef PCIECORE_H_
#define PCIECORE_H_

#include <IndustryStandard/Pci.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PciHostBridgeLib.h>
#include <Library/PciLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiLib.h>

#include "Pcie.h"
#include "PcieCoreCapCfg.h"

#ifndef BIT
#define BIT(nr)                         (1 << (nr))
#endif

#define MAX_REINIT                       3

#define LINK_CHECK_SUCCESS               0
#define LINK_CHECK_FAILED                -1
#define LINK_CHECK_WRONG_PARAMETER       1

#define PFA_REG_ENABLE                   0
#define PFA_REG_READ                     1
#define PFA_REG_CLEAR                    2

#define AMPERE_PCIE_VENDORID             0x1DEF
#define AC01_HOST_BRIDGE_DEVICEID_RCA    0xE100
#define AC01_HOST_BRIDGE_DEVICEID_RCB    0xE110
#define AC01_PCIE_BRIDGE_DEVICEID_RCA    0xE101
#define AC01_PCIE_BRIDGE_DEVICEID_RCB    0xE111

#define PCIE_MEMRDY_TIMEOUT              10          // 10 us
#define PCIE_PIPE_CLOCK_TIMEOUT          20000       // 20,000 us
#define PCIE_LTSSM_TRANSITION_TIMEOUT    100000      // 100 ms in total
#define PCIE_EP_LINKUP_TIMEOUT           (10 * 1000) // 10ms
#define PCIE_LINK_WAIT_INTERVAL_US       50

#define LINK_POLL_US_TIMER      1
#define IO_SPACE                0x2000
#define MMIO32_SPACE            0x8000000ULL
#define MMIO_SPACE              0x3FFE0000000ULL

#define TCU_OFFSET              0
#define HB_CSR_OFFSET           0x01000000
#define PCIE0_CSR_OFFSET        0x01010000
#define SNPSRAM_OFFSET          0x9000
#define SERDES_CSR_OFFSET       0x01200000
#define MMCONFIG_OFFSET         0x10000000


/* DATA LINK registers */
#define DLINK_VENDOR_CAP_ID       0x25
#define DLINK_VSEC                0x80000001
#define DATA_LINK_FEATURE_CAP_OFF 0X4

/* PL16 CAP registers */
#define PL16_CAP_ID                     0x26
#define PL16G_CAP_OFF_20H_REG_OFF       0x20
#define PL16G_STATUS_REG_OFF            0x0C
#define PL16G_STATUS_EQ_CPL_GET(val)    (val & 0x1)
#define PL16G_STATUS_EQ_CPL_P1_GET(val) ((val & 0x2) >> 1)
#define PL16G_STATUS_EQ_CPL_P2_GET(val) ((val & 0x4) >> 2)
#define PL16G_STATUS_EQ_CPL_P3_GET(val) ((val & 0x8) >> 3)
#define DSP_16G_TX_PRESET0_SET(dst,src) (((dst) & ~0xF) | (((UINT32) (src)) & 0xF))
#define DSP_16G_TX_PRESET1_SET(dst,src) (((dst) & ~0xF00) | (((UINT32) (src) << 8) & 0xF00))
#define DSP_16G_TX_PRESET2_SET(dst,src) (((dst) & ~0xF0000) | (((UINT32) (src) << 16) & 0xF0000))
#define DSP_16G_TX_PRESET3_SET(dst,src) (((dst) & ~0xF000000) | (((UINT32) (src) << 24) & 0xF000000))
#define DSP_16G_RXTX_PRESET0_SET(dst,src) (((dst) & ~0xFF) | (((UINT32) (src)) & 0xFF))
#define DSP_16G_RXTX_PRESET1_SET(dst,src) (((dst) & ~0xFF00) | (((UINT32) (src) << 8) & 0xFF00))
#define DSP_16G_RXTX_PRESET2_SET(dst,src) (((dst) & ~0xFF0000) | (((UINT32) (src) << 16) & 0xFF0000))
#define DSP_16G_RXTX_PRESET3_SET(dst,src) (((dst) & ~0xFF000000) | (((UINT32) (src) << 24) & 0xFF000000))

/* PCIe PF0_PORT_LOGIC registers */
#define PORT_LOCIG_VC0_P_RX_Q_CTRL_OFF      0x748
#define PORT_LOCIG_VC0_NP_RX_Q_CTRL_OFF     0x74C

/* TCU registers */
#define SMMU_GBPA    0x044

/* SNPSRAM Synopsys Memory Read/Write Margin registers */
#define SPRF_RMR                0x0
#define SPSRAM_RMR              0x4
#define TPRF_RMR                0x8
#define TPSRAM_RMR              0xC

//
// Host bridge registers
//
#define HBRCAPDMR               0x0
#define HBRCBPDMR               0x4
#define HBPDVIDR                0x10
#define HBPRBNR                 0x14
#define HBPREVIDR               0x18
#define HBPSIDR                 0x1C
#define HBPCLSSR                0x20

// HBRCAPDMR
#define RCAPCIDEVMAP_SET(dst, src) (((dst) & ~0x7) | (((UINT32) (src)) & 0x7))
#define RCAPCIDEVMAP_GET(val) ((val) & 0x7)

// HBRCBPDMR
#define RCBPCIDEVMAPLO_SET(dst, src) (((dst) & ~0x7) | (((UINT32) (src)) & 0x7))
#define RCBPCIDEVMAPLO_GET(val) ((val) & 0x7)

#define RCBPCIDEVMAPHI_SET(dst, src) (((dst) & ~0x70) | (((UINT32) (src) << 4) & 0x70))
#define RCBPCIDEVMAPHI_GET(val) (((val) & 0x7) >> 4)

// HBPDVIDR
#define PCIVENDID_SET(dst, src) (((dst) & ~0xFFFF) | (((UINT32) (src))  & 0xFFFF))
#define PCIVENDID_GET(val) ((val) & 0xFFFF)

#define PCIDEVID_SET(dst, src) (((dst) & ~0xFFFF0000) | (((UINT32) (src) << 16) & 0xFFFF0000))
#define PCIDEVID_GET(val) (((val) & 0xFFFF0000) >> 16)

// HBPRBNR
#define PCIRBNUM_SET(dst, src) (((dst) & ~0x1F) | (((UINT32) (src)) & 0x1F))

// HBPREVIDR
#define PCIREVID_SET(dst, src) (((dst) & ~0xFF) | (((UINT32) (src)) & 0xFF))

// HBPSIDR
#define PCISUBSYSVENDID_SET(dst, src) (((dst) & ~0xFFFF) | (((UINT32) (src)) & 0xFFFF))

#define PCISUBSYSID_SET(dst, src) (((dst) & ~0xFFFF0000) | (((UINT32) (src) << 16) & 0xFFFF0000))

// HBPCLSSR
#define CACHELINESIZE_SET(dst, src) (((dst) & ~0xFF) | (((UINT32) (src)) & 0xFF))

//
// PCIE core register
//
#define LINKCTRL                0x0
#define LINKSTAT                0x4
#define IRQSEL                  0xC
#define HOTPLUGSTAT             0x28
#define IRQENABLE               0x30
#define IRQEVENTSTAT            0x38
#define BLOCKEVENTSTAT          0x3c
#define RESET                   0xC000
#define CLOCK                   0xC004
#define MEMRDYR                 0xC104
#define RAMSDR                  0xC10C

// LINKCTRL
#define LTSSMENB_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))
#define DEVICETYPE_SET(dst, src) (((dst) & ~0xF0) | (((UINT32) (src) << 4) & 0xF0))
#define DEVICETYPE_GET(val) (((val) & 0xF0) >> 4)

// LINKSTAT
#define PHY_STATUS_MASK         (1 << 2)
#define SMLH_LTSSM_STATE_MASK   0x3f00
#define SMLH_LTSSM_STATE_GET(val) ((val & 0x3F00) >> 8)
#define RDLH_SMLH_LINKUP_STATUS_GET(val)    (val & 0x3)
#define PHY_STATUS_MASK_BIT     0x04
#define SMLH_LINK_UP_MASK_BIT   0x02
#define RDLH_LINK_UP_MASK_BIT   0x01

// IRQSEL
#define AER_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))
#define PME_SET(dst, src) (((dst) & ~0x2) | (((UINT32) (src) << 1) & 0x2))
#define LINKAUTOBW_SET(dst, src) (((dst) & ~0x4) | (((UINT32) (src) << 2) & 0x4))
#define BWMGMT_SET(dst, src) (((dst) & ~0x8) | (((UINT32) (src) << 3) & 0x8))
#define EQRQST_SET(dst, src) (((dst) & ~0x10) | (((UINT32) (src) << 4) & 0x10))
#define INTPIN_SET(dst, src) (((dst) & ~0xFF00) | (((UINT32) (src) << 8) & 0xFF00))

// SLOTCAP
#define SLOT_HPC_SET(dst, src) (((dst) & ~0x40) | (((UINT32) (src) << 6) & 0x40))

// SLOT_CAPABILITIES_REG, PCIE_CAP_SLOT_POWER_LIMIT_VALUE bits[14:7]
#define PCIE_SLOT_CAP_SLOT_POWER_LIMIT_VALUE_SET(dst, src) \
                            (((dst) & ~0x7F80) | (((UINT32)(src) << 7) & 0x7F80))

// HOTPLUGSTAT
#define PWR_IND_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))
#define ATTEN_IND_SET(dst, src) (((dst) & ~0x2) | (((UINT32) (src) << 1) & 0x2))
#define PWR_CTRL_SET(dst, src) (((dst) & ~0x4) | (((UINT32) (src) << 2) & 0x4))
#define EML_CTRL_SET(dst, src) (((dst) & ~0x8) | (((UINT32) (src) << 3) & 0x8))

// IRQENABLE
#define LINKUP_SET(dst, src) (((dst) & ~0x40) | (((UINT32) (src) << 6) & 0x40))

// IRQEVENTSTAT
#define BLOCK_INT_MASK          (1 << 4)
#define PCIE_INT_MASK           (1 << 3)

// BLOCKEVENTSTAT
#define LINKUP_MASK             (1 << 0)

// RESET
#define DWCPCIE_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))
#define RESET_MASK              0x1

// CLOCK
#define AXIPIPE_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))

// RAMSDR
#define SD_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))

//
// PHY registers
//
#define RSTCTRL                 0x0
#define PHYCTRL                 0x4
#define RAMCTRL                 0x8
#define RAMSTAT                 0xC
#define PLLCTRL                 0x10
#define PHYLPKCTRL              0x14
#define PHYTERMOFFSET0          0x18
#define PHYTERMOFFSET1          0x1C
#define PHYTERMOFFSET2          0x20
#define PHYTERMOFFSET3          0x24
#define RXTERM                  0x28
#define PHYDIAGCTRL             0x2C

// RSTCTRL
#define PHY_RESET_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))

// PHYCTRL
#define PWR_STABLE_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))

//
// PCIe config space registers
//
#define TYPE1_DEV_ID_VEND_ID_REG                0
#define TYPE1_CLASS_CODE_REV_ID_REG             0x8
#define TYPE1_CAP_PTR_REG                       0x34
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG       0x18
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG        0x3c
#define CON_STATUS_REG                          (PM_CAP + 0x4)
#define LINK_CAPABILITIES_REG                   (PCIE_CAP + 0xc)
#define LINK_CONTROL_LINK_STATUS_REG            (PCIE_CAP + 0x10)
#define SLOT_CAPABILITIES_REG                   (PCIE_CAP + 0x14)
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG      (PCIE_CAP + 0x28)
#define LINK_CAPABILITIES2_REG                  (PCIE_CAP + 0x2c)
#define LINK_CONTROL2_LINK_STATUS2_REG          (PCIE_CAP + 0x30)
#define UNCORR_ERR_STATUS_OFF                   (AER_CAP + 0x4)
#define UNCORR_ERR_MASK_OFF                     (AER_CAP + 0x8)
#define RESOURCE_CON_REG_VC0                    (VC_CAP + 0x14)
#define RESOURCE_CON_REG_VC1                    (VC_CAP + 0x20)
#define RESOURCE_STATUS_REG_VC1                 (VC_CAP + 0x24)
#define SD_CONTROL1_REG                         (RAS_DES_CAP+0xA0)
#define CCIX_TP_CAP_TP_HDR2_OFF                 (CCIX_TP_CAP + 0x8)
#define ESM_MNDTRY_RATE_CAP_OFF                 (CCIX_TP_CAP + 0xc)
#define ESM_STAT_OFF                            (CCIX_TP_CAP + 0x14)
#define ESM_CNTL_OFF                            (CCIX_TP_CAP + 0x18)
#define ESM_LN_EQ_CNTL_25G_0_OFF                (CCIX_TP_CAP + 0x2c)
#define PORT_LINK_CTRL_OFF                      0x710
#define FILTER_MASK_2_OFF                       0x720
#define GEN2_CTRL_OFF                           0x80c
#define GEN3_RELATED_OFF                        0x890
#define GEN3_EQ_CONTROL_OFF                     0x8A8
#define MISC_CONTROL_1_OFF                      0x8bc
#define AMBA_ERROR_RESPONSE_DEFAULT_OFF         0x8d0
#define AMBA_LINK_TIMEOUT_OFF                   0x8d4
#define AMBA_ORDERING_CTRL_OFF                  0x8d8
#define DTIM_CTRL0_OFF                          0xab0
#define AUX_CLK_FREQ_OFF                        0xb40
#define CCIX_CTRL_OFF                           0xc20

#define DEV_MASK 0x00F8000
#define BUS_MASK 0xFF00000
#define BUS_NUM(Addr) ((((UINT64)(Addr)) & BUS_MASK) >> 20)
#define DEV_NUM(Addr) ((((UINT64)(Addr)) & DEV_MASK) >> 15)
#define CFG_REG(Addr) (((UINT64)Addr) & 0x7FFF)

// TYPE1_DEV_ID_VEND_ID_REG
#define VENDOR_ID_SET(dst, src) (((dst) & ~0xFFFF) | (((UINT32) (src)) & 0xFFFF))
#define DEVICE_ID_SET(dst, src) (((dst) & ~0xFFFF0000) | (((UINT32) (src) << 16) & 0xFFFF0000))

// TYPE1_CLASS_CODE_REV_ID_REG
#define BASE_CLASS_CODE_SET(dst, src) (((dst) & ~0xFF000000) | (((UINT32) (src) << 24) & 0xFF000000))
#define SUBCLASS_CODE_SET(dst, src) (((dst) & ~0xFF0000) | (((UINT32) (src) << 16) & 0xFF0000))
#define PROGRAM_INTERFACE_SET(dst, src) (((dst) & ~0xFF00) | (((UINT32) (src) << 8) & 0xFF00))
#define REVISION_ID_SET(dst, src) (((dst) & ~0xFF) | (((UINT32) (src)) & 0xFF))

// SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG
#define DEFAULT_SUB_BUS       0xFF
#define SUB_BUS_SET(dst, src) (((dst) & ~0xFF0000) | (((UINT32) (src) << 16) & 0xFF0000))
#define SEC_BUS_SET(dst, src) (((dst) & ~0xFF00) | (((UINT32) (src) << 8) & 0xFF00))
#define PRIM_BUS_SET(dst, src) (((dst) & ~0xFF) | (((UINT32) (src)) & 0xFF))

// BRIDGE_CTRL_INT_PIN_INT_LINE_REG
#define INT_PIN_SET(dst, src) (((dst) & ~0xFF00) | (((UINT32) (src) << 8) & 0xFF00))

// CON_STATUS_REG
#define POWER_STATE_SET(dst, src) (((dst) & ~0x3) | (((UINT32) (src)) & 0x3))

// DEVICE_CONTROL2_DEVICE_STATUS2_REG
#define PCIE_CAP_CPL_TIMEOUT_VALUE_SET(dst, src) (((dst) & ~0xF) | (((UINT32) (src)) & 0xF))

// LINK_CAPABILITIES_REG
#define PCIE_CAP_ID                             0x10
#define LINK_CAPABILITIES_REG_OFF               0xC
#define LINK_CONTROL_LINK_STATUS_OFF            0x10
#define PCIE_CAP_MAX_LINK_WIDTH_X1              0x1
#define PCIE_CAP_MAX_LINK_WIDTH_X2              0x2
#define PCIE_CAP_MAX_LINK_WIDTH_X4              0x4
#define PCIE_CAP_MAX_LINK_WIDTH_X8              0x8
#define PCIE_CAP_MAX_LINK_WIDTH_X16             0x10
#define PCIE_CAP_MAX_LINK_WIDTH_GET(val) ((val & 0x3F0) >> 4)
#define PCIE_CAP_MAX_LINK_WIDTH_SET(dst, src) (((dst) & ~0x3F0) | (((UINT32) (src) << 4) & 0x3F0))
#define MAX_LINK_SPEED_25                       0x1
#define MAX_LINK_SPEED_50                       0x2
#define MAX_LINK_SPEED_80                       0x3
#define MAX_LINK_SPEED_160                      0x4
#define MAX_LINK_SPEED_320                      0x5
#define PCIE_CAP_MAX_LINK_SPEED_GET(val) ((val & 0xF))
#define PCIE_CAP_MAX_LINK_SPEED_SET(dst, src) (((dst) & ~0xF) | (((UINT32) (src)) & 0xF))
#define PCIE_CAP_SLOT_CLK_CONFIG_SET(dst, src) (((dst) & ~0x10000000) | (((UINT32) (src) << 28) & 0x10000000))
#define NO_ASPM_SUPPORTED                       0x0
#define L0S_SUPPORTED                           0x1
#define L1_SUPPORTED                            0x2
#define L0S_L1_SUPPORTED                        0x3
#define PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_SET(dst, src) (((dst) & ~0xC00) | (((UINT32)(src) << 10) & 0xC00))

// LINK_CONTROL_LINK_STATUS_REG
#define PCIE_CAP_DLL_ACTIVE_GET(val) ((val & 0x20000000) >> 29)
#define PCIE_CAP_NEGO_LINK_WIDTH_GET(val) ((val & 0x3F00000) >> 20)
#define PCIE_CAP_LINK_SPEED_GET(val) ((val & 0xF0000) >> 16)
#define PCIE_CAP_LINK_SPEED_SET(dst, src) (((dst) & ~0xF0000) | (((UINT32) (src) << 16) & 0xF0000))
#define CAP_LINK_SPEED_TO_VECTOR(val)          BIT((val)-1)
#define PCIE_CAP_EN_CLK_POWER_MAN_GET(val) ((val & 0x100) >> 8)
#define PCIE_CAP_EN_CLK_POWER_MAN_SET(dst, src) (((dst) & ~0x100) | (((UINT32) (src) << 8) & 0x100))
#define PCIE_CAP_RETRAIN_LINK_SET(dst, src) (((dst) & ~0x20) | (((UINT32) (src) << 5) & 0x20))
#define PCIE_CAP_COMMON_CLK_SET(dst, src) (((dst) & ~0x40) | (((UINT32) (src) << 6) & 0x40))
#define PCIE_CAP_LINK_TRAINING_GET(val)     ((val & 0x8000000) >> 27)
#define PCIE_CAP_LINK_DISABLE_SET(dst, src) (((dst) & ~0x10) | (((UINT32)(src) << 4) & 0x10))

// LINK_CAPABILITIES2_REG
#define LINK_SPEED_VECTOR_25                    BIT(0)
#define LINK_SPEED_VECTOR_50                    BIT(1)
#define LINK_SPEED_VECTOR_80                    BIT(2)
#define LINK_SPEED_VECTOR_160                   BIT(3)
#define LINK_SPEED_VECTOR_320                   BIT(4)
#define PCIE_CAP_SUPPORT_LINK_SPEED_VECTOR_GET(val) ((val & 0xFE) >> 1)
#define PCIE_CAP_SUPPORT_LINK_SPEED_VECTOR_SET(dst, src) (((dst) & ~0xFE) | (((UINT32) (src) << 1) & 0xFE))
#define PCIE_CAP_EQ_CPL_GET(val)        ((val & 0x20000) >> 17)
#define PCIE_CAP_EQ_CPL_P1_GET(val)     ((val & 0x40000) >> 18)
#define PCIE_CAP_EQ_CPL_P2_GET(val)     ((val & 0x80000) >> 19)
#define PCIE_CAP_EQ_CPL_P3_GET(val)     ((val & 0x100000) >> 20)

// LINK_CONTROL2_LINK_STATUS2_REG
#define PCIE_CAP_TARGET_LINK_SPEED_SET(dst, src) (((dst) & ~0xF) | (((UINT32) (src)) & 0xF))

// Secondary Capability
#define SPCIE_CAP_ID                0x19
#define CAP_OFF_0C                  0x0C
#define LINK_CONTROL3_REG_OFF       0x4
#define DSP_TX_PRESET0_SET(dst,src)  (((dst) & ~0xF) | (((UINT32) (src)) & 0xF))
#define DSP_TX_PRESET1_SET(dst,src)  (((dst) & ~0xF0000) | (((UINT32) (src) << 16) & 0xF0000))

// UNCORR_ERR_STATUS_OFF
#define CMPLT_TIMEOUT_ERR_STATUS_GET(val) ((val & 0x4000) >> 14)
#define CMPLT_TIMEOUT_ERR_STATUS_SET(dst, src) (((dst) & ~0x4000) | (((UINT32) (src) << 14) & 0x4000))

// UNCORR_ERR_MASK_OFF
#define CMPLT_TIMEOUT_ERR_MASK_SET(dst, src) (((dst) & ~0x4000) | (((UINT32) (src) << 14) & 0x4000))
#define SDES_ERR_MASK_SET(dst, src) (((dst) & ~0x20) | (((UINT32)(src) << 5) & 0x20))

// RESOURCE_STATUS_REG_VC1
#define VC_NEGO_PENDING_VC1_GET(val) ((val & 0x20000) >> 17)

// SD_CONTROL1_REG
#define FORCE_DETECT_LANE_EN_SET(dst, src) (((dst) & ~0x10000) | (((UINT32) (src) << 16) & 0x10000))

// CCIX_TP_CAP_TP_HDR2_OFF
#define ESM_REACH_LENGTH_GET(val) ((val & 0x60000) >> 17)
#define ESM_CALIBRATION_TIME_GET(val) ((val & 0x700000) >> 20)
#define ESM_CALIBRATION_TIME_SET(dst, src) (((dst) & ~0x700000) | (((UINT32) (src) << 20) & 0x700000))

// ESM_STAT_OFF
#define ESM_CALIB_CMPLT_GET(val) ((val & 0x80) >> 7)
#define ESM_CURNT_DATA_RATE_GET(val) ((val & 0x7F) >> 0)

// ESM_CNTL_OFF
#define QUICK_EQ_TIMEOUT_SET(dst, src) (((dst) & ~0x1C000000) | (((UINT32) (src) << 26) & 0x1C000000))
#define LINK_REACH_TARGET_GET(val) ((val & 0x1000000) >> 24)
#define LINK_REACH_TARGET_SET(dst, src) (((dst) & ~0x1000000) | (((UINT32) (src) << 24) & 0x1000000))
#define ESM_EXT_EQ3_DSP_TIMEOUT_GET(val) ((val & 0x700000) >> 20)
#define ESM_EXT_EQ3_DSP_TIMEOUT_SET(dst, src) (((dst) & ~0x700000) | (((UINT32) (src) << 20) & 0x700000))
#define ESM_EXT_EQ2_USP_TIMEOUT_GET(val) ((val & 0x70000) >> 16)
#define ESM_EXT_EQ2_USP_TIMEOUT_SET(dst, src) (((dst) & ~0x70000) | (((UINT32) (src) << 16) & 0x70000))
#define ESM_ENABLE_SET(dst, src) (((dst) & ~0x8000) | (((UINT32) (src) << 15) & 0x8000))
#define ESM_DATA_RATE1_SET(dst, src) (((dst) & ~0x7F00) | (((UINT32) (src) << 8) & 0x7F00))
#define ESM_PERFORM_CAL_SET(dst, src) (((dst) & ~0x80) | (((UINT32) (src) << 7) & 0x80))
#define ESM_DATA_RATE0_SET(dst, src) (((dst) & ~0x7F) | (((UINT32) (src)) & 0x7F))

// PORT_LINK_CTRL_OFF
#define LINK_CAPABLE_X1                 0x1
#define LINK_CAPABLE_X2                 0x3
#define LINK_CAPABLE_X4                 0x7
#define LINK_CAPABLE_X8                 0xF
#define LINK_CAPABLE_X16                0x1F
#define LINK_CAPABLE_X32                0x3F
#define LINK_CAPABLE_SET(dst, src) (((dst) & ~0x3F0000) | (((UINT32) (src) << 16) & 0x3F0000))
#define FAST_LINK_MODE_SET(dst, src) (((dst) & ~0x80) | (((UINT32) (src) << 7) & 0x80))

// FILTER_MASK_2_OFF
#define CX_FLT_MASK_VENMSG0_DROP_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))
#define CX_FLT_MASK_VENMSG1_DROP_SET(dst, src) (((dst) & ~0x2) | (((UINT32) (src) << 1) & 0x2))
#define CX_FLT_MASK_DABORT_4UCPL_SET(dst, src) (((dst) & ~0x4) | (((UINT32) (src) << 2) & 0x4))

// GEN2_CTRL_OFF
#define NUM_OF_LANES_X2                 0x2
#define NUM_OF_LANES_X4                 0x4
#define NUM_OF_LANES_X8                 0x8
#define NUM_OF_LANES_X16                0x10
#define NUM_OF_LANES_SET(dst, src) (((dst) & ~0x1F00) | (((UINT32) (src) << 8) & 0x1F00))

// GEN3_RELATED_OFF
#define RATE_SHADOW_SEL_SET(dst, src) (((dst) & ~0x3000000) | (((UINT32) (src) << 24) & 0x3000000))
#define EQ_PHASE_2_3_SET(dst, src) (((dst) & ~0x200) | (((UINT32) (src) << 9) & 0x200))
#define RXEQ_REGRDLESS_SET(dst, src) (((dst) & ~0x2000) | (((UINT32) (src) << 13) & 0x2000))

// GEN3_EQ_CONTROL_OFF
#define GEN3_EQ_FB_MODE(dst, src) (((dst) & ~0xF) | ((UINT32) (src) & 0xF))
#define GEN3_EQ_PRESET_VEC(dst, src) (((dst) & 0xFF0000FF) | (((UINT32) (src) << 8) & 0xFFFF00))
#define GEN3_EQ_INIT_EVAL(dst,src) (((dst) & ~0x1000000) | (((UINT32) (src) << 24) & 0x1000000))

// MISC_CONTROL_1_OFF
#define DBI_RO_WR_EN_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))

// AMBA_ERROR_RESPONSE_DEFAULT_OFF
#define AMBA_ERROR_RESPONSE_CRS_SET(dst, src) (((dst) & ~0x18) | (((UINT32) (src) << 3) & 0x18))
#define AMBA_ERROR_RESPONSE_GLOBAL_SET(dst, src) (((dst) & ~0x1) | (((UINT32) (src)) & 0x1))

// AMBA_LINK_TIMEOUT_OFF
#define LINK_TIMEOUT_PERIOD_DEFAULT_SET(dst, src) (((dst) & ~0xFF) | (((UINT32) (src)) & 0xFF))

// AMBA_ORDERING_CTRL_OFF
#define AX_MSTR_ZEROLREAD_FW_SET(dst, src) (((dst) & ~0x80) | (((UINT32) (src) << 7) & 0x80))

// DTIM_CTRL0_OFF
#define DTIM_CTRL0_ROOT_PORT_ID_SET(dst, src) (((dst) & ~0xFFFF) | (((UINT32) (src)) & 0xFFFF))

// AUX_CLK_FREQ_OFF
#define AUX_CLK_500MHZ 500
#define AUX_CLK_FREQ_SET(dst, src) (((dst) & ~0x1FF) | (((UINT32) (src)) & 0x1FF))

#define EXT_CAP_OFFSET_START 0x100

// EVENT_COUNTER_CONTROL_REG
#define ECCR_GROUP_EVENT_SEL_SET(dst, src) (((dst) & ~0xFFF0000) | (((UINT32)(src) << 16) & 0xFFF0000))
#define ECCR_GROUP_SEL_SET(dst, src) (((dst) & ~0xF000000) | (((UINT32)(src) << 24) & 0xF000000))
#define ECCR_EVENT_SEL_SET(dst, src) (((dst) & ~0xFF0000) | (((UINT32)(src) << 16) & 0xFF0000))
#define ECCR_LANE_SEL_SET(dst, src) (((dst) & ~0xF00) | (((UINT32)(src) << 8) & 0xF00))
#define ECCR_EVENT_COUNTER_ENABLE_SET(dst, src) (((dst) & ~0x1C) | (((UINT32)(src) << 2) & 0x1C))
#define ECCR_EVENT_COUNTER_CLEAR_SET(dst, src) (((dst) & ~0x3) | (((UINT32)(src)) & 0x3))

// PFA Registers offests
#define RAS_DES_CAP_ID                              0xB
#define EVENT_COUNTER_CONTROL_REG_OFF               0x8
#define EVENT_COUNTER_DATA_REG_OFF                  0xC

#define MAX_NUM_ERROR_CODE                          47
#define DEFAULT_COMMON_LANE_NUM                     15

enum LTSSM_STATE {
  S_DETECT_QUIET = 0,
  S_DETECT_ACT,
  S_POLL_ACTIVE,
  S_POLL_COMPLIANCE,
  S_POLL_CONFIG,
  S_PRE_DETECT_QUIET,
  S_DETECT_WAIT,
  S_CFG_LINKWD_START,
  S_CFG_LINKWD_ACEPT,
  S_CFG_LANENUM_WAI,
  S_CFG_LANENUM_ACEPT,
  S_CFG_COMPLETE,
  S_CFG_IDLE,
  S_RCVRY_LOCK,
  S_RCVRY_SPEED,
  S_RCVRY_RCVRCFG,
  S_RCVRY_IDLE,
  S_L0,
  S_L0S,
  S_L123_SEND_EIDLE,
  S_L1_IDLE,
  S_L2_IDLE,
  S_L2_WAKE,
  S_DISABLED_ENTRY,
  S_DISABLED_IDLE,
  S_DISABLED,
  S_LPBK_ENTRY,
  S_LPBK_ACTIVE,
  S_LPBK_EXIT,
  S_LPBK_EXIT_TIMEOUT,
  S_HOT_RESET_ENTRY,
  S_HOT_RESET,
  S_RCVRY_EQ0,
  S_RCVRY_EQ1,
  S_RCVRY_EQ2,
  S_RCVRY_EQ3,
  MAX_LTSSM_STATE
};

INT32
Ac01PcieCfgOut32 (
  VOID   *Addr,
  UINT32 Val
  );

INT32
Ac01PcieCfgOut16 (
  VOID   *Addr,
  UINT16 Val
  );

INT32
Ac01PcieCfgOut8 (
  VOID  *Addr,
  UINT8 Val
  );

INT32
Ac01PcieCfgIn32 (
  VOID   *Addr,
  UINT32 *Val
  );

INT32
Ac01PcieCfgIn16 (
  VOID   *Addr,
  UINT16 *Val
  );

INT32
Ac01PcieCfgIn8 (
  VOID  *Addr,
  UINT8 *Val
  );

INT32
Ac01PcieCoreSetup (
  AC01_RC *RC
  );

VOID
Ac01PcieCoreResetPort (
  AC01_RC *RC
  );

VOID
Ac01PcieClearConfigPort (
  AC01_RC *RC
  );

AC01_RC *
GetRCList (
  UINT8 Idx
  );

VOID
Ac01PcieCoreBuildRCStruct (
  AC01_RC *RC,
  UINT64  RegBase,
  UINT64  MmioBase,
  UINT64  Mmio32Base
  );

INT32
Ac01PcieCoreSetupRC (
  IN AC01_RC *RC,
  IN UINT8   ReInit,
  IN UINT8   ReInitPcieIndex
  );

VOID
Ac01PcieCoreUpdateLink (
  IN  AC01_RC *RC,
  OUT BOOLEAN *IsNextRoundNeeded,
  OUT INT8    *FailedPciePtr,
  OUT INT8    *FailedPcieCount
  );

VOID
Ac01PcieCoreEndEnumeration (
  AC01_RC *RC
  );

INT32
Ac01PcieCoreQoSLinkCheckRecovery (
  IN AC01_RC *RC,
  IN INTN    PcieIndex
  );

#endif /* PCIECORE_H_ */
