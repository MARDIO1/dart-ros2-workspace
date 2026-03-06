# VSCode 配置脚本 for dart_mcu
# 运行此脚本可以快速配置VSCode开发环境

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  dart_mcu VSCode 配置脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# 检查必要的工具
Write-Host "`n[1/4] 检查必要的工具..." -ForegroundColor Yellow

$tools = @{
    "CMake"   = "cmake --version"
    "ARM GCC" = "arm-none-eabi-gcc --version"
    "Git"     = "git --version"
}

foreach ($tool in $tools.GetEnumerator()) {
    try {
        $null = Invoke-Expression $tool.Value 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Host "  ✓ $($tool.Key) 已安装" -ForegroundColor Green
        }
        else {
            throw "Command failed"
        }
    }
    catch {
        Write-Host "  ✗ $($tool.Key) 未找到" -ForegroundColor Red
        Write-Host "    请安装: $($tool.Key)" -ForegroundColor Yellow
    }
}

# 检查VSCode扩展
Write-Host "`n[2/4] 检查VSCode扩展..." -ForegroundColor Yellow

$extensions = @(
    "ms-vscode.cmake-tools",
    "ms-vscode.cpptools", 
    "marus25.cortex-debug"
)

if (Get-Command code -ErrorAction SilentlyContinue) {
    $installedExts = code --list-extensions
    foreach ($ext in $extensions) {
        if ($installedExts -match $ext) {
            Write-Host "  ✓ $ext 已安装" -ForegroundColor Green
        }
        else {
            Write-Host "  ✗ $ext 未安装" -ForegroundColor Red
            Write-Host "    运行: code --install-extension $ext" -ForegroundColor Yellow
        }
    }
}
else {
    Write-Host "  ℹ VSCode 命令行工具未找到" -ForegroundColor Yellow
    Write-Host "    请手动安装以下扩展:" -ForegroundColor Yellow
    foreach ($ext in $extensions) {
        Write-Host "    - $ext" -ForegroundColor Yellow
    }
}

# 检查配置文件
Write-Host "`n[3/4] 检查配置文件..." -ForegroundColor Yellow

$configFiles = @(
    ".vscode/settings.json",
    ".vscode/tasks.json", 
    ".vscode/launch.json",
    "README_VSCODE.md"
)

foreach ($file in $configFiles) {
    if (Test-Path $file) {
        Write-Host "  ✓ $file 已存在" -ForegroundColor Green
    }
    else {
        Write-Host "  ✗ $file 未找到" -ForegroundColor Red
    }
}

# 测试构建
Write-Host "`n[4/4] 测试构建..." -ForegroundColor Yellow

if (Test-Path "build") {
    Write-Host "  ℹ 构建目录已存在，清理并重新构建..." -ForegroundColor Yellow
    
    # 清理构建目录
    if (Test-Path "build/CMakeCache.txt") {
        try {
            Remove-Item "build/*" -Exclude "CMakeFiles" -Recurse -Force -ErrorAction SilentlyContinue
            Write-Host "  ✓ 清理构建目录" -ForegroundColor Green
        }
        catch {
            Write-Host "  ⚠ 清理构建目录失败" -ForegroundColor Yellow
        }
    }
}

# 配置CMake
Write-Host "  ℹ 配置CMake..." -ForegroundColor Yellow
try {
    cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
    Write-Host "  ✓ CMake配置成功" -ForegroundColor Green
}
catch {
    Write-Host "  ✗ CMake配置失败" -ForegroundColor Red
    exit 1
}

# 构建项目
Write-Host "  ℹ 构建项目..." -ForegroundColor Yellow
try {
    cmake --build build --config Debug -j4
    Write-Host "  ✓ 构建成功" -ForegroundColor Green
    
    # 检查生成的文件
    $outputFiles = Get-ChildItem "build/*.elf", "build/*.bin", "build/*.hex" -ErrorAction SilentlyContinue
    foreach ($file in $outputFiles) {
        Write-Host "    - $($file.Name) ($([math]::Round($file.Length/1KB, 2)) KB)" -ForegroundColor Gray
    }
}
catch {
    Write-Host "  ✗ 构建失败" -ForegroundColor Red
    exit 1
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "  配置完成！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan

Write-Host "`n下一步操作：" -ForegroundColor Yellow
Write-Host "1. 用VSCode打开当前文件夹" -ForegroundColor White
Write-Host "2. 等待CMake扩展自动配置" -ForegroundColor White
Write-Host "3. 选择工具链: GCC 13.3.1 arm-none-eabi" -ForegroundColor White
Write-Host "4. 选择构建变体: Debug" -ForegroundColor White
Write-Host "5. 按 Ctrl+Shift+B 构建项目" -ForegroundColor White
Write-Host "6. 按 F5 开始调试" -ForegroundColor White

Write-Host "`n详细说明请查看 README_VSCODE.md" -ForegroundColor Gray

# 询问是否打开VSCode
$openVSCode = Read-Host "`n是否用VSCode打开当前文件夹? (y/n)"
if ($openVSCode -eq 'y' -or $openVSCode -eq 'Y') {
    if (Get-Command code -ErrorAction SilentlyContinue) {
        code .
        Write-Host "VSCode已启动" -ForegroundColor Green
    }
    else {
        Write-Host "找不到VSCode命令行工具" -ForegroundColor Yellow
        Write-Host "请手动用VSCode打开当前文件夹" -ForegroundColor White
    }
}
