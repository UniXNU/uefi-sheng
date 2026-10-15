# sheng bring-up candidate

This platform is not hardware validated and is not a ready-to-flash image.
It uses Kailua SM8550 support from Mu-Silicium revision
3aa95a72d469234755e2bcf2841ec6c0e83863e9. Board reservations come from the
compiled DTS include closure in ianchb/sm8550-mainline revision
42f3b40c702abbc37c3a04c95af008cd283aeaf3, not the QRD8550 DDR map.

The board map now reconciles all 34 pinned Linux DTS reservations with
27 stock memory-map regions from OS3.0.305.0.WNXMIXM Global firmware.
The archive MD5 is 31450b6677d9e46edc4937c3f797c51d; its extracted
uefiplat.cfg SHA-256 is
3208aee1f7a0c1264c5166dc14d1f026f9c64754c99c551b70888f26843a6cbb.

The resulting 72 descriptors cover 11 previously missing stock ranges,
including PIL gaps, display/Demura memory, DBI dump and CPU vectors.
Stock NoMap regions produce reservation HOBs without MMU mappings.
The 3 MiB port FD at 0xa7100000 lies inside the stock 4 MiB FD window
at 0xa7000000; its preceding 1 MiB remains reserved by port policy.
Stack, SEC, FV and DXE heap allocations agree with the stock layout.
This is firmware evidence, not a hardware or runtime RAM observation.

Numeric configuration values now come from that firmware, with explicit
single-CPU/headless bring-up overrides: display initialization, image-FV
loading, parallel decompression, multithreading and persistent logging are
disabled. NumCpus still reports the real eight-core topology; only one core
is active during bring-up. The numeric configuration library does not
represent the stock string parameters.

All 53 currently selected vendor inputs are available from the same stock
UEFI image. The import tool preserves module GUIDs, PE bytes and binary
DEPEX, and keeps them in the local cache behind Binaries/sheng. They are
not distributed here. Full firmware assembly and runtime driver protocol
compatibility still require qualification. Static coverage of the stock
TZApps dynamic reservation does not establish runtime partition sizes.

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

The full DEBUG CLANGPDB FD now builds with secure-boot support enabled.
Independent FV extraction finds 103 FFS files, verifies 51 selected vendor
executables and DEPEX payloads unchanged, and finds no dangling APRIORI
GUIDs. GLink is included consistently with its APRIORI entry; the genuine
ProjectInfoDxe follows PlatformInfo as in the stock startup list. This FD
is still an offline candidate and has not been tested on hardware.

The offline container now round-trips through its Android v0 header, gzip
stream, exact FD and appended DTB. BootShim's ARM64 image_size includes the
shim itself, and its copy path cleans the destination to PoC, invalidates
local instructions and synchronizes before branching. The 192-byte shim's
payload label equals its binary length. Bounded QEMU fixtures check every
payload byte, destination entry and x0 preservation for both relocation and
already-in-place paths. These run with MMU/data caches off and do not prove
hardware cache coherency. Stock boot.img uses header v4; accepting the legacy
v0 container and runtime placement remain ABL qualification work.
