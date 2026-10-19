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

## Bring-up state observed on the device (fastboot boot, no flashing)

The stock ABL hands over with the panel scanning out the 3048x2032 32-bit
linear frame buffer at the "Display Reserved" region (the stock sheng dtbo
carries a `splash_region` node for it; the appended DTB needs the same node
or ABL powers the panel down, see `scripts/sheng-abl-dtb.py` in the umbrella).
Line pitch equals the width in bytes; the early log is readable as is.

* `SHENG_VENDOR_HW = 0` (default): Mu drivers only, SimpleFbDxe publishes the
  GOP on that frame buffer. The panel stays on and UniXNU's menu shows.
* `SHENG_VENDOR_HW = 1`: adds the stock vendor hardware drivers. Their
  initialization (seen last: ClockDxe after SMEM/DAL/IPCC/GLink/CmdDb/NPA/RPMh/
  VCS) turns the panel black while the backlight stays on; the log goes with
  it. The stock DisplayDxe chain (`USE_CUSTOM_DISPLAY_DRIVER = 1`) needs this
  and has never produced a GOP here. RamManagerDxe also needs this stack.

SEC selects SP_ELx first: the previous stage enters with SPSel=0, which made
every exception run on a stale SP_EL1 next to the unwritable vector page.
`Resources/MuPatches/Gic-Spurious.patch` masks a GIC interrupt that has no
handler; the leftover IPCC interrupt (INTID 0x105) otherwise loops forever.
The umbrella `scripts/build-uefi-sheng.sh` applies it for the build only.
The log follows the frame buffer a platform driver hands over
(`ShengGopLogDxe`, used by the stock display chain only).

Generic Kailua code may still require additional ACPI/DT work.

UniXNU is embedded as an application plus its registrar. The sibling
`UniXNU` repository supplies the package; `UNIXNU_ROOT` can override
that location. Its core remains unchanged by board-specific code.

## Display chain (first-boot 亮屏 path)

The stock DisplayDxe is now packaged with the board's N81A 3048x2032
dual-DSI DSC panel XMLs (`Panel_N81A_36_02_0a{,_p0}_duldsi_dsc_vid.xml`)
under their original FFS GUIDs and UI names; the SSH-observed panel mode
`U:3048x2032p-0` matches. DisplayDxe contains genuine N81A detection,
power-up (PMIC rails disp_prim_lcd_n81a_vddi_s3g/l12b) and backlight
sequences. Its original DEPEX requires nine protocols; seven come from the
packaged vendor drivers (PlatformInfo, ChipInfo, Clock, TLMM, I2C and two
PMIC). The remaining two GUIDs (A966CC4F-EE6C-424D-A762-8ED3B5A4A501,
47861DFF-556F-4269-9B28-062EC3ECB30D) appear nowhere else in the entire
stock image and are not referenced by DisplayDxe code, so the board driver
ShengDisplayDepexDxe installs empty flag protocols for them; no vendor byte
changes. check-sheng-fv now reports 107 FFS files, 52 vendor modules with
unchanged executables/DEPEX, no dangling APRIORI, with ~645 KiB FD headroom.
The headless single-core config overrides stay for first boot: display
thread and image-FV logo are BDS features of the stock QcomBds that this
firmware does not ship; DisplayDxe initializes panels at its entry point.
Panel init touches real MDP/DSI MMIO and can only be observed on hardware.

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
