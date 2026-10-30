// Pixel themes render licensed font outlines as native XAML paths. No external
// font install, private font file access, dispatcher callbacks or new hooks.
struct StudioPixelTheme {
    const wchar_t* key;
    const wchar_t* anchor;
    unsigned long foreground, accent, shadow;
    double x, y;
    bool dateAbove, twoTone, strip;
};
static const StudioPixelTheme kStudioPixelThemes[] = {
    {L"pixelCoffee", L"bottomLeft", 0xFFFFF3DF, 0xFFD6B58B, 0xDD211724, 80,-65, true,false,false},
    {L"pixelDusk", L"topLeft", 0xFFF2EDE7, 0xFFCDA69B, 0xDD201E2A, 80,65, false,false,false},
    {L"pixelSky", L"topRight", 0xFFFFFFFF, 0xFFFFE5B0, 0xAA547E94,-80,110, true,false,false},
    {L"pixelNight", L"topLeft", 0xFFF3EDF8, 0xFF56C5DC, 0xDD252237, 80,65, false,true,false},
    {L"pixelSakura", L"topLeft", 0xFF384654, 0xFFAE718B, 0x77FFFFFF, 80,65, false,false,false},
    {L"pixelRain", L"topLeft", 0xFFE4EEFF, 0xFF83B7DB, 0xEE101526, 80,65, false,false,false},
    {L"pixelHollow", L"bottom", 0xFFECE8DC, 0xFFAFAB9D, 0xEE111110, 140,-45, false,false,true},
    {L"pixelSunset", L"topLeft", 0xFFFFE2B5, 0xFFEEBCA4, 0xAA75465D, 80,65, false,false,false},
};
StudioPixelTheme const& CurrentStudioPixelTheme() {
    static const StudioPixelTheme legacy{L"pixel",L"center",0xFFFFFFFF,0xFFDDE9FF,0xDD101526,0,0,false,false,false};
    if (g_settings.clockStyle==L"pixel") return legacy;
    for (auto const& theme : kStudioPixelThemes)
        if (g_settings.clockStyle == theme.key) return theme;
    return kStudioPixelThemes[0];
}
[[clang::no_destroy]] wuxc::Grid studioPixelClock{nullptr};
[[clang::no_destroy]] wuxc::Grid studioPixelDate{nullptr};
std::wstring studioPixelLastTime, studioPixelLastDate;
void ResetStudioPixel() {
    studioPixelClock = nullptr;
    studioPixelDate = nullptr;
    studioPixelLastTime.clear();
    studioPixelLastDate.clear();
}
StudioPixelGlyph const* FindStudioPixelGlyph(wchar_t c) {
    auto first = std::begin(kStudioPixelGlyphs), last = std::end(kStudioPixelGlyphs);
    auto it = std::lower_bound(first, last, c,
        [](StudioPixelGlyph const& glyph, wchar_t character) { return glyph.character < character; });
    return it != last && it->character == c ? it : nullptr;
}
std::wstring StudioPixelHex(wu::Color const& color) {
    wchar_t buffer[10];
    swprintf_s(buffer, L"#%02X%02X%02X%02X", color.A, color.R, color.G, color.B);
    return buffer;
}
void RenderStudioPixel(wuxc::Grid const& host, std::wstring const& text,
                       double size, wu::Color foreground, wu::Color accent,
                       bool twoTone, double tracking,std::wstring const& fontOverride=L"") {
    auto const& theme = CurrentStudioPixelTheme();
    host.Children().Clear();
    if (!fontOverride.empty()) {
        wuxc::TextBlock custom;
        custom.Text(text);
        custom.FontSize(size);
        custom.FontFamily(wuxm::FontFamily(fontOverride));
        custom.Foreground(wuxm::SolidColorBrush(foreground));
        auto separator=text.find_first_of(L":|");
        if (twoTone && separator!=std::wstring::npos) {
            custom.Text(L"");
            wuxd::Run hours; hours.Text(text.substr(0,separator));
            custom.Inlines().Append(hours);
            wuxd::Run divider; divider.Text(text.substr(separator,1));
            divider.Foreground(wuxm::SolidColorBrush(StudioColor(L"",0xFFBE8DAE)));
            custom.Inlines().Append(divider);
            wuxd::Run minutes; minutes.Text(text.substr(separator+1));
            minutes.Foreground(wuxm::SolidColorBrush(accent));
            custom.Inlines().Append(minutes);
        }
        custom.CharacterSpacing(static_cast<int>(tracking*1000/(std::max)(size,1.0)));
        host.Children().Append(custom);
        custom.Measure({100000,100000});
        auto desired=custom.DesiredSize();
        host.Width((std::max)(1.0,static_cast<double>(desired.Width)));
        host.Height((std::max)(1.0,static_cast<double>(desired.Height)));
        return;
    }
    // Unsupported scripts use a readable native text fallback, never missing
    // date glyphs. All mapped font characters (including Cyrillic) are embedded.
    for (wchar_t c : text) if (!FindStudioPixelGlyph(c)) {
        wuxc::TextBlock fallback;
        fallback.Text(text);
        fallback.FontSize(size);
        fallback.Foreground(wuxm::SolidColorBrush(foreground));
        host.Children().Append(fallback);
        fallback.Measure({100000,100000});
        auto desired=fallback.DesiredSize();
        host.Width((std::max)(1.0,static_cast<double>(desired.Width)));
        host.Height((std::max)(1.0,static_cast<double>(desired.Height)));
        return;
    }
    double width = 0;
    auto ampm = text.ends_with(L" AM") || text.ends_with(L" PM") ? text.size()-3 : std::wstring::npos;
    for (size_t i = 0; i < text.size(); ++i) {
        double scale = size / kStudioPixelEm;
        if (size > studio.dateSize && ampm != std::wstring::npos && i >= ampm) scale *= 0.4;
        width += FindStudioPixelGlyph(text[i])->advance * scale + tracking;
    }
    width = (std::max)(1.0, width - (text.empty() ? 0 : tracking));
    double height = size * kStudioPixelHeight / kStudioPixelEm;
    double shadow = studio.pixelShadow;
    std::wstring xaml = L"<Canvas xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Width='" +
        std::to_wstring(width + shadow) + L"' Height='" + std::to_wstring(height + shadow) + L"'>";
    auto split = text.find_first_of(L":|");
    for (int layer = shadow > 0 ? 0 : 1; layer < 2; ++layer) {
        double cursor = 0;
        for (size_t i = 0; i < text.size(); ++i) {
            auto glyph = FindStudioPixelGlyph(text[i]);
            double scale = size / kStudioPixelEm;
            bool small = size > studio.dateSize && ampm != std::wstring::npos && i >= ampm;
            if (small) scale *= 0.4;
            wu::Color color = foreground;
            if (twoTone && split != std::wstring::npos && i > split) color = accent;
            if (twoTone && i == split) color = StudioColor(L"", 0xFFBE8DAE);
            if (layer == 0) color = StudioColor(L"", theme.shadow);
            double offset = layer == 0 ? shadow : 0;
            double top = offset + (small ? size * 0.45 : 0);
            if (*glyph->data) {
                xaml += L"<Path Stretch='None' Canvas.Left='" + std::to_wstring(cursor + offset) +
                    L"' Canvas.Top='" + std::to_wstring(top) + L"' Fill='" + StudioPixelHex(color) +
                    L"' Data='F1 " + glyph->data + L"'><Path.RenderTransform><ScaleTransform ScaleX='" +
                    std::to_wstring(scale) + L"' ScaleY='" + std::to_wstring(scale) +
                    L"'/></Path.RenderTransform></Path>";
            }
            cursor += glyph->advance * scale + tracking;
        }
    }
    xaml += L"</Canvas>";
    auto canvas = wux::Markup::XamlReader::Load(xaml).as<wuxc::Canvas>();
    host.Width(width + shadow);
    host.Height(height + shadow);
    host.Children().Append(canvas);
}
void UpdateStudioPixelTime() {
    if (!studioPixelClock) return;
    SYSTEMTIME now;
    GetLocalTime(&now);
    bool twelve = g_settings.timeFormat == L"12h";
    int hour = twelve ? (now.wHour % 12 == 0 ? 12 : now.wHour % 12) : now.wHour;
    wchar_t buffer[48];
    swprintf_s(buffer, twelve ? L"%d:%02d" : L"%02d:%02d", hour, now.wMinute);
    std::wstring time = buffer;
    if (g_settings.showSeconds) {
        swprintf_s(buffer, L":%02d", now.wSecond);
        time += buffer;
    }
    if (twelve) time += now.wHour >= 12 ? L" PM" : L" AM";
    auto const& theme = CurrentStudioPixelTheme();
    if (theme.twoTone) time[time.find(L':')] = L'|';
    auto fg = StudioColor(studio.foreground, theme.foreground);
    auto accent = StudioColor(studio.accent, theme.accent);
    bool layoutChanged = time != studioPixelLastTime;
    if (layoutChanged) {
        RenderStudioPixel(studioPixelClock, time, g_settings.clockFontSize, fg, accent, theme.twoTone, 0,studio.clockFont);
        studioPixelLastTime = time;
    }
    if (studioPixelDate && g_settings.showDateCapsule) {
        wchar_t dateBuffer[256]{};
        PCWSTR locale = studio.pixelDateLocale.empty() || studio.pixelDateLocale == L"system"
            ? LOCALE_NAME_USER_DEFAULT : studio.pixelDateLocale.c_str();
        PCWSTR format = studio.dateFormat.empty() ? nullptr : studio.dateFormat.c_str();
        if (!GetDateFormatEx(locale, format ? 0 : DATE_LONGDATE, &now, format,
                             dateBuffer, ARRAYSIZE(dateBuffer), nullptr)) {
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &now, nullptr,
                            dateBuffer, ARRAYSIZE(dateBuffer), nullptr);
        }
        CharUpperBuffW(dateBuffer, static_cast<DWORD>(wcslen(dateBuffer)));
        std::wstring date = dateBuffer;
        if (date != studioPixelLastDate) {
            std::wstring dateFont=studio.dateFont;
            if (dateFont.empty() && g_settings.clockStyle==L"pixel") dateFont=L"Segoe UI Variable Text";
            RenderStudioPixel(studioPixelDate, date, studio.dateSize, accent, accent, false, studio.pixelDateTracking,dateFont);
            studioPixelLastDate = date;
            layoutChanged = true;
        }
    }
    if (theme.strip && layoutChanged && studioPixelDate &&
        (studio.clockAnchor.empty() || studio.clockAnchor == L"theme") &&
        (studio.dateAnchor.empty() || studio.dateAnchor == L"theme")) {
        double gap = 36;
        StudioPlace(studioPixelClock, L"bottom", (studioPixelDate.Width() + gap) / 2 + studio.clockX,
                    theme.y + studio.clockY);
        StudioPlace(studioPixelDate, L"bottom", -(studioPixelClock.Width() + gap) / 2 + studio.clockX + studio.dateX,
                    theme.y + studio.clockY - (studioPixelClock.Height() - studioPixelDate.Height()) / 2 + studio.dateY);
    }
    if (g_batteryStatusText && g_settings.showBatteryStatus) {
        auto battery = GetCurrentBatteryInfo();
        if (battery.hasBattery && (battery.percentage != g_timeCache.lastBatteryPct ||
                                   battery.isCharging != g_timeCache.lastIsCharging)) {
            swprintf_s(buffer, L"%d%%", battery.percentage);
            g_batteryStatusText.Text(buffer);
            if (g_batteryIconText) g_batteryIconText.Text(battery.isCharging ? L"\u26A1 " : L"\U0001F50B ");
            g_timeCache.lastBatteryPct = battery.percentage;
            g_timeCache.lastIsCharging = battery.isCharging;
        }
    }
}
wux::UIElement BuildStudioPixel() {
    ResetStudioPixel();
    auto const& theme = CurrentStudioPixelTheme();
    wuxc::Grid root;
    root.Name(L"WindhawkStudioPixelRoot");
    root.IsHitTestVisible(false);
    wuxc::Grid::SetRowSpan(root, 100);
    wuxc::Grid::SetColumnSpan(root, 100);
    wuxc::Canvas::SetZIndex(root, 100);
    root.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(
        static_cast<uint8_t>(255 * studio.dim / 100), 0, 0, 0)));
    auto anchor = studio.clockAnchor == L"theme" || studio.clockAnchor.empty()
        ? std::wstring(theme.anchor) : studio.clockAnchor;
    bool useTheme = studio.clockAnchor == L"theme" || studio.clockAnchor.empty();
    double x = (useTheme ? theme.x : 0) + studio.clockX;
    double y = (useTheme ? theme.y : 0) + studio.clockY;
    wuxc::Grid clock;
    StudioPlace(clock, anchor, x, y);
    root.Children().Append(clock);
    studioPixelClock = clock;
    if (g_settings.showDateCapsule) {
        wuxc::Grid date;
        bool follows = studio.dateAnchor == L"theme" || studio.dateAnchor.empty();
        double clockHeight = g_settings.clockFontSize * kStudioPixelHeight / kStudioPixelEm;
        double dateHeight = studio.dateSize * kStudioPixelHeight / kStudioPixelEm;
        bool bottom = anchor == L"bottom" || anchor == L"bottomLeft" || anchor == L"bottomRight";
        double dateOffset = theme.dateAbove ? -(bottom ? clockHeight + 16 : dateHeight + 12)
                                            : (bottom ? dateHeight + 12 : clockHeight + 12);
        double dateX = follows ? x : 0;
        double dateY = follows ? y + dateOffset : 0;
        if (theme.strip && follows) { dateX = -150 + studio.clockX; dateY = y - 35; }
        StudioPlace(date, follows ? anchor : studio.dateAnchor, dateX + studio.dateX, dateY + studio.dateY);
        root.Children().Append(date);
        studioPixelDate = date;
    }
    if (!studio.text.empty()) {
        wuxc::Grid text;
        RenderStudioPixel(text, studio.text, studio.textSize,
            StudioColor(studio.foreground, theme.foreground), StudioColor(studio.accent, theme.accent), false, 1);
        StudioPlace(text, studio.textAnchor, studio.textX, studio.textY);
        root.Children().Append(text);
    }
    if (g_settings.showBatteryStatus) {
        auto battery = CreateBatteryCapsule(false, true);
        if (battery) {
            auto element = battery.as<wux::FrameworkElement>();
            element.Margin(wux::Thickness{0,0,0,0});
            StudioPlace(element, studio.batteryAnchor, studio.batteryX, studio.batteryY);
            root.Children().Append(element);
        }
    }
    UpdateStudioPixelTime();
    return root;
}
