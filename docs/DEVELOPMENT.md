# Build notes

`lock-screen-studio.wh.cpp` is the standalone release source. `tools/extend.ps1` combines the preserved upstream engine, the `src` extensions, the settings schema and `docs/DESCRIPTION.md`. It also generates the repository README.

```powershell
./tools/extend.ps1
$env:TEMP = Join-Path $PWD 'build/temp'
$env:TMP = $env:TEMP
./tools/build.ps1 -WindhawkDirectory 'C:\Program Files\Windhawk'
./tools/test-weather-icons.ps1
```

The build helper regenerates using default publication URLs. If a different screenshot host/path is required, change the default in `tools/extend.ps1` as well.

Pixel glyph outlines are checked in; normal builds don't need Python or fontTools. `tools/generate-pixel-font.py` is used only to regenerate those outlines. `vendor/` retains the upstream source, MIT license, font and OFL notice. Generated source includes the required notices.

Runtime has been tested by the author on Windows 11 26H2 x64 with Windhawk 1.7.3. Automated compilation and placement/condition tests don't verify every Windows build or visual tree. See `TESTING.md` for individual development checks; earlier entries are historical development records.
