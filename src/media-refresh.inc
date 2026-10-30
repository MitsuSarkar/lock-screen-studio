// All work stays on LockApp's UI thread. Weak references cannot keep an old
// media tree alive and no event handlers can survive DLL unload.
[[clang::no_destroy]] winrt::weak_ref<wux::FrameworkElement> studioFastMedia;
ULONGLONG studioLastMediaRefresh = 0;
bool studioWasVisible = false;
ULONGLONG studioLastWidgetDiscovery = 0;
void ResetStudioMediaRefresh() {
    studioFastMedia = {};
    studioLastMediaRefresh = 0;
    studioWasVisible = false;
    studioLastWidgetDiscovery = 0;
}
wux::FrameworkElement FindStudioMedia(wux::DependencyObject const& node, int depth, int& budget) {
    if (!node || depth > 96 || --budget < 0 ||
        (g_activeClockElement && node == g_activeClockElement)) return nullptr;
    if (auto element = node.try_as<wux::FrameworkElement>())
        if (StudioMediaName(element.Name())) return element;
    int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count && budget > 0; ++i) try {
        auto match = FindStudioMedia(wuxm::VisualTreeHelper::GetChild(node, i), depth + 1, budget);
        if (match) return match;
    } catch (...) {}
    return nullptr;
}
bool StudioMediaAttached(wux::DependencyObject node, wux::DependencyObject const& content) {
    for (int i = 0; node && i < 96; ++i) {
        if (node == content) return true;
        node = wuxm::VisualTreeHelper::GetParent(node);
    }
    return false;
}
void RefreshStudioMedia(ULONGLONG now) {
    auto window = wux::Window::Current();
    if (!window || !window.Visible()) {
        studioWasVisible = false;
        return;
    }
    bool justVisible = !studioWasVisible;
    studioWasVisible = true;
    if (!justVisible && now - studioLastMediaRefresh < 125) return;
    studioLastMediaRefresh = now;
    auto content = window.Content();
    if (!content) return;
    bool widgetsWanted = studio.widgetMode==L"studio" || StudioCardSkinName(studio.widgetTheme) || !studio.widgetFont.empty() || studio.widgetX || studio.widgetY;
    if (widgetsWanted) {
        bool attached = studioWidget && StudioMediaAttached(studioWidget,content);
        if (!attached || justVisible || now-studioLastWidgetDiscovery >= 500) {
            SelectStudioWidget(content);
            studioLastWidgetDiscovery = now;
        }
        if (studioWidget) {
            if (studio.widgetMode==L"studio") UpdateStudioWidgets();
            else {
                if (studioWidgetMotion && studioWidgetMotion != studioWidget) InspectStudioNative(studioWidgetMotion,0);
                WalkStudioNative(studioWidget,0);
            }
        }
    }
    bool themed = StudioCardSkinName(studio.mediaTheme);
    if (!themed && studio.mediaFont.empty() && !studio.hideMedia && !studio.mediaX && !studio.mediaY) return;
    auto media = studioFastMedia.get();
    if (!media || !StudioMediaAttached(media, content)) {
        int budget = 8192;
        media = FindStudioMedia(content, 0, budget);
        studioFastMedia = media ? winrt::make_weak(media) : winrt::weak_ref<wux::FrameworkElement>{};
    }
    if (media) {
        // Do not finish/prune a full native scan after a subtree-only pass:
        // widget and other rule targets must keep their saved original values.
        WalkStudioNative(media, 0);
    }
}
