/** @file

**/

#ifndef __MMC_LIB_H__
#define __MMC_LIB_H__

#include <Uefi.h>
#include <Library/UefiLib.h>

/**
  Sends a 32-bit value to a POST card.

  Sends the 32-bit value specified by Value to a POST card, and returns Value.
  Some implementations of this library function may perform I/O operations
  directly to a POST card device.  Other implementations may send Value to
  ReportStatusCode(), and the status code reporting mechanism will eventually
  display the 32-bit value on the status reporting device.

  MmcPostCode() must actively prevent recursion.  If MmcPostCode() is called while
  processing another Post Code Library function, then
  MmcPostCode() must return Value immediately.

  @param  Value  The 32-bit value to write to the POST card.

  @return The 32-bit value to write to the POST card.

**/
EFI_STATUS
MmcPostCode (
  IN UINT32  Value
  );

#endif
