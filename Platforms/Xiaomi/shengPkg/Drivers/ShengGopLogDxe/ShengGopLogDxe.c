/** @file
  Hands the real scan-out frame buffer to the early frame buffer log.

  The DEBUG log draws into the "Display Reserved" region with build-time
  geometry. The stock DisplayDxe reprograms the panel and publishes its own
  graphics output protocol, after which the log no longer reaches the screen.
  This driver waits for that protocol and records its frame buffer base,
  size and pitch where FrameBufferSerialPortLib looks for it. It changes no
  display state and draws nothing itself.
**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/FrameBufferHandoff.h>
#include <Library/MemoryMapHelperLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Uefi.h>

STATIC VOID  *mRegistration;

STATIC
VOID
PaintBltMarker (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  White;
  EFI_STATUS                     Status;

  White.Blue     = 0xFF;
  White.Green    = 0xFF;
  White.Red      = 0xFF;
  White.Reserved = 0x00;

  Status = Gop->Blt (
                  Gop,
                  &White,
                  EfiBltVideoFill,
                  0,
                  0,
                  0,
                  0,
                  Gop->Mode->Info->HorizontalResolution,
                  Gop->Mode->Info->VerticalResolution / 4,
                  0
                  );
  DEBUG ((DEBUG_ERROR, "GOP: Blt fill marker %r\n", Status));
}

STATIC
VOID
PaintLinearMarker (
  IN volatile FB_HANDOFF  *Handoff
  )
{
  UINT32  Row;
  UINT32  Column;
  UINT8   *RowBase;

  for (Row = Handoff->Height / 4; Row < Handoff->Height / 2; Row++) {
    RowBase = (UINT8 *)(UINTN)(Handoff->Base + (UINTN)Row * Handoff->PitchBytes);
    for (Column = 0; Column < Handoff->Width; Column++) {
      *(volatile UINT32 *)(RowBase + (UINTN)Column * 4) = 0xFFFFFFFF;
    }
  }
}

STATIC
VOID
PublishGop (VOID)
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop;
  EFI_MEMORY_REGION_DESCRIPTOR  Region;
  volatile FB_HANDOFF           *Handoff;
  UINT32                        BytesPerPixel;

  Status = gBS->LocateProtocol (&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&Gop);
  if (EFI_ERROR (Status) || (Gop->Mode == NULL) || (Gop->Mode->Info == NULL)) {
    return;
  }

  DEBUG ((
    DEBUG_ERROR,
    "GOP: base 0x%lx size 0x%lx %ux%u pixels/line %u format %d\n",
    Gop->Mode->FrameBufferBase,
    (UINT64)Gop->Mode->FrameBufferSize,
    Gop->Mode->Info->HorizontalResolution,
    Gop->Mode->Info->VerticalResolution,
    Gop->Mode->Info->PixelsPerScanLine,
    (INT32)Gop->Mode->Info->PixelFormat
    ));

  //
  // Marker for hardware bring-up, independent of any text: the top quarter
  // of the screen is filled white through the protocol itself.
  //
  PaintBltMarker (Gop);

  //
  // A Blt-only mode has no linear frame buffer to draw into, and only
  // 32-bit linear formats match the glyph drawing of the log.
  //
  if ((Gop->Mode->FrameBufferBase == 0) ||
      ((Gop->Mode->Info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor) &&
       (Gop->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor)))
  {
    DEBUG ((DEBUG_ERROR, "GOP: no 32-bit linear frame buffer, log stays on its default\n"));
    return;
  }

  ZeroMem (&Region, sizeof (Region));
  LocateMemoryRegionByName ("Display Reserved", &Region);
  LocateMemoryRegionByName ("Display_Reserved", &Region);
  if ((Region.Address == 0) || (Region.Length < FB_HANDOFF_OFFSET)) {
    return;
  }

  BytesPerPixel = 4;
  Handoff       = (volatile FB_HANDOFF *)(UINTN)(Region.Address + Region.Length - FB_HANDOFF_OFFSET);

  Handoff->Base       = Gop->Mode->FrameBufferBase;
  Handoff->Width      = Gop->Mode->Info->HorizontalResolution;
  Handoff->Height     = Gop->Mode->Info->VerticalResolution;
  Handoff->PitchBytes = Gop->Mode->Info->PixelsPerScanLine * BytesPerPixel;
  MemoryFence ();
  Handoff->Magic = FB_HANDOFF_MAGIC;
  MemoryFence ();

  //
  // Second marker through the reported linear frame buffer: the second
  // quarter of the screen. Seeing it proves base, pitch and format.
  //
  PaintLinearMarker (Handoff);

  DEBUG ((DEBUG_ERROR, "GOP: log now follows the graphics output frame buffer\n"));
}

STATIC
VOID
EFIAPI
GopNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  PublishGop ();
}

EFI_STATUS
EFIAPI
ShengGopLogDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_EVENT  Event;

  Event = EfiCreateProtocolNotifyEvent (
            &gEfiGraphicsOutputProtocolGuid,
            TPL_CALLBACK,
            GopNotify,
            NULL,
            &mRegistration
            );
  if (Event == NULL) {
    return EFI_SUCCESS;
  }

  // The protocol may already exist.
  PublishGop ();
  return EFI_SUCCESS;
}
