/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef NVDATASTRUC_H_
#define NVDATASTRUC_H_

#define MAX_AC01_PCIE_SCREEN_ROOT_COMPLEX 16

#define PCIE_VARSTORE_NAME        L"PcieIfrNVData"
#define PCIE_VARSTORE_ID          0x1234
#define PCIE_FORM_ID              0x1235
#define PCIE_RC0_FORM_ID          0x1236
#define PCIE_RC1_FORM_ID          0x1237
#define PCIE_RC2_FORM_ID          0x1238
#define PCIE_RC3_FORM_ID          0x1239
#define PCIE_RC4_FORM_ID          0x123A
#define PCIE_RC5_FORM_ID          0x123B
#define PCIE_RC6_FORM_ID          0x123C
#define PCIE_RC7_FORM_ID          0x123D
#define PCIE_RC8_FORM_ID          0x123E
#define PCIE_RC9_FORM_ID          0x123F
#define PCIE_RC10_FORM_ID         0x1240
#define PCIE_RC11_FORM_ID         0x1241
#define PCIE_RC12_FORM_ID         0x1242
#define PCIE_RC13_FORM_ID         0x1243
#define PCIE_RC14_FORM_ID         0x1244
#define PCIE_RC15_FORM_ID         0x1245
#define PCIE_FORM_SET_GUID        { 0xe84e70d6, 0xe4b2, 0x4c6e, { 0x98,  0x51, 0xcb, 0x2b, 0xac, 0x77, 0x7d, 0xbb } }

#define PCIE_GOTO_ID_BASE         0x8040

#define PCIE_NVPARAM_VARSTORE_NAME  L"PcieIfrNVParamData"
#define PCIE_NVPARAM_VARSTORE_ID    0x1233

#pragma pack(1)

//
// NV data structure definition
//
typedef struct {
  BOOLEAN RCStatus[MAX_AC01_PCIE_SCREEN_ROOT_COMPLEX];
  UINT8   RCBifurLo[MAX_AC01_PCIE_SCREEN_ROOT_COMPLEX];
  UINT8   RCBifurHi[MAX_AC01_PCIE_SCREEN_ROOT_COMPLEX];
  UINT32  SmmuPmu;
} PCIE_VARSTORE_DATA;

//
// NVParam data structure definition
//
typedef struct {
  BOOLEAN PcieStrongOrdering;
} PCIE_NVPARAM_VARSTORE_DATA;

//
// Labels definition
//
#define LABEL_UPDATE             0x2223
#define LABEL_END                0x2224
#define LABEL_RC0_UPDATE         0x2225
#define LABEL_RC0_END            0x2226
#define LABEL_RC1_UPDATE         0x2227
#define LABEL_RC1_END            0x2228
#define LABEL_RC2_UPDATE         0x2229
#define LABEL_RC2_END            0x222A
#define LABEL_RC3_UPDATE         0x222B
#define LABEL_RC3_END            0x222C
#define LABEL_RC4_UPDATE         0x222D
#define LABEL_RC4_END            0x222E
#define LABEL_RC5_UPDATE         0x222F
#define LABEL_RC5_END            0x2230
#define LABEL_RC6_UPDATE         0x2231
#define LABEL_RC6_END            0x2232
#define LABEL_RC7_UPDATE         0x2233
#define LABEL_RC7_END            0x2234
#define LABEL_RC8_UPDATE         0x2235
#define LABEL_RC8_END            0x2236
#define LABEL_RC9_UPDATE         0x2237
#define LABEL_RC9_END            0x2238
#define LABEL_RC10_UPDATE        0x2239
#define LABEL_RC10_END           0x223A
#define LABEL_RC11_UPDATE        0x223B
#define LABEL_RC11_END           0x223C
#define LABEL_RC12_UPDATE        0x223D
#define LABEL_RC12_END           0x223E
#define LABEL_RC13_UPDATE        0x223F
#define LABEL_RC13_END           0x2240
#define LABEL_RC14_UPDATE        0x2241
#define LABEL_RC14_END           0x2242
#define LABEL_RC15_UPDATE        0x2243
#define LABEL_RC15_END           0x2244

#pragma pack()

#endif
