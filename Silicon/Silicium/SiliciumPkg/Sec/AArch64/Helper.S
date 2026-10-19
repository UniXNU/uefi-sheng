//
// Copyright (c) 2011-2017, ARM Limited. All rights reserved.
// SPDX-License-Identifier: BSD-2-Clause-Patent
//

#include <AsmMacroLib.h>

#include <AArch64/AArch64.h>

//
// HCR_EL2 Bits
//
.set HCR_FIQ_BIT,    (1 << 3)
.set HCR_IRQ_BIT,    (1 << 4)
.set HCR_SERROR_BIT, (1 << 5)

ASM_FUNC (SetupExceptionLevel1)
  // Re-Configure Access Control
  mov   x0, #CPACR_DEFAULT
  b     ASM_PFX (ArmWriteCpacr)

ASM_FUNC (SetupExceptionLevel2)
  // Clear SCTRL_EL2
  msr   sctlr_el2, xzr

  // Get current Hypervisor Configuration
  mrs   x0, hcr_el2

  // Redirect Interrupts to EL2
  orr   x0, x0, #HCR_FIQ_BIT
  orr   x0, x0, #HCR_IRQ_BIT
  orr   x0, x0, #HCR_SERROR_BIT

  // Write new Hypervisor Configuration
  msr   hcr_el2, x0

  // Setup EL1 Exceptions
  tst   x0, #ARM_HCR_E2H
  b.ne  SetupExceptionLevel1

  // Enable Architectural Features
  mov   x0, #AARCH64_CPTR_DEFAULT
  msr   cptr_el2, x0

  ret

//
// Cleans and invalidates every data cache level up to the Level of Coherence by set/way (the standard sequence: CLIDR_EL1 levels,
// CSSELR_EL1 to select, CCSIDR_EL1 geometry, DC CISW). The previous stage ran with its caches on and leaves lines behind that the UEFI,
// running with the MMU and caches off until ArmConfigureMmu, never wrote: once the caches are on, those stale lines would hide the
// DRAM contents (HOB tables, heap) written in between, or be written back over them. Cleaning keeps what the previous stage wrote.
// Corrupts x0-x11; callable with the MMU on or off.
//
ASM_FUNC (SecCleanInvalidateDataCacheAll)
  dmb   sy
  mrs   x0, clidr_el1
  and   x3, x0, #0x7000000          // LoC, shifted
  lsr   x3, x3, #23                 // LoC * 2
  cbz   x3, 5f
  mrs   x1, id_aa64mmfr2_el1
  ubfx  x8, x1, #20, #4             // CCIDX
  mov   x10, #0                     // level * 2
1:
  add   x2, x10, x10, lsr #1
  lsr   x1, x0, x2
  and   x1, x1, #7                  // cache type of this level
  cmp   x1, #2
  b.lt  4f                          // no data cache here
  msr   csselr_el1, x10
  isb
  mrs   x1, ccsidr_el1
  and   x2, x1, #7
  add   x2, x2, #4                  // log2 (line bytes)
  cbz   x8, 6f
  ubfx  x4, x1, #3, #21             // CCIDX: associativity - 1
  ubfx  x7, x1, #32, #24            // CCIDX: sets - 1
  b     7f
6:
  ubfx  x4, x1, #3, #10             // associativity - 1
  ubfx  x7, x1, #13, #15            // sets - 1
7:
  clz   w5, w4                      // bit position of the way field
2:
  mov   x9, x4
3:
  lsl   x6, x9, x5
  orr   x11, x10, x6                // way and level
  lsl   x6, x7, x2
  orr   x11, x11, x6                // set
  dc    cisw, x11
  subs  x9, x9, #1
  b.ge  3b
  subs  x7, x7, #1
  b.ge  2b
4:
  add   x10, x10, #2
  cmp   x3, x10
  b.gt  1b
5:
  mov   x10, #0
  msr   csselr_el1, x10
  dsb   sy
  isb
  ret

ASM_FUNCTION_REMOVE_IF_UNREFERENCED
