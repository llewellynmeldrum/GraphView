#include "ImGuiHandler.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

void ImGuiHandler::composeGraphGenerationWindow(Vec2& viewport, ImGuiIO& io, Vec2 pos) {
    IG::SetNextWindowSize(viewport * Vec2{0.4f, 0.4f});
    IG::Begin("Graph Generation", nullptr, ImGuiWindowFlags_None);

    // ImGui::InputInt(const char* label, int* v, int step = 1, int step_fast = 100,
    // ImGuiInputTextFlags flags = 0);
    int before = shared.graphInitConfig.V;
    IG::InputInt("V", &(shared.graphInitConfig.V));
    if (shared.graphInitConfig.V != before) println("{}->{}", before, shared.graphInitConfig.V);
    IG::SetTooltip("Number of vertices (nodes) in the graph.");
    IG::InputInt("E", &shared.graphInitConfig.E);
    IG::SetTooltip("Number of edges in the graph.");

    IG::SetTooltip("");
    IG::SliderFloat2("worldPos", glm::value_ptr(shared.temp.centerWorld), -100, 100);
    IG::SliderFloat("radius", &shared.temp.radWorld, 0, 100);
    IG::SliderFloat4("color", glm::value_ptr(shared.temp.uCol), 0.0f, 1.0f);

    if (ImGui::Button("Generate Graph")) {
        shared.uiRequestsGraphGeneration = true;
    }

    if (ImGui::Button("Print adjlist")) {
        shared.graphConfig.ptr->debug_print_adj();
    }
    if (ImGui::Button("Print edges")) {
        shared.graphConfig.ptr->debug_print_edges();
    }

    if (ImGui::Button("Print positions")) {
        shared.graphConfig.ptr->debug_print_positions();
    }
    IG::End();
}
void ImGuiHandler::composeUI() {
    auto& io = IG::GetIO();
    frameTimeSamples.write(io.DeltaTime * 1000.0f);

    if (glfwGetWindowAttrib(shared.p_viewport, GLFW_ICONIFIED) != 0) {
        ImGui_ImplGlfw_Sleep(10);
        return;
    }
    Vec2 viewport_sz = IG::GetMainViewport()->Size;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    IG::NewFrame();
    {
        auto rightCorner = composePerformanceWindow(viewport_sz, io, Vec2{0, 0});

        // composeWindowAttributeTest(viewport_sz, io, Vec2{100,100});

        composeGraphGenerationWindow(viewport_sz, io, Vec2{0, rightCorner.y});

        //        IG::SetNextWindowSize(viewport_sz * Vec2{0.4f, 0.4f});
        //        IG::ShowDebugLogWindow();
    }
    IG::Render();  // calls IG::endFrame()
}
Vec2 ImGuiHandler::composePerformanceWindow(Vec2& viewport, ImGuiIO& io, Vec2 pos) {
    Vec2 window_sz = viewport * Vec2{1.0f, 0.1f};
    IG::SetNextWindowPos(pos);
    IG::SetNextWindowSize(window_sz);

    IG::Begin("Performance", &showPerformanceWindow, staticWindowFlags & ImGuiWindowFlags_MenuBar);
    if (IG::BeginMenuBar()) {
        if (IG::BeginMenu("File")) {
            if (IG::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */
            }
            if (IG::MenuItem("Save", "Ctrl+S")) { /* Do stuff */
            }
            if (IG::MenuItem("Close", "Ctrl+W")) {
                showPerformanceWindow = false;
            }
            IG::EndMenu();
        }
        IG::EndMenuBar();
    }

    float fps = io.Framerate;
    float msPerFrame = 1000.0f / fps;
    IG::Text("%.3f ms/frame (%.1f FPS)", msPerFrame, fps);

    auto graphSize = window_sz;
    graphSize.height() *= 0.5;
    // IMGUI_API void          PlotLines(const char* label, const float* values, int values_count,
    // int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float
    // scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    IG::PlotLines("frametime", frameTimeSamples, frameTimeSamples.capacity(), 0, nullptr, 0.0f,
                  30.0f, graphSize);

    IG::End();
    return Vec2{pos.x, pos.y + window_sz.height()};
}

void ImGuiHandler::setTooltipWrap(const char* tooltip) {
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);  // Optional: wrap text
        ImGui::Text("%s", tooltip);
        ImGui::PopTextWrapPos();
        ImGui::End();
    }
}
void ImGuiHandler::imguiTextWithTooltip(const char* text, const char* tip) {
    ImGui::Text("%s", text);
    setTooltipWrap(tip);
}
