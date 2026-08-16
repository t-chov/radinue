param(
    [Parameter(Mandatory = $true)]
    [string]$MpvRoot
)

$ErrorActionPreference = "Stop"
$dllPath = Join-Path $MpvRoot "libmpv-2.dll"
$definitionPath = Join-Path $MpvRoot "mpv.def"
$libraryDirectory = Join-Path $MpvRoot "lib"
$libraryPath = Join-Path $libraryDirectory "mpv.lib"

if (-not (Test-Path $dllPath)) {
    throw "libmpv DLL not found at $dllPath"
}

New-Item -ItemType Directory -Force -Path $libraryDirectory | Out-Null

$exports = & dumpbin.exe /nologo /exports $dllPath |
    Select-String '^\s+[0-9]+\s+[0-9A-F]+\s+[0-9A-F]+\s+(\S+)\s*$' |
    ForEach-Object { $_.Matches[0].Groups[1].Value }

if ($exports.Count -eq 0) {
    throw "No exports were found in $dllPath"
}

@("LIBRARY libmpv-2.dll", "EXPORTS") + $exports |
    Set-Content -Path $definitionPath -Encoding ascii

& lib.exe /nologo "/def:$definitionPath" "/out:$libraryPath" /machine:x64
if ($LASTEXITCODE -ne 0) {
    throw "lib.exe failed with exit code $LASTEXITCODE"
}
