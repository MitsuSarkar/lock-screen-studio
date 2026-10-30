$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$source = Get-Content (Join-Path $root 'src/weather-icons.inc') -Raw
$start = $source.IndexOf('std::wstring StudioWeatherCondition(')
$end = $source.IndexOf('wux::UIElement BuildStudioWeatherIcon(')
$harness = @'
#include <string>
#include <vector>
#include <cwctype>
#include <cassert>
#include <iostream>
struct StudioWidgetSnapshot { std::vector<std::wstring> lines,conditionLabels; };
struct { std::wstring weatherIconCondition=L"auto"; } studio;
'@
$harness += $source.Substring($start,$end-$start)
$harness += @'
int main() {
    auto check=[](std::wstring label,std::wstring expected) {
        StudioWidgetSnapshot s; s.lines={L"Durg",L"22 °C"}; s.conditionLabels={label};
        assert(StudioWeatherCondition(s)==expected);
    };
    check(L"Sunny",L"sun"); check(L"Light rain",L"rain");
    check(L"Partly cloudy",L"partly"); check(L"Partly cloudy night",L"partlyNight");
    check(L"Thunderstorms with rain",L"storm"); check(L"Snow showers",L"snow");
    check(L"Freezing rain",L"sleet"); check(L"Clear night",L"night");
    check(L"Clear",L"sun"); check(L"Fog",L"fog"); check(L"Breezy",L"wind");
    check(L"Rain warning",L"unknown"); check(L"",L"unknown");
    StudioWidgetSnapshot s; s.lines={L"Light drizzle",L"22 °C"};
    assert(StudioWeatherCondition(s)==L"rain");
    studio.weatherIconCondition=L"sun";
    assert(StudioWeatherCondition(s)==L"sun");
    std::cout << "15 weather condition and override cases passed.\n";
}
'@
$testSource = Join-Path $root 'build/test-weather-icons.cpp'
$testExe = Join-Path $root 'build/test-weather-icons.exe'
[IO.File]::WriteAllText($testSource,$harness)
& 'C:\Program Files\Windhawk\Compiler\bin\clang++.exe' -std=c++23 -static -target x86_64-w64-mingw32 $testSource -o $testExe
if ($LASTEXITCODE -ne 0) { throw 'Weather test compilation failed' }
& $testExe
if ($LASTEXITCODE -ne 0) { throw 'Weather condition test failed' }
