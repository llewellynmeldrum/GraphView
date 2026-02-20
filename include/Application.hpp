#pragma once

#include "GLFWHandler.hpp"
#include "ImGuiHandler.hpp"
struct Application {
    SharedContext shared{this};
    GLFWHandler   platform{shared};
    ImGuiHandler  ui{shared};

    std::unique_ptr<Graph> graph;

    void generateGraph() {
        if (!shared.graphExists) {
            shared.graphConfig.ptr = std::make_unique<Graph>();
            shared.graphConfig.ptr->init(shared.graphInitConfig);
        } else {
            shared.graphConfig.ptr->reset(shared.graphInitConfig);
        }
        // configure graph based on the changes made in ui
        //		shared.graphInitConfig.E =
        shared.graphExists = true;
        shared.uiRequestsGraphGeneration = false;
        // might have to add a timer to prevent generation happening too fast
    }
    void start();
    void exit(int exitCode);

 private:
    void destroy();
};
