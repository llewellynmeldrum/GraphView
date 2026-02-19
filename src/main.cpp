#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdlib>
#include <print>
#include <vector>

#include "Colors.hpp"
#include "GLFWHandler.hpp"
#include "ImGuiHandler.hpp"
#include "MirroredRingBuf.hpp"
#include "SharedContext.hpp"
#include "Vectors.hpp"

struct Application {
    SharedContext shared;
    GLFWHandler   platform{shared};
    ImGuiHandler  ui{shared};
    AppConfig     cfg{
                .PRINT_KEY_EVENTS = false,
    };
    std::unique_ptr<Graph> graph;

    void generateGraph() {
        if (!shared.graphExists) {
            shared.graphConfig.ptr = std::make_unique<Graph>(shared.graphInitConfig);
            shared.graphConfig.ptr->init();
        } else {
            shared.graphConfig.ptr->reset();
        }
        shared.graphExists = true;
        shared.graphInitConfig.minPos;
        shared.uiRequestsGraphGeneration = false;
        // might have to add a timer to prevent generation happening too fast
    }
    void start();
    void exit(int exitCode);

 private:
    void destroy();
};
Application app;

int main() {
    app.start();
    while (!app.platform.shouldClose()) {
        app.platform.handleInputs();
        app.platform.handleUIUpdates();
        if (app.shared.uiRequestsGraphGeneration) {
            app.generateGraph();
        }
        app.ui.composeUI();
        app.platform.render();
    }
    app.exit(EXIT_SUCCESS);
}

void Application::start() {
    platform.init(this->cfg);
    ui.init(this->cfg);
}
void Application::destroy() {
    platform.destroy();
    ui.destroy();
}

void Application::exit(int exitCode) {
    this->destroy();
    printf("Exiting with code: %d\n", exitCode);
    std::exit(exitCode);
}

void ImGuiHandler::destroy() {
    glfwDestroyWindow(shared.p_viewport);
    glfwTerminate();
}

void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if (app.cfg.PRINT_KEY_EVENTS)
        println("Key:{}, scancode:{}, action:{}, mods:{}", key, scancode, action, mods);
    switch (key) {
    case 'C':
        if (mods == GLFW_MOD_CONTROL) app.exit(EXIT_SUCCESS);
        break;

    default: break;
    }
}
