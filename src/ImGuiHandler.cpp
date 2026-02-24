#include "ImGuiHandler.hpp"
#include "Graph.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#define ADDR(x) glm::value_ptr(x)
using IGH = ImGuiHandler;
ImVec2 glm2iv(glm::vec2 v) {
    return {v.x, v.y};
}
void IGH::drawOverlay() {
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
                         ImGuiWindowFlags_NoInputs);

    const auto* G = shared.graphs.ptr.get();
    // Set cursor to an absolute screen position (e.g., center of the screen)
    // we must use the currently generated graphs V, not the UI selected V to avoid OOB
    for (int i = 0; i < shared.graphs.ptr->init_cfg.V; i++) {
        ImVec2 pos = glm2iv(G->screenPos[i]);
        ImGui::SetCursorScreenPos(pos);
        ImGui::Text("%s", G->id[i].c_str());
    }

    ImGui::End();
}
Vec2 IGH::drawAlgorithmRunner(WindowConfig win) {
    IG::SetNextWindowPos(win.pos, ImGuiCond_FirstUseEver);
    IG::SetNextWindowSize(win.size *= IG::GetMainViewport()->Size, ImGuiCond_FirstUseEver);
    IG::Begin("Algorithm Runner", &win.shown, WindowFlags::DYNAMIC);

    auto& draw = shared.graphs.draw;
    auto& update = shared.graphs.update;
    auto& algos = shared.graphs.algos;
    if (IG::BeginCombo("<- select",
                       algos.list[algos.selected].c_str()))  // "##my_combo" hides the label
    {
        for (int i = 0; i < algos.list.size(); i++) {
            const bool is_selected = (algos.selected == i);
            if (IG::Selectable(algos.list[i].c_str(), is_selected)) algos.selected = i;

            // Set the initial focus when opening the combo for keyboard navigation
            if (is_selected) IG::SetItemDefaultFocus();
        }
        IG::EndCombo();
    }

    IG::Text("Run Algorithm:");
    IG::SameLine();
    if (IG::ArrowButton("##Run Algorithm Button", ImGuiDir_Right)) {
        println("Running BFS");
    }

    IG::End();
    return {win.pos.x, win.pos.y + win.pos.y};
    // bototm left
}
Vec2 IGH::drawGraphVisualSettings(WindowConfig win) {
    IG::SetNextWindowPos(win.pos, ImGuiCond_FirstUseEver);
    IG::SetNextWindowSize(win.size *= IG::GetMainViewport()->Size, ImGuiCond_FirstUseEver);
    IG::Begin("Graph Visuals", &win.shown, WindowFlags::DYNAMIC);

    auto& draw = shared.graphs.draw;
    auto& update = shared.graphs.update;
    if (IG::TreeNodeEx("Graph Updates", ImGuiTreeNodeFlags_DefaultOpen)) {
        IG::Checkbox("Enable force direction", &update.isForceDirected);
        if (!update.isForceDirected) {
            IG::BeginDisabled();
        }
        IG::SliderFloat("Current temperature", &update.currTemp,
                        shared.graphInitConfig.restingTemperature,
                        shared.graphInitConfig.initialTemperature);
        if (!update.isForceDirected) {
            IG::EndDisabled();
        }
        IG::TreePop();
    }

    if (IG::TreeNodeEx("Node Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        IG::SliderFloat("Node Radius (px)", &(draw.nodeSizeWorld), 0.0f, 100.0f);
        IG::ColorEdit4("Node Color", glm::value_ptr(draw.baseNodeColor));
        IG::TreePop();
    }

    if (IG::TreeNodeEx("Edge Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        IG::Checkbox("Edge tapering", &draw.enableEdgeTapering);

        IG::SliderFloat("Incoming edge width", &(draw.edgeTaperIncoming), 0.0f,
                        draw.nodeSizeWorld * 2.0);
        if (!draw.enableEdgeTapering) {
            IG::BeginDisabled();
            draw.edgeTaperOutgoing = draw.edgeTaperIncoming;
        }
        IG::SliderFloat("Outgoing edge width", &(draw.edgeTaperOutgoing), 0.0f,
                        draw.nodeSizeWorld * 2.0);

        if (!draw.enableEdgeTapering) {
            IG::EndDisabled();
        }
        IG::ColorEdit4("Edge Color", glm::value_ptr(draw.edgeColor));
        IG::Checkbox("Show bounds", &draw.showBounds);
        IG::TreePop();
    }

    if (IG::TreeNodeEx("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        IG::SliderFloat2("Smoothed pos", ADDR(shared.cam.smoothPos), -1000.0f, 1000.0f);
        IG::SliderFloat2("True position", ADDR(shared.cam.truePos), -1000.0f, 1000.0f);
        IG::SliderFloat("Camera smoothing", &shared.cam.smoothFactor, 0.0f, 2.0f);
        IG::SliderFloat("Camera zoom", &(shared.cam.zoom), shared.cam.MIN_ZOOM,
                        shared.cam.MAX_ZOOM);
        IG::TreePop();
    }

    IG::End();
    return {win.pos.x, win.pos.y + win.pos.y};
    // bototm left
}
Vec2 IGH::drawPlaybackControlsWindow(WindowConfig win) {
    IG::SetNextWindowPos(win.pos, ImGuiCond_FirstUseEver);
    IG::SetNextWindowSize(win.size *= IG::GetMainViewport()->Size, ImGuiCond_FirstUseEver);
    IG::Begin("Playback Controls", &win.shown, WindowFlags::DYNAMIC);

    auto& update = shared.graphs.update;
    // clang-format off
    std::string label = "Pause";
    if (update.isPaused) label = "Continue";
    if (IG::Button(label.c_str(),{80,30})){
        update.isPaused=!update.isPaused;
    } 
    IG::SliderFloat("Timescale",&update.timeScale,0.0f,5.0f,"%.3f",ImGuiSliderFlags_Logarithmic);
    if (update.timeScale==0) update.isPaused=true;
    if (update.timeScale>0) update.isPaused=false;
    IG::End();
    return {win.pos.x, win.pos.y + win.pos.y};
    // bototm left
}
Vec2 IGH::drawGraphGenerationWindow(WindowConfig win) {
    IG::SetNextWindowPos(win.pos, ImGuiCond_FirstUseEver);
    IG::SetNextWindowSize(win.size *= IG::GetMainViewport()->Size, ImGuiCond_FirstUseEver);
    IG::Begin("Graph Generation", &win.shown, WindowFlags::DYNAMIC);

    auto& initConfig = shared.graphInitConfig;
    IG::InputInt("V", &(initConfig.V));
    IGH::setTooltipWrap("Number of vertices (nodes) in the graph.");

    IG::InputInt("E", &initConfig.E);
    IGH::setTooltipWrap("Number of edges in the graph.");

    if (IG::Button("Generate Graph")) {
        shared.uiRequestsGraphGeneration = true;
    }
    IG::SameLine();
    IG::Button("Generate Graph (hold)");
    if (IG::IsItemActive()) {
        shared.uiRequestsGraphGeneration = true;
    }
    IG::SliderFloat("Initial Temp", &initConfig.initialTemperature, 0, 1000.0f);
    IG::SliderFloat("Resting Temp", &initConfig.restingTemperature, 0, 10.0f);
    IG::SliderFloat("Attraction Factor", &initConfig.attractionFactor, 0, 10.0f);
    IG::SliderFloat("Repulsion Factor", &initConfig.repulsionFactor, 0, 10.0f);
    IG::SliderFloat("Cooling Factor", &initConfig.coolingFactor, 0, 10.0f);
    IG::SliderInt("Substeps per cooling step", &initConfig.substeps, 1, 10);

    if (!shared.graphExists) {
        IG::BeginDisabled();
    }

    {
        IG::SliderFloat2("Screen Bounds (x)", glm::value_ptr(initConfig.xBounds),
                         -1000.0f, 1000.0f);
        IG::SliderFloat2("Screen Bounds (y)", glm::value_ptr(initConfig.yBounds),
                         -1000.0f, 1000.0f);
        if (ImGui::Button("Print adjlist")) {
            shared.graphs.ptr->debug_print_adj();
        }
        if (ImGui::Button("Print edges")) {
            shared.graphs.ptr->debug_print_edges();
        }

        if (ImGui::Button("Print positions")) {
            shared.graphs.ptr->debug_print_positions();
        }
    }
    if (!shared.graphExists) {
        IG::EndDisabled();
    }
    IG::End();
    return {win.pos.x, win.pos.y + win.pos.y};
    // bototm left
}
void IGH::drawUI() {
    auto& io = IG::GetIO();
    frameTimeSamples.write(io.DeltaTime * 1000.0f);

    if (glfwGetWindowAttrib(shared.p_viewport, GLFW_ICONIFIED) != 0) {
        ImGui_ImplGlfw_Sleep(10);
        return;
    }
    Vec2 viewport = IG::GetMainViewport()->Size;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    IG::NewFrame();
    {
        auto prev = drawPerformanceWindow({{0.5f, 0.1f}, {0, 0}, io});
        drawGraphGenerationWindow({{0.5f, 0.1f}, prev, io});
        drawGraphVisualSettings({{0.5, 0.1}, prev, io});
        drawPlaybackControlsWindow({{0.5,0.9},prev,io});
        drawAlgorithmRunner({{0.5,0.2},prev,io});
        drawOverlay();
    }
    IG::Render();  // calls IG::endFrame()
}
Vec2 IGH::drawPerformanceWindow(IGH::WindowConfig win) {
    IG::SetNextWindowPos(win.pos);
    IG::SetNextWindowSize(win.size *= IG::GetMainViewport()->Size, ImGuiCond_FirstUseEver);

    IG::Begin("Performance", &win.shown, WindowFlags::STATIC);
    if (IG::BeginMenuBar()) {
        if (IG::BeginMenu("File")) {
            if (IG::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */
            }
            if (IG::MenuItem("Save", "Ctrl+S")) { /* Do stuff */
            }
            if (IG::MenuItem("Close", "Ctrl+W")) {
                win.shown = true;
            }
            IG::EndMenu();
        }
        IG::EndMenuBar();
    }

    float fps = win.io.Framerate;
    float msPerFrame = 1000.0f / fps;
    IG::Text("%.3f ms/frame (%.1f FPS)", msPerFrame, fps);

    auto plotSize = win.size * Vec2{1, 0.5};
    IG::PlotLines("frametime", frameTimeSamples, frameTimeSamples.capacity(), 0, nullptr, 0.0f,
                  30.0f, plotSize);

    IG::End();
    return Vec2{win.pos.x + win.size.x, win.pos.y + win.size.y};
}

void IGH::setTooltipWrap(const char* tooltip) {
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);  // Optional: wrap text
        ImGui::Text("%s", tooltip);
        ImGui::PopTextWrapPos();
        ImGui::End();
    }
}
void IGH::imguiTextWithTooltip(const char* text, const char* tip) {
    ImGui::Text("%s", text);
    setTooltipWrap(tip);
}
