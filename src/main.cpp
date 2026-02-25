#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdlib>
#include <print>
#include <vector>

#include "Application.hpp"
#include "Colors.hpp"
#include "GLFWHandler.hpp"
#include "ImGuiHandler.hpp"
#include "MirroredRingBuf.hpp"
#include "SharedContext.hpp"
#include "Vectors.hpp"

Application app;

int main() {
    app.start();

    app.shared.uiRequestsGraphGeneration = true;
    while (!app.platform.shouldClose()) {
        app.platform.handleInputs();
        if (app.shared.uiRequestsGraphGeneration) {
            app.generateGraph();
        }
        app.ui.drawUI();
        app.platform.render();
    }
    app.exit(EXIT_SUCCESS);
}

void Application::start() {
    platform.init();
    ui.init();
    shared.renderer = &platform;
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
    glfwDestroyWindow(shared.OSWindow);
    glfwTerminate();
}
