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

#define IG_PRINT(s, ...)                                                                           \
    do {                                                                                           \
        string s = std::format(s, __VA_ARGS__);                                                    \
        IMGUI_DEBUG_LOG("%s\n", s.c_str());                                                        \
    } while (0)

struct Application;
struct SharedContext {
    // for messing with opengl
    Application* app;
    SharedContext(Application* _app) : app(_app) {}
    struct Temporary {
        std::array<float, 12> NDC_quad = {-1.f, -1.f, 1.f, -1.f, 1.f,  1.f,
                                          -1.f, -1.f, 1.f, 1.f,  -1.f, 1.f};

        glm::vec2 centerWorld{0.0f, 0.0f};
        glm::vec4 uCol{0.2f, 0.8f, 1.0f, 1.0f};
        float     radWorld{35.0f};
    } temp;

    float          mainScale;
    bool           uiRequestsGraphGeneration = false;
    const GLColor& bgColor = DDARKGREY;
    GLFWwindow*    p_viewport{nullptr};
    const char*    p_glslVersion{nullptr};

    bool            graphExists = false;
    GraphInitConfig graphInitConfig{};
    GraphConfig     graphConfig{};
    struct AppConfig {
        static constexpr bool ENABLE_VSYNC = true;
        bool                  PRINT_KEY_EVENTS = false;
    } cfg;
};
