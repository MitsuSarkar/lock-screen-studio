// Root-level cards mirror the native widget text without reparenting Windows'
// controls. Native bindings remain active; no external weather API is called.
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
struct StudioWidgetSnapshot {
    std::vector<std::wstring> lines;
    std::vector<std::wstring> conditionLabels;
    wuxm::ImageSource image{nullptr};
    double imageArea=0;
};
struct StudioHiddenWidget {
    wux::FrameworkElement source{nullptr};
    winrt::Windows::Foundation::IInspectable opacity{nullptr},hitTest{nullptr};
};
bool StudioTemperature(std::wstring const& value) {
    bool number=false;
    for(wchar_t c:value) if(c>=L'0' && c<=L'9') number=true;
    return number && value.find(L'\x00B0')!=std::wstring::npos;
}
void NormalizeStudioWidgetLines(std::vector<std::wstring>& lines) {
    std::vector<std::wstring> normalized;
    for(auto const& line:lines) {
        bool unit=line==L"\x00B0" L"C" || line==L"\x00B0" L"F" || line==L"\x00B0";
        bool numeric=!normalized.empty() && !normalized.back().empty();
        if (numeric) for(wchar_t c:normalized.back())
            numeric=numeric && ((c>=L'0' && c<=L'9') || c==L'-' || c==L'+' || c==L'.' || c==L'\x2212');
        if (unit && numeric) normalized.back()+=L" "+line;
        else normalized.push_back(line);
    }
    lines=std::move(normalized);
}
[[clang::no_destroy]] StudioHiddenWidget studioHiddenWidget;
[[clang::no_destroy]] wuxc::Grid studioWidgetsRoot{nullptr};
[[clang::no_destroy]] wuxc::Grid studioWidgetsParent{nullptr};
[[clang::no_destroy]] wuxc::Canvas studioWidgetsHost{nullptr};
std::wstring studioWidgetsSignature;
double studioWidgetsNaturalWidth=0,studioWidgetsNaturalHeight=0;
void RestoreStudioWidgetSource() {
    if (!studioHiddenWidget.source) return;
    auto restore=[](wux::FrameworkElement const& e,wux::DependencyProperty const& property,
                    winrt::Windows::Foundation::IInspectable const& value) {
        if (value==wux::DependencyProperty::UnsetValue()) e.ClearValue(property);
        else e.SetValue(property,value);
    };
    try { restore(studioHiddenWidget.source,wux::UIElement::OpacityProperty(),studioHiddenWidget.opacity); } catch (...) {}
    try { restore(studioHiddenWidget.source,wux::UIElement::IsHitTestVisibleProperty(),studioHiddenWidget.hitTest); } catch (...) {}
    studioHiddenWidget.source=nullptr;
    studioHiddenWidget.opacity=nullptr;
    studioHiddenWidget.hitTest=nullptr;
}
void HideStudioWidgetSource() {
    if (!studioWidget) return;
    if (studioHiddenWidget.source!=studioWidget) {
        RestoreStudioWidgetSource();
        studioHiddenWidget.opacity=studioWidget.ReadLocalValue(wux::UIElement::OpacityProperty());
        studioHiddenWidget.hitTest=studioWidget.ReadLocalValue(wux::UIElement::IsHitTestVisibleProperty());
        studioHiddenWidget.source=studioWidget;
    }
    if (studioWidget.Opacity()!=0) studioWidget.Opacity(0);
    if (studioWidget.IsHitTestVisible()) studioWidget.IsHitTestVisible(false);
}
void ResetStudioWidgets() {
    RestoreStudioWidgetSource();
    if (studioWidgetsRoot && studioWidgetsParent) try {
        uint32_t index;
        if (studioWidgetsParent.Children().IndexOf(studioWidgetsRoot,index))
            studioWidgetsParent.Children().RemoveAt(index);
    } catch (...) {}
    studioWidgetsHost=nullptr;
    studioWidgetsRoot=nullptr;
    studioWidgetsParent=nullptr;
    studioWidgetsSignature.clear();
    studioWidgetsNaturalWidth=studioWidgetsNaturalHeight=0;
}
void ReadStudioWidgetSnapshot(wux::DependencyObject const& node,StudioWidgetSnapshot& snapshot,int depth,int& budget) {
    if (!node || depth>96 || --budget<0) return;
    if (auto e=node.try_as<wux::FrameworkElement>()) {
        if (e.Name()==L"WindhawkStudioCardPixelLayer" || e.Visibility()!=wux::Visibility::Visible) return;
        auto add=[&](winrt::hstring const& label) {
            std::wstring value(label);
            if(!value.empty() && value.size()<=512 && snapshot.conditionLabels.size()<128 &&
               std::find(snapshot.conditionLabels.begin(),snapshot.conditionLabels.end(),value)==snapshot.conditionLabels.end())
                snapshot.conditionLabels.push_back(value);
        };
        try {
            add(wux::Automation::AutomationProperties::GetName(e));
            add(wux::Automation::AutomationProperties::GetHelpText(e));
            // Weather artwork commonly exposes its condition only as an
            // accessibility name, rather than a visible TextBlock.
            if(e.try_as<wuxc::Image>() || e.try_as<wuxc::IconElement>()) {
                auto peer=wux::Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(e);
                if(peer) { add(peer.GetName()); add(peer.GetHelpText()); }
            }
        } catch (...) {}
    }
    if (auto text=node.try_as<wuxc::TextBlock>()) {
        auto family=std::wstring(text.FontFamily().Source());
        auto value=std::wstring(text.Text());
        bool glyph=family.find(L"Icons")!=std::wstring::npos || family.find(L"MDL2")!=std::wstring::npos;
        value.erase(0,value.find_first_not_of(L" \t\r\n"));
        auto last=value.find_last_not_of(L" \t\r\n");
        if (last!=std::wstring::npos) value.erase(last+1);
        if (!glyph && !value.empty() && value.size()<=256 && value!=L"\u2026" && value!=L"...") {
            bool privateGlyph=false;
            for(wchar_t c:value) if(c>=0xE000 && c<=0xF8FF) privateGlyph=true;
            if (!privateGlyph && (snapshot.lines.empty() || snapshot.lines.back()!=value)) snapshot.lines.push_back(value);
        }
    }
    if (auto image=node.try_as<wuxc::Image>()) {
        double area=image.ActualWidth()*image.ActualHeight();
        if (image.Source() && image.ActualWidth()>=24 && image.ActualHeight()>=24 && area>snapshot.imageArea) {
            snapshot.image=image.Source(); snapshot.imageArea=area;
        }
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for(int i=0;i<count && budget>0;++i) try {
        ReadStudioWidgetSnapshot(wuxm::VisualTreeHelper::GetChild(node,i),snapshot,depth+1,budget);
    } catch (...) {}
}
wux::UIElement StudioWidgetLabel(std::wstring const& value,double size,wu::Color color,double width,bool bold=false) {
    if (studio.widgetTheme==L"pixel" && studio.widgetFont.empty() && value.size()<=40) {
        wuxc::Grid pixel;
        RenderStudioPixel(pixel,value,size,color,color,false,0);
        double scale=(std::min)(1.0,width/pixel.Width());
        wuxc::Canvas box;
        box.Width(pixel.Width()*scale); box.Height(pixel.Height()*scale);
        wuxm::ScaleTransform transform; transform.ScaleX(scale); transform.ScaleY(scale);
        pixel.RenderTransform(transform);
        box.Children().Append(pixel);
        return box;
    }
    wuxc::TextBlock label;
    label.Text(value); label.FontSize(size);
    label.Foreground(wuxm::SolidColorBrush(color));
    label.FontFamily(wuxm::FontFamily(studio.widgetFont.empty() ? L"Segoe UI Variable Text" : studio.widgetFont));
    if (bold) label.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    label.TextWrapping(wux::TextWrapping::Wrap);
    label.MaxWidth(width);
    return label;
}
// STUDIO_WEATHER_ICONS
wux::FrameworkElement BuildStudioWidgetCard(StudioWidgetSnapshot const& snapshot) {
    auto theme=StudioCardSkinName(studio.widgetTheme) ? studio.widgetTheme : L"smoke";
    auto palette=StudioCardColors(theme);
    bool custom=studio.widgetCustomStyle;
    auto fg=StudioColor(custom ? studio.widgetForeground : L"",palette.foreground);
    auto accent=StudioColor(custom ? studio.widgetAccent : L"",palette.accent);
    auto bg=StudioColor(custom ? studio.widgetBackground : L"",palette.background);
    bg.A=static_cast<uint8_t>(bg.A*(custom ? studio.widgetOpacity : theme==L"smoke" ? 78 : 100)/100);
    if (theme==L"minimal" && (!custom || studio.widgetBackground.empty())) bg.A=0;
    double radius=custom ? studio.widgetRadius :
        theme==L"macos" || theme==L"macosDark" ? 24 : theme==L"catppuccin" ? 20 :
        theme==L"nord" ? 8 : theme==L"gruvbox" ? 4 : theme==L"minimal" ? 12 : 18;
    auto borderColor=accent;
    double borderWidth=1;
    if(theme==L"macos" || theme==L"macosDark") {
        borderColor=StudioColor(L"",theme==L"macos" ? 0xFFD2D2D7 : 0xFF38383A);
        borderWidth=0.5;
    } else if(theme==L"nord") borderColor=StudioColor(L"",0xFF4C566A);
    else if(theme==L"catppuccin") borderColor=StudioColor(L"",0xFF45475A);
    else if(theme==L"gruvbox") borderColor=StudioColor(L"",0xFF928374);
    if(custom && !studio.widgetAccent.empty()) borderColor=accent;
    wuxc::Border card;
    card.Width(studio.widgetCardWidth);
    card.Padding(wux::ThicknessHelper::FromUniformLength(18));
    card.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(theme==L"pixel" ? 0 : radius));
    card.BorderThickness(wux::ThicknessHelper::FromUniformLength(theme==L"pixel" ? 0 : borderWidth));
    card.BorderBrush(wuxm::SolidColorBrush(borderColor));
    card.Background(wuxm::SolidColorBrush(bg));
    wuxc::StackPanel body; body.Spacing(8);
    double width=studio.widgetCardWidth-36;
    size_t temperature=snapshot.lines.size();
    for(size_t i=0;i<snapshot.lines.size();++i) if(StudioTemperature(snapshot.lines[i])) {
        temperature=i; break;
    }
    if(temperature<snapshot.lines.size() && studio.weatherIconVisible &&
       (studio.weatherIconPosition==L"left" || studio.weatherIconPosition==L"right"))
        width=(std::max)(40.0,width-(std::min)(double(studio.weatherIconSize),width-52)-12);
    size_t titleIndex=snapshot.lines.size();
    for(size_t i=0;i<snapshot.lines.size();++i) if(i!=temperature) { titleIndex=i; break; }
    auto title=titleIndex<snapshot.lines.size() ? snapshot.lines[titleIndex] :
        std::wstring(temperature<snapshot.lines.size() ? L"Weather" : L"Windows widget");
    auto titleColor=theme==L"macos" || theme==L"macosDark" ? fg : accent;
    body.Children().Append(StudioWidgetLabel(title,studio.widgetTextSize ? studio.widgetTextSize : 18,titleColor,width,true));
    bool weather=temperature<snapshot.lines.size();
    if (!weather && snapshot.image) {
        wuxc::Image icon; icon.Source(snapshot.image); icon.Width(64); icon.Height(64);
        icon.Stretch(wuxm::Stretch::Uniform);
        icon.HorizontalAlignment(wux::HorizontalAlignment::Left);
        body.Children().Append(icon);
    }
    // Preserve values verbatim. No guessed weather condition or invented data.
    size_t shown=0;
    for(size_t i=0;i<snapshot.lines.size() && shown<7;++i) {
        if(i==titleIndex) continue;
        auto value=snapshot.lines[i];
        if (value==L"See full forecast" || value==L"See more" || value==L"Learn more") continue;
        bool numeric=!value.empty();
        for(wchar_t c:value) numeric=numeric && ((c>=L'0' && c<=L'9') || c==L'-' || c==L'+' || c==L'.');
        if (numeric && i+1<snapshot.lines.size() && snapshot.lines[i+1].find(L'\x00B0')!=std::wstring::npos) {
            value+=L" "+snapshot.lines[++i];
            temperature=i;
        }
        double size=i==temperature ? 42 : (studio.widgetTextSize ? studio.widgetTextSize : 16);
        body.Children().Append(StudioWidgetLabel(value,size,fg,width,i==temperature));
        ++shown;
    }
    if(weather && studio.weatherIconVisible) {
        auto condition=StudioWeatherCondition(snapshot);
        wux::UIElement artwork{nullptr};
        if(condition==L"unknown" && snapshot.image) {
            wuxc::Image native; native.Source(snapshot.image); native.Width(72); native.Height(72);
            native.Stretch(wuxm::Stretch::Uniform); artwork=native;
        } else artwork=BuildStudioWeatherIcon(snapshot,theme,accent,fg);
        wuxc::Canvas icon;
        double size=studio.weatherIconSize;
        if(studio.weatherIconPosition==L"left" || studio.weatherIconPosition==L"right")
            size=(std::min)(size,double(studio.widgetCardWidth-36-52));
        else size=(std::min)(size,double(studio.widgetCardWidth-36));
        icon.Width(size); icon.Height(size);
        wuxm::ScaleTransform scale; scale.ScaleX(size/72); scale.ScaleY(size/72);
        artwork.RenderTransform(scale); icon.Children().Append(artwork);
        wuxm::TranslateTransform offset; offset.X(studio.weatherIconX); offset.Y(studio.weatherIconY);
        icon.RenderTransform(offset);
        auto position=studio.weatherIconPosition;
        bool side=position==L"left" || position==L"right";
        bool bottom=position.find(L"bottom")==0;
        bool right=position==L"right" || position==L"topRight" || position==L"bottomRight";
        icon.HorizontalAlignment(right ? wux::HorizontalAlignment::Right :
            position==L"top" || position==L"bottom" ? wux::HorizontalAlignment::Center : wux::HorizontalAlignment::Left);
        icon.VerticalAlignment(side ? wux::VerticalAlignment::Center : wux::VerticalAlignment::Top);
        wuxc::Grid layout;
        if(side) {
            wuxc::ColumnDefinition a,b;
            a.Width({right ? 1.0 : size+12,right ? wux::GridUnitType::Star : wux::GridUnitType::Pixel});
            b.Width({right ? size+12 : 1.0,right ? wux::GridUnitType::Pixel : wux::GridUnitType::Star});
            layout.ColumnDefinitions().Append(a); layout.ColumnDefinitions().Append(b);
            wuxc::Grid::SetColumn(icon,right ? 1 : 0); wuxc::Grid::SetColumn(body,right ? 0 : 1);
            double textWidth=width;
            body.Width(textWidth);
            // Re-measure wrapping and pixel labels in the remaining text column.
            for(auto const& child:body.Children()) if(auto label=child.try_as<wuxc::TextBlock>()) label.MaxWidth(textWidth);
        } else {
            wuxc::RowDefinition a,b;
            a.Height({1,wux::GridUnitType::Auto}); b.Height({1,wux::GridUnitType::Auto});
            layout.RowDefinitions().Append(a); layout.RowDefinitions().Append(b);
            wuxc::Grid::SetRow(icon,bottom ? 1 : 0); wuxc::Grid::SetRow(body,bottom ? 0 : 1);
            icon.Margin(wux::ThicknessHelper::FromLengths(0,bottom ? 12 : 0,0,bottom ? 0 : 12));
        }
        layout.Children().Append(body); layout.Children().Append(icon); card.Child(layout);
    } else card.Child(body);
    if(theme==L"pixel") {
        // One stepped silhouette paints both fill and outline. The content
        // Border is transparent, so square Windows/theme outlines cannot stack.
        card.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
        card.Measure({static_cast<float>(studio.widgetCardWidth),100000});
        double w=studio.widgetCardWidth,h=card.DesiredSize().Height;
        auto n=[](double v) { return std::to_wstring(v); };
        auto hex=[](wu::Color c) { wchar_t s[10]; swprintf_s(s,L"#%02X%02X%02X%02X",c.A,c.R,c.G,c.B); return std::wstring(s); };
        auto data=L"M9,1 H"+n(w-9)+L" V5 H"+n(w-5)+L" V9 H"+n(w-1)+
            L" V"+n(h-9)+L" H"+n(w-5)+L" V"+n(h-5)+L" H"+n(w-9)+L" V"+n(h-1)+
            L" H9 V"+n(h-5)+L" H5 V"+n(h-9)+L" H1 V9 H5 V5 H9 Z";
        auto xml=L"<Path xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Data='"+
            data+L"' Fill='"+hex(bg)+L"' Stroke='"+hex(accent)+L"' StrokeThickness='2'/>";
        wuxc::Grid frame; frame.Width(w); frame.Height(h);
        frame.Children().Append(wux::Markup::XamlReader::Load(xml).as<wux::UIElement>());
        frame.Children().Append(card);
        return frame;
    }
    return card;
}
void PlaceStudioWidgets() {
    if (!studioWidgetsHost || !studioWidgetsRoot || !studioWidgetsParent) return;
    double w=studioWidgetsParent.ActualWidth(),h=studioWidgetsParent.ActualHeight();
    if (w<=0 || h<=0) return;
    double padding=studio.widgetScreenPadding;
    double scale=(std::min)({1.0,(std::max)(1.0,w-2*padding)/studioWidgetsNaturalWidth,
        (std::max)(1.0,h-2*padding)/studioWidgetsNaturalHeight});
    double width=studioWidgetsNaturalWidth*scale,height=studioWidgetsNaturalHeight*scale;
    studioWidgetsHost.Width(width); studioWidgetsHost.Height(height);
    auto row=studioWidgetsHost.Children().GetAt(0).as<wuxc::StackPanel>();
    auto previous=row.RenderTransform().try_as<wuxm::ScaleTransform>();
    if (!previous || previous.ScaleX()!=scale) {
        wuxm::ScaleTransform transform; transform.ScaleX(scale); transform.ScaleY(scale);
        row.RenderTransform(transform);
    }
    auto anchor=studio.widgetAnchor.empty() ? std::wstring(L"bottom") : studio.widgetAnchor;
    bool left=anchor==L"left" || anchor==L"topLeft" || anchor==L"bottomLeft";
    bool right=anchor==L"right" || anchor==L"topRight" || anchor==L"bottomRight";
    bool top=anchor==L"top" || anchor==L"topLeft" || anchor==L"topRight";
    bool bottom=anchor==L"bottom" || anchor==L"bottomLeft" || anchor==L"bottomRight";
    double baseX=left ? 64 : right ? w-width-64 : (w-width)/2;
    double baseY=top ? 64 : bottom ? h-height-64 : (h-height)/2;
    double x=studio.widgetX,y=studio.widgetY;
    if (studio.widgetKeepOnScreen) {
        x=MitsuPlacement::ClampOffset(baseX,width,w,padding,x);
        y=MitsuPlacement::ClampOffset(baseY,height,h,padding,y);
    }
    // Canvas positioning directly in the root bypasses the native widget slot.
    studioWidgetsHost.HorizontalAlignment(wux::HorizontalAlignment::Left);
    studioWidgetsHost.VerticalAlignment(wux::VerticalAlignment::Top);
    auto old=studioWidgetsHost.RenderTransform().try_as<wuxm::TranslateTransform>();
    if (!old || old.X()!=baseX+x || old.Y()!=baseY+y) {
        wuxm::TranslateTransform transform; transform.X(baseX+x); transform.Y(baseY+y);
        studioWidgetsHost.RenderTransform(transform);
    }
}
void UpdateStudioWidgets() {
    if (studio.widgetMode!=L"studio" || studio.widgetTheme==L"stock") {
        if(studioWidgetsRoot) ResetStudioWidgets(); return;
    }
    if (!studioWidget || !g_lockRootGrid) return;
    std::vector<StudioWidgetSnapshot> snapshots;
    std::wstring signature;
    for(auto const& frame:StudioWidgetFrames()) {
        StudioWidgetSnapshot snapshot;
        int budget=4096;
        ReadStudioWidgetSnapshot(frame,snapshot,0,budget);
        if (snapshot.lines.empty()) continue;
        NormalizeStudioWidgetLines(snapshot.lines);
        signature+=L"[card]";
        for(auto const& line:snapshot.lines) signature+=std::to_wstring(line.size())+L":"+line;
        for(auto const& label:snapshot.conditionLabels) signature+=L"[condition]"+std::to_wstring(label.size())+L":"+label;
        signature+=std::to_wstring(reinterpret_cast<uintptr_t>(winrt::get_abi(snapshot.image)));
        snapshots.push_back(std::move(snapshot));
        if (snapshots.size()==4) break;
    }
    if (snapshots.empty()) { ResetStudioWidgets(); return; }
    bool attached=false;
    if (studioWidgetsRoot && studioWidgetsParent==g_lockRootGrid) {
        uint32_t index; attached=g_lockRootGrid.Children().IndexOf(studioWidgetsRoot,index);
    }
    if (!attached || signature!=studioWidgetsSignature) {
        if(studio.diagnostics) for(auto const& snapshot:snapshots)
            Wh_Log(L"Studio weather detection: %s; %u accessibility labels. Unknown means no recognized condition.",
                StudioWeatherCondition(snapshot).c_str(),static_cast<unsigned>(snapshot.conditionLabels.size()));
        // Build the replacement before hiding the original. A failed build
        // leaves the Windows card available instead of producing an empty UI.
        wuxc::Grid root;
        root.Name(L"WindhawkStudioWidgets"); root.IsHitTestVisible(false);
        wuxc::Grid::SetRowSpan(root,100); wuxc::Grid::SetColumnSpan(root,100);
        wuxc::Canvas::SetZIndex(root,110);
        wuxc::StackPanel row; row.Orientation(wuxc::Orientation::Horizontal); row.Spacing(16);
        row.HorizontalAlignment(wux::HorizontalAlignment::Left);
        row.VerticalAlignment(wux::VerticalAlignment::Top);
        for(auto const& snapshot:snapshots) row.Children().Append(BuildStudioWidgetCard(snapshot));
        row.Measure({100000,100000});
        auto desired=row.DesiredSize();
        if (desired.Width<=0 || desired.Height<=0) return;
        row.Width(desired.Width); row.Height(desired.Height);
        wuxc::Canvas host; host.Children().Append(row); root.Children().Append(host);
        ResetStudioWidgets();
        g_lockRootGrid.Children().Append(root);
        studioWidgetsRoot=root; studioWidgetsHost=host; studioWidgetsParent=g_lockRootGrid;
        studioWidgetsNaturalWidth=desired.Width; studioWidgetsNaturalHeight=desired.Height;
        studioWidgetsSignature=signature;
        Wh_Log(L"Studio replacement widgets: %u card(s), independent root layout.",static_cast<unsigned>(snapshots.size()));
    }
    PlaceStudioWidgets();
    HideStudioWidgetSource();
}
