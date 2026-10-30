// Shared native-card skins. Data, album art, weather icons and native controls
// remain owned by Windows. This is a XAML skin, not a replacement data service.
bool StudioCardSkinName(std::wstring const& name) {
    return name == L"pixel" || name == L"macos" || name == L"macosDark" ||
        name == L"smoke" || name == L"minimal" || name == L"nord" ||
        name == L"catppuccin" || name == L"gruvbox";
}
struct StudioCardPalette { unsigned long background, foreground, accent; };
StudioCardPalette StudioCardColors(std::wstring const& name) {
    if (name == L"pixel") return {0xFF241D2D,0xFFFFE5C2,0xFFE6B890};
    if (name == L"macos") return {0xFFF5F5F7,0xFF1D1D1F,0xFF007AFF};
    if (name == L"macosDark") return {0xFF1C1C1E,0xFFF5F5F7,0xFF0A84FF};
    if (name == L"nord") return {0xFF2E3440,0xFFECEFF4,0xFF88C0D0};
    if (name == L"catppuccin") return {0xFF1E1E2E,0xFFCDD6F4,0xFFCBA6F7};
    if (name == L"gruvbox") return {0xFF282828,0xFFEBDBB2,0xFFFABD2F};
    return {0xFF1C2030,0xFFF1F5F9,0xFF94A3B8};
}
void StyleStudioCardSurface(wux::FrameworkElement const& e, bool widget, bool outer = true) {
    auto const& theme = widget ? studio.widgetTheme : studio.mediaTheme;
    auto colors = StudioCardColors(theme);
    auto const& bg = widget ? studio.widgetBackground : studio.mediaBackground;
    auto background = StudioColor(bg, colors.background);
    background.A = static_cast<uint8_t>(background.A * (widget ? studio.widgetOpacity : studio.mediaOpacity) / 100);
    if (theme == L"minimal" && bg.empty()) background.A = 0;
    // The card owner draws the only background and outline. Template layers
    // below it must not retain either Windows' border or another themed border.
    if (!outer) background.A = 0;
    auto brush = wuxm::SolidColorBrush(background);
    auto borderBrush = wuxm::SolidColorBrush(StudioColor(widget ? studio.widgetAccent : studio.mediaAccent, colors.accent));
    auto foreground = wuxm::SolidColorBrush(StudioColor(widget ? studio.widgetForeground : studio.mediaForeground, colors.foreground));
    double radius = theme == L"pixel" ? 0 : (widget ? studio.widgetRadius : studio.mediaRadius);
    auto corners = wux::CornerRadiusHelper::FromUniformRadius(radius);
    auto thickness = wux::ThicknessHelper::FromUniformLength(outer ? (theme == L"pixel" ? 2 : 1) : 0);
    if (!outer) borderBrush = wuxm::SolidColorBrush(wu::Colors::Transparent());
    if (e.try_as<wuxc::Border>()) {
        StudioSet(e, wuxc::Border::BackgroundProperty(), brush);
        StudioSet(e, wuxc::Border::BorderBrushProperty(), borderBrush);
        StudioSet(e, wuxc::Border::BorderThicknessProperty(), winrt::box_value(thickness));
        StudioSet(e, wuxc::Border::CornerRadiusProperty(), winrt::box_value(corners));
    } else if (e.try_as<wuxc::Control>()) {
        StudioSet(e, wuxc::Control::BackgroundProperty(), brush);
        StudioSet(e, wuxc::Control::ForegroundProperty(), foreground);
        StudioSet(e, wuxc::Control::BorderBrushProperty(), borderBrush);
        StudioSet(e, wuxc::Control::BorderThicknessProperty(), winrt::box_value(thickness));
        StudioSet(e, wuxc::Control::CornerRadiusProperty(), winrt::box_value(corners));
    } else if (e.try_as<wuxc::Grid>()) {
        StudioSet(e, wuxc::Panel::BackgroundProperty(), brush);
        StudioSet(e, wuxc::Grid::BorderBrushProperty(), borderBrush);
        StudioSet(e, wuxc::Grid::BorderThicknessProperty(), winrt::box_value(thickness));
        StudioSet(e, wuxc::Grid::CornerRadiusProperty(), winrt::box_value(corners));
    } else if (e.try_as<wuxc::ContentPresenter>()) {
        StudioSet(e, wuxc::ContentPresenter::BackgroundProperty(), brush);
        StudioSet(e, wuxc::ContentPresenter::BorderBrushProperty(), borderBrush);
        StudioSet(e, wuxc::ContentPresenter::BorderThicknessProperty(), winrt::box_value(thickness));
        StudioSet(e, wuxc::ContentPresenter::CornerRadiusProperty(), winrt::box_value(corners));
    }
}
bool StudioCardSurfaceType(wux::FrameworkElement const& e) {
    return e.try_as<wuxc::Border>() || e.try_as<wuxc::Control>() ||
        e.try_as<wuxc::Grid>() || e.try_as<wuxc::ContentPresenter>();
}
wux::FrameworkElement FindStudioCardPart(wux::DependencyObject const& node, bool frame, int depth, int& budget) {
    if (!node || depth>32 || --budget<0) return nullptr;
    if (auto element=node.try_as<wux::FrameworkElement>()) {
        if (element.Name()==L"WindhawkStudioCardPixelLayer") return nullptr;
        if (frame) {
            auto type=std::wstring(winrt::get_class_name(element));
            if (type.find(L"LockCanvasWidgetFrame")!=std::wstring::npos) return element;
        } else if (element.Name()==L"WidgetFrameGrid" && element.try_as<wuxc::Grid>()) return element;
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i=0;i<count && budget>0;++i) try {
        auto result=FindStudioCardPart(wuxm::VisualTreeHelper::GetChild(node,i),frame,depth+1,budget);
        if (result) return result;
    } catch (...) {}
    return nullptr;
}
wux::FrameworkElement StudioCardOwner(wux::FrameworkElement const& e, bool widget) {
    wux::FrameworkElement frame{nullptr}, fallback{nullptr};
    wux::DependencyObject node=e;
    for (int depth=0;node && depth<96;++depth) {
        if (widget && node==studioWidget) break;
        if (auto element=node.try_as<wux::FrameworkElement>()) {
            auto name=element.Name();
            if (name==L"LockRootGrid" || name==L"MainPageContainer") break;
            if (widget) {
                // Prefer the actual per-card frame, not both it and its inner
                // WidgetFrameGrid. Fallback picks the individual slot surface.
                auto type=std::wstring(winrt::get_class_name(element));
                if (type.find(L"LockCanvasWidgetFrame")!=std::wstring::npos) frame=element;
                if (StudioCardSurfaceType(element)) fallback=element;
            } else if (StudioMediaName(name)) frame=element;
        }
        node=wuxm::VisualTreeHelper::GetParent(node);
    }
    if (widget) {
        // Slot presenters above a frame must also resolve to that frame's
        // surface. Otherwise both slot and frame would claim their own border.
        if (!frame) { int budget=512; frame=FindStudioCardPart(e,true,0,budget); }
        if (frame) {
            int budget=512;
            auto grid=FindStudioCardPart(frame,false,0,budget);
            return grid ? grid : frame;
        }
    }
    // A legacy single-card root can itself be the selected widget container.
    if (widget && !frame && !fallback && studioWidget && e==studioWidget &&
        e.Name()!=L"WidgetGroupPanel" && StudioCardSurfaceType(e)) return e;
    return frame ? frame : fallback;
}
void StyleStudioCardChrome(wux::FrameworkElement const& e, bool widget) {
    if (!StudioCardSurfaceType(e)) return;
    auto owner=StudioCardOwner(e,widget);
    if (!owner) return;
    if (e==owner) { StyleStudioCardSurface(e,widget,true); return; }
    auto name=e.Name();
    bool wrapper=name==L"WidgetFrameGrid" || name==L"RootGrid" ||
        name==L"BackgroundBorder" || name==L"LayoutRoot";
    // Also suppress unnamed same-size template chrome. Small controls, album
    // art and content panels are not given full-card outlines or backgrounds.
    bool fullCard=owner.ActualWidth()>0 && owner.ActualHeight()>0 &&
        e.ActualWidth()>=owner.ActualWidth()*0.75 &&
        e.ActualHeight()>=owner.ActualHeight()*0.75;
    if (wrapper || fullCard) StyleStudioCardSurface(e,widget,false);
}
bool StudioInsideWidget(wux::DependencyObject node) {
    for (int i = 0; node && i < 96; ++i) {
        if (studioWidget && node == studioWidget) return true;
        node = wuxm::VisualTreeHelper::GetParent(node);
    }
    return false;
}
bool StudioWidgetWrapper(wux::FrameworkElement const& e) {
    if (!studioWidget || !studioWidgetMotion || studioWidget==studioWidgetMotion) return false;
    auto node=studioWidget ? wuxm::VisualTreeHelper::GetParent(studioWidget) : nullptr;
    for (int i=0;node && i<96;++i) {
        if (node==e) return true;
        if (studioWidgetMotion && node==studioWidgetMotion) break;
        node=wuxm::VisualTreeHelper::GetParent(node);
    }
    return false;
}
struct StudioPixelLayer { wuxc::Grid parent{nullptr}; wuxc::Canvas canvas{nullptr}; };
struct StudioPixelLabel {
    winrt::weak_ref<wuxc::TextBlock> source;
    wuxc::Canvas layer{nullptr};
    wuxc::Grid image{nullptr};
    std::wstring text, color;
    double size = 0;
    unsigned long long scan = 0;
};
[[clang::no_destroy]] std::vector<StudioPixelLayer> studioPixelLayers;
[[clang::no_destroy]] std::vector<StudioPixelLabel> studioPixelLabels;
void ResetStudioCardOverlays() {
    studioPixelLabels.clear();
    for (auto& layer : studioPixelLayers) try {
        uint32_t index;
        if (layer.parent.Children().IndexOf(layer.canvas, index)) layer.parent.Children().RemoveAt(index);
    } catch (...) {}
    studioPixelLayers.clear();
}
void FinishStudioCardOverlays() {
    studioPixelLabels.erase(std::remove_if(studioPixelLabels.begin(), studioPixelLabels.end(),
        [](StudioPixelLabel const& label) {
            if (label.scan == studioNativeScan && label.source.get()) return false;
            try { uint32_t i; if (label.layer.Children().IndexOf(label.image, i)) label.layer.Children().RemoveAt(i); } catch (...) {}
            return true;
        }), studioPixelLabels.end());
    studioPixelLayers.erase(std::remove_if(studioPixelLayers.begin(), studioPixelLayers.end(),
        [](StudioPixelLayer const& layer) {
            if (layer.canvas.Children().Size()) return false;
            try { uint32_t i; if (layer.parent.Children().IndexOf(layer.canvas, i)) layer.parent.Children().RemoveAt(i); } catch (...) {}
            return true;
        }), studioPixelLayers.end());
}
void DropStudioPixelLabel(wuxc::TextBlock const& text) {
    studioPixelLabels.erase(std::remove_if(studioPixelLabels.begin(),studioPixelLabels.end(),
        [&text](StudioPixelLabel const& label) {
            if (label.source.get() != text) return false;
            uint32_t index;
            if (label.layer.Children().IndexOf(label.image,index)) label.layer.Children().RemoveAt(index);
            return true;
        }),studioPixelLabels.end());
    studioNative.erase(std::remove_if(studioNative.begin(),studioNative.end(),
        [&text](StudioNativeValue const& saved) {
            if (saved.element != text || saved.property != wux::UIElement::OpacityProperty()) return false;
            if (saved.value == wux::DependencyProperty::UnsetValue()) text.ClearValue(saved.property);
            else text.SetValue(saved.property,saved.value);
            return true;
        }),studioNative.end());
}
void StudioPixelizeLabel(wuxc::TextBlock const& text, wu::Color color) {
    // Keep wrapping paragraphs native. Single-line values (temperature, city,
    // title, artist) get pixel outlines without replacing their data bindings.
    if (text.Text().empty() || text.TextWrapping() != wux::TextWrapping::NoWrap ||
        text.Visibility() != wux::Visibility::Visible ||
        text.ActualWidth() <= 0 || text.ActualHeight() <= 0) {
        DropStudioPixelLabel(text);
        return;
    }
    wuxc::Grid parent{nullptr};
    auto node = wuxm::VisualTreeHelper::GetParent(text);
    for (int i = 0; node && i < 32; ++i) {
        if ((parent = node.try_as<wuxc::Grid>())) break;
        node = wuxm::VisualTreeHelper::GetParent(node);
    }
    if (!parent) return;
    wuxc::Canvas layer{nullptr};
    for (auto const& candidate : studioPixelLayers)
        if (candidate.parent == parent) { layer = candidate.canvas; break; }
    if (!layer) {
        layer = wuxc::Canvas();
        layer.Name(L"WindhawkStudioCardPixelLayer");
        layer.IsHitTestVisible(false);
        wuxc::Grid::SetRowSpan(layer,100);
        wuxc::Grid::SetColumnSpan(layer,100);
        wuxc::Canvas::SetZIndex(layer,99);
        parent.Children().Append(layer);
        studioPixelLayers.push_back({parent,layer});
    }
    auto found = std::find_if(studioPixelLabels.begin(),studioPixelLabels.end(),
        [&text](StudioPixelLabel const& label) { return label.source.get() == text; });
    if (found != studioPixelLabels.end() && found->layer != layer) {
        uint32_t index;
        if (found->layer.Children().IndexOf(found->image,index)) found->layer.Children().RemoveAt(index);
        studioPixelLabels.erase(found);
        found = studioPixelLabels.end();
    }
    if (found == studioPixelLabels.end()) {
        wuxc::Grid image;
        layer.Children().Append(image);
        studioPixelLabels.push_back({winrt::make_weak(text),layer,image});
        found = studioPixelLabels.end()-1;
    }
    found->scan = studioNativeScan;
    auto hex = StudioPixelHex(color);
    if (found->text != text.Text() || found->size != text.FontSize() || found->color != hex) {
        RenderStudioPixel(found->image,std::wstring(text.Text()),text.FontSize(),color,color,false,0);
        found->text = text.Text(); found->size = text.FontSize(); found->color = hex;
    }
    auto bounds = text.TransformToVisual(layer).TransformBounds(
        winrt::Windows::Foundation::Rect{0,0,static_cast<float>(text.ActualWidth()),static_cast<float>(text.ActualHeight())});
    double scale = (std::min)({1.0,bounds.Width/found->image.Width(),bounds.Height/found->image.Height()});
    if (!std::isfinite(scale) || scale <= 0) return;
    auto old = found->image.RenderTransform().try_as<wuxm::ScaleTransform>();
    if (!old || old.ScaleX() != scale) {
        wuxm::ScaleTransform transform; transform.ScaleX(scale); transform.ScaleY(scale);
        found->image.RenderTransform(transform);
    }
    double x = bounds.X;
    if (text.TextAlignment() == wux::TextAlignment::Center) x += (bounds.Width-found->image.Width()*scale)/2;
    if (text.TextAlignment() == wux::TextAlignment::Right) x += bounds.Width-found->image.Width()*scale;
    wuxc::Canvas::SetLeft(found->image,x);
    wuxc::Canvas::SetTop(found->image,bounds.Y+(bounds.Height-found->image.Height()*scale)/2);
    StudioSet(text,wux::UIElement::OpacityProperty(),winrt::box_value(0.0));
}
void StyleStudioCardText(wuxc::TextBlock const& text,bool widget) {
    auto family = text.FontFamily().Source();
    if (family == L"Segoe Fluent Icons" || family == L"Segoe MDL2 Assets" || family == L"Segoe UI Symbol") return;
    auto const& theme = widget ? studio.widgetTheme : studio.mediaTheme;
    auto const& font=widget ? studio.widgetFont : studio.mediaFont;
    if (!font.empty()) {
        DropStudioPixelLabel(text);
        StudioSet(text,wuxc::TextBlock::FontFamilyProperty(),wuxm::FontFamily(font));
    }
    if (!StudioCardSkinName(theme)) return;
    auto palette = StudioCardColors(theme);
    auto color = StudioColor(widget ? studio.widgetForeground : studio.mediaForeground,palette.foreground);
    StudioSet(text,wuxc::TextBlock::ForegroundProperty(),wuxm::SolidColorBrush(color));
    int size = widget ? studio.widgetTextSize : studio.mediaTextSize;
    if (size) StudioSet(text,wuxc::TextBlock::FontSizeProperty(),winrt::box_value(static_cast<double>((std::max)(10,size))));
    if (theme == L"pixel" && font.empty()) StudioPixelizeLabel(text,color);
}
void CollectStudioWidgetFrames(wux::DependencyObject const& node,std::vector<wux::FrameworkElement>& frames,int depth,int& budget) {
    if (!node || depth>96 || --budget<0 || frames.size()>=8) return;
    if (auto e=node.try_as<wux::FrameworkElement>()) {
        if (e.Name()==L"WindhawkStudioCardPixelLayer" || e.Name()==L"WindhawkStudioWidgets") return;
        auto type=std::wstring(winrt::get_class_name(e));
        if (type.find(L"LockCanvasWidgetFrame")!=std::wstring::npos || e.Name()==L"WidgetFrameGrid") {
            int remaining=512;
            auto grid=FindStudioCardPart(e,false,0,remaining);
            frames.push_back(grid ? grid : e);
            return;
        }
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for(int i=0;i<count && budget>0;++i) try {
        CollectStudioWidgetFrames(wuxm::VisualTreeHelper::GetChild(node,i),frames,depth+1,budget);
    } catch (...) {}
}
std::vector<wux::FrameworkElement> StudioWidgetFrames() {
    std::vector<wux::FrameworkElement> frames;
    int budget=8192;
    if (studioWidget) CollectStudioWidgetFrames(studioWidget,frames,0,budget);
    if (frames.empty() && studioWidget) frames.push_back(studioWidget);
    return frames;
}
void ClampStudioWidgetOffset(double& x,double& y) {
    if (!studio.widgetKeepOnScreen || !studioWidget || !studioWidgetMotion) return;
    auto window = wux::Window::Current();
    auto content = window ? window.Content().try_as<wux::FrameworkElement>() : nullptr;
    if (!content || content.ActualWidth() <= 0 || content.ActualHeight() <= 0) return;
    winrt::Windows::Foundation::Rect bounds{};
    bool first=true;
    for(auto const& frame:StudioWidgetFrames()) {
        if (frame.ActualWidth()<=0 || frame.ActualHeight()<=0) continue;
        auto rect=frame.TransformToVisual(content).TransformBounds(
            winrt::Windows::Foundation::Rect{0,0,static_cast<float>(frame.ActualWidth()),static_cast<float>(frame.ActualHeight())});
        if (first) { bounds=rect; first=false; }
        else {
            float right=(std::max)(bounds.X+bounds.Width,rect.X+rect.Width);
            float bottom=(std::max)(bounds.Y+bounds.Height,rect.Y+rect.Height);
            bounds.X=(std::min)(bounds.X,rect.X); bounds.Y=(std::min)(bounds.Y,rect.Y);
            bounds.Width=right-bounds.X; bounds.Height=bottom-bounds.Y;
        }
    }
    if (bounds.Width <= 0 || bounds.Height <= 0) { x=0; y=0; return; }
    // Include shared ancestor viewports, not just the display edges.
    double visibleLeft=0,visibleTop=0,visibleRight=content.ActualWidth(),visibleBottom=content.ActualHeight();
    auto parent=wuxm::VisualTreeHelper::GetParent(studioWidgetMotion);
    for(int depth=0;parent && depth<96;++depth) {
        if (auto e=parent.try_as<wux::FrameworkElement>()) {
            auto clip=e.Clip();
            bool viewport=e.try_as<wuxc::ScrollViewer>() || e.try_as<wuxc::ScrollContentPresenter>();
            if (clip || viewport) {
                auto local=clip ? clip.Rect() : winrt::Windows::Foundation::Rect{
                    0,0,static_cast<float>(e.ActualWidth()),static_cast<float>(e.ActualHeight())};
                if (clip && clip.Transform()) local=clip.Transform().TransformBounds(local);
                auto rect=e.TransformToVisual(content).TransformBounds(local);
                visibleLeft=(std::max)(visibleLeft,static_cast<double>(rect.X));
                visibleTop=(std::max)(visibleTop,static_cast<double>(rect.Y));
                visibleRight=(std::min)(visibleRight,static_cast<double>(rect.X+rect.Width));
                visibleBottom=(std::min)(visibleBottom,static_cast<double>(rect.Y+rect.Height));
            }
        }
        if (parent==content) break;
        parent=wuxm::VisualTreeHelper::GetParent(parent);
    }
    if (visibleRight<=visibleLeft || visibleBottom<=visibleTop) { x=0; y=0; return; }
    double previousX=0,previousY=0;
    for (auto const& saved : studioNative) {
        if (saved.element != studioWidgetMotion || saved.property != wux::UIElement::RenderTransformProperty()) continue;
        auto group = studioWidgetMotion.RenderTransform().try_as<wuxm::TransformGroup>();
        if (group && group.Children().Size())
            if (auto last = group.Children().GetAt(group.Children().Size()-1).try_as<wuxm::TranslateTransform>()) {
                previousX=last.X(); previousY=last.Y();
            }
        break;
    }
    auto transform=studioWidgetMotion.TransformToVisual(content);
    auto origin=transform.TransformPoint({0,0});
    auto unitX=transform.TransformPoint({1,0});
    auto unitY=transform.TransformPoint({0,1});
    auto safe=MitsuPlacement::KeepVisible(bounds.X-visibleLeft,bounds.Y-visibleTop,bounds.Width,bounds.Height,
        visibleRight-visibleLeft,visibleBottom-visibleTop,studio.widgetScreenPadding,
        {previousX,previousY},{x,y},{unitX.X-origin.X,unitX.Y-origin.Y},{unitY.X-origin.X,unitY.Y-origin.Y});
    x=safe.x; y=safe.y;
}
