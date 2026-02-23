#pragma once
// basically everyone uses these headers
#include "Colors.hpp"
#include "Graph.hpp"
#include "MirroredRingBuf.hpp"
#include "Vectors.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <random>

// forward declare so as to not have to include entire header
// TODO: Remove this when im not lazy
using std::print;
using std::println;

struct GLFWwindow;

struct Application;
struct SharedContext {
    struct Camera {
        static constexpr float w2sFactor = 2.0f;  // how many world units per pixel

        float aspectRatio = 1.0f;

        glm::vec2 origin = {0.0f, 0.0f};
        glm::vec2 smoothPos = {0.0f, 0.0f};
        glm::vec2 truePos = {0.0f, 0.0f};
        float     zoom = 1.0f, MAX_ZOOM = 15.0f, MIN_ZOOM = 0.05f;
        float     smoothFactor = 0.4f;
    } cam;
    Application* app;  // So submodules can call app->exit() if they need to
    SharedContext(Application* _app) : app(_app) {}

    // shared setup between imgui and glfw
    float       dpiScaling;
    const char* p_glslVersion{nullptr};

    bool           ignoreMouseInput = false;
    bool           ignoreKeyboardInput = false;
    bool           uiRequestsGraphGeneration = false;
    const GLColor& bgColor = DDARKGREY;
    GLFWwindow*    p_viewport{nullptr};

    bool            graphExists = false;
    GraphInitConfig graphInitConfig{};
    GraphConfig     graphConfig{};

    struct AppConfig {
        static constexpr bool ENABLE_VSYNC = true;
        bool                  PRINT_KEY_EVENTS = false;
    } cfg;
};
