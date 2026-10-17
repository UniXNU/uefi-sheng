#ifndef _FRAME_BUFFER_HANDOFF_H_
#define _FRAME_BUFFER_HANDOFF_H_

#include <Uefi.h>

//
// Geometry of the frame buffer that is really scanned out, published by a
// platform driver once the graphics output protocol exists. The early frame
// buffer log reads it on every write and falls back to its build-time
// geometry while Magic is not set.
//
// The record lives at the end of the "Display Reserved" region, below the
// log cursor. SEC clears that region at start, so a stale record cannot
// survive a restart of the firmware.
//
#define FB_HANDOFF_MAGIC   0x4F474246  // "FBGO"
#define FB_HANDOFF_OFFSET  0x40

typedef struct {
  UINT32                Magic;
  UINT32                Width;
  UINT32                Height;
  UINT32                PitchBytes;
  EFI_PHYSICAL_ADDRESS  Base;
} FB_HANDOFF;

#endif /* _FRAME_BUFFER_HANDOFF_H_ */
