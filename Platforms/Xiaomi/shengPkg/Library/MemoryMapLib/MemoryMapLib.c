// SPDX-License-Identifier: BSD-3-Clause
// Reservations: ianchb/sm8550-mainline 42f3b40c702abbc37c3a04c95af008cd283aeaf3.
// UEFI allocations remain experimental QRD8550 layout candidates. See PORTING.md.
#include <Library/MemoryMapLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FdtLib.h>
#include "ShengMemoryMap.h"

STATIC EFI_MEMORY_REGION_DESCRIPTOR gMemoryDescriptor[] = {
  {"hyp-region@80000000", 0x80000000, 0x00A00000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"cpusys-vm-region@80a00000", 0x80A00000, 0x00400000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"hyp-tags-region@80e00000", 0x80E00000, 0x003D0000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"XBL Ramdump", 0x81200000, 0x00280000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"XBL DT", 0x81A00000, 0x00040000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"AOP", 0x81C00000, 0x00060000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"aop-cmd-db-region@81c60000", 0x81C60000, 0x00020000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"aop-config-merged-region@81c800", 0x81C80000, 0x00074000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"SMEM", 0x81D00000, 0x00200000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"adsp-mhi-region@81f00000", 0x81F00000, 0x00020000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"global-sync-region@82600000", 0x82600000, 0x00100000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"tz-stat-region@82700000", 0x82700000, 0x00100000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"ipa-fw-region@9b080000", 0x9B080000, 0x00010000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"ipa-gsi-region@9b090000", 0x9B090000, 0x0000A000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"gpu-micro-code-region@9b09a000", 0x9B09A000, 0x00002000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"spss-region@9b100000", 0x9B100000, 0x00180000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"spu-tz-shared-region@9b280000", 0x9B280000, 0x00060000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"spu-modem-shared-region@9b2e000", 0x9B2E0000, 0x00020000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"camera-region@9b300000", 0x9B300000, 0x00800000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"video-region@9bb00000", 0x9BB00000, 0x00700000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"cvp-region@9c200000", 0x9C200000, 0x00700000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"cdsp-region@9c900000", 0x9C900000, 0x02000000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"q6-cdsp-dtb-region@9e900000", 0x9E900000, 0x00080000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"q6-adsp-dtb-region@9e980000", 0x9E980000, 0x00080000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"adspslpi-region@9ea00000", 0x9EA00000, 0x04680000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"rmtfs-region@ccd00000", 0xCCD00000, 0x00280000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"tz-reserved-region@d8000000", 0xD8000000, 0x00100000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"xbl-sc-region@d8100000", 0xD8100000, 0x00040000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"cpucp-fw-region@d8140000", 0xD8140000, 0x001C0000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"qtee-region@d8300000", 0xD8300000, 0x00500000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"ta-region@d8800000", 0xD8800000, 0x08A00000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"tz-tags-region@e1200000", 0xE1200000, 0x02740000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"hwfence-shbuf-region@e6440000", 0xE6440000, 0x002DD000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"llcc-lpi-region@ff800000", 0xFF800000, 0x00600000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"FD Reserved", 0xA7000000, 0x00100000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK},
  {"UEFI FD", 0xA7100000, 0x00300000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK},
  {"UEFI FD Reserved", 0xA7400000, 0x00200000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK},
  {"Info Blk", 0xA7601000, 0x00001000, AddMem, SYS_MEM, SYS_MEM_CAP, RtData, WRITE_BACK_XN},
  {"MMU PageTables", 0xA7602000, 0x00003000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"Log Buffer", 0xA7605000, 0x00008000, AddMem, SYS_MEM, SYS_MEM_CAP, RtData, WRITE_BACK_XN},
  {"UEFI Stack", 0xA760D000, 0x00040000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"SEC Heap", 0xA764D000, 0x0008C000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"Sched Heap", 0xA76D9000, 0x00400000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"FV Region", 0xA7AD9000, 0x00400000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"UEFI RESV", 0xA7ED9000, 0x00127000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"Kernel", 0xA8000000, 0x10000000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_BACK_XN},
  {"DXE Heap", 0xBAB00000, 0x0D1C0000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv, WRITE_BACK_XN},
  // DRAM between the carveouts above and the Linux reserved-memory table (reservations.json), inside
  // 0x80000000..0x100000000, which the SMEM RAM partition table lists as SDRAM. UEFI accepted these on hardware.
  {"DRAM gap 0xC7CC0000", 0xC7CC0000, 0x05040000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv, WRITE_BACK_XN},
  {"DRAM gap 0xCCF80000", 0xCCF80000, 0x0B080000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv, WRITE_BACK_XN},
  {"DRAM gap 0xE671D000", 0xE671D000, 0x190E3000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv, WRITE_BACK_XN},
  // Second DRAM bank from the SMEM usable-RAM partition table read on the tablet (entries 5 and 6:
  // 0x880000000 + 1 GiB and 0x8C0000000 + 5 GiB, category SDRAM). DxeCore got a synchronous external abort
  // clearing memory at 0x8BB300000, so the top of the first entry (to 0x8C0000000) is not given to UEFI.
  {"DRAM bank 1 low", 0x880000000, 0x3B300000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv, WRITE_BACK_XN},
  // Full five-GiB extent reported by the hardware SMEM partition table.
  // First four GiB reached userland; the added last GiB needs hardware validation.
  {"DRAM bank 1 high", 0x8C0000000, 0x140000000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv, WRITE_BACK_XN},
  {"IMEM Base", 0x14680000, 0x0002A000, NoHob, MMAP_IO, INITIALIZED, Conv, NS_DEVICE},
  {"IMEM Cookie Base", 0x146AA000, 0x00016000, AddDev, MMAP_IO, INITIALIZED, Conv, NS_DEVICE},
  {"IPC_ROUTER_TOP", 0x00400000, 0x00100000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"SECURITY CONTROL", 0x00780000, 0x00007000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"QUP", 0x00800000, 0x00300000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"PRNG_CFG_PRNG", 0x010C0000, 0x0000C000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"CRYPTO0 CRYPTO", 0x01DC0000, 0x00040000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"CORE_TOP_CSR", 0x01F00000, 0x00100000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"PERIPH_SS", 0x08800000, 0x00100000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"USB", 0x0A600000, 0x00200000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"AOSS", 0x0B000000, 0x04000000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"TLMM", 0x0F000000, 0x01000000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"SMMU", 0x15000000, 0x00200000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"APSS_HM", 0x17000000, 0x02000000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"NOMAP 811D0000", 0x811D0000, 0x00030000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"RSRV0 gap0", 0x81480000, 0x00580000, AddMem, SYS_MEM, SYS_MEM_CAP, Reserv, WRITE_BACK_XN},
  {"XBL Ramdump", 0x81A40000, 0x001C0000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"RSRV1", 0x86E00000, 0x02A00000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_BACK_XN},
  {"PIL Reserved gap0", 0x8A800000, 0x10880000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"PIL Reserved gap1", 0x9B09C000, 0x00064000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"Display Demura gap0", 0xA3080000, 0x02500000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_THROUGH_XN},
  {"DBI Dump", 0xA6100000, 0x00F00000, NoHob, MMAP_IO, INITIALIZED, Conv, UNCACHED_UNBUFFERED_XN},
  {"CPU Vectors", 0xA7600000, 0x00001000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK},
  {"Display Reserved", 0xB8000000, 0x02B00000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_THROUGH_XN},
  {"NOMAP FFE00000", 0xFFE00000, 0x00200000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN}
};

#define SHENG_RANGE_LIMIT MAX_ARM_MEMORY_REGION_DESCRIPTOR_COUNT
#define SHENG_PAGE_MASK 0xfffULL

typedef struct { UINT64 Base; UINT64 End; } SHENG_RAM_RANGE;
STATIC EFI_MEMORY_REGION_DESCRIPTOR mPrepared[SHENG_RANGE_LIMIT];
STATIC UINTN mPreparedCount;

STATIC BOOLEAN
ShengEnabled (CONST VOID *Fdt, INT32 Node)
{
  INT32 Length;
  CONST CHAR8 *Status = FdtGetProp (Fdt, Node, "status", &Length);
  return Status == NULL ||
    (Length == 3 && CompareMem (Status, "ok", 3) == 0) ||
    (Length == 5 && CompareMem (Status, "okay", 5) == 0);
}

STATIC BOOLEAN
ShengRanges (CONST VOID *Fdt, INT32 Node, SHENG_RAM_RANGE *Ranges, UINTN *Count)
{
  INT32 Length;
  CONST UINT8 *Reg = FdtGetProp (Fdt, Node, "reg", &Length);
  if (Reg == NULL) { return TRUE; }
  if (Length < 0 || (Length % 16) != 0) { return FALSE; }
  for (INT32 At = 0; At < Length; At += 16) {
    UINT64 Base = Fdt64ToCpu (ReadUnaligned64 (Reg + At));
    UINT64 Size = Fdt64ToCpu (ReadUnaligned64 (Reg + At + 8));
    if (Size == 0) { continue; }
    if (Size > MAX_UINT64 - Base || *Count == SHENG_RANGE_LIMIT) { return FALSE; }
    Ranges[(*Count)++] = (SHENG_RAM_RANGE){Base, Base + Size};
  }
  return TRUE;
}

STATIC BOOLEAN
ShengTwoCells (CONST VOID *Fdt, INT32 Node, CONST CHAR8 *Name)
{
  INT32 Length;
  CONST VOID *Value = FdtGetProp (Fdt, Node, Name, &Length);
  return Value != NULL && Length == 4 && Fdt32ToCpu (ReadUnaligned32 (Value)) == 2;
}

STATIC BOOLEAN
ShengCut (SHENG_RAM_RANGE *Ranges, UINTN *Count, SHENG_RAM_RANGE Cut)
{
  SHENG_RAM_RANGE Out[SHENG_RANGE_LIMIT];
  UINTN Used = 0;
  for (UINTN I = 0; I < *Count; I++) {
    SHENG_RAM_RANGE R = Ranges[I];
    if (R.End <= Cut.Base || R.Base >= Cut.End) {
      if (Used == SHENG_RANGE_LIMIT) { return FALSE; }
      Out[Used++] = R;
    } else {
      if (R.Base < Cut.Base) {
        if (Used == SHENG_RANGE_LIMIT) { return FALSE; }
        Out[Used++] = (SHENG_RAM_RANGE){R.Base, Cut.Base};
      }
      if (R.End > Cut.End) {
        if (Used == SHENG_RANGE_LIMIT) { return FALSE; }
        Out[Used++] = (SHENG_RAM_RANGE){Cut.End, R.End};
      }
    }
  }
  CopyMem (Ranges, Out, Used * sizeof (*Out));
  *Count = Used;
  return TRUE;
}

STATIC BOOLEAN
ShengAppend (EFI_MEMORY_REGION_DESCRIPTOR Descriptor, UINT64 Base, UINT64 End, UINTN *Count)
{
  if (End <= Base) { return TRUE; }
  if ((Base | End) & SHENG_PAGE_MASK) { return FALSE; }
  if (*Count >= SHENG_RANGE_LIMIT - 1) { return FALSE; }
  Descriptor.Address = Base;
  Descriptor.Length = End - Base;
  mPrepared[(*Count)++] = Descriptor;
  return TRUE;
}

STATIC EFI_MEMORY_REGION_DESCRIPTOR
ShengFreeDescriptor (VOID)
{
  EFI_MEMORY_REGION_DESCRIPTOR Result = {
    "FDT RAM", 0, 0, AddMem, SYS_MEM, SYS_MEM_CAP, Conv, WRITE_BACK_XN
  };
  return Result;
}

STATIC BOOLEAN
ShengInitrd (CONST VOID *Fdt, SHENG_RAM_RANGE *Range)
{
  INT32 Chosen = FdtPathOffset (Fdt, "/chosen");
  INT32 StartLength, EndLength;
  if (Chosen < 0) { return FALSE; }
  CONST VOID *Start = FdtGetProp (Fdt, Chosen, "linux,initrd-start", &StartLength);
  CONST VOID *End = FdtGetProp (Fdt, Chosen, "linux,initrd-end", &EndLength);
  if (Start == NULL || End == NULL || (StartLength != 4 && StartLength != 8) ||
      (EndLength != 4 && EndLength != 8)) { return FALSE; }
  Range->Base = StartLength == 4 ? Fdt32ToCpu (ReadUnaligned32 (Start)) :
    Fdt64ToCpu (ReadUnaligned64 (Start));
  Range->End = EndLength == 4 ? Fdt32ToCpu (ReadUnaligned32 (End)) :
    Fdt64ToCpu (ReadUnaligned64 (End));
  return Range->Base < Range->End && Range->End <= MAX_UINT64 - SHENG_PAGE_MASK;
}


VOID
ShengPrepareMemoryMap (IN CONST VOID *Fdt)
{
  SHENG_RAM_RANGE Free[SHENG_RANGE_LIMIT];
  SHENG_RAM_RANGE Reserved[SHENG_RANGE_LIMIT];
  SHENG_RAM_RANGE Inputs[2];
  UINTN FreeCount = 0, ReservedCount = 0, Count = 0;
  INT32 Root, Memory, Res, Length, Mpss;
  CONST VOID *Value;
  BOOLEAN MpssDisabled;

  mPreparedCount = 0;
  if (Fdt == NULL || ((UINTN)Fdt & 3) || FdtCheckHeader (Fdt) != 0) { return; }
  Root = FdtPathOffset (Fdt, "/");
  Memory = FdtPathOffset (Fdt, "/memory");
  Res = FdtPathOffset (Fdt, "/reserved-memory");
  // These corrections are qualified against the 8 GiB sheng layout only.
  if (Root < 0 || Memory < 0 || Res < 0 ||
      !ShengTwoCells (Fdt, Root, "#address-cells") || !ShengTwoCells (Fdt, Root, "#size-cells") ||
      !ShengTwoCells (Fdt, Res, "#address-cells") || !ShengTwoCells (Fdt, Res, "#size-cells")) { return; }
  Value = FdtGetProp (Fdt, Memory, "ddr_size", &Length);
  if (Value == NULL || Length != 4 || Fdt32ToCpu (ReadUnaligned32 (Value)) != 8 ||
      !ShengEnabled (Fdt, Memory) || !ShengRanges (Fdt, Memory, Free, &FreeCount) ||
      FreeCount == 0 || !ShengInitrd (Fdt, &Inputs[0])) { return; }
  BOOLEAN HasHighBank = FALSE;
  for (UINTN I = 0; I < FreeCount; I++) {
    UINT64 Base = Free[I].Base;
    UINT64 End = Free[I].End;
    if (!((Base >= 0x80000000ULL && End <= 0x100000000ULL) ||
          (Base >= 0x880000000ULL && End <= 0xa00000000ULL))) { return; }
    if (Base == 0x8c0000000ULL && End == 0xa00000000ULL) { HasHighBank = TRUE; }
    for (UINTN J = 0; J < I; J++) {
      if (Base < Free[J].End && End > Free[J].Base) { return; }
    }
  }
  if (!HasHighBank) { return; }
  for (INT32 Node = FdtFirstSubnode (Fdt, Res); Node >= 0; Node = FdtNextSubnode (Fdt, Node)) {
    if (ShengEnabled (Fdt, Node) && !ShengRanges (Fdt, Node, Reserved, &ReservedCount)) { return; }
  }
  // Preserve the FDT reservation block, including entries outside /reserved-memory.
  UINT32 Total = FdtTotalSize (Fdt);
  UINT32 Offset = Fdt32ToCpu (ReadUnaligned32 ((CONST UINT8 *)Fdt + 16));
  BOOLEAN Terminated = FALSE;
  if (Total < 40 || Total > SIZE_4MB || Offset < 40 || (Offset & 7)) { return; }
  while (Offset <= Total && Total - Offset >= 16) {
    UINT64 Base = Fdt64ToCpu (ReadUnaligned64 ((CONST UINT8 *)Fdt + Offset));
    UINT64 Size = Fdt64ToCpu (ReadUnaligned64 ((CONST UINT8 *)Fdt + Offset + 8));
    Offset += 16;
    if (Base == 0 && Size == 0) { Terminated = TRUE; break; }
    if (Size > MAX_UINT64 - Base || ReservedCount == SHENG_RANGE_LIMIT) { return; }
    Reserved[ReservedCount++] = (SHENG_RAM_RANGE){Base, Base + Size};
  }
  if (!Terminated) { return; }
  for (UINTN I = 0; I < ReservedCount; I++) {
    if (!ShengCut (Free, &FreeCount, Reserved[I])) { return; }
  }
  // Keep the ADSP's fixed DMA buffer and both ABL handoff inputs out of DXE allocations.
  if (!ShengCut (Free, &FreeCount, (SHENG_RAM_RANGE){0xd0000000, 0xd0010000}) ||
      (UINT64)(UINTN)Fdt > MAX_UINT64 - Total - SHENG_PAGE_MASK) { return; }
  Inputs[1] = (SHENG_RAM_RANGE){(UINT64)(UINTN)Fdt, (UINT64)(UINTN)Fdt + Total};
  for (UINTN I = 0; I < ARRAY_SIZE (Inputs); I++) {
    if (!((Inputs[I].Base >= 0x80000000ULL && Inputs[I].End <= 0x100000000ULL) ||
          (Inputs[I].Base >= 0x880000000ULL && Inputs[I].End <= 0xa00000000ULL))) { return; }
    Inputs[I].Base &= ~SHENG_PAGE_MASK;
    Inputs[I].End = (Inputs[I].End + SHENG_PAGE_MASK) & ~SHENG_PAGE_MASK;
    if (!ShengCut (Free, &FreeCount, Inputs[I])) { return; }
  }
  Mpss = FdtPathOffset (Fdt, "/reserved-memory/mpss_region@89800000");
  Value = Mpss < 0 ? NULL : FdtGetProp (Fdt, Mpss, "status", &Length);
  MpssDisabled = Value != NULL && Length == 9 && CompareMem (Value, "disabled", 9) == 0;

  for (UINTN I = 0; I < ARRAY_SIZE (gMemoryDescriptor); I++) {
    EFI_MEMORY_REGION_DESCRIPTOR D = gMemoryDescriptor[I];
    BOOLEAN Reclaim = AsciiStrCmp (D.Name, "NOMAP 811D0000") == 0 ||
      AsciiStrCmp (D.Name, "RSRV0 gap0") == 0 ||
      (D.Address == 0x81a40000 && AsciiStrCmp (D.Name, "XBL Ramdump") == 0) ||
      AsciiStrCmp (D.Name, "RSRV1") == 0 || AsciiStrCmp (D.Name, "PIL Reserved gap1") == 0 ||
      AsciiStrCmp (D.Name, "NOMAP FFE00000") == 0 ||
      (MpssDisabled && AsciiStrCmp (D.Name, "PIL Reserved gap0") == 0);
    // Existing free memory is also bounded by the live bank table. In particular,
    // the stale 0xe671d000 run must not include the inaccessible firmware gap.
    BOOLEAN ExistingFree = D.MemoryType == Conv && D.ResourceType == SYS_MEM;
    if (!Reclaim && !ExistingFree) {
      if (!ShengAppend (D, D.Address, D.Address + D.Length, &Count)) { return; }
      continue;
    }
    SHENG_RAM_RANGE Keep[SHENG_RANGE_LIMIT] = {{D.Address, D.Address + D.Length}};
    UINTN KeepCount = 1;
    for (UINTN J = 0; J < FreeCount; J++) {
      UINT64 Base = MAX (D.Address, Free[J].Base);
      UINT64 End = MIN (D.Address + D.Length, Free[J].End);
      Base = (Base + 0xfff) & ~0xfffULL;
      End &= ~0xfffULL;
      if (Base >= End) { continue; }
      EFI_MEMORY_REGION_DESCRIPTOR Available = ExistingFree ? D : ShengFreeDescriptor ();
      if (!ShengAppend (Available, Base, End, &Count) ||
          !ShengCut (Keep, &KeepCount, (SHENG_RAM_RANGE){Base, End})) { return; }
    }
    // Unproven parts stay reserved, including active firmware and peripheral ranges.
    for (UINTN J = 0; J < KeepCount; J++) {
      EFI_MEMORY_REGION_DESCRIPTOR Held = D;
      if (ExistingFree) {
        Held.ResourceType = MEM_RES;
        Held.MemoryType = Reserv;
        Held.HobOption = HobOnlyNoCacheSetting;
        // Boot inputs are explicitly mapped below.
        for (UINTN K = 0; K < ARRAY_SIZE (Inputs); K++) {
          if (Inputs[K].Base < Keep[J].End && Inputs[K].End > Keep[J].Base) {
            Held.HobOption = AddMem;
            break;
          }
        }
      }
      if (!ShengAppend (Held, Keep[J].Base, Keep[J].End, &Count)) { return; }
    }
  }
  // Map handoff inputs that lie in gaps in the original firmware map.
  for (UINTN I = 0; I < ARRAY_SIZE (Inputs); I++) {
    SHENG_RAM_RANGE Missing[SHENG_RANGE_LIMIT] = {Inputs[I]};
    UINTN MissingCount = 1;
    for (UINTN J = 0; J < Count; J++) {
      if (!ShengCut (Missing, &MissingCount, (SHENG_RAM_RANGE){mPrepared[J].Address,
          mPrepared[J].Address + mPrepared[J].Length})) { return; }
    }
    EFI_MEMORY_REGION_DESCRIPTOR Held = {
      "ABL input", 0, 0, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_BACK_XN
    };
    for (UINTN J = 0; J < MissingCount; J++) {
      if (!ShengAppend (Held, Missing[J].Base, Missing[J].End, &Count)) { return; }
    }
  }
  // Add bank fragments omitted by the static map, including the 30 MiB high run.
  for (UINTN I = 0; I < Count; I++) {
    if (!ShengCut (Free, &FreeCount, (SHENG_RAM_RANGE){mPrepared[I].Address,
        mPrepared[I].Address + mPrepared[I].Length})) { return; }
  }
  for (UINTN I = 0; I < FreeCount; I++) {
    UINT64 Base = (Free[I].Base + 0xfff) & ~0xfffULL;
    UINT64 End = Free[I].End & ~0xfffULL;
    if (!ShengAppend (ShengFreeDescriptor (), Base, End, &Count)) { return; }
  }
  mPreparedCount = Count;
}

VOID
GetMemoryMap (
  OUT EFI_MEMORY_REGION_DESCRIPTOR **MemoryDescriptor,
  OUT UINT8                         *MemoryDescriptorCount)
{
  // Pass Data
  *MemoryDescriptor      = mPreparedCount ? mPrepared : gMemoryDescriptor;
  *MemoryDescriptorCount = mPreparedCount ? (UINT8)mPreparedCount : ARRAY_SIZE (gMemoryDescriptor);
}
