  INF MdeModulePkg/Core/Dxe/DxeMain.inf
  INF MdeModulePkg/Universal/PCD/Dxe/Pcd.inf

!if $(SHENG_VENDOR_HW) == 1
  INF Binaries/sheng/QcomPkg/Drivers/EnvDxe/EnvDxe.inf
!endif

  INF MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  INF MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf
  INF ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  INF MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  INF MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf

!if $(SHENG_VENDOR_HW) == 1
  INF Binaries/sheng/QcomPkg/Drivers/ShmBridgeDxe/ShmBridgeDxeLA.inf
  INF Binaries/sheng/QcomPkg/Drivers/TzDxe/ScmDxeCompat.inf
  INF Binaries/sheng/QcomPkg/Drivers/TzDxe/TzDxeLA.inf
!endif

  INF MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  INF MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  INF MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
  INF EmbeddedPkg/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf
  INF MdeModulePkg/Universal/ResetSystemRuntimeDxe/ResetSystemRuntimeDxe.inf
  INF EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  INF EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  INF MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  INF ArmPkg/Drivers/ArmGicDxe/ArmGicDxe.inf
  INF ArmPkg/Drivers/TimerDxe/TimerDxe.inf

!if $(SHENG_VENDOR_HW) == 1
  INF Binaries/sheng/QcomPkg/Drivers/ChipInfoDxe/ChipInfoDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/IPCCDxe/IPCCDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/GLinkDxe/GLinkDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/PlatformInfoDxe/PlatformInfoDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/ProjectInfoDxe/ProjectInfoDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/DALSYSDxe/DALSYSDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/HALIOMMUDxe/HALIOMMUDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/HWIODxe/HWIODxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/I2CDxe/I2CDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/TLMMDxe/TLMMDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/PmicDxe/PmicDxeLa.inf
!endif

  INF MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  INF shengPkg/Drivers/ShengBootMemoryDxe/ShengBootMemoryDxe.inf
  # The XNU loader is part of the firmware: a driver loaded from a volume is
  # refused by the image verification of this build.
  INF DarwinBootPkg/Driver/DarwinBoot.inf
  INF MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  INF MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  INF FatPkg/EnhancedFatDxe/Fat.inf
  INF MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf

!if $(SHENG_VENDOR_HW) == 1
  INF Binaries/sheng/QcomPkg/Drivers/UFSDxe/UFSDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/CmdDbDxe/CmdDbDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/PwrUtilsDxe/PwrUtilsDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/RpmhDxe/RpmhDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/NpaDxe/NpaDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/ULogDxe/ULogDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/VcsDxe/VcsDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/ClockDxe/ClockDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/SPMIDxe/SPMIDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/ICBDxe/ICBDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/SmemDxe/SmemDxe.inf
!endif
  INF MdeModulePkg/Universal/BdsDxe/BdsDxe.inf

!if $(SHENG_VENDOR_HW) == 1
  INF Binaries/sheng/QcomPkg/Drivers/RscDxe/RscDxe.inf
!endif

  INF EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf
  INF MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  INF MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  INF MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  INF shengPkg/Drivers/ShengKeyInputDxe/ShengKeyInputDxe.inf
  INF MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf

  # Headless until stock sheng framebuffer data is available.
!if $(SHENG_VENDOR_HW) == 1
  INF Binaries/sheng/QcomPkg/Drivers/GpiDxe/GpiDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/SPIDxe/SPIDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/SdccDxe/SdccDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/FeatureEnablerDxe/FeatureEnablerDxe.inf

  # USE_CUSTOM_DISPLAY_DRIVER = 1: stock sheng display chain (board DEPEX glue,
  # unchanged vendor DisplayDxe with the N81A panel data from RAW.inc).
  # Otherwise SimpleFbDxe publishes a GOP on the frame buffer the previous
  # stage left scanning out.
!endif
!if $(USE_CUSTOM_DISPLAY_DRIVER) == 1
  INF shengPkg/Drivers/ShengGopLogDxe/ShengGopLogDxe.inf
  INF shengPkg/Drivers/ShengDisplayDepexDxe/ShengDisplayDepexDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/DisplayDxe/DisplayDxe.inf
!else
  INF SiliciumPkg/Drivers/SimpleFbDxe/SimpleFbDxe.inf
!endif

  # Headless until stock sheng framebuffer data is available.

!if $(SHENG_VENDOR_HW) == 1
  INF Binaries/sheng/QcomPkg/Drivers/ButtonsDxe/ButtonsDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/ChargerExDxe/ChargerExDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/PmicGlinkDxe/PmicGlinkDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/QcomChargerDxe/QcomChargerDxeLA.inf
  INF Binaries/sheng/QcomPkg/Drivers/UsbPwrCtrlDxe/UsbPwrCtrlDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/TsensDxe/TsensDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/LimitsDxe/LimitsDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/DDRInfoDxe/DDRInfoDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/UsbfnDwc3Dxe/UsbfnDwc3Dxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/XhciPciEmulationDxe/XhciPciEmulationDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/XhciDxe/XhciDxe.inf
!endif

  INF MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

!if $(SHENG_VENDOR_HW) == 1
  INF Binaries/sheng/QcomPkg/Drivers/UsbMsdDxe/UsbMsdDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/UsbDeviceDxe/UsbDeviceDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/UsbConfigDxe/UsbConfigDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/UsbInitDxe/UsbInitDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/UCDxe/UCDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/RNGDxe/RngDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/PILDxe/PILDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/PILProxyDxe/PILProxyDxe.inf
  INF Binaries/sheng/QcomPkg/Drivers/SPSSDxe/SPSSDxe.inf
!endif
