// Original vector artwork: no icon font, image download or weather service.
// Unknown/localized conditions use a neutral thermometer, never guessed weather.
#include <winrt/Windows.UI.Xaml.Automation.h>
std::wstring StudioWeatherCondition(StudioWidgetSnapshot const& snapshot) {
    if(!studio.weatherIconCondition.empty() && studio.weatherIconCondition!=L"auto")
        return studio.weatherIconCondition;
    std::wstring text;
    auto labels=snapshot.conditionLabels;
    labels.insert(labels.end(),snapshot.lines.begin(),snapshot.lines.end());
    for (auto line:labels) {
        for (auto& c:line) c=static_cast<wchar_t>(towlower(c));
        if(line.find(L"warning")!=std::wstring::npos || line.find(L"alert")!=std::wstring::npos ||
           line.find(L"advisory")!=std::wstring::npos || line.find(L"forecast")!=std::wstring::npos) continue;
        text+=line; text+=L" ";
    }
    auto has=[&](wchar_t const* value) { return text.find(value)!=std::wstring::npos; };
    if(has(L"thunder") || has(L"lightning")) return L"storm";
    if(has(L"sleet") || has(L"freezing rain") || has(L"wintry mix")) return L"sleet";
    if(has(L"snow") || has(L"blizzard") || has(L"flurr")) return L"snow";
    if(has(L"rain") || has(L"drizzle") || has(L"shower")) return L"rain";
    if(has(L"fog") || has(L"mist") || has(L"haze")) return L"fog";
    bool night=has(L"night") || has(L"moon");
    if(has(L"partly") || has(L"mostly sunny") || has(L"mostly clear")) return night ? L"partlyNight" : L"partly";
    if(has(L"cloud") || has(L"overcast")) return L"cloud";
    if(night) return L"night";
    if(has(L"sunny") || has(L"clear")) return L"sun";
    if(has(L"wind") || has(L"breezy")) return L"wind";
    return L"unknown";
}
wux::UIElement BuildStudioWeatherIcon(StudioWidgetSnapshot const& snapshot,
                                      std::wstring const& theme,wu::Color accent,wu::Color foreground) {
    auto condition=StudioWeatherCondition(snapshot);
    bool pixel=theme==L"pixel",outline=theme==L"minimal";
    // Each theme has its own finish: square pixel art, fine outline, two-tone
    // Nordic, pastel Catppuccin, warm Gruvbox, or layered macOS-style color.
    auto hex=[](wu::Color c) {
        wchar_t value[10]; swprintf_s(value,L"#%02X%02X%02X%02X",c.A,c.R,c.G,c.B);
        return std::wstring(value);
    };
    auto warm=StudioColor(L"",theme==L"catppuccin" ? 0xFFF9E2AF :
        theme==L"gruvbox" ? 0xFFFABD2F : theme==L"nord" ? 0xFFEBCB8B :
        theme==L"pixel" ? 0xFFFFCF7D : 0xFFFFBD59);
    auto cool=StudioColor(L"",theme==L"catppuccin" ? 0xFF89B4FA :
        theme==L"gruvbox" ? 0xFF83A598 : theme==L"nord" ? 0xFF81A1C1 : 0xFF70BFFF);
    std::wstring xaml=L"<Canvas xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Width='72' Height='72'>";
    auto path=[&](wchar_t const* data,wu::Color color,bool fill=true) {
        xaml+=L"<Path Data='"+std::wstring(data)+L"' Stroke='"+hex(color)+L"' StrokeThickness='"+
            std::wstring(pixel ? L"2" : outline ? L"1.5" : L"2")+L"' Fill='"+
            std::wstring(fill && !outline ? hex(color) : L"Transparent")+L"'/>";
    };
    bool cloudy=condition==L"cloud" || condition==L"partly" || condition==L"partlyNight" || condition==L"rain" ||
        condition==L"storm" || condition==L"snow" || condition==L"sleet";
    if(condition==L"sun" || condition==L"partly") {
        path(pixel ? L"M22 10H38V14H42V30H38V34H22V30H18V14H22Z" :
            L"M42 22A12 12 0 1 1 18 22A12 12 0 1 1 42 22Z",warm);
        path(L"M30 2V6M30 38V42M10 22H14M46 22H50M15 7L18 10M42 34L45 37M15 37L18 34M42 10L45 7",warm,false);
    }
    if(condition==L"night" || condition==L"partlyNight") path(pixel ? L"M36 8H24V14H18V36H24V42H48V36H54V30H36V24H30V14H36Z" :
        L"M38 8C19 5 12 27 24 39C35 49 51 43 56 31C39 36 27 23 38 8Z",cool);
    if(cloudy) {
        path(pixel ? L"M10 34H16V24H22V18H36V24H42V28H56V34H62V46H56V50H10V46H6V38H10Z" :
            L"M16 48C2 48 3 30 17 30C17 12 42 12 44 29C64 25 70 48 54 48Z",foreground);
        if(theme==L"macos" || theme==L"macosDark" || theme==L"smoke")
            path(L"M17 44H53",accent,false);
    }
    if(condition==L"rain" || condition==L"sleet")
        path(pixel ? L"M18 54V62H22V54ZM34 54V66H38V54ZM50 54V62H54V54Z" :
            L"M22 55L18 63M38 55L34 65M54 55L50 63",cool,pixel);
    if(condition==L"snow" || condition==L"sleet")
        path(L"M23 55V67M17 61H29M19 57L27 65M19 65L27 57M48 55V67M42 61H54",accent,false);
    if(condition==L"storm") path(pixel ? L"M34 48H46L38 56H46L28 70L34 58H28Z" :
        L"M35 48H46L38 57H45L28 70L33 59H27Z",warm);
    if(condition==L"storm") path(pixel ? L"M14 54H18V62H14ZM52 54H56V62H52Z" :
        L"M17 54C9 64 22 66 17 54ZM54 54C46 64 59 66 54 54Z",cool);
    if(condition==L"fog" || condition==L"wind")
        path(pixel ? L"M10 22H50V26H10ZM18 36H62V40H18ZM8 50H48V54H8Z" :
            condition==L"wind" ? L"M9 25H45C60 25 56 10 48 15M17 37H58M8 49H38C53 49 51 63 43 59" :
            L"M14 24H53M8 36H63M18 48H55",accent,pixel);
    if(condition==L"unknown") {
        path(pixel ? L"M28 12H40V44H46V62H40V66H28V62H22V44H28Z" :
            L"M29 43V17C29 7 41 7 41 17V43C54 50 49 66 35 66C21 66 16 50 29 43Z",foreground,false);
        path(L"M35 24V53M30 55H40",accent,false);
    }
    xaml+=L"</Canvas>";
    auto icon=wux::Markup::XamlReader::Load(xaml).as<wuxc::Canvas>();
    wux::Automation::AutomationProperties::SetName(icon,
        condition==L"unknown" ? L"Weather" : L"Weather: "+condition);
    return icon;
}
