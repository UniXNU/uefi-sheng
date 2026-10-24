# sheng firmware platform

The platform targets Xiaomi Pad 6S Pro, Qualcomm SM8550. It uses Kailua
platform support and the sheng memory reservations from the pinned DTS and
stock firmware. Source and firmware inputs are recorded in the platform
configuration and import tools.

## Memory and firmware inputs

The memory map reconciles 34 Linux DTS reservations with 27 stock memory-map
regions from OS3.0.305.0.WNXMIXM Global firmware. The firmware archive MD5 is
`31450b6677d9e46edc4937c3f797c51d`. The extracted `uefiplat.cfg` SHA-256 is
`3208aee1f7a0c1264c5166dc14d1f026f9c64754c99c551b70888f26843a6cbb`.

The 72 descriptors include PIL gaps, display/Demura memory, DBI dump, and
CPU vectors. NoMap regions produce reservation HOBs. The 3 MiB FD at
`0xa7100000` occupies the stock 4 MiB FD window at `0xa7000000`.

The vendor import tools retain module GUIDs, PE bytes, and binary DEPEX in
local firmware inputs. Select firmware inputs using the device configuration.

## Display and interrupts

ABL supplies a 3048x2032, 32-bit linear framebuffer. Its appended FDT needs
`/reserved-memory/splash_region` to retain panel scanout.

With `SHENG_VENDOR_HW = 0`, SimpleFbDxe publishes GOP for the framebuffer.
The observed device boot displays the UniXNU menu. With `SHENG_VENDOR_HW = 1`,
vendor initialization reaches ClockDxe and the panel becomes black while the
backlight remains on. The cause of that display failure is unresolved.

The stock DisplayDxe configuration includes the N81A panel XMLs and their
original FFS GUIDs. ShengDisplayDepexDxe supplies the required platform
protocol markers. Panel initialization requires hardware verification.

SEC selects SP_ELx. `Resources/MuPatches/Gic-Spurious.patch` masks interrupts
with an unregistered handler, including IPCC INTID `0x105`.

## UniXNU integration

UniXNU supplies the application package and registrar. DarwinBoot loads the
XNU kernel and ADT sidecar. UniXNU-sheng selects board firmware and boot-image
parameters. UniXNU-sm8550-common supplies shared XNU platform operations.

`UniXNURamDiskDxe` comes from UniXNU's shared package. It publishes the
FDT initrd as Block I/O; DiskIoDxe and FAT expose the payload filesystem.
`ShengBootMemoryDxe` adds DRAM within the board's mapping limits.
Both modules are listed in the platform DSC and firmware volume.

Build through the UniXNU resolver using [Building.md](../../../Building.md).
Keep architecture, device-tree, and packaging documentation in the
[UniXNU site](https://unixnu.com/porting/uefi-firmware).

## Verification report

Record the firmware configuration and source revisions, build result,
container inspection, device boot, GOP output, and interrupt behavior.
Record QEMU observations with their machine and boot inputs. Record pending
hardware checks with the required device or firmware inputs.
