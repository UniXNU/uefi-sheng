#ifndef _FRAME_BUFFER_H_
#define _FRAME_BUFFER_H_

//
// Frame Buffer Details
//
#define FB_WIDTH     (mFbGeometry.Width)
#define FB_HEIGHT    (mFbGeometry.Height)
#define FB_PITCH     (mFbGeometry.PitchBytes)
#define FB_BPP      (FixedPcdGet8  (PcdFrameBufferColorDepth) / 8)
#define FB_DELAY_US  FixedPcdGet32 (PcdFrameBufferDelay)
#define FB_SCALE     FixedPcdGet8  (PcdFrameBufferFontScale)
#define FB_FREEZE    FixedPcdGetBool (PcdFrameBufferFreezeWhenFull)

// YPos Marker for a Log that stopped at the Bottom of the Screen
#define FB_FROZEN_YPOS  0xFFFFFFFF

//
// Frame Buffer Geometry in Use: the build-time Default or the handed-off one
//
typedef struct {
  UINTN   Base;
  UINT32  Width;
  UINT32  Height;
  UINT32  PitchBytes;
} FB_GEOMETRY;

extern FB_GEOMETRY  mFbGeometry;

//
// Frame Buffer Position
//
typedef struct {
  UINT32 XPos;
  UINT32 YPos;
} EFI_FRAME_BUFFER_POSITION;

#endif /* _FRAME_BUFFER_H_ */
