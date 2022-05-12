/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef PCIEPATCHACPI_H_
#define PCIEPATCHACPI_H_

EFI_STATUS
EFIAPI
AcpiPatchPciMem32 (
  INT8 *PciSegEnabled
  );

EFI_STATUS
EFIAPI
AcpiInstallMcfg (
  INT8 *PciSegEnabled
  );

EFI_STATUS
EFIAPI
AcpiInstallIort (
  INT8 *PciSegEnabled
  );

#endif /* PCIEPATCHACPI_H_ */
