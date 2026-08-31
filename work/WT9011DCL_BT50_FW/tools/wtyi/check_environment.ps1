param(
    [switch]$NoInstall
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptDir "..\..")
$ReportPath = Join-Path $ScriptDir "environment_report.md"
$VenvDir = Join-Path $RepoRoot ".venv"

function Find-FirstExisting {
    param([string[]]$Paths)
    foreach ($path in $Paths) {
        if ($path -and (Test-Path $path)) {
            return (Resolve-Path $path).Path
        }
    }
    return $null
}

function Find-CommandPath {
    param([string]$Name)
    $cmd = Get-Command $Name -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($cmd) {
        return $cmd.Source
    }
    return $null
}

function Run-Version {
    param([string]$Exe, [string[]]$ArgList)
    if (-not $Exe -or -not (Test-Path $Exe)) {
        return "missing"
    }
    try {
        $oldPreference = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        $output = & $Exe @ArgList 2>&1 | Select-Object -First 3
        return (($output | ForEach-Object { $_.ToString() }) -join " ").Trim()
    } catch {
        return "error: $($_.Exception.Message)"
    } finally {
        $ErrorActionPreference = $oldPreference
    }
}

function Test-PythonModule {
    param([string]$PythonExe, [string]$ModuleName)
    if (-not $PythonExe -or -not (Test-Path $PythonExe)) {
        return $false
    }
    try {
        $oldPreference = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        & $PythonExe -c "import $ModuleName" *> $null
        return ($LASTEXITCODE -eq 0)
    } catch {
        return $false
    } finally {
        $ErrorActionPreference = $oldPreference
    }
}

$codeblocks = Find-FirstExisting @(
    "D:\23178\JL\CodeBlocks\codeblocks.exe",
    "D:\23178\JL\tools\CodeBlocks\codeblocks.exe"
)
$clang = Find-FirstExisting @("C:\JL\pi32\bin\clang.exe")
$lto = Find-FirstExisting @("C:\JL\pi32\bin\q32s-lto-wrapper.exe")
$llvmAr = Find-FirstExisting @("C:\JL\pi32\bin\llvm-ar.exe")
$make = Find-FirstExisting @(
    (Join-Path $RepoRoot "tools\utils\make.exe"),
    "C:\JL\mc\bin\make.exe"
)
$python = Find-FirstExisting @(
    "$env:LOCALAPPDATA\Programs\Python\Python38\python.exe",
    "D:\23178\python.exe",
    (Find-CommandPath "python")
)
$pip = if ($python) { "$python -m pip" } else { $null }

$burnFiles = @(
    "cpu\bd19\tools\isd_download.exe",
    "cpu\bd19\tools\fw_add.exe",
    "cpu\bd19\tools\ufw_maker.exe",
    "cpu\bd19\tools\download\data_trans\download.bat",
    "cpu\bd19\tools\uboot.boot",
    "cpu\bd19\tools\bd19loader.bin"
) | ForEach-Object {
    $path = Join-Path $RepoRoot $_
    [pscustomobject]@{
        File = $_
        Exists = Test-Path $path
        Path = $path
    }
}

$pythonReady = $false
$pyserialOk = $false
$pyinstallerOk = $false
$venvPython = Join-Path $VenvDir "Scripts\python.exe"

if ($python) {
    $pyserialOk = Test-PythonModule $python "serial"
    $pyinstallerOk = Test-PythonModule $python "PyInstaller"

    if ((-not $pyserialOk -or -not $pyinstallerOk) -and -not $NoInstall) {
        if (-not (Test-Path $venvPython)) {
            & $python -m venv $VenvDir
        }
        & $venvPython -m pip install --upgrade "pip<25"
        & $venvPython -m pip install pyserial "pyinstaller<7"
        $python = $venvPython
        $pyserialOk = Test-PythonModule $python "serial"
        $pyinstallerOk = Test-PythonModule $python "PyInstaller"
    } elseif (Test-Path $venvPython) {
        $python = $venvPython
        $pyserialOk = Test-PythonModule $python "serial"
        $pyinstallerOk = Test-PythonModule $python "PyInstaller"
    }
    $pythonReady = $pyserialOk -and $pyinstallerOk
}

$tempPathItems = @()
foreach ($path in @(
    (Split-Path $make -Parent),
    "C:\JL\pi32\bin",
    "C:\JL\mc\bin",
    (Split-Path $python -Parent)
)) {
    if ($path -and (Test-Path $path) -and ($tempPathItems -notcontains $path)) {
        $tempPathItems += $path
    }
}
$tempPath = ($tempPathItems -join ";") + ";%PATH%"

$checks = @(
    [pscustomobject]@{ Item = "Code::Blocks"; Path = $codeblocks; Version = if ($codeblocks) { "present" } else { "missing" } },
    [pscustomobject]@{ Item = "JieLi clang"; Path = $clang; Version = if ($clang) { "present" } else { "missing" } },
    [pscustomobject]@{ Item = "q32s-lto-wrapper"; Path = $lto; Version = if ($lto) { "present" } else { "missing" } },
    [pscustomobject]@{ Item = "llvm-ar"; Path = $llvmAr; Version = if ($llvmAr) { "present" } else { "missing" } },
    [pscustomobject]@{ Item = "make"; Path = $make; Version = if ($make) { "present" } else { "missing" } },
    [pscustomobject]@{ Item = "Python"; Path = $python; Version = Run-Version -Exe $python -ArgList @("--version") },
    [pscustomobject]@{ Item = "pip"; Path = $pip; Version = if ($python) { Run-Version -Exe $python -ArgList @("-m", "pip", "--version") } else { "missing" } },
    [pscustomobject]@{ Item = "pyserial"; Path = $python; Version = if ($pyserialOk) { "present" } else { "missing" } },
    [pscustomobject]@{ Item = "PyInstaller"; Path = $python; Version = if ($pyinstallerOk) { Run-Version -Exe $python -ArgList @("-m", "PyInstaller", "--version") } else { "missing" } }
)

$missing = @()
foreach ($check in $checks) {
    if (-not $check.Path -or $check.Version -eq "missing") {
        $missing += $check.Item
    }
}
foreach ($burn in $burnFiles) {
    if (-not $burn.Exists) {
        $missing += $burn.File
    }
}

$lines = New-Object System.Collections.Generic.List[string]
$lines.Add("# WT9011DCL-BT50 Environment Report")
$lines.Add("")
$lines.Add("Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')")
$lines.Add("")
$lines.Add("## Tool Checks")
$lines.Add("")
$lines.Add("| Item | Status | Path | Version |")
$lines.Add("| --- | --- | --- | --- |")
foreach ($check in $checks) {
    $status = if ($check.Path -and $check.Version -ne "missing") { "OK" } else { "MISSING" }
    $version = $check.Version -replace '\|','/'
    $lines.Add(('| {0} | {1} | `{2}` | {3} |' -f $check.Item, $status, $check.Path, $version))
}
$lines.Add("")
$lines.Add("## Burn Tool File Presence")
$lines.Add("")
$lines.Add("| File | Status | Path |")
$lines.Add("| --- | --- | --- |")
foreach ($burn in $burnFiles) {
    $status = if ($burn.Exists) { "OK" } else { "MISSING" }
    $lines.Add(('| `{0}` | {1} | `{2}` |' -f $burn.File, $status, $burn.Path))
}
$lines.Add("")
$lines.Add("## Temporary PATH")
$lines.Add("")
$lines.Add("This is not written to the system environment.")
$lines.Add("")
$lines.Add('```bat')
$lines.Add("set PATH=$tempPath")
$lines.Add('```')
$lines.Add("")
$lines.Add("## Python Environment")
$lines.Add("")
$lines.Add(('- Venv path: `{0}`' -f $VenvDir))
$lines.Add("- Python dependencies ready: $pythonReady")
$lines.Add("")
$lines.Add("## Result")
$lines.Add("")
if ($missing.Count -eq 0) {
    $lines.Add("Environment check passed.")
} else {
    $lines.Add("Environment check has missing items:")
    foreach ($item in $missing) {
        $lines.Add("- $item")
    }
}

Set-Content -Path $ReportPath -Value $lines -Encoding UTF8

Write-Host "Environment report: $ReportPath"
if ($missing.Count -gt 0) {
    Write-Host "Missing: $($missing -join ', ')"
    exit 1
}
exit 0
