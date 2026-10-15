# sheng bring-up candidate

This platform is not hardware validated and is not a ready-to-flash image.
It uses Kailua SM8550 support from Mu-Silicium revision
3aa95a72d469234755e2bcf2841ec6c0e83863e9. Board reservations come from the
compiled DTS include closure in ianchb/sm8550-mainline revision
42f3b40c702abbc37c3a04c95af008cd283aeaf3, not the QRD8550 DDR map.

The FD at 0xa7100000, SEC allocations, stack, kernel staging area, and DXE
heap are inherited layout candidates. They do not overlap that source's
static reservations, but this does not prove that they are usable RAM on a
specific tablet. The Linux memory node has size zero until its bootloader
fills it. Runtime RAM, display, XBL and hypervisor reservations remain to
be checked against captured firmware data.

The configuration library and required vendor driver names are provisional
QRD8550 references. Every binary path has been changed to `Binaries/sheng`;
no QRD binary is silently used. Stock sheng XBL/UEFI modules and its
`uefiplat.cfg` are needed to resolve those dependencies and confirm the
configuration. Neither those modules nor panel data are distributed here.

Initial bring-up is headless. Do not enable SimpleFbDxe until the actual
framebuffer address, reservation, pixel layout, stride and dimensions are
known. QRD panel XMLs, display reservations and board ACPI are not included.
Generic Kailua code may still require additional ACPI/DT work.

CrabApple is embedded as an application plus its registrar. The sibling
`crabapple` repository supplies the package; `CRABAPPLE_ROOT` can override
that location. Its core remains unchanged by board-specific code.

The Android container matches debian-sheng's legacy header and offsets.
This is source-level evidence for the container recipe, not a bootloader
acceptance test. A compiled DTB must be staged as `Resources/DTBs/sheng.dtb`
before packaging; its runtime fixups must be preserved before kernel use.
