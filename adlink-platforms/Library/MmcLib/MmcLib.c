/** @file
  The instance of Post Code Library that layers on top of a Debug Library instance.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/PL011UartLib.h>
#include <Library/MmcLib.h>

EFI_STATUS
MmcPostCode (
  IN UINT32  Value
  )
{
  UINTN   numofbytes;
  UINT8   IpmiCmdBuf[] = {"[C0 00 80 11]\r\n"};
  UINTN   IpmiCmdBufSize = sizeof(IpmiCmdBuf);

  AsciiSPrint ((CHAR8 *)IpmiCmdBuf, sizeof(IpmiCmdBuf), "[C0 00 80 %2X]\r\n", (UINT8)Value);

  numofbytes = PL011UartWrite ((UINTN)PcdGet64 (PcdSerialDbgRegisterBase), IpmiCmdBuf, IpmiCmdBufSize);

  if (numofbytes == 0) {
    DEBUG ((DEBUG_ERROR, "%a Failed to Write MMC POST code data\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

