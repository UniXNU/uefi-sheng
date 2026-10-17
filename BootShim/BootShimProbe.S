// Diagnostic BootShim for headless bring-up. Not a production entry path.
//
// The stock bootloader hands over with the MMU and caches off, and USB is
// gone after ExitBootServices, so progress is reported through the panel
// and through a deliberate reset:
//
//   1. Paint the top FB_FILL bytes of the scan-out buffer white. White is
//      white in every 32-bit RGB/BGR/XR30 layout, so a wrong pixel order
//      cannot hide the marker.
//   2. PROBE_RESET=1: wait PROBE_DELAY_S seconds on the generic timer, then
//      PSCI SYSTEM_RESET through SMC. A self-reboot proves this code ran,
//      independently of the display.
//      PROBE_RESET=0: continue exactly like BootShim.S (copy the FD and
//      enter SEC). SEC clears "Display Reserved" before its first log line,
//      so the marker disappearing shows SEC got that far.
//
// The ARM64 kernel header matches BootShim.S, which the bootloader's gzip
// path requires.

#define PSCI_SYSTEM_RESET 0x84000009

_Head:
  adr   x1, _Payload
  b     _Start

#if REQUIRES_KERNEL_HEADER == 1
_StackBase:
  .quad FD_BASE         /* Text Offset */

_StackSize:
  .quad FD_SIZE + (_Payload - _Head) /* Complete shim and payload */
  .quad 0               /* Flags */
  .quad 0               /* Reserved */
  .quad 0               /* Reserved */
  .quad 0               /* Reserved */
  .ascii "ARM\x64"      /* ARM64 Magic */
  .long 0               /* Reserved */
#endif

_Start:
  mov   x11, x1
  mov   x12, x0

  /* Marker: fill the top of the frame buffer white. */
  ldr   x0, =FB_BASE
  ldr   x2, =FB_FILL
  mov   x3, #-1
_Paint:
  stp   x3, x3, [x0], #16
  subs  x2, x2, #16
  b.ne  _Paint
  dsb   sy

#if PROBE_RESET == 1
  /* Wait PROBE_DELAY_S seconds so the marker is visible before reset. */
  mrs   x4, cntfrq_el0
  cbnz  x4, 1f
  ldr   x4, =19200000   /* fallback when the frequency register is unset */
1:
  mov   x5, #PROBE_DELAY_S
  mul   x4, x4, x5
  isb
  mrs   x5, cntpct_el0
  add   x4, x4, x5
2:
  isb
  mrs   x5, cntpct_el0
  cmp   x5, x4
  b.lo  2b

  ldr   w0, =PSCI_SYSTEM_RESET
  mov   x1, xzr
  mov   x2, xzr
  mov   x3, xzr
  smc   #0
_Dead:
  wfe
  b     _Dead
#else
  mov   x0, x12
  mov   x4, x11
  ldr   x5, =FD_BASE
  cmp   x4, x5
  beq   _Entry
  ldr   x6, =FD_SIZE

_CopyLoop:
  ldp   x2, x3, [x4], #16
  stp   x2, x3, [x5], #16
  subs  x6, x6, #16
  b.ne  _CopyLoop
  ldr   x5, =FD_BASE

_Entry:
  /* Publish the copied instructions before executing them. */
  dsb   sy
  mrs   x9, ctr_el0
  ubfx  x9, x9, #16, #4
  mov   x10, #4
  lsl   x9, x10, x9
  sub   x10, x9, #1
  bic   x8, x5, x10
  ldr   x7, =FD_SIZE
  add   x7, x5, x7

_CleanLoop:
  dc    cvac, x8
  add   x8, x8, x9
  cmp   x8, x7
  b.lo  _CleanLoop
  dsb   sy
  ic    iallu
  dsb   sy
  isb
  br    x5
#endif

.text
.ltorg
.align 4

_Payload:
