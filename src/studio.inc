// Studio extension. All XAML operations run on the LockApp UI thread.
struct StudioSettings {
    std::wstring font, foreground, accent, panel, dateFormat, text;
    std::wstring clockFont, dateFont, widgetFont, mediaFont;
    std::wstring clockAnchor, dateAnchor, batteryAnchor, textAnchor;
    int weight, opacity, radius, dim, dateSize, textSize;
    int clockX, clockY, dateX, dateY, batteryX, batteryY, textX, textY;
    int widgetX, widgetY, mediaX, mediaY;
    std::wstring widgetTarget, mediaTheme, mediaBackground, mediaForeground, mediaAccent;
    int mediaOpacity, mediaRadius, mediaTextSize;
    std::wstring widgetTheme, widgetBackground, widgetForeground, widgetAccent, widgetMoveContainer;
    int widgetOpacity, widgetRadius, widgetTextSize, widgetScreenPadding;
    bool widgetKeepOnScreen;
    std::wstring widgetMode, widgetAnchor;
    int widgetCardWidth;
    bool weatherIconVisible;
    bool widgetCustomStyle;
    std::wstring weatherIconPosition,weatherIconCondition;
    int weatherIconX,weatherIconY,weatherIconSize;
    std::wstring pixelDateLocale;
    int pixelDateTracking, pixelShadow;
    bool hideMedia, diagnostics;
} studio;
bool studioDumpDone = false;
struct StudioRule {
    std::wstring name, color;
    int x, y, opacity, fontSize;
    bool hide;
};
std::vector<StudioRule> studioRules;

std::wstring StudioString(PCWSTR key) {
    auto value = WindhawkUtils::StringSetting::make(key);
    return value.get() ? value.get() : L"";
}
int StudioInt(PCWSTR key, int low, int high) {
    return (std::clamp)(Wh_GetIntSetting(key), low, high);
}
void LoadStudioSettings() {
    studio.font = StudioString(L"fontFamily");
    studio.clockFont = StudioString(L"clockFontFamily");
    studio.dateFont = StudioString(L"dateFontFamily");
    studio.widgetFont = StudioString(L"widgetFontFamily");
    studio.mediaFont = StudioString(L"mediaFontFamily");
    studio.foreground = StudioString(L"foreground");
    studio.accent = StudioString(L"accent");
    studio.panel = StudioString(L"panelColor");
    studio.dateFormat = StudioString(L"dateFormat");
    studio.pixelDateLocale = StudioString(L"pixelDateLocale");
    studio.pixelDateTracking = StudioInt(L"pixelDateTracking", 0, 12);
    studio.pixelShadow = StudioInt(L"pixelShadow", 0, 8);
    studio.text = StudioString(L"customText");
    studio.clockAnchor = StudioString(L"clockAnchor");
    studio.dateAnchor = StudioString(L"dateAnchor");
    studio.batteryAnchor = StudioString(L"batteryAnchor");
    studio.textAnchor = StudioString(L"textAnchor");
    studio.weight = StudioInt(L"fontWeight", 100, 900);
    studio.opacity = StudioInt(L"panelOpacity", 0, 100);
    studio.radius = StudioInt(L"cornerRadius", 0, 100);
    studio.dim = StudioInt(L"backgroundDim", 0, 90);
    studio.dateSize = StudioInt(L"dateFontSize", 8, 160);
    studio.textSize = StudioInt(L"textFontSize", 8, 160);
    studio.clockX = StudioInt(L"clockX", -4000, 4000);
    studio.clockY = StudioInt(L"clockY", -4000, 4000);
    studio.dateX = StudioInt(L"dateX", -4000, 4000);
    studio.dateY = StudioInt(L"dateY", -4000, 4000);
    studio.batteryX = StudioInt(L"batteryX", -4000, 4000);
    studio.batteryY = StudioInt(L"batteryY", -4000, 4000);
    studio.textX = StudioInt(L"textX", -4000, 4000);
    studio.textY = StudioInt(L"textY", -4000, 4000);
    studio.widgetX = StudioInt(L"widgetX", -4000, 4000);
    studio.widgetY = StudioInt(L"widgetY", -4000, 4000);
    studio.mediaX = StudioInt(L"mediaX", -4000, 4000);
    studio.mediaY = StudioInt(L"mediaY", -4000, 4000);
    studio.widgetTarget = StudioString(L"widgetTarget");
    studio.mediaTheme = StudioString(L"mediaTheme");
    studio.mediaBackground = StudioString(L"mediaBackground");
    studio.mediaForeground = StudioString(L"mediaForeground");
    studio.mediaAccent = StudioString(L"mediaAccent");
    studio.mediaOpacity = StudioInt(L"mediaOpacity", 0, 100);
    studio.mediaRadius = StudioInt(L"mediaCornerRadius", 0, 80);
    studio.mediaTextSize = StudioInt(L"mediaTextSize", 10, 40);
    studio.widgetTheme = StudioString(L"widgetTheme");
    studio.widgetBackground = StudioString(L"widgetBackground");
    studio.widgetForeground = StudioString(L"widgetForeground");
    studio.widgetAccent = StudioString(L"widgetAccent");
    studio.widgetMoveContainer = StudioString(L"widgetMoveContainer");
    studio.widgetOpacity = StudioInt(L"widgetOpacity", 0, 100);
    studio.widgetRadius = StudioInt(L"widgetCornerRadius", 0, 80);
    studio.widgetTextSize = StudioInt(L"widgetTextSize", 0, 40);
    studio.widgetKeepOnScreen = Wh_GetIntSetting(L"widgetKeepOnScreen") != 0;
    studio.widgetScreenPadding = StudioInt(L"widgetScreenPadding", 0, 100);
    studio.widgetMode = StudioString(L"widgetMode");
    studio.widgetAnchor = StudioString(L"widgetAnchor");
    studio.widgetCardWidth = StudioInt(L"widgetCardWidth", 180, 600);
    studio.widgetCustomStyle = Wh_GetIntSetting(L"widgetCustomStyle") != 0;
    if(!studio.widgetCustomStyle) {
        studio.widgetBackground.clear(); studio.widgetForeground.clear(); studio.widgetAccent.clear();
        studio.widgetOpacity=studio.widgetTheme==L"smoke" ? 78 : 100;
        studio.widgetRadius=studio.widgetTheme==L"macos" || studio.widgetTheme==L"macosDark" ? 24 :
            studio.widgetTheme==L"catppuccin" ? 20 : studio.widgetTheme==L"nord" ? 8 :
            studio.widgetTheme==L"gruvbox" ? 4 : studio.widgetTheme==L"minimal" ? 12 : 18;
    }
    studio.weatherIconVisible = Wh_GetIntSetting(L"weatherIconVisible") != 0;
    studio.weatherIconPosition = StudioString(L"weatherIconPosition");
    studio.weatherIconCondition = StudioString(L"weatherIconCondition");
    studio.weatherIconX = StudioInt(L"weatherIconX", -1000, 1000);
    studio.weatherIconY = StudioInt(L"weatherIconY", -1000, 1000);
    studio.weatherIconSize = StudioInt(L"weatherIconSize", 24, 160);
    studio.hideMedia = Wh_GetIntSetting(L"hideMedia") != 0;
    studio.diagnostics = Wh_GetIntSetting(L"diagnostics") != 0;
    studioRules.clear();
    for (int i = 0; i < 100; ++i) {
        std::wstring prefix = L"elementRules[" + std::to_wstring(i) + L"].";
        auto name = StudioString((prefix + L"name").c_str());
        if (name.empty()) break;
        StudioRule rule;
        rule.name = name;
        rule.color = StudioString((prefix + L"color").c_str());
        rule.x = StudioInt((prefix + L"x").c_str(), -4000, 4000);
        rule.y = StudioInt((prefix + L"y").c_str(), -4000, 4000);
        rule.opacity = StudioInt((prefix + L"opacity").c_str(), 0, 100);
        rule.fontSize = StudioInt((prefix + L"fontSize").c_str(), 0, 300);
        rule.hide = Wh_GetIntSetting((prefix + L"hide").c_str()) != 0;
        studioRules.push_back(std::move(rule));
    }
}
wu::Color StudioColor(std::wstring const& input, unsigned long fallback) {
    auto color = fallback;
    if (input.size() == 7 || input.size() == 9) {
        bool valid = input[0] == L'#';
        for (size_t i = 1; i < input.size(); ++i)
            valid = valid && ((input[i] >= L'0' && input[i] <= L'9') ||
                     (input[i] >= L'a' && input[i] <= L'f') ||
                     (input[i] >= L'A' && input[i] <= L'F'));
        if (valid) {
            color = wcstoul(input.c_str() + 1, nullptr, 16);
            if (input.size() == 7) color |= 0xFF000000;
        } else Wh_Log(L"Invalid Studio color: %s", input.c_str());
    } else if (!input.empty()) Wh_Log(L"Invalid Studio color: %s", input.c_str());
    return wu::ColorHelper::FromArgb((color >> 24) & 255, (color >> 16) & 255,
                                   (color >> 8) & 255, color & 255);
}
void StudioPlace(wux::FrameworkElement const& element, std::wstring anchor, double x, double y) {
    bool left = anchor == L"left" || anchor == L"topLeft" || anchor == L"bottomLeft";
    bool right = anchor == L"right" || anchor == L"topRight" || anchor == L"bottomRight";
    bool top = anchor == L"top" || anchor == L"topLeft" || anchor == L"topRight";
    bool bottom = anchor == L"bottom" || anchor == L"bottomLeft" || anchor == L"bottomRight";
    element.HorizontalAlignment(left ? wux::HorizontalAlignment::Left :
        right ? wux::HorizontalAlignment::Right : wux::HorizontalAlignment::Center);
    element.VerticalAlignment(top ? wux::VerticalAlignment::Top :
        bottom ? wux::VerticalAlignment::Bottom : wux::VerticalAlignment::Center);
    wuxm::TranslateTransform transform;
    transform.X(x); transform.Y(y);
    element.RenderTransform(transform);
}
// PIXEL_THEME_IMPLEMENTATION
wux::UIElement BuildStudio() {
    if (IsStudioPixelStyle(g_settings.clockStyle)) return BuildStudioPixel();
    auto style = g_settings.clockStyle;
    unsigned long fg = 0xFFECEFF4, accent = 0xFF88C0D0, bg = 0xFF2E3440;
    if (style == L"catppuccin") { fg=0xFFCDD6F4; accent=0xFFCBA6F7; bg=0xFF1E1E2E; }
    if (style == L"gruvbox") { fg=0xFFEBDBB2; accent=0xFFFABD2F; bg=0xFF282828; }
    if (style == L"hyprland") { fg=0xFFCFFAFE; accent=0xFF2DD4BF; bg=0xFF0F172A; }
    if (style == L"kde") { fg=0xFFEFF0F1; accent=0xFF3DAEE9; bg=0xFF232629; }
    if (style == L"gnome") { fg=0xFFFFFFFF; accent=0xFFFFFFFF; bg=0xFF202020; }
    auto foreground = wuxm::SolidColorBrush(StudioColor(studio.foreground, fg));
    auto highlight = wuxm::SolidColorBrush(StudioColor(studio.accent, accent));
    auto panelColor = StudioColor(studio.panel, bg);
    panelColor.A = static_cast<uint8_t>(panelColor.A * studio.opacity / 100);
    wuxc::Grid root;
    root.Name(L"WindhawkStudioRoot");
    root.IsHitTestVisible(false);
    // Independent overlay spans the parent, including Grid row/column layouts.
    wuxc::Grid::SetRowSpan(root, 100);
    wuxc::Grid::SetColumnSpan(root, 100);
    wuxc::Canvas::SetZIndex(root, 100);
    root.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(
        static_cast<uint8_t>(255 * studio.dim / 100), 0, 0, 0)));
    std::wstring anchor = studio.clockAnchor;
    bool terminal = style == L"hyprland";
    if (anchor == L"theme" || anchor.empty()) anchor = terminal ? L"left" : L"center";
    double themeX = terminal ? 100 : 0;
    wuxc::Border card;
    card.Padding(wux::Thickness{28, 8, 28, 8});
    card.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(studio.radius));
    if (style != L"gnome" || !studio.panel.empty())
        card.Background(wuxm::SolidColorBrush(panelColor));
    if (terminal || style == L"kde") {
        card.BorderBrush(highlight);
        card.BorderThickness(wux::Thickness{1,1,1,1});
    }
    wuxc::TextBlock clock;
    clock.Name(L"StudioClock");
    clock.FontSize(g_settings.clockFontSize);
    clock.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(studio.weight)});
    std::wstring font = studio.font.empty() ?
        (terminal ? L"Cascadia Code" : L"Segoe UI Variable Display") : studio.font;
    clock.FontFamily(wuxm::FontFamily(studio.clockFont.empty() ? font : studio.clockFont));
    clock.Foreground(foreground);
    card.Child(clock);
    StudioPlace(card, anchor, themeX + studio.clockX, studio.clockY);
    root.Children().Append(card);
    g_auraTimeTextBlock = clock;
    if (g_settings.showDateCapsule) {
        wuxc::TextBlock date;
        date.Name(L"StudioDate");
        date.FontSize(studio.dateSize);
        date.FontFamily(wuxm::FontFamily(studio.dateFont.empty() ? font : studio.dateFont));
        date.Foreground(highlight);
        bool follows = studio.dateAnchor == L"theme" || studio.dateAnchor.empty();
        StudioPlace(date, follows ? anchor : studio.dateAnchor,
                    (follows ? themeX : 0) + studio.dateX,
                    (follows ? g_settings.clockFontSize * 0.85 + studio.clockY : 0) + studio.dateY);
        root.Children().Append(date);
        g_dateTextBlock = date;
    }
    if (!studio.text.empty()) {
        wuxc::TextBlock text;
        text.Text(studio.text);
        text.FontSize(studio.textSize);
        text.FontFamily(wuxm::FontFamily(font));
        text.Foreground(highlight);
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
    return root;
}
void ApplyStudioClockFonts(wux::DependencyObject const& node,int depth=0) {
    if (!node || depth>96) return;
    if (auto text=node.try_as<wuxc::TextBlock>()) {
        bool date=text==g_dateTextBlock || text==g_macosDateTextBlock || text==g_macosDateShadowBlock;
        bool clock=text==g_hourTextBlock || text==g_hourBotTextBlock || text==g_minTextBlock ||
            text==g_minBotTextBlock || text==g_secTextBlock || text==g_ampmTextBlock ||
            text==g_auraTimeTextBlock || text==g_auraShadowTextBlock ||
            text==g_macosTimeTextBlock || text==g_macosShadowTextBlock;
        auto const& family=date ? studio.dateFont : studio.clockFont;
        if ((date || clock) && !family.empty()) text.FontFamily(wuxm::FontFamily(family));
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for(int i=0;i<count;++i) ApplyStudioClockFonts(wuxm::VisualTreeHelper::GetChild(node,i),depth+1);
}
struct StudioNativeValue {
    wux::FrameworkElement element{nullptr};
    wux::DependencyProperty property{nullptr};
    winrt::Windows::Foundation::IInspectable value{nullptr};
    unsigned long long scan = 0;
};
[[clang::no_destroy]] std::vector<StudioNativeValue> studioNative;
unsigned long long studioNativeScan = 0;
[[clang::no_destroy]] wux::FrameworkElement studioWidget{nullptr};
[[clang::no_destroy]] wux::FrameworkElement studioWidgetMotion{nullptr};
bool studioWidgetLogged = false;
bool studioMediaLogged = false;
std::vector<std::wstring> studioLoggedNames;
void ResetStudioMediaRefresh();
void ResetStudioCardOverlays();
void FinishStudioCardOverlays();
void ResetStudioWidgets();
void RestoreStudioNative() {
    ResetStudioMediaRefresh();
    ResetStudioWidgets();
    ResetStudioCardOverlays();
    for (auto const& item : studioNative) try {
        if (item.value == wux::DependencyProperty::UnsetValue()) item.element.ClearValue(item.property);
        else item.element.SetValue(item.property, item.value);
    } catch (...) {}
    studioNative.clear();
    studioWidget = nullptr;
    studioWidgetMotion = nullptr;
    studioWidgetLogged = false;
    studioMediaLogged = false;
    studioLoggedNames.clear();
}
void StudioSave(wux::FrameworkElement const& e, wux::DependencyProperty const& p) {
    for (auto& item : studioNative) if (item.element == e && item.property == p) {
        item.scan = studioNativeScan;
        return;
    }
    studioNative.push_back({e, p, e.ReadLocalValue(p), studioNativeScan});
}
void FinishStudioNativeScan() {
    FinishStudioCardOverlays();
    // A fallback container can be replaced by WidgetGroupPanel later. Restore
    // the old target so offsets never accumulate on both parent and child.
    studioNative.erase(std::remove_if(studioNative.begin(), studioNative.end(),
        [](StudioNativeValue const& item) {
            if (item.scan == studioNativeScan) return false;
            try {
                if (item.value == wux::DependencyProperty::UnsetValue()) item.element.ClearValue(item.property);
                else item.element.SetValue(item.property, item.value);
            } catch (...) {}
            return true;
        }), studioNative.end());
}
// Prefer the actual cards panel over an outer canvas that Windows can animate
// or clip. Select once per scan so only one group receives the offset.
void FindStudioWidget(wux::DependencyObject const& node, int depth, int& bestRank) {
    if (!node || depth > 96 || (g_activeClockElement && node == g_activeClockElement)) return;
    if (auto e = node.try_as<wux::FrameworkElement>()) {
        auto name = e.Name();
        int rank = 100;
        if (!studio.widgetTarget.empty() && studio.widgetTarget != L"auto") {
            if (name == studio.widgetTarget) rank = 0;
        } else {
            if (name == L"WidgetGroupPanel") rank = 0;
            else if (name == L"LockWidgetCanvas") rank = 1;
            else if (name == L"AdaptiveCardDetailContainer") rank = 2;
            else if (name == L"DetailTextPanel") rank = 3;
        }
        if (rank < bestRank) { studioWidget = e; bestRank = rank; }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) try {
        FindStudioWidget(wuxm::VisualTreeHelper::GetChild(node, i), depth + 1, bestRank);
    } catch (...) {}
}
bool StudioWidgetOnlyBranch(wux::DependencyObject const& node,wux::DependencyObject const& branch,int depth,int& budget) {
    if (!node || depth>96 || --budget<0) return false;
    if (node==branch) return true;
    if (g_activeClockElement && node==g_activeClockElement) return false;
    if (auto e=node.try_as<wux::FrameworkElement>()) {
        auto name=e.Name();
        if (name==L"Time" || name==L"Date" || name==L"TimeAndDatePanel" ||
            name==L"MediaControls" || name==L"MediaTransportControls") return false;
        if (e.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::ScrollBar>()) return true;
        if (depth>0 && (e.try_as<wuxc::TextBlock>() || e.try_as<wuxc::Image>() ||
            e.try_as<wuxc::IconElement>() || e.try_as<wuxc::Control>())) return false;
        wuxm::Brush background{nullptr};
        if (auto grid=e.try_as<wuxc::Panel>()) background=grid.Background();
        else if (auto border=e.try_as<wuxc::Border>()) background=border.Background();
        else if (auto control=e.try_as<wuxc::Control>()) background=control.Background();
        if (background && background.try_as<wuxm::ImageBrush>()) return false;
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for(int i=0;i<count;++i)
        if (!StudioWidgetOnlyBranch(wuxm::VisualTreeHelper::GetChild(node,i),branch,depth+1,budget)) return false;
    return true;
}
void SelectStudioWidget(wux::DependencyObject const& content) {
    auto previous = studioWidget;
    auto previousMotion = studioWidgetMotion;
    studioWidget = nullptr;
    int bestRank = 100;
    FindStudioWidget(content, 0, bestRank);
    studioWidgetMotion = studioWidget;
    if (studioWidget && studio.widgetMoveContainer != L"panel") {
        wux::DependencyObject branch=studioWidget;
        auto parent = wuxm::VisualTreeHelper::GetParent(studioWidget);
        for (int i = 0; parent && i < 96 && parent != content; ++i) {
            if (auto element = parent.try_as<wux::FrameworkElement>()) {
                auto name = element.Name();
                if (name == L"LockRootGrid" || name == L"MainPageContainer") break;
                if (name == L"LockWidgetCanvas" || name == L"AdaptiveCardDetailContainer" || name == L"DetailTextPanel")
                    studioWidgetMotion = element;
                else {
                    int budget=4096;
                    if (!StudioWidgetOnlyBranch(parent,branch,0,budget)) break;
                    studioWidgetMotion=element;
                }
            }
            branch=parent;
            parent = wuxm::VisualTreeHelper::GetParent(parent);
        }
    }
    if (previousMotion && previousMotion != studioWidgetMotion) {
        studioNative.erase(std::remove_if(studioNative.begin(),studioNative.end(),
            [&previousMotion](StudioNativeValue const& saved) {
                if (saved.element != previousMotion || saved.property != wux::UIElement::RenderTransformProperty()) return false;
                if (saved.value == wux::DependencyProperty::UnsetValue()) saved.element.ClearValue(saved.property);
                else saved.element.SetValue(saved.property,saved.value);
                return true;
            }),studioNative.end());
    }
    if (studioWidget != previous) studioWidgetLogged = false;
    if (!studioWidgetLogged && (studio.widgetX || studio.widgetY || studio.diagnostics)) {
        if (studioWidget) {
            Wh_Log(L"Studio widgets: target=%s, offset=(%d,%d)",
                studioWidget.Name().c_str(), studio.widgetX, studio.widgetY);
            if (studioWidgetMotion) Wh_Log(L"Studio widget motion host: %s (%s)",
                studioWidgetMotion.Name().c_str(),winrt::get_class_name(studioWidgetMotion).c_str());
            studioWidgetLogged = true;
        } else if (!studioDumpDone) {
            Wh_Log(L"Studio widgets: no matching group yet; retrying. Enable diagnostics to inspect names.");
        }
    }
}
void StudioSet(wux::FrameworkElement const& e, wux::DependencyProperty const& p,
               winrt::Windows::Foundation::IInspectable const& value) {
    StudioSave(e, p);
    // Fast media refresh must not continually invalidate unchanged visuals.
    auto current = e.GetValue(p);
    if (current == value) return;
    if (auto requested=value.try_as<wuxm::FontFamily>()) {
        if (auto actual=current.try_as<wuxm::FontFamily>())
            if (requested.Source()==actual.Source()) return;
    }
    if (auto requested = value.try_as<wuxm::SolidColorBrush>()) {
        if (auto actual = current.try_as<wuxm::SolidColorBrush>())
            if (requested.Color() == actual.Color() && requested.Opacity() == actual.Opacity()) return;
    }
    if (auto requested = value.try_as<winrt::Windows::Foundation::IReference<double>>()) {
        if (auto actual = current.try_as<winrt::Windows::Foundation::IReference<double>>())
            if (requested.Value() == actual.Value()) return;
    }
    if (auto requested = value.try_as<winrt::Windows::Foundation::IReference<wux::Thickness>>()) {
        if (auto actual = current.try_as<winrt::Windows::Foundation::IReference<wux::Thickness>>())
            if (requested.Value() == actual.Value()) return;
    }
    if (auto requested = value.try_as<winrt::Windows::Foundation::IReference<wux::CornerRadius>>()) {
        if (auto actual = current.try_as<winrt::Windows::Foundation::IReference<wux::CornerRadius>>())
            if (requested.Value() == actual.Value()) return;
    }
    e.SetValue(p, value);
}
bool StudioMediaName(winrt::hstring const& name) {
    return name == L"MediaControls" || name == L"MediaTransportControls";
}
bool StudioInsideMedia(wux::DependencyObject node) {
    for (int depth = 0; node && depth < 96; ++depth) {
        node = wuxm::VisualTreeHelper::GetParent(node);
        if (auto parent = node.try_as<wux::FrameworkElement>())
            if (StudioMediaName(parent.Name())) return true;
    }
    return false;
}
// CARD_THEME_IMPLEMENTATION
void InspectStudioNative(wux::FrameworkElement const& e, int depth) {
    auto name = e.Name();
    if (auto text=e.try_as<wuxc::TextBlock>()) {
        if (name==L"Time" && !studio.clockFont.empty())
            StudioSet(text,wuxc::TextBlock::FontFamilyProperty(),wuxm::FontFamily(studio.clockFont));
        if (name==L"Date" && !studio.dateFont.empty())
            StudioSet(text,wuxc::TextBlock::FontFamilyProperty(),wuxm::FontFamily(studio.dateFont));
    }
    if (studio.diagnostics && !name.empty()) {
        std::wstring key = std::wstring(name) + L" (" + std::wstring(winrt::get_class_name(e)) + L")";
        if (std::find(studioLoggedNames.begin(), studioLoggedNames.end(), key) == studioLoggedNames.end()) {
            studioLoggedNames.push_back(key);
            Wh_Log(L"Studio tree [%d]: %s", depth, key.c_str());
        }
    }
    // Match one outer container per feature to avoid applying offsets twice.
    bool widget = studio.widgetMode!=L"studio" && studioWidgetMotion && e == studioWidgetMotion;
    bool insideWidget = StudioInsideWidget(e);
    bool insideMedia = StudioInsideMedia(e);
    bool media = StudioMediaName(name) && !insideMedia;
    bool skinMedia = StudioCardSkinName(studio.mediaTheme);
    if ((skinMedia || !studio.mediaFont.empty()) && (media || insideMedia)) {
        if (skinMedia) StyleStudioCardChrome(e,false);
        if (auto text = e.try_as<wuxc::TextBlock>()) {
            StyleStudioCardText(text,false);
        }
    }
    if (studio.widgetMode!=L"studio" && insideWidget && (StudioCardSkinName(studio.widgetTheme) || !studio.widgetFont.empty())) {
        if (StudioCardSkinName(studio.widgetTheme)) StyleStudioCardChrome(e,true);
        if (auto text = e.try_as<wuxc::TextBlock>()) StyleStudioCardText(text,true);
    }
    if (studio.widgetMode!=L"studio" && !insideWidget && StudioCardSkinName(studio.widgetTheme) && StudioWidgetWrapper(e))
        StyleStudioCardChrome(e,true);
    if (media && !studioMediaLogged) {
        Wh_Log(L"Studio media: target=%s, theme=%s, offset=(%d,%d)",
               name.c_str(), studio.mediaTheme.c_str(), studio.mediaX, studio.mediaY);
        studioMediaLogged = true;
    }
    StudioRule const* rule = nullptr;
    // Protect layout hosts from accidentally collapsing the entire screen.
    if (name != L"LockRootGrid" && name != L"MainPageContainer")
        for (auto const& candidate : studioRules)
            if (candidate.name == name) { rule = &candidate; break; }
    if (!widget && !media && !rule) return;
    double x = widget ? studio.widgetX : studio.mediaX;
    double y = widget ? studio.widgetY : studio.mediaY;
    if (rule) { x = rule->x; y = rule->y; }
    if (widget && !rule) ClampStudioWidgetOffset(x,y);
    if (x || y || widget) {
        auto prop = wux::UIElement::RenderTransformProperty();
        StudioSave(e, prop);
        wuxm::Transform original{nullptr};
        for (auto const& item : studioNative)
            if (item.element == e && item.property == prop && item.value != wux::DependencyProperty::UnsetValue())
                original = item.value.try_as<wuxm::Transform>();
        wuxm::TransformGroup group;
        if (original) group.Children().Append(original);
        wuxm::TranslateTransform t; t.X(x); t.Y(y);
        group.Children().Append(t);
        bool unchanged = false;
        if (auto current = e.RenderTransform().try_as<wuxm::TransformGroup>()) {
            auto children = current.Children();
            if (children.Size() == (original ? 2u : 1u)) {
                auto last = children.GetAt(children.Size() - 1).try_as<wuxm::TranslateTransform>();
                unchanged = last && last.X() == x && last.Y() == y &&
                    (!original || children.GetAt(0) == original);
            }
        }
        if (!unchanged) e.RenderTransform(group);
    }
    if ((media && studio.hideMedia) || (rule && rule->hide)) {
        StudioSave(e, wux::UIElement::VisibilityProperty());
        e.Visibility(wux::Visibility::Collapsed);
    }
    if (rule) {
        StudioSave(e, wux::UIElement::OpacityProperty());
        e.Opacity(rule->opacity / 100.0);
        if (auto text = e.try_as<wuxc::TextBlock>()) {
            if (rule->fontSize > 0) {
                StudioSave(e, wuxc::TextBlock::FontSizeProperty());
                text.FontSize(rule->fontSize);
            }
            if (!rule->color.empty()) {
                StudioSave(e, wuxc::TextBlock::ForegroundProperty());
                text.Foreground(wuxm::SolidColorBrush(StudioColor(rule->color, 0xFFFFFFFF)));
            }
        }
    }
}
void WalkStudioNative(wux::DependencyObject const& node, int depth) {
    if (!node || depth > 96 || (g_activeClockElement && node == g_activeClockElement)) return;
    if (auto e = node.try_as<wux::FrameworkElement>()) {
        if (e.Name() == L"WindhawkStudioCardPixelLayer" || e.Name()==L"WindhawkStudioWidgets") return;
        try { InspectStudioNative(e, depth); }
        catch (winrt::hresult_error const& error) {
            if (studio.diagnostics) Wh_Log(L"Studio native element %s failed: %s", e.Name().c_str(), error.message().c_str());
        }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) try {
        WalkStudioNative(wuxm::VisualTreeHelper::GetChild(node, i), depth + 1);
    } catch (...) {}
}
// STUDIO_WIDGET_IMPLEMENTATION
