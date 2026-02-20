#pragma once
#include "SharedContext.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>
namespace IG = ImGui;
struct ImGuiHandler {
    ImGuiWindowFlags _flags = 0;
    SharedContext&   shared;
    ImGuiContext*    context = nullptr;

    ImGuiHandler(SharedContext& _shared) : shared(_shared) {}

    inline void init() {
        IMGUI_CHECKVERSION();
        context = IG::CreateContext();
        auto& io = IG::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        IG::StyleColorsDark();
        ImGuiStyle& style = IG::GetStyle();
        style.ScaleAllSizes(shared.mainScale);
        style.FontScaleDpi = shared.mainScale;

        ImGui_ImplGlfw_InitForOpenGL(shared.p_viewport, true);
        ImGui_ImplOpenGL3_Init(shared.p_glslVersion);
    }

    void drawUI();
    void destroy();
    struct WindowConfig {
        const Vec2     pos{0.0f, 0.0f};
        const Vec2     size{1.0f, 1.0f};
        const ImGuiIO& io;
        bool           shown = false;
    };

 private:
    MirroredRingBuf<float, 250> frameTimeSamples;

    // window functions shall take in a windowConfig
    // and return the bottom right corner pos, for tiling
    Vec2 drawPerformanceWindow(WindowConfig win);
    Vec2 drawWindowAttributeTest(WindowConfig win);
    Vec2 drawGraphGenerationWindow(WindowConfig win);
    Vec2 drawGraphVisualSettings(WindowConfig win);

    void imguiTextWithTooltip(const char* text, const char* tooltip);
    void setTooltipWrap(const char* tooltip);

#define WFLAG(flag) ImGuiWindowFlags_##flag
    enum WindowFlags {
        STATIC = WFLAG(NoMove) | WFLAG(NoResize) | WFLAG(NoScrollbar) | WFLAG(NoScrollWithMouse) |
                 ImGuiWindowFlags_NoDecoration,
        DYNAMIC = ImGuiWindowFlags_AlwaysAutoResize,
    };
};
