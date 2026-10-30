param(
    [string]$GithubUsername = 'MitsuSarkar',
    [string]$ScreenshotBaseUrl = 'https://raw.githubusercontent.com/MitsuSarkar/lock-screen-studio/main/docs/screenshots'
)
$ErrorActionPreference = 'Stop'
if($GithubUsername -notmatch '^[A-Za-z0-9-]+$') { throw 'Invalid GitHub username' }
if($ScreenshotBaseUrl -notmatch '^https://(raw\.githubusercontent\.com|i\.imgur\.com)/') { throw 'Windhawk screenshot URLs must use raw.githubusercontent.com or i.imgur.com' }
$root = Split-Path $PSScriptRoot -Parent
$s = [IO.File]::ReadAllText((Join-Path $root 'vendor/lock-screen-clock-styler.wh.cpp')).Replace("`r`n", "`n")
$start = $s.IndexOf('#include <windows.h>')
$s = $s.Substring($start)
$settings = @'
// ==WindhawkMod==
// @id              lock-screen-studio
// @name            Lock Screen Studio
// @description     Make your lock screen your own with clock, weather and media themes, custom fonts and flexible layouts.
// @version         0.0.1
// @author          Mitsu
// @github          https://github.com/{{GITHUB_USERNAME}}
// @license         MIT
// @include         LockApp.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
{{PUBLIC_DESCRIPTION}}
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- clockStyle: nord
  $name: Theme
  $options:
    - pixelCoffee: Pixel Coffee (bottom-left)
    - pixelDusk: Pixel Dusk (top-left)
    - pixelSky: Pixel Sky (top-right)
    - pixelNight: Pixel Night (two-tone)
    - pixelSakura: Pixel Sakura (light)
    - pixelRain: Pixel Rain (cool blue)
    - pixelHollow: Pixel Hollow (bottom strip)
    - pixelSunset: Pixel Sunset (warm shadow)
    - gnome: GNOME Minimal
    - kde: KDE Breeze
    - hyprland: Hyprland Terminal
    - nord: Nord
    - catppuccin: Catppuccin Mocha
    - gruvbox: Gruvbox
    - pixel: Original LED Matrix
    - flip: Original Modern Flip
    - vintage: Original Vintage Flip
    - macos: Original Sonoma
    - cyberpunk: Original Cyberpunk
    - aura: Original Aura
    - defaultOnly: Windows stock clock
- timeFormat: 24h
  $options:
    - 24h: 24 hour
    - 12h: 12 hour
- showSeconds: false
- clockFontSize: 140
- fontFamily: ""
  $name: Legacy shared font family
  $description: Shared fallback for typography themes. Prefer the independent clock/date font fields below.
- clockFontFamily: ""
  $name: Clock font family
  $description: Type an installed font family, e.g. Cascadia Code. Empty uses the theme default. Overrides the embedded font in the eight Pixel themes.
- dateFontFamily: ""
  $name: Date font family
  $description: Independent installed font family for the date. Empty uses the theme default.
- widgetFontFamily: ""
  $name: Weather and widget font family
  $description: Type an installed font family. Empty uses native/pixel theme text. Install custom fonts for all users so LockApp can read them.
- mediaFontFamily: ""
  $name: Media font family
  $description: Independent installed font family for song and artist labels. Icons retain their symbol fonts.
- fontWeight: 300
  $description: Studio clock weight, 100 to 900.
- foreground: ""
  $description: Studio clock color. Empty uses theme default.
- accent: ""
  $description: Studio date and text color. Empty uses theme default.
- panelColor: ""
  $description: Studio card background. Empty uses theme default.
- panelOpacity: 72
  $description: Studio card opacity, 0 to 100. GNOME starts without a card.
- cornerRadius: 24
- backgroundDim: 15
  $description: Studio wallpaper darkening, 0 to 90 percent.
- clockAnchor: theme
  $description: theme, topLeft, top, topRight, left, center, right, bottomLeft, bottom, bottomRight.
- clockX: 0
- clockY: 0
- dateAnchor: theme
- dateX: 0
- dateY: 0
- dateFontSize: 26
- dateFormat: "dddd, d MMMM"
  $description: Windows date format, e.g. ddd, dd MMM yyyy. Empty uses long date.
- pixelDateLocale: en-US
  $description: Locale for the eight Pixel themes. Use system for your Windows locale. Font includes Latin and Cyrillic; unsupported characters fall back to a system TextBlock.
- pixelDateTracking: 2
  $description: Extra spacing between pixel date characters, in DIPs, 0 to 12.
- pixelShadow: 2
  $description: Pixel clock and date shadow offset, in DIPs, 0 to 8.
- showDateCapsule: true
- showBatteryStatus: true
- batteryAnchor: bottom
- batteryX: 0
- batteryY: -60
- customText: ""
  $description: Optional static text widget; visible while locked.
- textAnchor: bottom
- textX: 0
- textY: -110
- textFontSize: 20
- hideDefaultWidgets: false
- widgetMode: native
  $name: Widget rendering mode
  $options:
    - native: Style Windows widgets (legacy movement)
    - studio: Studio cards (free positioning, mirrors Windows text)
  $description: Studio cards draw outside Windows' clipped widget container and hide its original visuals after data is available. Windows remains the data source; its buttons are not copied.
- widgetAnchor: bottom
  $name: Studio widget anchor
  $options:
    - topLeft: Top left
    - top: Top center
    - topRight: Top right
    - left: Center left
    - center: Center
    - right: Center right
    - bottomLeft: Bottom left
    - bottom: Bottom center
    - bottomRight: Bottom right
- widgetCardWidth: 320
  $description: Studio card width in DIPs, 180 to 600. The row scales down to fit smaller displays.
- weatherIconVisible: true
  $name: Show weather icon (turn off to remove)
- weatherIconCondition: auto
  $name: Weather icon condition
  $description: Auto reads Windows condition labels and accessibility descriptions. If Windows supplies neither, keeps its original image when available. Manual choices are overrides, not live forecasts.
  $options:
    - auto: Automatic from Windows
    - sun: Sunny
    - partly: Partly cloudy (day)
    - cloud: Cloudy
    - rain: Rain
    - storm: Thunderstorm
    - snow: Snow
    - sleet: Sleet
    - fog: Fog
    - wind: Wind
    - night: Clear night
    - partlyNight: Partly cloudy (night)
- weatherIconPosition: left
  $name: Weather icon placement inside Studio card
  $options:
    - left: Left of temperature and details
    - right: Right of temperature and details
    - topLeft: Top left
    - top: Top center
    - topRight: Top right
    - bottomLeft: Bottom left
    - bottom: Bottom center
    - bottomRight: Bottom right
- weatherIconX: 0
  $description: Horizontal icon offset in DIPs. Positive moves right. Large offsets can put the icon outside its card.
- weatherIconY: 0
  $description: Vertical icon offset in DIPs. Positive moves down.
- weatherIconSize: 72
  $description: Studio weather icon size in DIPs, 24 to 160. These icon options apply to Studio cards mode.
- widgetX: 0
  $description: Offset the native widget group when found; can move it off screen.
- widgetY: 0
- widgetTarget: auto
  $description: Auto detects WidgetGroupPanel and older containers. Override with an exact XAML name from diagnostics if Windows changes it.
- widgetTheme: macosDark
  $name: Weather and widget card theme
  $options:
    - stock: Windows default
    - pixel: Cozy Pixel
    - macos: Mac Light
    - macosDark: Mac Dark
    - smoke: Smoke
    - minimal: Minimal Outline
    - nord: Nord
    - catppuccin: Catppuccin
    - gruvbox: Gruvbox
- widgetBackground: ""
- widgetCustomStyle: false
  $name: Customize widget theme colors and surface
  $description: Off uses the selected theme's original palette, opacity, borders and corners. Turn on to apply your saved background, foreground, accent, opacity and corner radius. Fonts, position and icon settings work independently. Windows original restores the actual Windows card.
- widgetForeground: ""
- widgetAccent: ""
- widgetOpacity: 92
- widgetCornerRadius: 24
- widgetTextSize: 0
  $description: 0 keeps native text sizes and weather temperature hierarchy; otherwise 10 to 40 DIPs.
- widgetKeepOnScreen: true
  $description: Clamp the widget group within the screen instead of allowing it to disappear beyond an edge.
- widgetMoveContainer: outer
  $description: outer moves the whole widget-only viewport to avoid inner clipping; panel uses legacy direct movement.
  $options:
    - outer: Outer widget container (recommended)
    - panel: Inner widget panel (legacy)
- widgetScreenPadding: 16
  $description: Minimum space from screen edges, 0 to 100 DIPs.
- hideMedia: false
- mediaX: 0
- mediaY: 0
- mediaTheme: smoke
  $name: Media card theme
  $options:
    - stock: Windows default
    - smoke: Smoke card
    - minimal: Minimal outline
    - pixel: Cozy pixel card
    - macos: Mac Light
    - macosDark: Mac Dark
    - nord: Nord
    - catppuccin: Catppuccin
    - gruvbox: Gruvbox
- mediaBackground: ""
  $description: "Optional #RRGGBB or #AARRGGBB override."
- mediaForeground: ""
- mediaAccent: ""
- mediaOpacity: 88
  $description: Card background opacity, 0 to 100. Playback controls stay opaque.
- mediaCornerRadius: 18
  $description: Smoke/minimal card corners. Pixel theme always uses square corners.
- mediaTextSize: 16
  $description: Text size inside the native media card, 10 to 40.
- diagnostics: false
  $description: Log newly discovered visual tree names, including widgets loaded later, to identify build changes.
- elementRules:
  - - name: ""
    - x: 0
    - y: 0
    - opacity: 100
    - hide: false
    - fontSize: 0
    - color: ""
  $description: Advanced native element rules by exact XAML name. Leave first name empty to disable. Do not target the root or credential UI. Use diagnostics to discover names. fontSize/color affect TextBlocks only.
- hideDefaultClock: true
- clockAlignment: auto
  $description: Original designs only. auto, center, left, right, bottomLeft.
- verticalOffset: 0
  $description: Original designs only.
- glassOpacity: 45
  $description: Original Flip design only, 0 to 255.
*/
// ==/WindhawkModSettings==

// Derived from Lock Screen Clock Styler v0.5.0, Copyright (c) 2026 ali0zdas.
/*
MIT License
Copyright (c) 2026 ali0zdas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
'@
$description = [IO.File]::ReadAllText((Join-Path $root 'docs/DESCRIPTION.md')).Replace("`r`n", "`n")
$settings = $settings.Replace('{{PUBLIC_DESCRIPTION}}', $description.Replace('{{SCREENSHOT_BASE}}', $ScreenshotBaseUrl.TrimEnd('/'))).Replace('{{GITHUB_USERNAME}}', $GithubUsername)
$readme = $description.Replace('{{SCREENSHOT_BASE}}', 'docs/screenshots')
$readme += "`n`n---`n`nVersion **0.0.1** · By **Mitsu** · [Publishing guide](docs/PUBLISHING.md) · [Build notes](docs/DEVELOPMENT.md)`n"
[IO.File]::WriteAllText((Join-Path $root 'README.md'), $readme)
$extra = [IO.File]::ReadAllText((Join-Path $root 'src/studio.inc'))
$pixel = [IO.File]::ReadAllText((Join-Path $root 'src/pixel-font.inc')) + "`n" + [IO.File]::ReadAllText((Join-Path $root 'src/pixel-themes.inc'))
$extra = $extra.Replace('// PIXEL_THEME_IMPLEMENTATION', $pixel)
$cards = [IO.File]::ReadAllText((Join-Path $root 'src/widget-placement.h')) + "`n" + [IO.File]::ReadAllText((Join-Path $root 'src/card-themes.inc'))
$extra = $extra.Replace('// CARD_THEME_IMPLEMENTATION', $cards)
$extra += "`n" + [IO.File]::ReadAllText((Join-Path $root 'src/media-refresh.inc'))
$extra = $extra.Replace('// STUDIO_WIDGET_IMPLEMENTATION', [IO.File]::ReadAllText((Join-Path $root 'src/studio-widgets.inc')))
$extra = $extra.Replace('// STUDIO_WEATHER_ICONS', [IO.File]::ReadAllText((Join-Path $root 'src/weather-icons.inc')))
$s = $s.Replace('struct ModSettings {', @'
bool IsStudioStyle(std::wstring const& style) {
    return IsStudioPixelStyle(style) || style == L"gnome" || style == L"kde" || style == L"hyprland" ||
           style == L"nord" || style == L"catppuccin" || style == L"gruvbox";
}
struct ModSettings {
'@)
$s = $s.Replace('bool IsStudioStyle(', "bool IsStudioPixelStyle(std::wstring const& style) {`n    return style == L`"pixelCoffee`" || style == L`"pixelDusk`" || style == L`"pixelSky`" || style == L`"pixelNight`" || style == L`"pixelSakura`" || style == L`"pixelRain`" || style == L`"pixelHollow`" || style == L`"pixelSunset`";`n}`nbool IsStudioStyle(")
$s = $s.Replace('        g_settings.clockStyle != L"defaultOnly") {', '        g_settings.clockStyle != L"defaultOnly" && !IsStudioStyle(g_settings.clockStyle)) {')
# Insert the extension before update code, after upstream battery helpers.
$s = $s.Replace('// Helper to apply root horizontal', $extra + "`n// Helper to apply root horizontal")
$s = $s.Replace('void LoadSettings() {', "void LoadStudioSettings();`nvoid LoadSettings() {`n    LoadStudioSettings();")
$s = $s.Replace('} else if (g_settings.clockStyle == L"aura") {', '} else if (g_settings.clockStyle == L"aura" || IsStudioStyle(g_settings.clockStyle)) {')
$s = $s.Replace('wux::UIElement BuildClockElement() {', "wux::UIElement BuildClockElement() {`n    if (g_settings.clockStyle == L`"pixel`" && !studio.clockFont.empty()) return BuildStudioPixel();`n    if (IsStudioStyle(g_settings.clockStyle)) return BuildStudio();")
# Apply separate fonts to the legacy text-based clock designs as well.
$s = $s.Replace('wux::UIElement BuildClockElement() {', 'wux::UIElement BuildClockElementCore() {')
$factoryEnd = $s.IndexOf('// UI thread discovery and synchronous dispatch')
if ($factoryEnd -lt 0) { throw 'Clock factory end anchor not found' }
$s = $s.Insert($factoryEnd, @'
wux::UIElement BuildClockElement() {
    auto element = BuildClockElementCore();
    ApplyStudioClockFonts(element);
    return element;
}

'@)
$s = $s.Replace('void UpdateTimeAndDate() {', "void UpdateTimeAndDate() {`n    if (IsStudioPixelStyle(g_settings.clockStyle) || (g_settings.clockStyle == L`"pixel`" && !studio.clockFont.empty())) { UpdateStudioPixelTime(); return; }")
$s = $s.Replace('void ResetClockElementReferences() {', "void ResetClockElementReferences() {`n    ResetStudioPixel();")
$s = $s.Replace('#include <winrt/Windows.UI.Xaml.h>', "#include <winrt/Windows.UI.Xaml.h>`n#include <winrt/Windows.UI.Xaml.Markup.h>`n#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>")
$s = $s.Replace('GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, dateBuf, ARRAYSIZE(dateBuf)) > 0', 'GetDateFormatW(LOCALE_USER_DEFAULT, studio.dateFormat.empty() ? DATE_LONGDATE : 0, &st, studio.dateFormat.empty() ? nullptr : studio.dateFormat.c_str(), dateBuf, ARRAYSIZE(dateBuf)) > 0')
$s = $s.Replace('void RestoreDefaultElements() {', "void RestoreDefaultElements() {`n    RestoreStudioNative();")
$s = $s.Replace('auto content = wux::Window::Current().Content();', @'
auto window = wux::Window::Current();
    if (!window) return;
    auto content = window.Content();
'@)
$s = $s.Replace('    bool hideClock =', "    ++studioNativeScan;`n    SelectStudioWidget(content);`n    WalkStudioNative(content, 0);`n    UpdateStudioWidgets();`n    FinishStudioNativeScan();`n    bool hideClock =")
$s = $s.Replace('                        g_settings.hideDefaultWidgets);', '                        g_settings.hideDefaultWidgets && studio.widgetMode != L"studio");')
$s = $s.Replace('name == L"DetailTextPanel" || name == L"LockWidgetCanvas")', 'name == L"DetailTextPanel" || name == L"LockWidgetCanvas" || name == L"WidgetGroupPanel")')
# Never hide the Windows clock before custom UI has successfully attached.
$s = $s.Replace('SyncTrackedElements(g_hiddenClockElements, scan.clockElements, hideClock);', '')
$s = $s.Replace('    UpdateTimeAndDate();' + "`n}", "    UpdateTimeAndDate();`n}")
$needle = "    UpdateTimeAndDate();`n}`n`n// Reloads settings"
$s = $s.Replace($needle, "    SyncTrackedElements(g_hiddenClockElements, scan.clockElements, hideClock && IsClockAttached());`n    studioDumpDone = true;`n    UpdateTimeAndDate();`n}`n`n// Reloads settings")
$s = $s.Replace('    RestoreDefaultElements();' + "`n    RemoveActiveClockElement();", "    RestoreDefaultElements();`n    studioDumpDone = false;`n    RemoveActiveClockElement();")
$s = $s.Replace('    if (!sameRoot) {', "    if (!sameRoot) {`n        RestoreDefaultElements();")
# Media must not wait for the clock tick or the full-tree scan.
$tickStart = $s.IndexOf('void OnUiThreadTick() {')
$tickEnd = $s.IndexOf('LRESULT CALLBACK GetMessageHookProc', $tickStart)
if ($tickStart -lt 0 -or $tickEnd -lt 0) { throw 'UI tick source anchors not found' }
$s = $s.Substring(0, $tickStart) + @'
void OnUiThreadTick() {
    ULONGLONG now = GetTickCount64();
    if (g_settingsDirty.exchange(false)) {
        try {
            RebuildOnUiThread(nullptr);
            g_lastFullScan = now;
        } catch (...) {
            g_settingsDirty = true;
            throw;
        }
        return;
    }

    // Runs independently of clock rendering. Cached media subtree only;
    // discovery is needed only while the card is absent or replaced.
    RefreshStudioMedia(now);

    int intervalMs = g_settings.showSeconds ? 500 : 1000;
    if ((now - g_lastUiTick) < static_cast<ULONGLONG>(intervalMs)) return;
    g_lastUiTick = now;
    bool hasClockStyle = (g_settings.clockStyle != L"defaultOnly");
    bool needScan = (!g_lockRootGrid || (hasClockStyle && !g_activeClockElement) ||
                     (now - g_lastFullScan) >= 3000);
    if (needScan) {
        g_lastFullScan = now;
        AttachToTree();
    } else if (hasClockStyle) {
        UpdateTimeAndDate();
    }
}

'@ + $s.Substring($tickEnd)
# Wake quickly only while the lock screen window is visible.
$s = $s.Replace('while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT) {', @'
while (WaitForSingleObject(g_stopEvent,
            IsWindowVisible(g_coreWindow.load()) ? 125 : 500) == WAIT_TIMEOUT) {
'@)
# Settings callbacks only signal; the UI hook performs the actual reload.
$callbackStart = $s.IndexOf('void Wh_ModSettingsChanged() {')
$s = $s.Substring(0, $callbackStart) + @'
void Wh_ModSettingsChanged() {
    g_settingsDirty = true;
    HWND window = g_coreWindow.load();
    if (window) PostMessageW(window, WM_NULL, 0, 0);
}
'@
$s = $s.Replace('Windhawk_LockScreenClockStyler_RunOnUiThread_v1', 'Windhawk_LockScreenStudio_RunOnUiThread_v1')
$s = $s.Replace('Lock Screen Clock Styler initializing', 'Lock Screen Studio initializing')
$s = $s.Replace('Lock Screen Clock Styler uninitializing', 'Lock Screen Studio uninitializing')
$s = $s.Replace('Lock Screen Clock Styler uninitialized', 'Lock Screen Studio uninitialized')
# Always enumerate process threads too: LockApp windows can live on another desktop.
$s = $s.Replace('#include <windows.h>', "#include <windows.h>`n#include <tlhelp32.h>")
$s = $s.Replace('    return found;' + "`n}`n`nDWORD WINAPI WorkerThreadProc", @'
    if (!found) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            THREADENTRY32 entry{sizeof(entry)};
            if (Thread32First(snapshot, &entry)) do {
                if (entry.th32OwnerProcessID == GetCurrentProcessId())
                    EnumThreadWindows(entry.th32ThreadID, FindCoreWindowProc,
                                      reinterpret_cast<LPARAM>(&found));
                if (found) break;
            } while (Thread32Next(snapshot, &entry));
            CloseHandle(snapshot);
        }
    }
    return found;
}

DWORD WINAPI WorkerThreadProc
'@)
$output = [regex]::Replace(($settings + "`n" + $s).Replace("`r`n", "`n"), '(?m)[ \t]+$', '')
[IO.File]::WriteAllText((Join-Path $root 'lock-screen-studio.wh.cpp'), $output)
