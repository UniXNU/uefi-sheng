/**
  Copyright (c) 2011-2017, ARM Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PrePiLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformSecLib.h>
#include <Library/PrePiHobListPointerLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/MemoryMapHelperLib.h>
#include <Library/TimerLib.h>
#include <Library/HobLib.h>

#include "Sec.h"

// First argument (x0) of the previous stage, saved by ModuleEntryPoint.S
extern UINT64  gSecEntryArg0;

// The library constructors ProcessLibraryConstructorList () runs for this module, in its generated order, called one by one so the screen
// shows which one a stall is in (an entry here that the generated list lacks fails to link, which is the check).
// Helper.S: clean and invalidate every data cache level by set/way (see there).
extern VOID SecCleanInvalidateDataCacheAll (VOID);

extern RETURN_STATUS EFIAPI BaseDebugLibSerialPortConstructor (VOID);
extern RETURN_STATUS EFIAPI ExtractGuidedSectionLibConstructor (VOID);
extern RETURN_STATUS EFIAPI ArmMmuBaseLibConstructor (VOID);
extern RETURN_STATUS EFIAPI LzmaDecompressLibConstructor (VOID);

// Walks the HOB list with a bound, so a zero-length or runaway entry is reported instead of looping forever in the constructors and in
// the FV decompression that follow (both search the list).
STATIC
VOID
CheckHobList (VOID)
{
  EFI_PEI_HOB_POINTERS  Hob;
  UINTN                 Count = 0;

  Hob.Raw = GetHobList ();
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobLength < sizeof (EFI_HOB_GENERIC_HEADER) || Count >= 16384) {
      DEBUG ((EFI_D_ERROR, "SEC: HOB list broken at entry %u: type 0x%x length %u\n", (UINT32)Count, (UINT32)Hob.Header->HobType, (UINT32)Hob.Header->HobLength));
      return;
    }

    Count++;
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  DEBUG ((EFI_D_WARN, "SEC: HOB list ok, %u entries\n", (UINT32)Count));
}

#ifndef MDEPKG_NDEBUG
STATIC
VOID
ClearFrameBuffer (VOID)
{
  EFI_MEMORY_REGION_DESCRIPTOR FrameBufferRegion;

  // Locate "Display Reserved" Memory Region
  LocateMemoryRegionByName ("Display Reserved", &FrameBufferRegion);
  LocateMemoryRegionByName ("Display_Reserved", &FrameBufferRegion);

  // Clear Memory Region
  if (FrameBufferRegion.Address != 0 && FrameBufferRegion.Length != 0) {
    ZeroMem ((VOID *)FrameBufferRegion.Address, FrameBufferRegion.Length);
  }
}

STATIC
VOID
PrintFirmwareVersion (VOID)
{
  // Clear Frame Buffer
  ClearFrameBuffer ();

  // Print UEFI Version Message
  DEBUG ((EFI_D_WARN, "\n"));
  DEBUG ((EFI_D_WARN, "Project Silicium for %a %a\n",            FixedPcdGetPtr (PcdSmbiosSystemManufacturer), FixedPcdGetPtr (PcdSmbiosSystemModel)));
  DEBUG ((EFI_D_WARN, "Firmware Version %s Build at %a on %a\n", PcdGetPtr (PcdFirmwareVersionString), __TIME__, __DATE__));
  DEBUG ((EFI_D_WARN, "\n"));
}
#endif

STATIC
EFI_STATUS
InitializeMemory (VOID)
{
  EFI_STATUS                    Status;
  EFI_MEMORY_REGION_DESCRIPTOR  UefiMemoryRegion;
  EFI_HOB_HANDOFF_INFO_TABLE   *HobList;

  // Locate "DXE Heap" Memory Region
  LocateMemoryRegionByName ("DXE Heap", &UefiMemoryRegion);
  LocateMemoryRegionByName ("DXE_Heap", &UefiMemoryRegion);

  // Save UEFI Memory Details
  UINTN UefiMemoryBase = UefiMemoryRegion.Address;
  UINTN UefiMemorySize = UefiMemoryRegion.Length;

  // Verify UEFI Memory Base
  if (UefiMemoryBase == 0 && UefiMemorySize == 0) {
    DEBUG ((EFI_D_ERROR, "Failed to Locate \"DXE Heap\" Memory Region!\n"));
    return EFI_NOT_FOUND;
  }

  // Initialize HOB List
  HobList = HobConstructor ((VOID *)UefiMemoryBase, UefiMemorySize, (VOID *)UefiMemoryBase, (VOID *)(UefiMemoryBase + UefiMemorySize));
  PrePeiSetHobList (HobList);

  // Initialize MMU & Memory HOBs
  Status = MemoryPeim (UefiMemoryBase, UefiMemorySize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Build Stack HOB
  BuildStackHob (FixedPcdGet64 (PcdCPUCoresStackBase), FixedPcdGet32 (PcdCPUCorePrimaryStackSize));

  return EFI_SUCCESS;
}

STATIC
VOID
SecMain (VOID)
{
  EFI_STATUS Status;

  // Do Platform Specific Initialization
  PlatformInitialize ();

#ifndef MDEPKG_NDEBUG
  // Initialize Serial Port
  Status = SerialPortInitialize ();
  if (EFI_ERROR (Status)) {
    return;
  }

  // Print Firmware Version
  PrintFirmwareVersion ();

  // Initialize Debug Agent
  InitializeDebugAgent (DEBUG_AGENT_INIT_POSTMEM_SEC, NULL, NULL);
  SaveAndSetDebugTimerInterrupt (TRUE);
#endif

  // Initialize Memory
  Status = InitializeMemory ();
  if (EFI_ERROR (Status)) {
    return;
  }

  DEBUG ((EFI_D_WARN, "SEC: memory HOBs done\n"));

  // Build CPU HOB
  BuildCpuHob (ArmGetPhysicalAddressBits (), PcdGet8 (PcdPrePiCpuIoSize));

  // Set the Boot Mode
  SetBootMode (BOOT_WITH_DEFAULT_SETTINGS);

  // Initialize Platform HOBs
  Status = PlatformPeim ();
  DEBUG ((EFI_D_WARN, "SEC: platform HOBs done (%r)\n", Status));
  if (EFI_ERROR (Status)) {
    return;
  }

  // Pass on what the previous stage handed over in x0 (a DT address for a
  // Linux-style boot loader), so a DXE driver can use it.
  {
    UINT64  EntryArg = gSecEntryArg0;

    BuildGuidDataHob (&gSiliciumEntryArgHobGuid, &EntryArg, sizeof (EntryArg));
  }

  CheckHobList ();

  // Run Library Cconstructors
  DEBUG ((EFI_D_WARN, "SEC: constructor debug\n"));
  BaseDebugLibSerialPortConstructor ();
  DEBUG ((EFI_D_WARN, "SEC: constructor guided-section\n"));
  ExtractGuidedSectionLibConstructor ();
  DEBUG ((EFI_D_WARN, "SEC: constructor mmu\n"));
  ArmMmuBaseLibConstructor ();
  DEBUG ((EFI_D_WARN, "SEC: constructor lzma\n"));
  LzmaDecompressLibConstructor ();
  DEBUG ((EFI_D_WARN, "SEC: constructors done\n"));
  CheckHobList ();

  // Decompress all FVs
  Status = DecompressFirstFv ();
  DEBUG ((EFI_D_WARN, "SEC: FV decompressed (%r)\n", Status));
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to Decompress FV! Status = %r\n", Status));
    return;
  }

  // Load DXE Core
  DEBUG ((EFI_D_WARN, "SEC: loading DXE core\n"));
  Status = LoadDxeCoreFromFv (NULL, 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to Load DXE Core! Status = %r\n", Status));
  }
}

VOID
SecEntry (VOID)
{
  // The previous stage ran with its caches on. Clean and invalidate them before anything else: the UEFI writes its HOB tables and heap with
  // the MMU and caches off, and lines the previous stage left would hide those writes (or be written back over them) once the caches are on.
  SecCleanInvalidateDataCacheAll ();

  // Disable Data Cache
  ArmDisableDataCache ();

  // Invalidate Instruction Cache
  ArmInvalidateInstructionCache ();

  // Enable Instruction Cache
  ArmEnableInstructionCache ();

  // Write back and invalidate Stack & UEFI FD D-Cache. A plain invalidate would discard what the entry code just stored there
  // (live stack frames, gSecEntryArg0) if the previous stage left the data cache on.
  WriteBackInvalidateDataCacheRange ((VOID *)FixedPcdGet64 (PcdCPUCoresStackBase), FixedPcdGet32 (PcdCPUCorePrimaryStackSize));
  WriteBackInvalidateDataCacheRange ((VOID *)FixedPcdGet64 (PcdFdBaseAddress),     FixedPcdGet32 (PcdFdSize));

  // Verify Exception Vector Table
  ASSERT (((UINTN)SecVectorTable & ARM_VECTOR_TABLE_ALIGNMENT) == 0);

  // Enable new Exception Vector Table
  ArmWriteVBar ((UINTN)SecVectorTable);

  // Enter Main SEC Function
  SecMain ();
}
