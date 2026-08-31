param(
    [Parameter(Mandatory = $true)]
    [ValidateLength(1, 30)]
    [string]$Name
)

$sdk = "D:\23178\JL\sdk\fw-AC63_BT_SDK"
$cfg = Join-Path $sdk "apps\spp_and_le\modules\user_cfg.c"

if ($Name -notmatch '^[A-Za-z0-9_\-]+$') {
    throw "Use only letters, numbers, underscore, or dash. Example: MY_JL_BT"
}

$text = Get-Content -LiteralPath $cfg -Raw
$newText = $text -replace '\.edr_name\s*=\s*"[^"]*"', ".edr_name        = `"$Name`""
if ($newText -eq $text) {
    throw "Could not find .edr_name in $cfg"
}

Set-Content -LiteralPath $cfg -Value $newText -Encoding UTF8
Write-Host "Bluetooth name set to: $Name"

$env:Path = "$sdk\tools\utils;C:\JL\pi32\bin;C:\JL\mc\bin;$env:Path"
Push-Location $sdk
try {
    & "$sdk\tools\utils\make.exe" ac632n_spp_and_le
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}

Write-Host ""
Write-Host "Build complete."
Write-Host "Firmware:"
Write-Host "$sdk\cpu\bd19\tools\download\data_trans\update.ufw"
Write-Host "$sdk\cpu\bd19\tools\download\data_trans\jl_isd.fw"
