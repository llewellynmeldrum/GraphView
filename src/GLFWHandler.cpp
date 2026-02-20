#include "GLFWHandler.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <OpenGL/gl.h>
#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "SharedContext.hpp"
#include <GLFW/glfw3.h>

#include "GLContext.hpp"

void GLFWHandler::init(const AppConfig& cfg) {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        LOG::err("Failed to initialize glfw.");
        std::exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    auto primaryMonitor = glfwGetPrimaryMonitor();

    shared.p_glslVersion = getGLSLVersion();
    shared.mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(primaryMonitor);
    int screenWidth = glfwGetVideoMode(primaryMonitor)->width;
    int screenHeight = glfwGetVideoMode(primaryMonitor)->height;

    shared.p_viewport =
            glfwCreateWindow((screenWidth / 2.0) * shared.mainScale,
                             screenHeight * shared.mainScale, "Viewport title", nullptr, nullptr);
    glfwSetWindowPos(shared.p_viewport, 0, 0);

    if (!shared.p_viewport) {
        LOG::err("Failed to initialize glfw - viewport (window) ptr is null.");
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(shared.p_viewport);
    gl.init();
    glfwSwapInterval(cfg.ENABLE_VSYNC);  // enables vsync
    glfwSetKeyCallback(shared.p_viewport, key_callback);
    const char* s = (const char*)glGetString(GL_VERSION);
    println("OPENGL VERSION:{}", s);
}

void GLFWHandler::render() {
    int fbufferWidth, fbufferHeight;
    glfwGetFramebufferSize(shared.p_viewport, &fbufferWidth, &fbufferHeight);
    glViewport(0, 0, fbufferWidth, fbufferHeight);
    glClearColor(COLOR(shared.bgColor));
    glClear(GL_COLOR_BUFFER_BIT);

    gl.drawCircle(shared.temp.uPos, shared.temp.uRad, shared.temp.uCol);
    //    gl.drawCircle();

    if (shared.graphExists) {  // println("[{},{}]", viewportHeight, viewportWidth);
        drawGraph(shared.graphConfig.ptr.get());
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(shared.p_viewport);
}
// clang-format on
void GLFWHandler::handleInputs() {
    glfwPollEvents();
}
void GLFWHandler::handleUIUpdates() {
    if (shared.uiRequestsGraphGeneration) {
        Vec2 window = getGLFWWindowSize();
        println("[{},{}]", window.x, window.y);
        float margin = shared.graphConfig.draw.edgeMargin;
        // 10% from the left and 10% from the right
        float minX = 0 + (margin * window.width());
        float minY = 0 + (margin * window.height());

        float maxX = 0 + ((1.0f - margin) * window.width());
        float maxY = 0 + ((1.0f - margin) * window.height());
        shared.graphInitConfig.minPos = Vec2{minX, minY};
        shared.graphInitConfig.maxPos = Vec2{maxX, maxY};
    }
}

void GLFWHandler::drawLine(const Vec2& s, const Vec2& t) {
    //   println("Drawing Line from [{:02f},{:02f}]->[{:02f},{:02f}]", s.x, s.y, t.x, t.y);
}

void GLFWHandler::drawCircle(const Vec2& p, float extent) {
    //    println("Drawing Circle centered at [{:02f},{:02f}]], diameter={:02f}.", p.x, p.y,
    //    extent);
}

inline void GLFWHandler::drawNode(const Graph* G, Graph::Node u) {
    const Vec2& pos = G->nodePositions[u];
    drawCircle(pos, shared.graphConfig.draw.NodeSize_px);
}

inline void GLFWHandler::drawEdge(const Graph* G, Graph::Edge edge) {
    const Vec2& upos = G->nodePositions[edge.u];
    const Vec2& vpos = G->nodePositions[edge.v];
    drawLine(upos, vpos);
}
void GLFWHandler::drawGraph(const Graph* G) {
    for (size_t u = 0; u < G->init_cfg.V; u++) {
        drawNode(G, u);
    }
    for (size_t e = 0; e < G->init_cfg.E; e++) {
        drawEdge(G, G->edges[e]);
    }
}

void GLFWHandler::renderUI() {
}

Vec2 GLFWHandler::getGLFWWindowSize() {
    int windowWidth, windowHeight;
    glfwGetWindowSize(shared.p_viewport, &windowWidth, &windowHeight);
    return Vec2{windowWidth, windowHeight};
}

bool GLFWHandler::shouldClose() {
    return glfwWindowShouldClose(shared.p_viewport);
}
void GLFWHandler::destroy() {
    gl.cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

const char* GLFWHandler::getGLSLVersion() {
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glslVersion = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glslVersion = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glslVersion = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    return glslVersion;
}
