/** @file
  The tablet's power and volume keys as a SimpleTextIn device.

  The boot manager navigates with ordinary console keys. This driver reads the three hardware keys through the SPMI PMIC arbiter
  (ShengSpmiKeys.h has the register facts) and presents them as keys: volume up = Up, volume down = Down, power = Enter. It only
  reads; polling stops when boot services end, so nothing is left running for the kernel, which uses the same arbiter later.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleTextIn.h>
#include <Guid/EventGroup.h>

#include "ShengSpmiKeys.h"

#define POLL_MS         20
#define REPEAT_DELAY    25     // polls (500 ms) before a held volume key repeats
#define REPEAT_PERIOD   7      // polls (140 ms) between repeats
#define QUEUE_SIZE      16
#define MAX_FAILURES    50     // consecutive failed samples before polling stops

#define SPMI_CORE_BASE    0x0C400000UL
#define SPMI_OBSRVR_BASE  0x0C440000UL

typedef struct {
  VENDOR_DEVICE_PATH          Vendor;
  EFI_DEVICE_PATH_PROTOCOL    End;
} SHENG_KEY_DEVICE_PATH;

STATIC SHENG_KEY_DEVICE_PATH  mPath = {
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, { sizeof (VENDOR_DEVICE_PATH), 0 } },
    { 0x74921D8A, 0x54BD, 0x4855, { 0x94, 0xD4, 0x16, 0xC6, 0x9C, 0xC9, 0xCB, 0x7F } }
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 } }
};

STATIC struct sk_arbiter  mArbiter;
STATIC struct sk_keys     mKeys;
STATIC struct sk_filter   mFilter;
STATIC EFI_INPUT_KEY      mQueue[QUEUE_SIZE];
STATIC UINTN              mHead, mCount;
STATIC UINTN              mFailures;
STATIC EFI_EVENT          mTimer, mExit;
STATIC EFI_HANDLE         mHandle;

STATIC
EFI_STATUS
EFIAPI
KeyReset (
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *This,
  IN BOOLEAN                         ExtendedVerification
  )
{
  EFI_TPL  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  mHead  = 0;
  mCount = 0;
  gBS->RestoreTPL (Tpl);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
KeyRead (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *This,
  OUT EFI_INPUT_KEY                   *Key
  )
{
  EFI_TPL  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  if (mCount == 0) {
    gBS->RestoreTPL (Tpl);
    return EFI_NOT_READY;
  }

  *Key  = mQueue[mHead];
  mHead = (mHead + 1) % QUEUE_SIZE;
  mCount--;
  gBS->RestoreTPL (Tpl);
  return EFI_SUCCESS;
}

STATIC EFI_SIMPLE_TEXT_INPUT_PROTOCOL  mInput = { KeyReset, KeyRead, NULL };

// WaitForKey is checked by WaitForEvent/CheckEvent: it must be signaled again while keys remain queued.
STATIC
VOID
EFIAPI
KeyWait (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  if (mCount != 0) {
    gBS->SignalEvent (Event);
  }
}

STATIC
VOID
Push (
  IN UINT16  Scan,
  IN CHAR16  Unicode
  )
{
  if (mCount == QUEUE_SIZE) {
    return;
  }

  mQueue[(mHead + mCount) % QUEUE_SIZE].ScanCode    = Scan;
  mQueue[(mHead + mCount) % QUEUE_SIZE].UnicodeChar = Unicode;
  mCount++;
  gBS->SignalEvent (mInput.WaitForKey);
}

STATIC
VOID
EFIAPI
Poll (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  UINT8  Pressed;
  UINT8  Out;

  if (sk_sample (&mArbiter, &mKeys, &Pressed) != 0) {
    if (++mFailures >= MAX_FAILURES) {
      gBS->SetTimer (mTimer, TimerCancel, 0);
      DEBUG ((DEBUG_ERROR, "ShengKeyInput: too many failed samples, polling stopped\n"));
    }

    return;
  }

  mFailures = 0;
  Out       = sk_step (&mFilter, Pressed, REPEAT_DELAY, REPEAT_PERIOD);
  if (Out & SK_KEY_VOLUP) {
    Push (SCAN_UP, 0);
  }

  if (Out & SK_KEY_VOLDOWN) {
    Push (SCAN_DOWN, 0);
  }

  if (Out & SK_KEY_POWER) {
    Push (SCAN_NULL, CHAR_CARRIAGE_RETURN);
  }
}

STATIC
VOID
EFIAPI
OnExitBootServices (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  gBS->SetTimer (mTimer, TimerCancel, 0);
}

EFI_STATUS
EFIAPI
ShengKeyInputDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  int         Rc;

  mArbiter.core = (volatile UINT32 *)SPMI_CORE_BASE;
  mArbiter.obs  = (volatile UINT32 *)SPMI_OBSRVR_BASE;
  mArbiter.ee   = 0;

  Rc = sk_open (&mArbiter, &mKeys);
  if (Rc != 0) {
    DEBUG ((DEBUG_ERROR, "ShengKeyInput: arbiter lookup failed (%d), no keys\n", Rc));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "ShengKeyInput: PON apid %d, volume up apid %d\n", mKeys.pon, mKeys.gpio));

  Status = gBS->CreateEvent (EVT_NOTIFY_WAIT, TPL_NOTIFY, KeyWait, NULL, &mInput.WaitForKey);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->CreateEvent (EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, Poll, NULL, &mTimer);
  if (!EFI_ERROR (Status)) {
    Status = gBS->CreateEventEx (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, OnExitBootServices, NULL, &gEfiEventExitBootServicesGuid, &mExit);
  }

  if (!EFI_ERROR (Status)) {
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &mHandle,
                    &gEfiDevicePathProtocolGuid,
                    &mPath,
                    &gEfiSimpleTextInProtocolGuid,
                    &mInput,
                    NULL
                    );
  }

  if (!EFI_ERROR (Status)) {
    Status = gBS->SetTimer (mTimer, TimerPeriodic, EFI_TIMER_PERIOD_MILLISECONDS (POLL_MS));
  }

  return Status;
}
