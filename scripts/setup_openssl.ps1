# Deploy OpenSSL 1.1.1 runtime DLLs for Qt HTTPS (Kimi / DeepSeek / M365)
$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$Dest = Join-Path $Root "third_party\openssl\win64"
$Installer = Join-Path $Dest "openssl_light.exe"
$InstallerUrl = "https://slproweb.com/download/Win64OpenSSL_Light-1_1_1w.exe"

New-Item -ItemType Directory -Force -Path $Dest | Out-Null

if (-not (Test-Path $Installer)) {
    Write-Host "Downloading OpenSSL 1.1.1w Light..."
    curl.exe -L -o $Installer $InstallerUrl
}

$InstallDir = Join-Path $Dest "install"
if (-not (Test-Path (Join-Path $InstallDir "libssl-1_1-x64.dll"))) {
    Write-Host "Installing OpenSSL to $InstallDir ..."
    if (Test-Path $InstallDir) { Remove-Item $InstallDir -Recurse -Force }
    Start-Process -FilePath $Installer -ArgumentList "/VERYSILENT","/SUPPRESSMSGBOXES","/CURRENTUSER","/DIR=$InstallDir" -Wait
}

$srcCrypto = Join-Path $InstallDir "libcrypto-1_1-x64.dll"
$srcSsl = Join-Path $InstallDir "libssl-1_1-x64.dll"
if (-not (Test-Path $srcSsl)) {
    $srcCrypto = Join-Path $InstallDir "bin\libcrypto-1_1-x64.dll"
    $srcSsl = Join-Path $InstallDir "bin\libssl-1_1-x64.dll"
}

if (-not (Test-Path $srcSsl)) {
    throw "OpenSSL DLLs not found after install. Run installer manually: $Installer"
}

Copy-Item $srcCrypto (Join-Path $Dest "libcrypto-1_1-x64.dll") -Force
Copy-Item $srcSsl (Join-Path $Dest "libssl-1_1-x64.dll") -Force

foreach ($buildDir in @("build\release", "build\debug", "build\Desktop_Qt_5_15_2_MinGW_64_bit-Debug\debug")) {
    $target = Join-Path $Root $buildDir
    if (Test-Path $target) {
        Copy-Item (Join-Path $Dest "libcrypto-1_1-x64.dll") $target -Force
        Copy-Item (Join-Path $Dest "libssl-1_1-x64.dll") $target -Force
        Write-Host "Copied to $target"
    }
}

Write-Host "OpenSSL deployment complete."
