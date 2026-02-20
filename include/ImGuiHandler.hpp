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

    void composeUI();
    void destroy();

 private:
    void imguiTextWithTooltip(const char* text, const char* tooltip);
    // perf window
    bool showPerformanceWindow = true;
    Vec2 composePerformanceWindow(Vec2& viewport, ImGuiIO& io, Vec2 pos);
    void composeWindowAttributeTest(Vec2& viewport, ImGuiIO& io, Vec2 pos);
    void composeGraphGenerationWindow(Vec2& viewport, ImGuiIO& io, Vec2 pos);

    void setTooltipWrap(const char* tooltip);
    using WinFlag = ImGuiWindowFlags;

    WinFlag saveIniSettings = true;

    WinFlag staticWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    WinFlag staticWindowFlagsNoCollapse =
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            (saveIniSettings ? ImGuiWindowFlags_NoSavedSettings : 0x0);
    MirroredRingBuf<float, 100> frameTimeSamples;
};
