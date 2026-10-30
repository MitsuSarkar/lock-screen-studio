param(
    [Parameter(Mandatory = $true)]
    [string]$WindhawkDirectory
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$compiler = Join-Path $WindhawkDirectory 'Compiler/bin/clang++.exe'
$engineLibrary = Join-Path $WindhawkDirectory 'Engine/64/windhawk.lib'
if (!(Test-Path -LiteralPath $engineLibrary)) {
    $ini = Join-Path $WindhawkDirectory 'windhawk.ini'
    if (Test-Path -LiteralPath $ini) {
        $engineSetting = Get-Content -LiteralPath $ini | Where-Object { $_ -match '^EnginePath=' } | Select-Object -First 1
        if ($engineSetting) {
            $enginePath = $engineSetting.Substring('EnginePath='.Length)
            $engineLibrary = Join-Path (Join-Path $WindhawkDirectory $enginePath) '64/windhawk.lib'
        }
    }
}
if (!(Test-Path -LiteralPath $compiler) -or !(Test-Path -LiteralPath $engineLibrary)) {
    throw 'Expected Compiler/bin/clang++.exe and Engine/64/windhawk.lib under WindhawkDirectory. Use the Windhawk editor if your installation has a different layout.'
}
& (Join-Path $PSScriptRoot 'extend.ps1')
$build = Join-Path $root 'build'
New-Item -ItemType Directory -Force $build | Out-Null
$source = [IO.File]::ReadAllText((Join-Path $root 'lock-screen-studio.wh.cpp'))
$version = [regex]::Match($source, '(?m)^// @version\s+(\S+)').Groups[1].Value
if (!$version) { throw 'Missing version metadata' }
$arguments = @(
    '-std=c++23', '-O2', '-shared', '-DUNICODE', '-D_UNICODE',
    '-DWINVER=0x0A00', '-D_WIN32_WINNT=0x0A00', '-D_WIN32_IE=0x0A00',
    '-DNTDDI_VERSION=0x0A000008', '-D__USE_MINGW_ANSI_STDIO=0', '-DWH_MOD',
    '-DWH_MOD_ID=L"lock-screen-studio"', ('-DWH_MOD_VERSION=L"' + $version + '"'),
    $engineLibrary, '-x', 'c++', (Join-Path $root 'lock-screen-studio.wh.cpp'),
    '-include', 'windhawk_api.h', '-target', 'x86_64-w64-mingw32',
    '-Wl,--export-all-symbols', '-o', (Join-Path $build 'lock-screen-studio.dll'),
    '-lole32', '-loleaut32', '-lruntimeobject'
)
Push-Location (Join-Path $WindhawkDirectory 'Compiler')
try {
    Set-Content -LiteralPath (Join-Path $build 'compile.log') -Value ''
    & $compiler @arguments 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
    if ($LASTEXITCODE -ne 0) { throw "Compiler exited with code $LASTEXITCODE" }
    Add-Content -LiteralPath (Join-Path $build 'compile.log') -Value "Compiled Lock Screen Studio $version successfully (x64)."
} finally {
    Pop-Location
}
Write-Output 'Compiled successfully. Runtime validation in LockApp is still required.'
