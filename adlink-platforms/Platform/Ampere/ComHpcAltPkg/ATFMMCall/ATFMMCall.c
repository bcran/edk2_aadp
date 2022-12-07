 /**
 *
 * Copyright (c) 2018-2020, Ampere Computing LLC
 *
 *  This program and the accompanying materials
 *  are licensed and made available under the terms and conditions of the BSD License
 *  which accompanies this distribution.  The full text of the license may be found at
 *  http://opensource.org/licenses/bsd-license.php
 *
 *  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 *
 **/

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/ShellLib.h>
#include <Library/TimeBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FlashLib.h>

#define VERSION L"0.2"

/**********
   STRING
 **********/

// String Compare Case Insensitive and Length Control

STATIC INTN StrniCmp (CHAR16 *String, CHAR16 *String2, int Count)
{
  while ((*String != L'\0') &&
         (CharToUpper (*String) == CharToUpper (*String2)) && Count != 0) {
    String++;
    String2++;
    Count--;
  }
  if (Count == 0) return 0;
  return CharToUpper (*String) - CharToUpper (*String2);
}

// String Compare Case Insensitive

STATIC INTN StriCmp (CHAR16 *String, CHAR16 *String2)
{
  return StrniCmp (String, String2, -1);
}

// Hex Character to Byte

STATIC BOOLEAN HexChrToU8 (CHAR16 Chr, UINT8 *Val)
{
  if (Chr >= L'0' && Chr <= L'9') {
    *Val = (UINT8)(Chr - L'0');
  }
  else
  if (Chr >= L'A' && Chr <= L'F') {
    *Val = (UINT8)(Chr - L'A'+10);
  }
  else
  if (Chr >= L'a' && Chr <= L'f') {
    *Val = (UINT8)(Chr - L'a'+10);
  }
  else {
    return FALSE;
  }
  return TRUE;
}

// Two Hex Characters to Byte

STATIC BOOLEAN HexChr2ToU8 (CHAR16 ChrH, CHAR16 ChrL, UINT8 *Val)
{
  UINT8 Tmp;

  if (!HexChrToU8 (ChrH, &Tmp)) return FALSE;
  if (!HexChrToU8 (ChrL,  Val)) return FALSE;
  *Val += (Tmp << 4);
  return TRUE;
}

STATIC BOOLEAN HexStrToUINT(CHAR16 *Str, UINTN *Val)
{
  CHAR16 Chr;
  UINT8 Tmp;

  *Val = 0;
  while ( (Chr = *Str++) != L'\0') {
    if (!HexChrToU8 (Chr, &Tmp)) return FALSE;
    *Val<<= 4;
    *Val |= Tmp;
  }
  return TRUE;
}

// String to Unsigned Integer for oct(0), dec(1..9) and hex(0x) supported

STATIC BOOLEAN StrToUINT(CHAR16 *Str, UINTN *Val)
{
  CHAR16 Chr;
  UINT32 Base = 10;

  if (*Str == L'0') {
    Str++;
    if (*Str == L'x' || *Str == L'X') return HexStrToUINT(Str+1, Val);
    Base = 8;
  }

  *Val = 0;
  while ( (Chr = *Str++) != L'\0') {
    *Val *= Base;
    if (Chr < L'0' || Chr > L'0'+Base-1) return FALSE;
    *Val += (Chr-L'0');
  }
  return TRUE;
}

// Hex Buffer Dump

STATIC VOID Print_HexBuffer (UINT8 *Buf, UINTN Base, UINT32 Size)
{
  UINTN Addr = Base & ~0xf, End = Base + Size;
  CHAR16 Text[17] = L"................";

  Print (L"                0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
  
  while (Addr < End) {

    Print (L"%012x : ", Addr);
    
    for (UINTN i = 0; i < 16; i++, Addr++) {
      if (Addr >= Base && Addr < End) {
        Print (L"%02x ", *Buf);
        Text[i] = (*Buf >= 0x20 && *Buf <= 0x7E)? L' '+*Buf-0x20 : L'.';
        Buf++;
      }
      else {
        Print (L"-- ");
        Text[i] = L'.';
      }
    }
    Print (L"%s\n", Text);
  }
}

// Banner Print

STATIC VOID Print_Banner (CHAR16 *String)
{
  Print (L"Ampere %s, %s build %04d/%02d/%02d\n", String, VERSION, TIME_BUILD_YEAR, TIME_BUILD_MONTH, TIME_BUILD_DAY);
  Print (L"-----\n");
}

/*********
   FLASH
 *********/
 
STATIC VOID Flash_Help (VOID)
{
  Print (L"Usage: Flash Info  basename\n");
  Print (L"             Erase address length\n");
  Print (L"             Write address string\n");
  Print (L"             Read  address length\n");
  Print (L"+ basename - FailSafe | NvRam | NvRam2\n");
  Print (L"+ address  - byte address (number | basename:offset)\n");
  Print (L"+ length   - byte length (number)\n");
  Print (L"+ string   - unicode, ascii (\\ headed) or hex (0x headed) string\n");
  Print (L"+ number and offset support formats oct (0 headed), dec and hex (0x headed)\n");
}

STATIC struct
{
  CHAR16 *Name;
  CHAR16 *FName;
  EFI_STATUS (*Func) (UINTN *Base, UINT32 *Size);
}
BaseInfo[] =
{
  { L"FailSafe", L"FlashGetFailSafeInfo", FlashGetFailSafeInfo},
  { L"NvRam2",   L"FlashGetNvRam2Info",   FlashGetNvRam2Info},
  { L"NvRam",    L"FlashGetNvRamInfo",    FlashGetNvRamInfo},
  { NULL }
};

STATIC EFI_STATUS Flash_GetInfo (CHAR16 *Str, UINTN *Base, UINT32 *Size, int *Index)
{
  CHAR16 *Name;

  for (UINTN i = 0; (Name = BaseInfo[i].Name) != NULL; i++) {

    if (StrniCmp (Str, Name, StrLen (Name)) == 0) {
      *Index = i;
      EFI_STATUS Status = BaseInfo[i].Func (Base, Size);
      if (EFI_ERROR (Status)) *Base = -1; // call error
      return Status;
    }
  }

  *Base = 0; // normal error
  return EFI_INVALID_PARAMETER;
}

/*
  FlashGetFailSafeInfo (OUT UINTN *FailSafeBase, OUT UINT32 *FailSafeSize);
  FlashGetNvRamInfo    (OUT UINTN *NvRamBase,    OUT UINT32 *NvRamSize);
  FlashGetNvRam2Info   (OUT UINTN *NvRam2Base,   OUT UINT32 *NvRam2Size);
  FlashEraseCommand    (IN  UINTN  ByteAddress,  IN  UINT32  Length);
  FlashWriteCommand    (IN  UINTN  ByteAddress,  IN  VOID   *Buffer, IN UINT32 Length);
  FlashReadCommand     (IN  UINTN  ByteAddress,  OUT VOID   *Buffer, IN UINT32 Length);
*/

STATIC EFI_STATUS Flash_Main (IN UINTN Argc, IN CHAR16 **Argv)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN Base, Val64;
  UINT32 Size, Len;
  CHAR16 *Str, *Parm, *Func = NULL;
  UINT8 *Buf = NULL;
  int index = 0;

  Print_Banner(L"MM Flash Tool");

while (1) {

  if (Argc < 3) {
unsupported:
    Status = EFI_UNSUPPORTED;
    break;
  }

// parse address for all commands, can be a number or basename:offset

  Status = Flash_GetInfo (Argv[2], &Base, &Size, &index);
  Len = StrLen (BaseInfo[index].Name);

// FlashGetFailSofeInfo, FlashGetNvRamInfo, FlashGetNvRam2Info

  if (StriCmp (Argv[1], L"Info") == 0) {

    if (Argc != 3) goto unsupported;

    if (EFI_ERROR (Status)) {

      if (Base == 0) {
errv2:  Parm = Argv[2];
        Status = EFI_INVALID_PARAMETER;
      }
      else {
errif:  Func = BaseInfo[index].FName;
      }
    }
    else {
      if (Argv[2][Len] != L'\0') goto errv2;
      Print (L"%s Base: 0x%x, Size: 0x%x(%d)\n", BaseInfo[index].Name, Base, Size, Size);
    }
    break;
  }

// Flash<Erase|Write|Read>Command

  if (Argc != 4) goto unsupported;

  if (EFI_ERROR (Status)) {

    if (Base != 0) goto errif;
    if (!StrToUINT(Argv[2], &Base)) goto errv2;
  }
  else {
    CHAR16 Chr = Argv[2][Len];
    if (Chr == L':') {
      if (!StrToUINT (Argv[2]+Len+1, &Val64)) goto errv2;
      Base += Val64;
    }
    else
    if (Chr != L'\0') goto errv2;
  }
  
// FlashEraseCommand

  if (StriCmp (Argv[1], L"Erase") == 0) {

      if (!StrToUINT(Argv[3], &Val64)) {
errv3:Parm = Argv[3];
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    Size = (UINT32)Val64;
    if (Size == 0) goto errv3;

    Status = FlashEraseCommand (Base, Size);
    if (EFI_ERROR (Status)) {
      Func = L"FlashEraseCommand";
      break;
    }
    
    Print (L"Erase completed for %d bytes from 0x%x.\n", Size, Base);
    break;
  }

// FlashWriteCommand

  if (StriCmp (Argv[1], L"Write") == 0) {

    Str = Argv[3];

    if (Str[0] == L'\\') { // ascii
    
      if ((Size = StrLen (++Str)) == 0) goto errv3;
      Size++; // zero-terminated

      Buf = AllocateZeroPool (Size);
      if (Buf == NULL) {
erres:  Status = EFI_OUT_OF_RESOURCES;
        break;
      }

      for (UINT32 i = 0; i < Size; i++, Str++) {
        Buf[i] = (UINT8)(*Str - L'\0');
      }
      Status = FlashWriteCommand (Base, Buf, Size);
    }
    else
    if (Str[0] == L'0' && (Str[1] == L'x' || Str[1] == L'X')) { // hex

      Str += 2;
      Size = StrLen (Str);
      if ((Size & 1) || Size == 0) goto errv3;
      Size /= 2;

      Buf = AllocateZeroPool (Size);
      if (Buf == NULL) goto erres;

      for (UINT32 i = 0; *Str != L'\0'; i++, Str += 2) {
        if (!HexChr2ToU8 (Str[0], Str[1], Buf+i)) goto errv3;
      }
      Status = FlashWriteCommand (Base, Buf, Size);
    }
    else { // unicode string
      if ((Size = StrSize (Argv[3])) == 0) goto errv3;
      Status = FlashWriteCommand (Base, Argv[3], Size);
    }

    if (EFI_ERROR (Status)) {
      Func = L"FlashWriteCommand";
      break;
    }

    Print (L"Write completed for %d bytes from 0x%x.\n", Size, Base);
    break;
  }

// FlashReadCommand

  if (StriCmp (Argv[1], L"Read") == 0) {

    if (!StrToUINT(Argv[3], &Val64)) goto errv3;
    Size = (UINT32)Val64;
    if (Size == 0) goto errv3;

    Buf = AllocateZeroPool (Size);
    if (Buf == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
    
    Status = FlashReadCommand (Base, Buf, Size);
    if (EFI_ERROR (Status)) {
      Func = L"FlashReadCommand";
      break;
    }

    Print_HexBuffer (Buf, Base, Size);
    break;
  }
  Status = EFI_UNSUPPORTED;
  break;
}

  if (Buf) FreePool (Buf);

  if (EFI_ERROR (Status)) {
    if (Func) 
      Print (L"%s returns error code: 0x%llx!\n", Func, Status);
    else
      switch (Status) {
        case EFI_UNSUPPORTED:
          Flash_Help ();
          break;
        case EFI_INVALID_PARAMETER:
          Print (L"Invalid argument '%s'!\n", Parm);
          break;
        case EFI_OUT_OF_RESOURCES:
          Print (L"Out of resources, request for %ld byte(s)!\n", Size);
          break;
        default:
          Print (L"Uknown error code: 0x%llx!\n", Status);
          break;
      }
  }
  Print (L"\n");

  return Status;
}

/********
   MAIN
 ********/

STATIC struct
{
  CHAR16 *Name;
  EFI_STATUS (*Entry) (UINTN Argc, CHAR16 **Argv);
}
Command[] =
{
  { L"Flash", Flash_Main },
  { NULL }
};

STATIC VOID App_Help (VOID)
{
  Print_Banner(L"MM Tool");
  Print (L"Usage: ATFMMCall ");
  for (UINTN i=0; ; ) {
    Print (Command[i].Name);
    if (Command[++i].Name == NULL) break;
    Print (L"|");
  }
  Print (L" arguments\n");
  Print (L"+ arguments are command dependent, type 'ATFMMCall command' for details.\n\n");
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
AppEntryPoint (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS Status;
  EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters;

  Status = gBS->HandleProtocol (ImageHandle, &gEfiShellParametersProtocolGuid, (VOID**)&ShellParameters);
  if (EFI_ERROR (Status)) {
    Print (L"Please use UEFI SHELL to run this application!\n");
    return Status;
  }

  Status = ShellInitialize ();
  if (EFI_ERROR (Status)) {
    Print (L"Failed to initialize Shell library!\n");
    return Status;
  }

  if (ShellParameters->Argc > 1) {
    for (UINTN i = 0; Command[i].Name != NULL; i++) {
      if (StriCmp (ShellParameters->Argv[1], Command[i].Name) == 0)  {
        return Command[i].Entry (ShellParameters->Argc-1, &ShellParameters->Argv[1]);
      }
    }
  }

  App_Help ();
  return EFI_UNSUPPORTED;    
}

