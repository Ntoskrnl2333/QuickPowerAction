# QuickPowerAction

[![Unit Tests](https://github.com/Ntoskrnl2333/QuickPowerAction/actions/workflows/unittest.yml/badge.svg?branch=develop)](https://github.com/Ntoskrnl2333/QuickPowerAction/actions/workflows/unittest.yml)
[![Nightly Build](https://github.com/Ntoskrnl2333/QuickPowerAction/actions/workflows/nightly.yml/badge.svg?branch=develop)](https://github.com/Ntoskrnl2333/QuickPowerAction/actions/workflows/nightly.yml)
[![Release](https://github.com/Ntoskrnl2333/QuickPowerAction/actions/workflows/release.yml/badge.svg)](https://github.com/Ntoskrnl2333/QuickPowerAction/actions/workflows/release.yml)

Windows 快捷电源操作工具。提供关机、重启、锁屏、注销、睡眠、休眠的快速执行入口。

## 功能

- **6 种电源操作**：关机、重启、锁屏、注销、睡眠、休眠
- **睡眠模式选择**：支持 S1（浅度睡眠）、S2（中度睡眠）、S3（深度睡眠）
- **强制执行**：强制关闭正在运行的应用程序
- **自动适配**：根据系统实际支持的电源操作启用/禁用对应按钮，不支持的操作自动置灰

## 系统要求

- 操作系统：Windows 7 / 8 / 10 / 11
- 运行时：无需额外运行时依赖（静态编译 MFC）
- 架构：x86（32 位） / x64（64 位）

> 某些操作（如关机、重启）需要管理员权限。建议以管理员身份运行。

## 构建

### 前置条件

- Visual Studio 2022
- 工作负载：**使用 C++ 的桌面开发**（含 MFC 组件）
- [ImageMagick](https://imagemagick.org/)（用于图标生成，`choco install imagemagick`）

### 构建步骤

```bash
# 生成图标资源
cd res/icon && build.bat

# 还原 NuGet 包
nuget restore QuickPowerAction.sln

# 构建 Debug
msbuild QuickPowerAction.sln /p:Configuration=Debug /p:Platform=x64 /m

# 构建 Release
msbuild QuickPowerAction.sln /p:Configuration=Release /p:Platform=x64 /m

# 运行单元测试（Test 配置）
msbuild QuickPowerAction.sln /p:Configuration=Test /p:Platform=x64 /m
.\x64\Test\QuickPowerAction.exe
```

### 构建配置

| 配置 | 用途 | 子系统 |
|------|------|--------|
| Debug | 调试构建 | Windows |
| Release | 发布构建 | Windows |
| Test | 单元测试（使用 [doctest](https://github.com/doctest/doctest)） | Console |

## 项目结构

```
QuickPowerAction/
├── .github/workflows/
│   ├── unittest.yml      # 单元测试 CI（push 到任意分支／PR）
│   ├── nightly.yml       # 每夜构建（push 到 develop）
│   └── release.yml       # 正式发布（push 到 master）
├── res/
│   ├── icon/             # 图标源文件（PNG + Krita）
│   │   ├── 16.png        # 各尺寸 PNG 源
│   │   ├── 48.png
│   │   ├── 256.png
│   │   ├── 16.kra        # Krita 工程文件（可编辑源）
│   │   └── build.bat     # 图标生成脚本（ImageMagick）
│   ├── .gitignore
│   ├── QuickPowerAction.ico  # 生成的应用图标
│   ├── QuickPowerAction.bmp  # 生成的关于对话框位图
│   └── QuickPowerAction.rc2  # 资源文件
├── QuickPowerAction.rc   # Windows 资源定义
├── CoreLayer.h/cpp       # 电源操作核心层
├── CoreLayer_test.cpp    # 核心层单元测试
├── QuickPowerActionDlg.h/cpp  # 主对话框
├── QuickPowerAction.sln  # Visual Studio 解决方案
└── QuickPowerAction.vcxproj
```

## 发布渠道

| 渠道 | 分支 | 更新频率 | 发布类型 |
|------|------|----------|----------|
| **Nightly** | `develop` | 每次推送 | Pre-release（含 Debug + Release .exe + .pdb） |
| **Release** | `master` | 手动合并 | 正式 Release（仅 Release .exe + .pdb） |

从 [Releases 页面](https://github.com/Ntoskrnl2333/QuickPowerAction/releases) 下载。

## 许可证

[Apache-2.0](https://www.apache.org/licenses/LICENSE-2.0)
