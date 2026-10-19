//
// Copyright (c) 2011 - 2025, Arm Limited. All rights reserved.
// SPDX-License-Identifier: BSD-2-Clause-Patent
//

#include <AsmMacroLib.h>

#include <Library/ArmLib.h>

//
// SCTRL_EL1 Bits
//
.set CTRL_SPAN_BIT, BIT23

ASM_FUNC (_ModuleEntryPoint)
  // The previous stage may enter with SPSel=0. Exceptions are then taken on
  // the stale SP_EL1 instead of the stack set up below, so select SP_ELx
  // first; the stack pointer written in MainEntryPoint is then SP_EL1.
  msr   SPSel, #1
  isb

  // Before the first store of this stage: clean and invalidate the data caches the previous stage filled (x0 is kept in x20).
  mov   x20, x0
  bl    ASM_PFX (SecCleanInvalidateDataCacheAll)
  mov   x0, x20

  // Keep x0 for the platform. It is the address of the DT a Linux-style boot
  // loader hands over. The slot is in this section, which is in RAM here.
  adr   x1, ASM_PFX (gSecEntryArg0)
  str   x0, [x1]

  // Do Platform Specific Initialization
  bl    ASM_PFX (PlatformAssemblyInitialize)

  // Disable SPAN
  EL1_OR_EL2 (x0)
1:mrs   x0, sctlr_el1
  orr   x0, x0, #CTRL_SPAN_BIT
  msr   sctlr_el1, x0
2:nop

  // Enable Floating Point
  bl    ASM_PFX (ArmEnableVFP)

  // Setup Exception Vector
  EL1_OR_EL2 (x0)
1:bl    ASM_PFX (SetupExceptionLevel1)
  b     ASM_PFX (MainEntryPoint)
2:bl    ASM_PFX (SetupExceptionLevel2)
  b     ASM_PFX (MainEntryPoint)

ASM_PFX (MainEntryPoint):
  // Get Stack Details
  MOV64 (x0, FixedPcdGet64 (PcdCPUCoresStackBase))
  MOV32 (x1, FixedPcdGet32 (PcdCPUCorePrimaryStackSize))

  // Set Stack End Address
  add   x2, x0, x1

  // Set Stack Pointer
  mov   sp, x2

  // Clear Stack
  mov   x3, xzr
0:stp   x3, x3, [x0], #16
  cmp   x0, x2
  b.lt  0b

  // Set Jump Location
  ldr   x0, =ASM_PFX (SecEntry)

  // Clear Frame Pointer
  mov   x29, xzr

  // Jump to C Code
  blr   x0

  // Do Infinite Spin
  b     .

  // First argument of the previous stage, written once at entry
  .align 3
  .global ASM_PFX (gSecEntryArg0)
ASM_PFX (gSecEntryArg0):
  .quad 0
