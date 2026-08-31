$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptDir "..\..")
$BuildLogs = Join-Path $RepoRoot "build_logs"
$ToolsDir = Join-Path $RepoRoot "cpu\bd19\tools"
$Timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
$LogPath = Join-Path $BuildLogs "build_only_$Timestamp.log"
$ManifestPath = Join-Path $BuildLogs "firmware_manifest_$Timestamp.txt"
$LatestManifest = Join-Path $BuildLogs "latest_firmware_manifest.txt"

New-Item -ItemType Directory -Force -Path $BuildLogs | Out-Null

function Write-Log {
    param([string]$Text)
    $Text | Tee-Object -FilePath $LogPath -Append
}

function Invoke-Logged {
    param(
        [string]$FilePath,
        [string[]]$Arguments,
        [string]$WorkingDirectory = $RepoRoot
    )
    Write-Log ("> {0} {1}" -f $FilePath, ($Arguments -join " "))
    Push-Location $WorkingDirectory
    try {
        $oldPreference = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        & $FilePath @Arguments 2>&1 | Tee-Object -FilePath $LogPath -Append
        $exit = $LASTEXITCODE
        $ErrorActionPreference = $oldPreference
        if ($exit -ne 0) {
            throw "Command failed with exit code $exit"
        }
    } finally {
        Pop-Location
    }
}

$make = Join-Path $RepoRoot "tools\utils\make.exe"
$clangBin = "C:\JL\pi32\bin"
$objdump = Join-Path $clangBin "llvm-objdump.exe"
$objcopy = Join-Path $clangBin "llvm-objcopy.exe"
$lz4 = Join-Path $ToolsDir "lz4_packet.exe"

foreach ($required in @($make, $objdump, $objcopy, $lz4)) {
    if (-not (Test-Path $required)) {
        throw "Required tool missing: $required"
    }
}

$env:OS = "Windows_NT"
$env:PATH = "$(Join-Path $RepoRoot 'tools\utils');$clangBin;C:\JL\mc\bin;$env:PATH"

$generatedPost = "../../../../build_logs/generated_download_$Timestamp.bat"
$elfRel = "../../../../cpu/bd19/tools/sdk.elf"

Write-Log "# WT9011DCL-BT50 build-only log"
Write-Log "Timestamp: $Timestamp"
Write-Log "Repo: $RepoRoot"
Write-Log "Rule: do not run download.bat or isd_download.exe"

Invoke-Logged -FilePath $make -Arguments @(
    "-C", "apps/spp_and_le/board/bd19",
    "-f", "Makefile",
    "POST_SCRIPT=$generatedPost",
    "pre_build",
    $elfRel
)

Push-Location $ToolsDir
try {
    $sdkElf = Join-Path $ToolsDir "sdk.elf"
    if (-not (Test-Path $sdkElf)) {
        throw "Missing linked ELF: $sdkElf"
    }

    Write-Log "> $objdump -D -address-mask=0x1ffffff -print-dbg sdk.elf > sdk.lst"
    & $objdump -D -address-mask=0x1ffffff -print-dbg sdk.elf > sdk.lst
    if ($LASTEXITCODE -ne 0) {
        throw "objdump failed with exit code $LASTEXITCODE"
    }

    $sections = @(
        @(".text", "text.bin"),
        @(".data", "data.bin"),
        @(".data_code", "data_code.bin"),
        @(".overlay_aec", "aec.bin"),
        @(".overlay_aac", "aac.bin"),
        @(".overlay_aptx", "aptx.bin"),
        @(".common", "common.bin")
    )
    foreach ($section in $sections) {
        Invoke-Logged -FilePath $objcopy -Arguments @("-O", "binary", "-j", $section[0], "sdk.elf", $section[1]) -WorkingDirectory $ToolsDir
    }

    $bankArgs = New-Object System.Collections.Generic.List[string]
    for ($i = 0; $i -le 20; $i++) {
        $bank = "bank$i.bin"
        Invoke-Logged -FilePath $objcopy -Arguments @("-O", "binary", "-j", ".overlay_bank$i", "sdk.elf", $bank) -WorkingDirectory $ToolsDir
        $bankArgs.Add($bank)
        $bankArgs.Add("0x0")
    }

    Invoke-Logged -FilePath $lz4 -Arguments (@("-dict", "text.bin", "-input", "common.bin", "0") + $bankArgs.ToArray() + @("-o", "bank.bin")) -WorkingDirectory $ToolsDir

    $copyCmd = "copy /b text.bin+data.bin+data_code.bin+aec.bin+aac.bin+bank.bin+aptx.bin app.bin"
    Write-Log "> cmd.exe /c $copyCmd"
    cmd.exe /c $copyCmd 2>&1 | Tee-Object -FilePath $LogPath -Append
    if ($LASTEXITCODE -ne 0) {
        throw "app.bin copy failed with exit code $LASTEXITCODE"
    }

    Remove-Item -Force -ErrorAction SilentlyContinue bank*.bin, common.bin, text.bin, data.bin, data_code.bin, aac.bin, aec.bin, aptx.bin
} finally {
    Pop-Location
}

$gitCommit = (& git -C $RepoRoot rev-parse HEAD).Trim()
$sdkElfPath = Join-Path $ToolsDir "sdk.elf"
$appBinPath = Join-Path $ToolsDir "app.bin"
$sdkHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $sdkElfPath).Hash
$appHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $appBinPath).Hash
$sdkItem = Get-Item -LiteralPath $sdkElfPath
$appItem = Get-Item -LiteralPath $appBinPath

$manifest = @(
    "BUILD_TIME=$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')",
    "GIT_COMMIT=$gitCommit",
    "BUILD_LOG=$LogPath",
    "SDK_ELF=$sdkElfPath",
    "SDK_ELF_SIZE=$($sdkItem.Length)",
    "SDK_ELF_SHA256=$sdkHash",
    "APP_BIN=$appBinPath",
    "APP_BIN_SIZE=$($appItem.Length)",
    "APP_BIN_SHA256=$appHash"
)
Set-Content -Path $ManifestPath -Value $manifest -Encoding ASCII
Set-Content -Path $LatestManifest -Value $manifest -Encoding ASCII

Write-Log "Manifest: $ManifestPath"
Write-Log "APP_BIN=$appBinPath"
Write-Log "APP_BIN_SIZE=$($appItem.Length)"
Write-Log "APP_BIN_SHA256=$appHash"
Write-Host "BUILD OK"
Write-Host "Manifest: $ManifestPath"
Write-Host "Firmware: $appBinPath"
Write-Host "SHA256: $appHash"
