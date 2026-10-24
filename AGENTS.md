# Agent instructions

## Responsibility

Maintain sheng UEFI integration, memory reservations, platform initialization,
firmware packaging, and hardware handoff.

## Required reading

1. Read README.md, Building.md, and Platforms/Xiaomi/shengPkg/PORTING.md.
2. Read [firmware integration](https://unixnu.com/porting/uefi-firmware),
   [sheng](https://unixnu.com/devices/sheng), and
   [SM8550](https://unixnu.com/porting/sm8550).
3. Read [porting](https://unixnu.com/porting/), the
   [repository specification](https://unixnu.com/porting/device-repository),
   [component catalog](https://unixnu.com/reference/components), and
   [device-tree specification](https://unixnu.com/porting/device-tree).
4. Read the selected component README, build interface, and implementation.
5. Read [payload](https://unixnu.com/concepts/payload),
   [building](https://unixnu.com/building), and
   [bring-up](https://unixnu.com/porting/bring-up) before image assembly.
6. Read the [migration guide](https://unixnu.com/porting/migration) for migration.

## Implementation

Inspect existing SoC support and catalog components before adding code.
Keep common SM8550 operations in shared support and sheng peripheral policy
in this repository or UniXNU-sheng, according to the execution stage.
Preserve protocol GUIDs, runtime identifiers, and firmware filenames.
Maintain the documentation site in UniXNU and link its relevant pages here.

## Verification

Build with the selected UniXNU device and OS profiles using Building.md.
Run existing firmware checks. Report build commands, revisions, required
inputs, and results. Record hardware boot and peripheral observations with
the device and firmware configuration.

## Device porting record

The firmware section of [sheng's porting record](https://unixnu.com/devices/sheng#firmware)
records handoff inputs, development findings, evidence and completion criteria.
Update `UniXNU-sheng/porting.toml` when firmware work changes those inputs or
results. Maintainers and agents use the [same workflow](https://unixnu.com/porting/records).
