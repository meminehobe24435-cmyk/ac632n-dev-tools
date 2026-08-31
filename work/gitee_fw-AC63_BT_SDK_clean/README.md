[tag download]:https://github.com/Jieli-Tech/fw-AC63_BT_SDK/tags
[tag_badgen]:https://img.shields.io/github/v/tag/Jieli-Tech/fw-AC63_BT_SDK?style=plastic&logo=bluetooth&labelColor=ffffff&color=informational&label=Tag&logoColor=blue

# fw-AC63_BT_SDK  [![tag][tag_badgen]][tag download]

<div align="center">

**杰理 AC63 系列通用蓝牙 SDK 固件程序**

[English](./README-en.md) · [文档中心](https://doc.zh-jieli.com/AC63/zh-cn/master/index.html) · [SDK 版本历史](https://doc.zh-jieli.com/AC63/zh-cn/master/other/version/index.html) · [报告问题](https://github.com/Jieli-Tech/fw-AC63_BT_SDK/issues)

</div>

---

## 📋 目录

- [一、概述](#一概述)
- [二、支持的芯片与平台](#二支持的芯片与平台)
- [三、环境搭建](#三环境搭建)
- [四、快速开始](#四快速开始)
- [五、工程结构](#五工程结构)
- [六、应用选择指南](#六应用选择指南)
- [七、编译指南](#七编译指南)
- [八、烧录与升级](#八烧录与升级)
- [九、配置说明](#九配置说明)
- [十、常见问题](#十常见问题)
- [十一、社区与支持](#十一社区与支持)
- [十二、认证信息](#十二认证信息)
- [十三、免责声明](#十三免责声明)

---

## 一、概述

`fw-AC63_BT_SDK` 是杰理科技为 AC63 系列芯片提供的通用蓝牙 SDK 固件开发包。本 SDK 基于 Zephyr RTOS 实时操作系统，提供完整的蓝牙协议栈和丰富的应用示例，支持以下应用场景：

| 应用类型 | 典型产品 |
|---------|---------|
| **SPP + BLE 透传/数传** | 数据采集、智能设备、FindMy、Dongle |
| **HID 人机交互** | 蓝牙键盘、鼠标、遥控器、自拍器、游戏手柄 |
| **Bluetooth Mesh** | 智能照明、传感器网络、天猫精灵/涂鸦/腾讯连连接入 |

本仓库包含 SDK Release 版本代码及示例工程，需配合对应命名规则的库文件 (`lib.a`) 和子仓库进行编译。同时引用了 [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr) 等开源项目。

---

## 二、支持的芯片与平台

### 2.1 芯片系列

| 芯片平台 | 芯片型号 | 适用应用 |
|---------|---------|---------|
| **bd19** | AC6321A / AC6323A / AC6328A / AC6328B / AC6329B / AC6329C / AC6329E / AC6329F / AC632N | spp_and_le / hid / mesh |
| **br23** | AC6351D / AC635N | spp_and_le / hid / mesh |
| **br25** | AC6363F / AC6366C / AC6368A / AC6368B / AC6369C / AC6369F / AC636N | spp_and_le / hid / mesh |
| **br34** | AC6381A / AC6385A / AC638N | spp_and_le / hid / mesh |

### 2.2 蓝牙协议支持

| 蓝牙规范 | QDID | 状态 |
|---------|------|------|
| Core v5.4 | [QDID 222830](https://launchstudio.bluetooth.com/ListingDetails/193923) | ✅ 已认证 |

---

## 三、环境搭建

### 3.1 前提条件

| 系统 | 说明 |
|------|------|
| **Windows** | ✅ 推荐使用 Code::Blocks IDE 编译 |
| **Linux** | ✅ 支持 Makefile 命令行编译 |

### 3.2 安装编译工具链

1. 下载并安装 **杰理编译工具链**：[下载链接](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/dev_env/index.html)
2. Linux 用户可从此处下载：[pkgman.jieliapp.com](http://pkgman.jieliapp.com/doc/all)
   - 下载后解压到 `/opt/jieli` 目录
   - 确保 `/opt/jieli/common/bin/clang` 存在
3. 安装完成后，打开终端/命令提示符验证：

```bash
# 验证工具链是否安装成功
clang --version
```

### 3.3 安装烧录工具

| 工具 | 用途 | 获取方式 |
|------|------|---------|
| **USB 升级工具** | 将固件烧录到目标板 | [申请链接](https://item.taobao.com/item.htm?id=620295020803) · [使用文档](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/forced_upgrade/index.html) |
| **生产烧写工具** | 量产/裸片烧写 | [使用文档](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo2/index.html) |
| **无线测试盒** | 空中升级/射频标定/产品测试 | [申请链接](https://item.taobao.com/item.htm?id=620942507511) · [使用文档](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/testbox_1tuo2/index.html) |

---

## 四、快速开始

### 4.1 克隆仓库

```bash
git clone https://github.com/Jieli-Tech/fw-AC63_BT_SDK.git
cd fw-AC63_BT_SDK
```

### 4.2 选择合适的工程

根据你的产品需求选择应用工程：

```
SDK 根目录
├── apps/spp_and_le/    # SPP + BLE 透传/数传应用
├── apps/hid/           # HID 人机交互设备应用
└── apps/mesh/          # Bluetooth Mesh 物联应用
```

### 4.3 选择芯片型号和板级配置

每个应用目录下都有 `board/` 子目录，按芯片平台划分：

```
apps/hid/board/
├── bd19/   # AC632N 系列 (32个板级配置)
├── br23/   # AC635N 系列
├── br25/   # AC636N 系列
└── br34/   # AC638N 系列
```

每个板级目录下包含：
- `Makefile` - 编译脚本
- `board_*.cbp` - Code::Blocks 工程文件
- `board_xxx.c` - 板级初始化代码
- `board_xxx_cfg.h` - 板级配置（引脚、外设等）
- `board_xxx_global_build_cfg.h` - 全局编译配置（功能开关）

### 4.4 编译并烧录

**方式一：Code::Blocks（推荐 Windows 用户）**

```bash
# 1. 进入对应的板级目录
cd apps/hid/board/bd19/

# 2. 双击打开 .cbp 工程文件（如 AC632N_hid.cbp）
# 3. 在 Code::Blocks 中点击 Build → Build (Ctrl+F9)
# 4. 编译成功后，使用 USB 升级工具烧录生成的 .hex 文件
```

**方式二：Makefile 命令行**

```bash
# Windows 用户
双击 tools/make_prompt.bat 打开命令行环境

# Linux/macOS 用户
cd SDK 根目录

# 编译完整工程
make ac632n_spp_and_le

# 编译完成后，在对应 board 目录下找到生成的 .hex 文件
```

> **💡 提示**：所有支持的 target 名称见 [Makefile](https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK/blob/master/Makefile) 开头的注释。

**方式三：VS Code 编译**

仓库已预配置 VS Code 任务，按 `Ctrl+Shift+B` 即可选择编译目标。

---

## 五、工程结构

```
fw-AC63_BT_SDK/
├── apps/                          # 应用层代码
│   ├── common/                    # 公共模块（跨工程共享）
│   │   ├── audio/                 #   音频编解码、音量控制
│   │   ├── bt_common/             #   蓝牙通用接口
│   │   ├── cJSON/                 #   JSON 解析库
│   │   ├── debug/                 #   调试工具
│   │   ├── device/                #   外设驱动（按键、USB、传感器等）
│   │   ├── jl_kws/                #   杰理关键词唤醒
│   │   ├── music/                 #   音乐播放
│   │   ├── update/                #   固件升级
│   │   └── third_party_profile/   #   第三方协议（SigMesh、涂鸦、腾讯连连、HiLink）
│   ├── spp_and_le/                # 📌 SPP + BLE 应用
│   ├── hid/                       # 📌 HID 应用（键盘/鼠标/遥控器/游戏手柄）
│   └── mesh/                      # 📌 Mesh 应用
├── cpu/                           # CPU 相关代码与库文件
│   ├── bd19/ → br34/              #   各芯片平台的 lib.a 库文件 + 工具脚本
│   ├── .../
│   └── br34/
├── include_lib/                   # 头文件（bt协议栈、驱动、媒体、系统等）
├── doc/                           # 文档资源
│   ├── datasheet/                 #   芯片数据手册
│   ├── architure/                 #   SDK 架构文档
│   ├── FAQ/                      #   常见问题
│   └── .../
├── tools/                         # 编译工具与脚本
│   └── make_prompt.bat            #   Windows 编译命令行入口
├── Makefile                       # 顶层 Makefile（统一编译入口）
├── default.workspace              # Code::Blocks 工作空间
└── .vscode/                       # VS Code 配置（tasks.json 预定义编译任务）
```

### 5.1 关键目录说明

| 目录 | 作用 |
|------|------|
| `apps/*/board/` | **板级配置**：引脚定义、外设初始化、编译选项 |
| `apps/*/examples/` | **示例应用**：可直接参考或修改的参考实现 |
| `apps/*/include/` | **应用头文件**：模块接口定义 |
| `apps/*/config/` | **库配置**：各模块的裁剪配置（决定编译哪些库功能） |
| `cpu/*/liba/` | **预编译库**：`*.a` 静态库文件（btctrler、btstack、media 等） |
| `cpu/*/tools/` | **烧录工具**：download.bat、fw_add.exe、isd_download.exe 等 |

---

## 六、应用选择指南

### 6.1 SPP + BLE (`apps/spp_and_le/`)

| 项目 | 说明 |
|------|------|
| **适用场景** | 数据透传、扫码枪、蓝牙 Dongle、FindMy、信标、多机连接 |
| **关键特性** | SPP 经典蓝牙 + BLE 双模，支持 AT 指令控制 |
| **参考文档** | [SPP_LE 开发文档](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/spple/index.html) |

### 6.2 HID (`apps/hid/`)

| 项目 | 说明 |
|------|------|
| **适用场景** | 蓝牙键盘、鼠标、遥控器、自拍器、游戏手柄（吃鸡王座）、语音遥控器 |
| **示例应用** | `examples/mouse_single/` 鼠标 / `examples/keyboard/` 键盘 / `examples/gamebox/` 游戏手柄 / `examples/voice_remote_control/` 语音遥控 |
| **参考文档** | [HID 开发文档](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/hid/index.html) |

### 6.3 Mesh (`apps/mesh/`)

| 项目 | 说明 |
|------|------|
| **适用场景** | 智能照明、传感器网络、天猫精灵/涂鸦/腾讯连连接入 |
| **示例应用** | `generic_onoff_server` / `light_lightness_server` / `AliGenie_fan` / `TUYA_light` / `tencent_mesh` |
| **参考文档** | [Mesh 开发文档](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/mesh/index.html) |

### 6.4 即将推出

| 应用 | 说明 |
|------|------|
| **IoT (IPv6 / 6LoWPAN)** | 基于 IPv6 的物联网应用 |
| **2.4G 私有无线** | 厂商自定义 2.4G 无线协议 |

---

## 七、编译指南

### 7.1 编译命令速查表

以下命令在 SDK 根目录下执行：

| 目标 | 芯片 | 应用 | 命令 |
|------|------|------|------|
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
| **全部** | 全部 | 全部 | `make all` |
| **清理全部** | 全部 | 全部 | `make clean` |

### 7.2 清理单个工程

```bash
make clean_ac632n_spp_and_le    # 清理 ac632n_spp_and_le 编译产物
make clean_ac632n_hid           # 清理 ac632n_hid 编译产物
make clean_ac632n_mesh          # 清理 ac632n_mesh 编译产物
```

### 7.3 Linux 编译注意事项

```bash
# 1. 确保文件描述符限制足够大（链接阶段需要打开大量文件）
ulimit -n 8096

# 2. 进入 SDK 根目录执行编译
make ac632n_spp_and_le -j`nproc`
```

### 7.4 常见编译错误

| 错误提示 | 解决方法 |
|---------|---------|
| `clang: command not found` | 未安装杰理编译工具链，或环境变量未配置 |
| `Too many open files` | Linux 下执行 `ulimit -n 8096` 增加文件描述符限制 |
| `cannot find -lxxx` | 缺少对应的 `.a` 库文件，检查 `cpu/*/liba/` 目录 |
| `undefined reference to ...` | 功能裁剪配置未包含对应模块，检查 `lib_*_config.c` |

---

## 八、烧录与升级

### 8.1 首次烧录

1. **连接硬件**：将开发板通过 USB 或 UART 连接到 PC
2. **进入编程模式**：按住开发板上的烧录按键，然后复位或重新上电
3. **打开 USB 升级工具**：启动 `isd_download.exe`
4. **选择固件**：选择编译生成的 `.hex` 文件
5. **开始烧录**：点击下载按钮，等待烧录完成

> **⚠️ 注意**：烧录前请确保 USB 升级工具正确连接且目标板已进入编程模式。
> 详细的 INI (cpu\xx\tools\isd_config_rule.c)配置说明请参考：[下载脚本配置文档](https://doc.zh-jieli.com/AC63/zh-cn/master/getting_started/project_download/INI_config.html)以及[官网说明](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/common_function/index.html)

### 8.2 OTA 升级

支持单备份和双备份蓝牙 OTA 升级，详见：[OTA 开发文档](https://doc.zh-jieli.com/AC63/zh-cn/master/module_demo/ota/index.html)

---

## 九、配置说明

### 9.1 功能裁剪配置

在每个应用工程的 `config/` 目录下，通过以下配置文件可以灵活裁剪 SDK 功能，减小固件体积：

```bash
apps/hid/config/
├── lib_btctrler_config.c    # 蓝牙控制器配置
├── lib_btstack_config.c     # 蓝牙协议栈配置
├── lib_driver_config.c      # 驱动模块配置
├── lib_media_config.c       # 媒体模块配置
├── lib_profile_config.c     # 蓝牙 Profile 配置
├── lib_system_config.c      # 系统模块配置
├── lib_update_config.c      # 升级模块配置
└── log_config.c             # 日志输出配置
```

### 9.2 板级配置

每个板级目录下的 `board_xxx_cfg.h` 包含：

- **引脚映射**：UART / SPI / I2C / GPIO 等外设的引脚分配
- **外设使能**：开启或关闭特定外设模块
- **时钟配置**：CPU 频率、外设时钟源

`board_xxx_global_build_cfg.h` 包含：

- **功能开关**：按需启用/禁用特定功能
- **内存配置**：堆栈大小、缓冲池大小

---

## 十、常见问题

### 10.1 开发流程相关

**Q: 如何创建自己的工程？**
A: 复制 `apps/` 下对应应用的 `board/` 目录中与芯片型号最接近的板级配置，修改 `board_xxx_cfg.h` 中的引脚和外设配置即可。

**Q: 如何添加新的芯片型号支持？**
A: 在 `cpu/` 下创建对应的平台目录，提供 `liba/` 库文件和 `tools/` 烧录工具，然后在 `apps/*/board/` 下添加对应的板级目录。

### 10.2 编译相关

**Q: Windows 下编译报错 `make` 不是有效命令？**
A: 使用 `tools/make_prompt.bat` 进入预配置的命令行环境，该脚本已设置好所有环境变量和 `make` 的路径。

**Q: 如何加快编译速度？**
A: 使用 `-j` 参数进行并行编译，如 `make ac632n_spp_and_le -j4`。

### 10.3 调试技巧

- **串口日志**：通过 `log_config.c` 配置日志输出等级和通道
- **GPIO  Debug**：利用空闲 GPIO 输出调试波形，测量时序
- **更多问题**：请参考 [FAQ 文档](./doc/FAQ/)

---

## 十一、社区与支持

### 技术交流

| 平台 | 群号/链接 | 状态 |
|------|-----------|------|
| **钉钉 1群** | `31691148` | ❌ 已满 |
| **钉钉 2群** | `3375034077` | ❌ 已满 |
| **钉钉 3群** | `107855006323` | ✅ 可加入 |

### 资源链接

| 资源 | 链接 |
|------|------|
| 📖 **在线文档中心** | [doc.zh-jieli.com/AC63](https://doc.zh-jieli.com/AC63/zh-cn/master/index.html) |
| 📄 **芯片数据手册** | [SoC 数据手册扼要](https://doc.zh-jieli.com/vue/#/docs/ac63) / [本地下载](./doc/datasheet) |
| 📚 **SDK 版本历史** | [版本发布记录](https://doc.zh-jieli.com/AC63/zh-cn/master/other/version/index.html) |
| 🏗️ **SDK 架构文档** | [模块架构说明](./doc/architure) |
| 🛒 **开发板购买** | [杰理官方店铺](https://shop321455197.taobao.com/) |
| 🐛 **问题反馈** | [Gitee Issues](https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK/issues) |

---

## 十二、认证信息

本 SDK 支持的蓝牙协议栈已通过蓝牙 SIG 认证：

| 蓝牙规范 | QDID | 认证链接 |
|---------|------|---------|
| Core v5.4 | QDID 222830 | [查看认证详情](https://launchstudio.bluetooth.com/ListingDetails/193923) |

---

## 十三、免责声明

`fw-AC63_BT_SDK` 支持 AC63 系列芯片开发。AC63 系列芯片支持通用蓝牙常见应用，可作为开发、评估、样品及量产使用，对应 SDK 版本请见 [Tags](https://github.com/Jieli-Tech/fw-AC63_BT_SDK/tags) 和 [Releases](https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK/tags)。

---

<div align="center">
  <sub>Copyright © 2024-2026 珠海杰理科技股份有限公司. All rights reserved.</sub>
</div>