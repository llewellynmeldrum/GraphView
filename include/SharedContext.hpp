#pragma once
// basically everyone uses these headers
#include "Colors.hpp"
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
struct GLFWHandler;

struct Application;
struct Graph;
struct SharedContext {
    struct Camera {
        static constexpr float w2sFactor = 2.0f;  // how many world units per pixel

        float aspectRatio = 1.0f;

        glm::vec2 origin = {0.0f, 0.0f};
        glm::vec2 smoothPos = {0.0f, 0.0f};
        glm::vec2 truePos = {0.0f, 0.0f};
        float     zoom = 0.4f, MAX_ZOOM = 15.0f, MIN_ZOOM = 0.05f;
        float     smoothFactor = 0.4f;
    } cam;

    // shared setup between imgui and glfw
    float       dpiScaling;
    const char* p_glslVersion{nullptr};

    bool                         ignoreMouseInput = false;
    bool                         ignoreKeyboardInput = false;
    bool                         uiRequestsGraphGeneration = false;
    const GLColor&               bgColor = DDARKGREY;
    GLFWwindow*                  p_viewport{nullptr};
    std::unique_ptr<GLFWHandler> renderer;

    bool graphExists = false;
    struct GraphInitConfig {
        bool      enableSelfEdges = true;
        bool      weighted = false;
        glm::vec2 xBounds = glm::vec2{-500.0f, 500.0f};
        glm::vec2 yBounds = glm::vec2{-500.0f, 500.0f};
        int       V = 25;
        int       E = 25;
        float     initialTemperature = 100.0f;  // start temp of force-direction
        float     restingTemperature = 0.1f;    // end temp of force-direction
        float     attractionFactor = 1.0f;
        float     repulsionFactor = 1.0f;
        float     coolingFactor = 0.96f;
        int       substeps = 1;
    } graphInitConfig;
    struct GraphConfig {
        std::unique_ptr<Graph> ptr;
        bool                   isHidden = false;
        struct DrawSettings {
            float     nodeSizeWorld = 17.5f;
            glm::vec4 baseNodeColor = {1.0, 1.0, 1.0, 1.0};
            glm::vec4 edgeColor = {1, 0.9, 1, 0.6};
            bool      enableEdgeTapering = true;
            float     edgeTaperOutgoing = 12.5f;
            float     edgeTaperIncoming = 2.0f;
            bool      showBounds = false;

            bool redGreenEdgeColoring = true;
        } draw;

        struct UpdateSettings {
            bool  isForceDirected = true;
            float timeScale = 1.0f;
            bool  isPaused = false;
            float currTemp;
        } update;
        struct Algorithms {
            enum ID {
                BFS = 0,
                DFS = 1,
            };
            std::vector<std::string> list = {"BFS", "DFS"};
            int                      selected = 0;
        } algos;
    } graphs;

    struct AppConfig {
        static constexpr bool ENABLE_VSYNC = true;
        bool                  PRINT_KEY_EVENTS = false;
    } cfg;
};
