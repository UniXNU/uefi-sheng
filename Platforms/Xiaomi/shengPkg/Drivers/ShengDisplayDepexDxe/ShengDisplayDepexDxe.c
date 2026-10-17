/** @file
  sheng board glue for the stock DisplayDxe dependency expression.

  The unchanged stock sheng DisplayDxe DEPEX requires nine protocol GUIDs.
  Seven are installed by the stock vendor drivers already packaged in this
  firmware. The remaining two GUIDs appear nowhere else in the stock
  sheng image and are not referenced by any code inside DisplayDxe itself;
  they are vestigial dependencies of the stock build's dispatcher
  environment. This board driver installs empty flag protocols for them so
  the genuine EDK2 dispatcher can satisfy the original, unmodified DEPEX.
  No vendor byte is changed and no display state is touched here.
**/

#include <Library/UefiBootServicesTableLib.h>
#include <Uefi.h>

//
// A966CC4F-EE6C-424D-A762-8ED3B5A4A501
// Orphan GUID pushed by the stock DisplayDxe DEPEX.
//
STATIC CONST EFI_GUID mShengDisplayDepexProtocol1Guid = {
  0xA966CC4F, 0xEE6C, 0x424D, { 0xA7, 0x62, 0x8E, 0xD3, 0xB5, 0xA4, 0xA5, 0x01 }
};

//
// 47861DFF-556F-4269-9B28-062EC3ECB30D
// Orphan GUID pushed by the stock DisplayDxe DEPEX.
//
STATIC CONST EFI_GUID mShengDisplayDepexProtocol2Guid = {
  0x47861DFF, 0x556F, 0x4269, { 0x9B, 0x28, 0x06, 0x2E, 0xC3, 0xEC, 0xB3, 0x0D }
};

EFI_STATUS
EFIAPI
ShengDisplayDepexDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HANDLE  Handle;

  Handle = NULL;
  gBS->InstallMultipleProtocolInterfaces (
         &Handle,
         &mShengDisplayDepexProtocol1Guid,
         NULL,
         &mShengDisplayDepexProtocol2Guid,
         NULL,
         NULL
         );

  return EFI_SUCCESS;
}
