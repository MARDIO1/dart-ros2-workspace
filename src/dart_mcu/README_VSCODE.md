# VSCode 配置指南 for dart_mcu

## 前提条件

1. **安装 VSCode 扩展**：
   - CMake Tools (ms-vscode.cmake-tools)
   - C/C++ (ms-vscode.cpptools)

2. **工具链**：
   - CMake 3.28.1 或更高版本
   - ARM GCC 工具链 (已安装到 T:/SDK/STM32CubeCLT_1.20.0/GNU-tools-for-STM32/bin/)
   - ST-Link GDB Server (随 STM32CubeCLT 安装，或确保在 PATH 中)

## 配置说明

### 1. CMake 配置
项目已配置好 `.vscode/settings.json`，CMake 扩展会自动使用以下设置：
- 构建目录：`build/`
- 生成器：MinGW Makefiles
- 工具链：ARM GCC (arm-none-eabi-*)
- 构建类型：Debug

### 2. 构建任务
按 `Ctrl+Shift+P` 输入 `Tasks: Run Task` 可以选择以下任务：
- **CMake: Build** - 默认构建任务 (快捷键 `Ctrl+Shift+B`)
- **CMake: Configure** - 重新配置CMake
- **CMake: Clean** - 清理构建目录
- **CMake: Rebuild** - 清理并重新构建

### 3. 调试配置
按 `F5` 或点击调试侧边栏，选择以下配置：
- **ST-LINK Debug** - 由 VS Code 自动启动 ST-Link GDB Server 并连接调试

### 4. 解决 CMake 可执行文件错误

如果出现 "CMake 可执行文件错误"，请检查：

1. **CMake 路径配置**：
   - 打开 VSCode 设置 (`Ctrl+,`)
   - 搜索 "cmake.cmakePath"
   - 设置为 "cmake" 或完整路径如 "C:/Program Files/CMake/bin/cmake.exe"

2. **工具链路径**：
   - 确保 ARM GCC 工具链在系统 PATH 中
   - 或修改 `.vscode/settings.json` 中的 `cmake.configureSettings`

3. **重新加载窗口**：
   - 按 `Ctrl+Shift+P` 输入 `Developer: Reload Window`

## 构建步骤

1. 打开项目文件夹
2. 等待 CMake 扩展自动配置（右下角会有提示）
3. 选择构建套件：`GCC 13.3.1 arm-none-eabi`
4. 选择构建变体：`Debug`
5. 点击底部状态栏的构建按钮或按 `Ctrl+Shift+B`

## 调试步骤

1. 连接 ST-Link 调试器到 STM32
2. 按 `F5` 启动调试，VS Code 会自动拉起 ST-Link GDB Server
3. 使用调试控制台：继续、单步、断点等

## 常见问题

### Q: CMake 配置失败
A: 检查 CMake 和 ARM GCC 是否安装正确，路径是否在系统环境变量中。

### Q: 构建失败，找不到 arm-none-eabi-gcc
A: 确保工具链路径正确，或手动在 `.vscode/settings.json` 中设置完整路径。

### Q: 调试器无法连接
A: 检查 ST-Link 连接，确认 ST-Link 驱动和 STM32CubeProgrammer 可用，再检查是否有其他进程占用了 `61234` 端口。

### Q: VSCode 按钮不工作
A: 可能需要重新加载窗口或重新安装 CMake Tools 扩展。

## 项目结构

```
dart_mcu/
├── .vscode/           # VSCode 配置文件
│   ├── settings.json  # 工作区设置
│   ├── tasks.json     # 构建任务
│   └── launch.json    # 调试配置
├── build/             # 构建输出目录
├── Core/              # 核心代码
├── Drivers/           # 硬件驱动
├── Middlewares/       # 中间件 (FreeRTOS, micro-ROS)
├── USB_DEVICE/        # USB 设备代码
└── CMakeLists.txt     # CMake 构建配置
```

## 联系方式

如有问题，请参考项目文档或联系开发者。
