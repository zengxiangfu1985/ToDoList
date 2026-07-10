# Build portable package and publish to GitHub Releases.
param(
    [Parameter(Mandatory = $true)]
    [string]$Tag,
    [string]$ReleaseNotes = "",
    [string]$NotesFile = "",
    [switch]$Draft,
    [switch]$Prerelease,
    [switch]$SkipPackage,
    [switch]$SkipUpload,
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string]$ConfigPath = (Join-Path $PSScriptRoot "github-release.config.json")
)

$ErrorActionPreference = "Stop"

function Read-JsonFile($Path) {
    if (-not (Test-Path $Path)) {
        throw "Config not found: $Path"
    }
    return Get-Content $Path -Raw | ConvertFrom-Json
}

function Ensure-GhCli {
    $gh = Get-Command gh -ErrorAction SilentlyContinue
    if (-not $gh) {
        throw @"
GitHub CLI (gh) is required but not found in PATH.
Install: https://cli.github.com/
Then run: gh auth login
"@
    }
    & gh auth status 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        throw "GitHub CLI is not authenticated. Run: gh auth login"
    }
}

function Normalize-Tag([string]$Value) {
    if ($Value -match '^v') { return $Value }
    return "v$Value"
}

$config = Read-JsonFile $ConfigPath
$owner = [string]$config.owner
$repo = [string]$config.repo
$branch = [string]$config.default_branch
if (-not $owner -or $owner -eq "your-github-user") {
    throw "Set scripts/github-release.config.json owner/repo before publishing."
}

$tag = Normalize-Tag $Tag
$version = $tag.TrimStart('v')
$AppVersionFile = Join-Path $ProjectRoot "src\utils\app_version.cpp"
$versionContent = Get-Content $AppVersionFile -Raw
if ($versionContent -match 'kVersion\s*=\s*"([^"]+)"') {
    $appVersion = $Matches[1]
} else {
    $appVersion = $version
}
if ($appVersion -ne $version) {
    Write-Warning "Tag $tag differs from app version $appVersion in app_version.cpp"
}

if ($NotesFile) {
    if (-not (Test-Path $NotesFile)) { throw "Notes file not found: $NotesFile" }
    $ReleaseNotes = Get-Content $NotesFile -Raw
}
if (-not $ReleaseNotes) {
    $ReleaseNotes = "ToDoList $tag portable release."
}

$zipName = "ToDoList-Portable-$appVersion.zip"
$packageUrl = "https://github.com/$owner/$repo/releases/download/$tag/$zipName"
$updateJsonUrl = "https://raw.githubusercontent.com/$owner/$repo/$branch/dist/update.json"

Write-Host "Repository: $owner/$repo"
Write-Host "Tag:        $tag"
Write-Host "Package:    $packageUrl"
Write-Host "Update URL: $updateJsonUrl"

Ensure-GhCli

if (-not $SkipPackage) {
    $packageScript = Join-Path $PSScriptRoot "package-portable.ps1"
    & $packageScript `
        -ProjectRoot $ProjectRoot `
        -OutputName "ToDoList-Portable-$appVersion" `
        -ReleaseNotes $ReleaseNotes `
        -PackageUrl $packageUrl `
        -GitHubOwner $owner `
        -GitHubRepo $repo `
        -GitHubBranch $branch
    if ($LASTEXITCODE -ne 0) { throw "package-portable.ps1 failed" }
}

$zipPath = Join-Path $ProjectRoot "dist\$zipName"
$updateJsonPath = Join-Path $ProjectRoot "dist\update.json"
if (-not (Test-Path $zipPath)) { throw "Package zip not found: $zipPath" }
if (-not (Test-Path $updateJsonPath)) { throw "update.json not found: $updateJsonPath" }

$manifest = Get-Content $updateJsonPath -Raw | ConvertFrom-Json
$manifestSha = [string]$manifest.latest.package.sha256
$manifestSize = [int64]$manifest.latest.package.size
$zipSha = (Get-FileHash $zipPath -Algorithm SHA256).Hash.ToLower()
$zipSize = (Get-Item $zipPath).Length
if ($manifestSha -ne $zipSha -or $manifestSize -ne $zipSize) {
    throw @"
update.json does not match release zip.
  zip:      size=$zipSize sha256=$zipSha
  manifest: size=$manifestSize sha256=$manifestSha
Re-run package-portable.ps1 and push the regenerated dist/update.json.
"@
}

$isDraft = [bool]$Draft -or [bool]$config.draft
$isPrerelease = [bool]$Prerelease -or [bool]$config.prerelease

Write-Host "Creating GitHub release..."
$releaseArgs = @(
    "release", "create", $tag, $zipPath,
    "--repo", "$owner/$repo",
    "--title", "ToDoList $tag",
    "--notes", $ReleaseNotes
)
if ($isDraft) { $releaseArgs += "--draft" }
if ($isPrerelease) { $releaseArgs += "--prerelease" }

if (-not $SkipUpload) {
    & gh @releaseArgs
    if ($LASTEXITCODE -ne 0) { throw "gh release create failed" }
} else {
    Write-Host "SkipUpload set; release not created."
}

Write-Host ""
Write-Host "Published."
Write-Host "Release:    https://github.com/$owner/$repo/releases/tag/$tag"
Write-Host "Zip URL:    $packageUrl"
Write-Host "update.json: $updateJsonPath"
Write-Host ""
Write-Host "Next: commit and push dist/update.json so clients can check updates:"
Write-Host "  git add dist/update.json"
Write-Host "  git commit -m ""chore: publish update manifest for $tag"""
Write-Host "  git push origin $branch"
Write-Host ""
Write-Host "Purge jsDelivr cache (required after pushing update.json):"
$jsDelivrPurgeUrl = "https://purge.jsdelivr.net/gh/$owner/$repo@$branch/dist/update.json"
Write-Host "  $jsDelivrPurgeUrl"
try {
    Invoke-WebRequest -Uri $jsDelivrPurgeUrl -Method Get -UseBasicParsing -TimeoutSec 30 | Out-Null
    Write-Host "jsDelivr cache purged."
} catch {
    Write-Warning "jsDelivr purge failed (run manually in browser): $jsDelivrPurgeUrl"
}
