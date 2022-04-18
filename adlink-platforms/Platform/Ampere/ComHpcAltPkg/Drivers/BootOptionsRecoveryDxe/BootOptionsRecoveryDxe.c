/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

VOID
EFIAPI
RecoveryCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  DEBUG ((DEBUG_INFO, "%a: Do recover boot options\n", __FUNCTION__));

  EfiBootManagerConnectAll ();
  EfiBootManagerRefreshAllBootOption ();

  gBS->CloseEvent (Event);
}

EFI_STATUS
EFIAPI
BootOptionsRecoveryDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_EVENT  EndOfDxeEvent;

  DEBUG ((DEBUG_INFO, "%a: NVRAM Clear is %d\n", __FUNCTION__, PcdGetBool (PcdNvramErased)));

  if (PcdGetBool (PcdNvramErased)) {
    DEBUG ((DEBUG_INFO, "%a: Register event to recover boot options\n", __FUNCTION__));
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    RecoveryCallback,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}
