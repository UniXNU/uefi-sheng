# Building sheng firmware

Clone the firmware and device repositories beside UniXNU:

```sh
git clone https://github.com/UniXNU/uefi-sheng.git
git clone https://github.com/UniXNU/UniXNU-sheng.git
cd UniXNU
./unixnu defconfig sheng_defconfig -D ../UniXNU-sheng
./unixnu build package firmware --uefi ../uefi-sheng
```

The shared resolver selects the SM8550 and OS profiles and component sources.
The device profile defines the firmware DSC, defines, preflight commands,
and output path. Host requirements and input preparation are documented in
[firmware integration](https://unixnu.com/porting/uefi-firmware).

The firmware embeds `UniXNU`, `UniXNUDxe`, `DarwinBoot` and the shared
`UniXNURamDiskDxe`, with DiskIoDxe and FAT support. Keep each INF in the DSC
and FDF. The RAM disk driver publishes the bootloader's initrd so UniXNU can
read `EFI/UniXNU/unixnu.toml` and list the payload entries.

Provide the selected device's payload inputs and compiled firmware FDT before packaging:

```sh
./unixnu build payload --unique-chip-id <hex> --trust-cache <file> --data-ramdisk <seed>
./unixnu bootimg --uefi ../uefi-sheng --dtb /path/to/sheng.dtb
```

The kernel, ADT, and payload steps are documented in
[building](https://unixnu.com/building). Firmware verification results belong
with the selected device and configuration.
