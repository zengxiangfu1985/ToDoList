# Package ToDoList portable green edition (Windows x64 / Qt 5.15 MinGW)
param(
    [string]$QtDir = $env:QT_DIR,
    [string]$MingwBin = $env:MINGW_BIN,
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string]$OutputName = "",
    [string]$ReleaseNotes = "ToDoList portable release.",
    [string]$PackageUrl = "",
    [string]$GitHubOwner = "",
    [string]$GitHubRepo = "",
    [string]$GitHubBranch = "main"
)

$ErrorActionPreference = "Stop"

if (-not $QtDir) { $QtDir = "D:\Qt\5.15.2\mingw81_64" }
if (-not $MingwBin) { $MingwBin = "D:\Qt\Tools\mingw810_64\bin" }

$QtBin = Join-Path $QtDir "bin"
$QtPlugins = Join-Path $QtDir "plugins"
$IconDir = Join-Path $ProjectRoot "resources\icons"
$OpenSslDir = Join-Path $ProjectRoot "third_party\openssl\win64"
$DistRoot = Join-Path $ProjectRoot "dist"
$BuildDir = Join-Path $ProjectRoot "build\release"
$UpdaterBuildDir = Join-Path $ProjectRoot "build\updater"
$Qmake = Join-Path $QtDir "bin\qmake.exe"
$AppVersionFile = Join-Path $ProjectRoot "src\utils\app_version.cpp"

function Read-AppVersionInfo {
    param([string]$Path)
    $content = Get-Content $Path -Raw
    if ($content -match 'kVersion\s*=\s*"([^"]+)"') { $version = $Matches[1] } else { $version = "1.0.0" }
    if ($content -match 'kBuildNumber\s*=\s*(\d+)') { $build = [int]$Matches[1] } else { $build = 0 }
    return @{ Version = $version; Build = $build }
}

$VersionInfo = Read-AppVersionInfo $AppVersionFile

if (-not $GitHubOwner -or -not $GitHubRepo) {
    $ghConfigPath = Join-Path $PSScriptRoot "github-release.config.json"
    if (Test-Path $ghConfigPath) {
        $ghConfig = Get-Content $ghConfigPath -Raw | ConvertFrom-Json
        if (-not $GitHubOwner) { $GitHubOwner = [string]$ghConfig.owner }
        if (-not $GitHubRepo) { $GitHubRepo = [string]$ghConfig.repo }
        if ($ghConfig.default_branch) { $GitHubBranch = [string]$ghConfig.default_branch }
    }
}

$updateJsonUrl = ""
$jsDelivrJsonUrl = ""
$gitCommit = ""
if ($GitHubOwner -and $GitHubRepo -and $GitHubOwner -ne "your-github-user") {
    $updateJsonUrl = "https://raw.githubusercontent.com/$GitHubOwner/$GitHubRepo/$GitHubBranch/dist/update.json"
    Push-Location $ProjectRoot
    try {
        $gitCommit = (git rev-parse HEAD 2>$null)
    } finally {
        Pop-Location
    }
    if ($gitCommit) {
        $jsDelivrJsonUrl = "https://cdn.jsdelivr.net/gh/$GitHubOwner/$GitHubRepo@$gitCommit/dist/update.json"
    } else {
        $jsDelivrJsonUrl = "https://cdn.jsdelivr.net/gh/$GitHubOwner/$GitHubRepo@$GitHubBranch/dist/update.json"
    }
    if (-not $PackageUrl) {
        $PackageUrl = "https://github.com/$GitHubOwner/$GitHubRepo/releases/download/v$($VersionInfo.Version)/ToDoList-Portable-$($VersionInfo.Version).zip"
    }
}

$ExeCandidates = @(
    (Join-Path $ProjectRoot "build\release\release\ToDoList.exe"),
    (Join-Path $ProjectRoot "build\Desktop_Qt_5_15_2_MinGW_64_bit-Release\release\ToDoList.exe"),
    (Join-Path $ProjectRoot "build\release\ToDoList.exe")
)

$UpdaterCandidates = @(
    (Join-Path $ProjectRoot "build\updater\release\ToDoListUpdater.exe"),
    (Join-Path $ProjectRoot "build\updater\ToDoListUpdater.exe")
)

Write-Host "[0/10] Regenerate icon assets"
$GenerateScript = Join-Path $PSScriptRoot "generate-app-icon.py"
if (Test-Path $GenerateScript) {
    python $GenerateScript
}

Write-Host "[1/10] Rebuild main executable"
if (Test-Path $Qmake) {
    Push-Location $BuildDir
    try {
        & $Qmake (Join-Path $ProjectRoot "ToDoList.pro") -spec win32-g++ CONFIG+=release
        $env:PATH = "$MingwBin;$QtBin;" + $env:PATH
        mingw32-make -j4 release
        if ($LASTEXITCODE -ne 0) { throw "Main build failed with exit code $LASTEXITCODE" }
    } finally {
        Pop-Location
    }
}

Write-Host "[2/10] Rebuild updater executable"
if (Test-Path $Qmake) {
    New-Item -ItemType Directory -Path $UpdaterBuildDir -Force | Out-Null
    Push-Location $UpdaterBuildDir
    try {
        & $Qmake (Join-Path $ProjectRoot "tools\updater\ToDoListUpdater.pro") -spec win32-g++ CONFIG+=release
        $env:PATH = "$MingwBin;$QtBin;" + $env:PATH
        mingw32-make -j4 release
        if ($LASTEXITCODE -ne 0) { throw "Updater build failed with exit code $LASTEXITCODE" }
    } finally {
        Pop-Location
    }
}

$BuildExe = $null
$NewestTime = [datetime]::MinValue
foreach ($candidate in $ExeCandidates) {
    if (Test-Path $candidate) {
        $time = (Get-Item $candidate).LastWriteTime
        if ($time -gt $NewestTime) {
            $NewestTime = $time
            $BuildExe = $candidate
        }
    }
}

$UpdaterExe = $null
$NewestUpdaterTime = [datetime]::MinValue
foreach ($candidate in $UpdaterCandidates) {
    if (Test-Path $candidate) {
        $time = (Get-Item $candidate).LastWriteTime
        if ($time -gt $NewestUpdaterTime) {
            $NewestUpdaterTime = $time
            $UpdaterExe = $candidate
        }
    }
}

if (-not $BuildExe) { throw "Release executable not found. Build with mingw32-make first." }
if (-not $UpdaterExe) { throw "ToDoListUpdater.exe not found. Build updater first." }

Write-Host "Using executable: $BuildExe"
Write-Host "Using updater:    $UpdaterExe"

if (-not $OutputName) {
    $OutputName = "ToDoList-Portable-$($VersionInfo.Version)"
}

$OutDir = Join-Path $DistRoot $OutputName
if (Test-Path $OutDir) { Remove-Item $OutDir -Recurse -Force }
New-Item -ItemType Directory -Path $OutDir -Force | Out-Null

function Copy-IfExists($src, $dest) {
    if (Test-Path $src) {
        Copy-Item $src $dest -Force
        return $true
    }
    return $false
}

function Copy-Plugin($category, $name) {
    $src = Join-Path $QtPlugins (Join-Path $category $name)
    $destDir = Join-Path $OutDir $category
    New-Item -ItemType Directory -Path $destDir -Force | Out-Null
    if (-not (Copy-IfExists $src (Join-Path $destDir $name))) {
        Write-Warning "Missing plugin: $category\$name"
    }
}

Write-Host "[3/10] Copy executables"
Copy-Item $BuildExe (Join-Path $OutDir "ToDoList.exe")
Copy-Item $UpdaterExe (Join-Path $OutDir "ToDoListUpdater.exe")

Write-Host "[4/10] Copy icon files"
Copy-IfExists (Join-Path $IconDir "app.png") (Join-Path $OutDir "app.png") | Out-Null
Copy-IfExists (Join-Path $IconDir "app.ico") (Join-Path $OutDir "app.ico") | Out-Null

Write-Host "[5/10] Copy Qt runtime DLLs"
$qtDlls = @("Qt5Core.dll", "Qt5Gui.dll", "Qt5Network.dll", "Qt5Sql.dll", "Qt5Widgets.dll", "Qt5Svg.dll", "Qt5WinExtras.dll")
foreach ($dll in $qtDlls) {
    Copy-IfExists (Join-Path $QtBin $dll) $OutDir | Out-Null
}

Write-Host "[6/10] Copy MinGW runtime DLLs"
$mingwDlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")
foreach ($dll in $mingwDlls) {
    Copy-IfExists (Join-Path $MingwBin $dll) $OutDir | Out-Null
}

Write-Host "[7/10] Copy OpenSSL and plugins"
Copy-IfExists (Join-Path $OpenSslDir "libssl-1_1-x64.dll") $OutDir | Out-Null
Copy-IfExists (Join-Path $OpenSslDir "libcrypto-1_1-x64.dll") $OutDir | Out-Null
Copy-Plugin "platforms" "qwindows.dll"
Copy-Plugin "sqldrivers" "qsqlite.dll"
Copy-Plugin "styles" "qwindowsvistastyle.dll"
Copy-Plugin "bearer" "qgenericbearer.dll"
Copy-Plugin "imageformats" "qico.dll"
Copy-Plugin "imageformats" "qsvg.dll"
Copy-Plugin "imageformats" "qgif.dll"
Copy-Plugin "imageformats" "qjpeg.dll"

Write-Host "[8/10] Create data dir and helper files"
New-Item -ItemType Directory -Path (Join-Path $OutDir "data") -Force | Out-Null
$CopyHelperScript = Join-Path $PSScriptRoot "copy-portable-helper-files.py"
if (-not (Test-Path $CopyHelperScript)) { throw "Missing helper copy script: $CopyHelperScript" }
python $CopyHelperScript $OutDir
if ($LASTEXITCODE -ne 0) { throw "Failed to create portable helper files" }

$updateConfigTemplate = Join-Path $ProjectRoot "resources\update-config.default.json"
$updateConfigOut = Join-Path $OutDir "update-config.json"
if (Test-Path $updateConfigTemplate) {
    Copy-Item $updateConfigTemplate $updateConfigOut -Force
    if ($updateJsonUrl -and $jsDelivrJsonUrl) {
        $updateConfigObj = Get-Content $updateConfigOut -Raw | ConvertFrom-Json
        foreach ($source in $updateConfigObj.sources) {
            if ($source.name -eq "jsDelivr") {
                $source.url = $jsDelivrJsonUrl
            } elseif ($source.name -eq "GitHub") {
                $source.url = $updateJsonUrl
            }
        }
        $updateConfigObj.active_source = "GitHub"
        $updateConfigObj | ConvertTo-Json -Depth 6 | Set-Content $updateConfigOut -Encoding UTF8
    }
}

$usageEndpointTemplate = Join-Path $ProjectRoot "resources\usage-endpoint.default.json"
$usageEndpointOut = Join-Path $OutDir "usage-endpoint.json"
if (Test-Path $usageEndpointTemplate) {
    Copy-Item $usageEndpointTemplate $usageEndpointOut -Force
}
$usageEndpointDist = Join-Path $ProjectRoot "dist\usage-endpoint.json"
if (Test-Path $usageEndpointDist) {
    Copy-Item $usageEndpointDist $usageEndpointOut -Force
}

Write-Host "[9/10] Generate update manifests"
$manifestFiles = Get-ChildItem $OutDir -Recurse -File | ForEach-Object {
    $_.FullName.Substring($OutDir.Length + 1).Replace("\", "/")
} | Where-Object {
    $_ -notlike "data/*" -and $_ -ne "update.manifest.json" -and $_ -ne "file-manifest.txt"
} | Sort-Object

$manifestFiles | Set-Content (Join-Path $OutDir "file-manifest.txt") -Encoding UTF8

$zipName = "$OutputName.zip"
$zipPath = Join-Path $DistRoot $zipName

$packageManifest = [ordered]@{
    version = $VersionInfo.Version
    build = $VersionInfo.Build
    min_app_version = "1.0.0"
    release_notes = $ReleaseNotes
    mandatory = $false
    db_schema = 1
}
$packageManifestJson = ($packageManifest | ConvertTo-Json -Depth 5)
Set-Content (Join-Path $OutDir "update.manifest.json") $packageManifestJson -Encoding UTF8

if (Test-Path $zipPath) { Remove-Item $zipPath -Force }

Write-Host "[10/10] Create release zip"
Push-Location $OutDir
try {
    Compress-Archive -Path * -DestinationPath $zipPath -Force
} finally {
    Pop-Location
}
$zipSize = (Get-Item $zipPath).Length
$sha256 = (Get-FileHash $zipPath -Algorithm SHA256).Hash.ToLower()

$remotePackage = [ordered]@{
    file_name = $zipName
    size = $zipSize
    sha256 = $sha256
    url = $PackageUrl
}
# ghproxy 对大文件不稳定，不在清单中配置镜像；客户端优先 GitHub Releases 直链下载

$remoteLatest = [ordered]@{
    version = $VersionInfo.Version
    build = $VersionInfo.Build
    min_app_version = "1.0.0"
    release_notes = $ReleaseNotes
    mandatory = $false
    db_schema = 1
    package = $remotePackage
}
$remoteManifest = [ordered]@{
    schema = 1
    channel = "stable"
    latest = $remoteLatest
}
$remoteManifestJson = ($remoteManifest | ConvertTo-Json -Depth 6)
$updateJsonOut = Join-Path $DistRoot "update.json"
[System.IO.File]::WriteAllText($updateJsonOut, $remoteManifestJson, [System.Text.UTF8Encoding]::new($false))

Write-Host ""
Write-Host "Done."
Write-Host "Folder:  $OutDir"
Write-Host "Zip:     $zipPath"
Write-Host "Version: $($VersionInfo.Version) (build $($VersionInfo.Build))"
Write-Host "SHA256:  $sha256"
if ($updateJsonUrl) {
    Write-Host "Update:  $updateJsonUrl"
}
Get-ChildItem $OutDir | Format-Table Name, Length -AutoSize
