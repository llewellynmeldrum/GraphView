#include "imgui.h"
#include <string>
#include <vector>

#include <ImGuiHandler.hpp>
struct WindowFlag {
    ImGuiWindowFlags mask;
    const char*      name;
    const char*      tooltip;
    bool             toggle;
    std::string      id = "##" + (std::string)name;
};

ImGuiWindowFlags updateFlags(ImGuiWindowFlags flags, WindowFlag& wf) {
    if (wf.toggle) {
        printf("after (%d&%d=%d)\n", flags, wf.mask, flags | wf.mask);
        flags |= wf.mask;
    } else {
        flags &= ~(wf.mask);
    }

    return flags;
}

//clang-format off
std::vector<WindowFlag> windowFlags = {
        {ImGuiWindowFlags_None, "ImGuiWindowFlags_None", "No flags"},
        {ImGuiWindowFlags_NoTitleBar, "ImGuiWindowFlags_NoTitleBar",
         "Disable title-bar                                                                        "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoResize, "ImGuiWindowFlags_NoResize",
         "Disable user resizing with the lower-right grip                                          "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoMove, "ImGuiWindowFlags_NoMove",
         "Disable user moving the window                                                           "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoScrollbar, "ImGuiWindowFlags_NoScrollbar",
         "Disable scrollbars (window can still scroll with mouse or programmatically)              "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoScrollWithMouse, "ImGuiWindowFlags_NoScrollWithMouse",
         "Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be "
         "forwarded to the parent unless NoScrollbar is also set.                                  "
         "                                              "},
        {ImGuiWindowFlags_NoCollapse, "ImGuiWindowFlags_NoCollapse",
         "Disable user collapsing window by double-clicking on it. Also referred to as Window Menu "
         "Button (e.g. within a docking node).                                                     "
         "                                              "},
        {ImGuiWindowFlags_AlwaysAutoResize, "ImGuiWindowFlags_AlwaysAutoResize",
         "Resize every window to its content every frame                                           "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoBackground, "ImGuiWindowFlags_NoBackground",
         "Disable drawing background color (WindowBg, etc.) and outside border. Similar as using "
         "SetNextWindowBgAlpha(0.0f).                                                              "
         "                                                "},
        {ImGuiWindowFlags_NoSavedSettings, "ImGuiWindowFlags_NoSavedSettings",
         "Never load/save settings in .ini file                                                    "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoMouseInputs, "ImGuiWindowFlags_NoMouseInputs",
         "Disable catching mouse, hovering test with pass through.                                 "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_MenuBar, "ImGuiWindowFlags_MenuBar",
         "Has a menu-bar                                                                           "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_HorizontalScrollbar, "ImGuiWindowFlags_HorizontalScrollbar",
         "Allow horizontal scrollbar to appear (off by default). You may use "
         "SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. "
         "Read code in imgui_demo in the Horizontal Scrolling section.      "},
        {ImGuiWindowFlags_NoFocusOnAppearing, "ImGuiWindowFlags_NoFocusOnAppearing",
         "Disable taking focus when transitioning from hidden to visible state                     "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoBringToFrontOnFocus, "ImGuiWindowFlags_NoBringToFrontOnFocus",
         "Disable bringing window to front when taking focus (e.g. clicking on it or "
         "programmatically giving it focus)                                                        "
         "                                                            "},
        {ImGuiWindowFlags_AlwaysVerticalScrollbar, "ImGuiWindowFlags_AlwaysVerticalScrollbar",
         "Always show vertical scrollbar (even if ContentSize.y < Size.y)                          "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_AlwaysHorizontalScrollbar, "ImGuiWindowFlags_AlwaysHorizontalScrollbar",
         "Always show horizontal scrollbar (even if ContentSize.x < Size.x)                        "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoNavInputs, "ImGuiWindowFlags_NoNavInputs",
         "No keyboard/gamepad navigation within the window                                         "
         "                                                                                         "
         "                                              "},
        {ImGuiWindowFlags_NoNavFocus, "ImGuiWindowFlags_NoNavFocus",
         "No focusing toward this window with keyboard/gamepad navigation (e.g. skipped by "
         "Ctrl+Tab)                                                                                "
         "                                                      "},
        {ImGuiWindowFlags_UnsavedDocument, "ImGuiWindowFlags_UnsavedDocument",
         "Display a dot next to the title. When used in a tab/docking context, tab is selected "
         "when clicking the X + closure is not assumed (will wait for user to stop submitting the "
         "tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the "
         "tab may reappear at end of tab bar."},

};
// clang-format on

Vec2 ImGuiHandler::drawWindowAttributeTest(WindowConfig win) {
    // Window which has window attributes toggleable via checkboxes
    IG::Begin("Testing Attributes", &win.shown, _flags);
    IG::Text("%d", _flags);
    for (auto& wf : windowFlags) {
        IG::PushID(wf.mask);
        imguiTextWithTooltip(wf.name, wf.id.c_str());
        IG::Text("%d", wf.toggle);
        IG::SameLine();
        IG::Checkbox(wf.id.c_str(), &(wf.toggle));
        _flags = updateFlags(_flags, wf);
        IG::PopID();
    }
    IG::End();
    return {0, 0};
}
