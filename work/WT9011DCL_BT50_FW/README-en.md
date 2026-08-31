[tag download]:https://github.com/Jieli-Tech/fw-AC63_BT_SDK/tags
[tag_badgen]:https://img.shields.io/github/v/tag/Jieli-Tech/fw-AC63_BT_SDK?style=plastic&logo=bluetooth&labelColor=ffffff&color=informational&label=Tag&logoColor=blue

# fw-AC63_BT_SDK  [![tag][tag_badgen]][tag download]

<div align="center">

**Generic Bluetooth SDK Firmware for JL AC63 Series SoCs**

[中文](./README.md) · [Documentation](https://doc.zh-jieli.com/AC63/zh-cn/master/index.html) · [Release History](https://doc.zh-jieli.com/AC63/zh-cn/master/other/version/index.html) · [Report Issues](https://github.com/Jieli-Tech/fw-AC63_BT_SDK/issues)

</div>

---

## 📋 Table of Contents

- [1. Overview](#1-overview)
- [2. Supported Chips & Platforms](#2-supported-chips--platforms)
- [3. Environment Setup](#3-environment-setup)
- [4. Quick Start](#4-quick-start)
- [5. Project Structure](#5-project-structure)
- [6. Application Selection Guide](#6-application-selection-guide)
- [7. Build Guide](#7-build-guide)
- [8. Flashing & Upgrading](#8-flashing--upgrading)
- [9. Configuration](#9-configuration)
- [10. FAQ](#10-faq)
- [11. Community & Support](#11-community--support)
- [12. Certifications](#12-certifications)
- [13. Disclaimer](#13-disclaimer)

---

## 1. Overview

`fw-AC63_BT_SDK` is Jieli Technology's generic Bluetooth SDK firmware development package for the AC63 series of SoCs. Built on top of the Zephyr RTOS, this SDK provides a complete Bluetooth protocol stack and a rich set of application examples for the following use cases:

| Application Type | Typical Products |
|-----------------|-----------------|
| **SPP + BLE** | Data passthrough, smart devices, FindMy, USB Dongle |
| **HID** | Bluetooth keyboard, mouse, remote control, shutter release, gamepad |
| **Bluetooth Mesh** | Smart lighting, sensor networks, Alibaba/Tuya/Tencent integrations |

This repository contains SDK release source code and example projects. It must be used with the corresponding prebuilt library files (`lib.a`) and sub-repositories that follow the same naming conventions. It also incorporates open-source projects such as [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr).

---

## 2. Supported Chips & Platforms

### 2.1 SoC Series

| Platform | Chip Models | Supported Applications |
|---------|------------|----------------------|
| **bd19** | AC6321A / AC6323A / AC6328A / AC6328B / AC6329B / AC6329C / AC6329E / AC6329F / AC632N | spp_and_le / hid / mesh |
| **br23** | AC6351D / AC635N | spp_and_le / hid / mesh |
| **br25** | AC6363F / AC6366C / AC6368A / AC6368B / AC6369C / AC6369F / AC636N | spp_and_le / hid / mesh |
| **br34** | AC6381A / AC6385A / AC638N | spp_and_le / hid / mesh |

### 2.2 Bluetooth Specification Support

| Bluetooth Core | QDID | Status |
|---------------|------|--------|
| Core v5.4 | [QDID 222830](https://launchstudio.bluetooth.com/ListingDetails/193923) | ✅ Certified |

---

## 3. Environment Setup

### 3.1 Prerequisites

| OS | Notes |
|---|-------|
| **Windows** | ✅ Recommended: use Code::Blocks IDE for building |
| **Linux** | ✅ Command-line Makefile build supported |

### 3.2 Install the Toolchain

1. Download and install the **JL Toolchain**: [Download Link](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/dev_env/index.html)
2. Linux users can download from: [pkgman.jieliapp.com](http://pkgman.jieliapp.com/doc/all)
   - Extract to `/opt/jieli`
   - Verify that `/opt/jieli/common/bin/clang` exists
3. Verify the installation:

```bash
clang --version
```

### 3.3 Install Flashing Tools

| Tool | Purpose | How to Get |
|------|---------|-----------|
| **USB Updater** | Flash firmware to target board | [Apply](https://item.taobao.com/item.htm?id=620295020803) · [Docs](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/forced_upgrade/index.html) |
| **Production Burner** | Mass production / bare chip programming | [Docs](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo2/index.html) |
| **Wireless Tester** | OTA / RF calibration / production testing | [Apply](https://item.taobao.com/item.htm?id=620942507511) · [Docs](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/testbox_1tuo2/index.html) |

---

## 4. Quick Start

### 4.1 Clone the Repository

```bash
git clone https://github.com/Jieli-Tech/fw-AC63_BT_SDK.git
cd fw-AC63_BT_SDK
```

### 4.2 Choose Your Application

```
SDK Root
├── apps/spp_and_le/    # SPP + BLE data transfer
├── apps/hid/           # HID devices (keyboard, mouse, remote)
└── apps/mesh/          # Bluetooth Mesh
```

### 4.3 Select Chip and Board Configuration

Each application has a `board/` directory organized by chip platform:

```
apps/hid/board/
├── bd19/   # AC632N series (32 board configs)
├── br23/   # AC635N series
├── br25/   # AC636N series
└── br34/   # AC638N series
```

Each board directory contains:
- `Makefile` - Build script
- `board_*.cbp` - Code::Blocks project file
- `board_xxx.c` - Board initialization code
- `board_xxx_cfg.h` - Board configuration (pins, peripherals)
- `board_xxx_global_build_cfg.h` - Global build configuration (feature toggles)

### 4.4 Build and Flash

**Option 1: Code::Blocks (recommended for Windows)**

```bash
# 1. Enter the board directory
cd apps/hid/board/bd19/

# 2. Double-click the .cbp project file (e.g., AC632N_hid.cbp)
# 3. In Code::Blocks: Build → Build (Ctrl+F9)
# 4. Use the USB Updater to flash the generated .hex file
```

**Option 2: Makefile (command line)**

```bash
# Windows: double-click tools/make_prompt.bat to open the build shell

# Linux/macOS: run from SDK root directory
make ac632n_spp_and_le
```

> **💡 Tip**: See the [Makefile](https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK/blob/master/Makefile) header comments for all supported target names.

**Option 3: VS Code**

Pre-configured VS Code tasks are available. Press `Ctrl+Shift+B` to select a build target.

---

## 5. Project Structure

```
fw-AC63_BT_SDK/
├── apps/                          # Application layer
│   ├── common/                    # Shared modules (cross-project)
│   │   ├── audio/                 #   Audio codec, volume control
│   │   ├── bt_common/             #   Common Bluetooth interface
│   │   ├── cJSON/                 #   JSON parser library
│   │   ├── debug/                 #   Debug utilities
│   │   ├── device/                #   Peripheral drivers (key, USB, sensors)
│   │   ├── jl_kws/                #   Jieli keyword spotting
│   │   ├── music/                 #   Music playback
│   │   ├── update/                #   Firmware update
│   │   └── third_party_profile/   #   Third-party protocols (SigMesh, Tuya, Tencent, HiLink)
│   ├── spp_and_le/                # 📌 SPP + BLE application
│   ├── hid/                       # 📌 HID application (keyboard/mouse/remote/gamepad)
│   └── mesh/                      # 📌 Mesh application
├── cpu/                           # CPU-specific code and libraries
│   ├── bd19/ → br34/              #   Per-platform lib.a + tool scripts
│   ├── .../
│   └── br34/
├── include_lib/                   # Header files (BT stack, drivers, media, system)
├── doc/                           # Documentation
│   ├── datasheet/                 #   SoC datasheets
│   ├── architure/                 #   SDK architecture docs
│   ├── FAQ/                       #   Frequently asked questions
│   └── .../
├── tools/                         # Build tools and scripts
│   └── make_prompt.bat            #   Windows build shell launcher
├── Makefile                       # Top-level Makefile (unified entry point)
├── default.workspace              # Code::Blocks workspace
└── .vscode/                       # VS Code configuration (pre-defined build tasks)
```

### 5.1 Key Directories

| Directory | Purpose |
|-----------|---------|
| `apps/*/board/` | **Board configs**: pin definitions, peripheral init, build options |
| `apps/*/examples/` | **Reference apps**: ready-to-use implementations |
| `apps/*/include/` | **App headers**: module interface definitions |
| `apps/*/config/` | **Library config**: feature toggles that control which library functionality is included |
| `cpu/*/liba/` | **Prebuilt libraries**: `*.a` static libraries (btctrler, btstack, media, etc.) |
| `cpu/*/tools/` | **Flashing tools**: download.bat, fw_add.exe, isd_download.exe |

---

## 6. Application Selection Guide

### 6.1 SPP + BLE (`apps/spp_and_le/`)

| Item | Description |
|------|-------------|
| **Use Cases** | Data passthrough, barcode scanner, Bluetooth dongle, FindMy, beacon, multi-link |
| **Features** | Classic SPP + BLE dual mode, AT command support |
| **Documentation** | [SPP_LE Developer Guide](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/spple/index.html) |

### 6.2 HID (`apps/hid/`)

| Item | Description |
|------|-------------|
| **Use Cases** | Bluetooth keyboard, mouse, remote control, shutter release, gamepad, voice remote |
| **Examples** | `examples/mouse_single/` mouse / `examples/keyboard/` keyboard / `examples/gamebox/` gamepad / `examples/voice_remote_control/` voice remote |
| **Documentation** | [HID Developer Guide](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/hid/index.html) |

### 6.3 Mesh (`apps/mesh/`)

| Item | Description |
|------|-------------|
| **Use Cases** | Smart lighting, sensor networks, Alibaba Genie/Tuya/Tencent integrations |
| **Examples** | `generic_onoff_server` / `light_lightness_server` / `AliGenie_fan` / `TUYA_light` / `tencent_mesh` |
| **Documentation** | [Mesh Developer Guide](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/mesh/index.html) |

### 6.4 Coming Soon

| Application | Description |
|------------|-------------|
| **IoT (IPv6 / 6LoWPAN)** | IPv6-based IoT applications |
| **2.4G Proprietary** | Vendor-custom 2.4G wireless protocol |

---

## 7. Build Guide

### 7.1 Build Command Reference

Run these commands from the SDK root directory:

| Target | Chip | Application | Command |
|--------|------|------------|---------|
| AC632N | bd19 | spp_and_le | `make ac632n_spp_and_le` |
| AC635N | br23 | spp_and_le | `make ac635n_spp_and_le` |
| AC636N | br25 | spp_and_le | `make ac636n_spp_and_le` |
| AC638N | br34 | spp_and_le | `make ac638n_spp_and_le` |
| AC632N | bd19 | hid | `make ac632n_hid` |
| AC635N | br23 | hid | `make ac635n_hid` |
| AC636N | br25 | hid | `make ac636n_hid` |
| AC638N | br34 | hid | `make ac638n_hid` |
| AC632N | bd19 | mesh | `make ac632n_mesh` |
| AC635N | br23 | mesh | `make ac635n_mesh` |
| AC636N | br25 | mesh | `make ac636n_mesh` |
| AC638N | br34 | mesh | `make ac638n_mesh` |
| **All** | All | All | `make all` |
| **Clean All** | All | All | `make clean` |

### 7.2 Clean a Single Project

```bash
make clean_ac632n_spp_and_le    # Clean ac632n_spp_and_le build artifacts
make clean_ac632n_hid           # Clean ac632n_hid build artifacts
make clean_ac632n_mesh          # Clean ac632n_mesh build artifacts
```

### 7.3 Linux Build Notes

```bash
# Increase the file descriptor limit (required for linking)
ulimit -n 8096

# Build from the SDK root
make ac632n_spp_and_le -j`nproc`
```

### 7.4 Common Build Errors

| Error Message | Solution |
|--------------|----------|
| `clang: command not found` | Toolchain not installed or not in PATH |
| `Too many open files` | Run `ulimit -n 8096` on Linux |
| `cannot find -lxxx` | Missing `.a` library file; check `cpu/*/liba/` |
| `undefined reference to ...` | Feature config excludes the required module; check `lib_*_config.c` |

---

## 8. Flashing & Upgrading

### 8.1 First-Time Flashing

1. **Connect hardware**: Connect the development board to your PC via USB or UART
2. **Enter programming mode**: Hold the programming button, then reset or re-power the board
3. **Open the USB Updater**: Launch `isd_download.exe`
4. **Select firmware**: Choose the compiled `.hex` file
5. **Start flashing**: Click the download button and wait for completion

> **⚠️ Note**: For detailed configuration instructions of the INI file (cpu\xx\tools\isd_config_rule.c), please refer to the [download script configuration document](https://doc.zh-jieli.com/AC63/zh-cn/master/getting_started/project_download/INI_config.html) and [official website documentation](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/common_function/index.html).

### 8.2 OTA Update

Single-bank and dual-bank OTA updates are supported. See: [OTA Developer Guide](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/ota/index.html)

---

## 9. Configuration

### 9.1 Feature Toggles

Each application's `config/` directory contains configuration files for feature customization:

```bash
apps/hid/config/
├── lib_btctrler_config.c    # Bluetooth controller config
├── lib_btstack_config.c     # Bluetooth stack config
├── lib_driver_config.c      # Driver module config
├── lib_media_config.c       # Media module config
├── lib_profile_config.c     # Bluetooth profile config
├── lib_system_config.c      # System module config
├── lib_update_config.c      # Update module config
└── log_config.c             # Log output config
```

### 9.2 Board Configuration

`board_xxx_cfg.h` contains:

- **Pin mapping**: UART / SPI / I2C / GPIO pin assignments
- **Peripheral enable**: Enable/disable specific peripheral modules
- **Clock configuration**: CPU frequency, peripheral clock sources

`board_xxx_global_build_cfg.h` contains:

- **Feature toggles**: Enable/disable specific features
- **Memory configuration**: Stack sizes, buffer pool sizes

---

## 10. FAQ

### 10.1 Development Workflow

**Q: How do I create my own project?**
A: Copy the board directory closest to your chip model under `apps/*/board/`, then modify `board_xxx_cfg.h` for pin and peripheral configuration.

**Q: How do I add support for a new chip?**
A: Create a new platform directory under `cpu/` with `liba/` libraries and `tools/` flashing utilities, then add the corresponding board directory under `apps/*/board/`.

### 10.2 Build Issues

**Q: Windows build fails with `make: command not found`?**
A: Use `tools/make_prompt.bat` to launch the pre-configured build shell. It sets up all environment variables and `make` paths automatically.

**Q: How do I speed up the build?**
A: Use the `-j` flag for parallel builds, e.g., `make ac632n_spp_and_le -j4`.

### 10.3 Debugging Tips

- **Serial logs**: Configure log level and output channel via `log_config.c`
- **GPIO debug**: Use unused GPIO pins to output debug waveforms for timing analysis
- **More help**: See the [FAQ documentation](./doc/FAQ/)

---

## 11. Community & Support

### Technical Discussion

| Platform | ID/Link | Status |
|----------|---------|--------|
| **DingTalk Group 1** | `31691148` | ❌ Full |
| **DingTalk Group 2** | `3375034077` | ❌ Full |
| **DingTalk Group 3** | `107855006323` | ✅ Open |

### Resources

| Resource | Link |
|----------|------|
| 📖 **Online Documentation** | [doc.zh-jieli.com/AC63](https://doc.zh-jieli.com/AC63/zh-cn/master/index.html) |
| 📄 **SoC Datasheet** | [Online Summary](https://doc.zh-jieli.com/vue/#/docs/ac63) / [Local Download](./doc/datasheet) |
| 📚 **SDK Release History** | [Version History](https://doc.zh-jieli.com/AC63/zh-cn/master/other/version/index.html) |
| 🏗️ **SDK Architecture** | [Module Architecture](./doc/architure) |
| 🛒 **Dev Board Purchase** | [Jieli Official Store](https://shop321455197.taobao.com/) |
| 🐛 **Issue Tracker** | [Gitee Issues](https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK/issues) |

---

## 12. Certifications

The Bluetooth protocol stack in this SDK is certified by the Bluetooth SIG:

| Bluetooth Core | QDID | Certification Link |
|---------------|------|-------------------|
| Core v5.4 | QDID 222830 | [View Details](https://launchstudio.bluetooth.com/ListingDetails/193923) |

---

## 13. Disclaimer

`fw-AC63_BT_SDK` supports development with the AC63 series of SoCs. The AC63 series supports common Bluetooth applications and can be used for development, evaluation, prototyping, and mass production. Refer to the [Tags](https://github.com/Jieli-Tech/fw-AC63_BT_SDK/tags) and [Releases](https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK/tags) for corresponding SDK versions.

---

<div align="center">
  <sub>Copyright © 2024-2026 Zhuhai Jieli Technology Co., Ltd. All rights reserved.</sub>
</div>