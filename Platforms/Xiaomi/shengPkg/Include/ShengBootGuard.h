// SPDX-License-Identifier: MIT
#ifndef SHENG_BOOT_GUARD_H
#define SHENG_BOOT_GUARD_H
#include <Base.h>
#define BOOT_RESET_CORE_TYPES
typedef UINT8 uint8_t;
typedef UINT32 uint32_t;
#include "BootResetCore.h"

STATIC inline INT64
ShengWatchdogCall (UINT64 Function, UINT64 Arg1, UINT64 Arg2)
{
  register UINT64 X0 __asm__("x0") = Function;
  register UINT64 X1 __asm__("x1") = Arg1;
  register UINT64 X2 __asm__("x2") = Arg2;
  register UINT64 X3 __asm__("x3") = 0;
  __asm__ volatile ("smc #0" : "+r" (X0), "+r" (X1), "+r" (X2), "+r" (X3) :
                    : "memory", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11",
                      "x12", "x13", "x14", "x15", "x16", "x17");
  return (INT64)X0;
}

STATIC inline UINT64
ShengBootTicks (VOID)
{
  UINT64 Value;
  __asm__ volatile ("isb\n\tmrs %0, cntpct_el0" : "=r" (Value));
  return Value;
}

STATIC inline UINT64
ShengBootFrequency (VOID)
{
  UINT64 Value;
  __asm__ volatile ("mrs %0, cntfrq_el0" : "=r" (Value));
  return Value;
}
#endif
