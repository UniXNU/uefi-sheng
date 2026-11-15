// SPDX-License-Identifier: MIT
#include <Library/PlatformSecLib.h>
#include <Library/ArmSmmuDetachLib.h>
#include <Library/DebugLib.h>
#include "../../Include/ShengBootGuard.h"

VOID
PlatformInitialize (VOID)
{
  struct br_arbiter Arbiter = {
    (volatile uint32_t *)(UINTN)0xc400000,
    (volatile uint32_t *)(UINTN)0xc42d000, 0
  };
  struct br_found Found;
  uint8_t Before = 0xff, After = 0xff;
  INTN Mark = br_find (&Arbiter, &Found);
  if (Mark == 0 && Found.owner == Arbiter.ee) {
    Mark = br_set_bootloader ((volatile uint32_t *)(UINTN)0xc440000,
                             (volatile uint32_t *)(UINTN)0xc500000,
                             &Arbiter, Found.apid, &Before, &After);
  } else if (Mark == 0) {
    Mark = -1;
  }
  INT64 Status = ShengWatchdogCall (BR_WDT_STATUS, 0, 0);
  INT64 Set = -1, Enable = -1;
  if (Mark == 0 && Status == 0) {
    ShengWatchdogCall (BR_WDT_CONTROL, BR_WDT_DISABLE, 0);
    Set = ShengWatchdogCall (BR_WDT_SET_TIME, BR_WDT_TIMEOUT_MS, BR_WDT_TIMEOUT_MS);
    if (Set == 0) Enable = ShengWatchdogCall (BR_WDT_CONTROL, BR_WDT_ENABLE, 0);
  }
  DEBUG ((DEBUG_ERROR, "Sheng guard: mark=%d %02x->%02x status=%Ld set=%Ld enable=%Ld\n",
          Mark, Before, After, Status, Set, Enable));
  if (Mark != 0 || Status != 0 || Set != 0 || Enable != 0) {
    ShengWatchdogCall (0x84000009, 0, 0);
    for (;;) __asm__ volatile ("wfe");
  }
  CONST UINT16 MdpStreams[] = { 0x1C00, 0x1C01, 0x1C03, 0x1C04, 0x2801, 0x2C01 };
  ArmSmmuDetach (MdpStreams, ARRAY_SIZE (MdpStreams));
}
