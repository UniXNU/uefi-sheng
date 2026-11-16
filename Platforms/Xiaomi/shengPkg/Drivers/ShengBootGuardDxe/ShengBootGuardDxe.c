// SPDX-License-Identifier: MIT
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include "../../Include/ShengBootGuard.h"

STATIC EFI_EVENT PingEvent;
STATIC UINT64 Deadline;

STATIC VOID EFIAPI
PingWatchdog (EFI_EVENT Event, VOID *Context)
{
  if (ShengBootTicks () < Deadline) {
    ShengWatchdogCall (BR_WDT_PING, 0, 0);
  } else {
    gBS->SetTimer (PingEvent, TimerCancel, 0);
  }
}

STATIC VOID EFIAPI
HandOffWatchdog (EFI_EVENT Event, VOID *Context)
{
  gBS->SetTimer (PingEvent, TimerCancel, 0);
  ShengWatchdogCall (BR_WDT_PING, 0, 0);
}

EFI_STATUS EFIAPI
ShengBootGuardDxeEntry (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_EVENT ExitEvent;
  UINT64 Frequency = ShengBootFrequency ();
  if (Frequency == 0 || ShengWatchdogCall (BR_WDT_STATUS, 0, 0) != 0) return EFI_UNSUPPORTED;
  Deadline = ShengBootTicks () + Frequency * 180;
  EFI_STATUS Status = gBS->CreateEvent (EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
                                       PingWatchdog, NULL, &PingEvent);
  if (EFI_ERROR (Status)) return Status;
  Status = gBS->CreateEventEx (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, HandOffWatchdog, NULL,
                               &gEfiEventExitBootServicesGuid, &ExitEvent);
  if (EFI_ERROR (Status)) { gBS->CloseEvent (PingEvent); return Status; }
  ShengWatchdogCall (BR_WDT_PING, 0, 0);
  Status = gBS->SetTimer (PingEvent, TimerPeriodic, 20000000);
  DEBUG ((DEBUG_ERROR, "Sheng guard: DXE ping=2s budget=180s status=%r\n", Status));
  return Status;
}
