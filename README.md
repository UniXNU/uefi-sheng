# uefi-sheng

UEFI platform integration for the Xiaomi Pad 6S Pro (sheng, SM8550), using
Project Silicium and Project Mu.

The platform supplies memory reservations, board peripherals, display
handoff, and the firmware volume. UniXNU supplies the boot application and
DarwinBoot loader. UniXNU-sm8550-common supplies shared XNU platform support.

- [Firmware integration](https://unixnu.com/porting/uefi-firmware)
- [sheng device support](https://unixnu.com/devices/sheng)
- [Build instructions](Building.md)
- [Platform details](Platforms/Xiaomi/shengPkg/PORTING.md)
- [Platform status](Status.md)

## Credits

[Microsoft](https://github.com/microsoft/) for their [Project Mu](https://microsoft.github.io/mu/) which Powers this Project. <br>
[WOA-Project](https://github.com/WOA-Project/) for their [mu_andromeda_platforms](https://github.com/WOA-Project/mu_andromeda_platforms/) which is used as Base for this Project. <br>
[Project Aloha](https://github.com/Project-Aloha/) for their [mu_aloha_platforms](https://github.com/Project-Aloha/mu_aloha_platforms/) which is used as Reference for some SoCs. <br>
[Open Surface RT](https://openrt.gitbook.io/open-surfacert/) for their Help on Tegra Related Stuff.

[sonic011gamer](https://github.com/sonic011gamer/) and [halal-beef](https://github.com/halal-beef/) for their Amazing Work on Exynos Related Stuff.

## License

All code except drivers in `GPLDrivers` & `GPLApplications` & `GPLLibrary` directories are licensed under BSD-2-Clause. <br />
GPL Drivers are licensed under `GPLv2` or later license.
