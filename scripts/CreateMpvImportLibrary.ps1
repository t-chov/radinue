param(
    [Parameter(Mandatory = $true)]
    [string]$MpvRoot
)

$ErrorActionPreference = "Stop"
$dllPath = Join-Path $MpvRoot "libmpv-2.dll"
$definitionPath = Join-Path $MpvRoot "mpv.def"
$libraryDirectory = Join-Path $MpvRoot "lib"
$libraryPath = Join-Path $libraryDirectory "mpv.lib"

function Find-MsvcTool {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Name
    )

    $pathCommand = Get-Command "$Name.exe" -ErrorAction SilentlyContinue
    if ($null -ne $pathCommand) {
        return $pathCommand.Source
    }

    $vswherePath = Join-Path ${env:ProgramFiles(x86)} `
        "Microsoft Visual Studio/Installer/vswhere.exe"
    if (-not (Test-Path $vswherePath)) {
        throw "Could not find vswhere.exe or $Name.exe"
    }

    $installationPath = & $vswherePath -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath
    if ([string]::IsNullOrWhiteSpace($installationPath)) {
        throw "Could not find a Visual Studio installation with MSVC x64 tools"
    }

    $tools = Get-ChildItem `
        -Path (Join-Path $installationPath "VC/Tools/MSVC/*/bin/Hostx64/x64/$Name.exe") |
        Sort-Object LastWriteTime -Descending
    if ($tools.Count -eq 0) {
        throw "Could not find $Name.exe in the Visual Studio installation"
    }

    return $tools[0].FullName
}

if (-not (Test-Path $dllPath)) {
    throw "libmpv DLL not found at $dllPath"
}

New-Item -ItemType Directory -Force -Path $libraryDirectory | Out-Null

$dumpbinPath = Find-MsvcTool -Name "dumpbin"
$libPath = Find-MsvcTool -Name "lib"

$exports = & $dumpbinPath /nologo /exports $dllPath |
    Select-String '^\s+[0-9]+\s+[0-9A-F]+\s+[0-9A-F]+\s+(\S+)\s*$' |
    ForEach-Object { $_.Matches[0].Groups[1].Value }

if ($exports.Count -eq 0) {
    throw "No exports were found in $dllPath"
}

@("LIBRARY libmpv-2.dll", "EXPORTS") + $exports |
    Set-Content -Path $definitionPath -Encoding ascii

& $libPath /nologo "/def:$definitionPath" "/out:$libraryPath" /machine:x64
if ($LASTEXITCODE -ne 0) {
    throw "lib.exe failed with exit code $LASTEXITCODE"
}
