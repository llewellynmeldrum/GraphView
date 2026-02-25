#include "ImGuiHandler.hpp"
#include "GLFWHandler.hpp"
#include "Graph.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#define ADDR(x) glm::value_ptr(x)
using RunState = SharedContext::GraphConfig::Algorithms::RunState;
using AlgoID = SharedContext::GraphConfig::Algorithms::AlgoID;
using IGH = ImGuiHandler;
ImVec2 glm2iv(glm::vec2 v) {
    return {v.x, v.y};
}
struct PulseConfig {
    float height = 100;
    float speed = 5;
    float shiftX = 3.8;
    float shiftY = 150;
    bool  enable = true;
    float alpha(float t) { return height * sin(speed * (t + shiftX)) + shiftY; }
} pulse;

void IGH::drawNodeIDOverlay(NodeOverlayConfig cfg) {
    auto*      G = shared.graphs.ptr.get();
    const auto input = shared.renderer->input;
    auto       algos = shared.graphs.ptr->cfg.algos;
    using SelectionType = SharedContext::GraphConfig::Algorithms::SelectionType;
    IG::PushFont(H1);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const auto  size = static_cast<ImVec2>(IG::CalcTextSize(G->id[0].c_str()));
    for (int node = 0; node < G->init_cfg.V; node++) {
        const auto& screenPos = G->screenPos[node];
        const auto& id = G->id[node];
        ImVec2      pos = glm2iv(screenPos);
        pos.y -= 20.0f;

        ImVec2 p_min = pos;
        p_min.x -= size.x / 2.0f;
        p_min.y -= size.y / 2.0f;
        ImVec2 p_max = ImVec2(p_min.x + size.x, p_min.y + size.y);

        // clang-format off

        if (IG::IsMouseHoveringRect(p_min, p_max)) {
            IG::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (shared.renderer->input.clickedThisFrame) {
                if (input.clickedThisFrame) {
                    switch (algos.state) {
                        case RunState::SelectingAlgorithm:{
                            // why tf are we in this function then
                        }
                        break;
                        case RunState::SelectingSource:{
                            algos.sourceNode = node;
                            algos.state=RunState::SelectingDest;
                        }
                        break;
                    case RunState::SelectingDest: {
                            algos.destNode = node;
                            algos.state=RunState::Ready;
                        }
                        break;
                    }
                }
            }
        }
        ImU32 color;
        if (algos.destNode == node || algos.sourceNode == node) {
            color = cfg.selected_col;
        } else {
            color = IM_COL32(cfg.col, cfg.col, cfg.col, pulse.alpha(glfwGetTime()));
        }
        draw_list->AddRectFilled(p_min, p_max, color);
        IG::SetCursorScreenPos(p_min);
        IG::TextColored({1, 1, 1, 1}, "%s", id.c_str());
    }
    println("src:{},dst:{}",algos.sourceNode, algos.destNode);
    IG::PopFont();
}
void IGH::drawOverlay() {
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
                         ImGuiWindowFlags_NoInputs);

    // Set cursor to an absolute screen position (e.g., center of the screen)
    // we must use the currently generated graphs V, not the UI selected V to avoid OOB

    if (shared.graphs.algos.state == (RunState::SelectingSource || RunState::SelectingDest)) {
        drawNodeIDOverlay({
                .blinkAllNodes = true,
                .col = IM_COL32(150, 150, 150, 255),
                .selected_col = IM_COL32(255, 255, 255, 255),
        });
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
    std::string comboText = algos.selectedAlgorithm == AlgoID::NO_ALGO ? "<select algorithm>" : algos.list[algos.selectedAlgorithm];
    if (IG::BeginCombo("<- select",comboText.c_str())) {
        for (int i = 0; i < algos.list.size(); i++) {
            const bool is_selected = (algos.selectedAlgorithm == i);
            if (IG::Selectable(algos.list[i].c_str(), is_selected)){
                algos.selectedAlgorithm = i;
                algos.state = RunState::SelectingSource;
            }

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

    if (glfwGetWindowAttrib(shared.OSWindow, GLFW_ICONIFIED) != 0) {
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
