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
#include "Application.hpp"
#include "SharedContext.hpp"
#include <GLFW/glfw3.h>

#include "GLContext.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 GLFWHandler::makeViewProjection(int width, int height, float zoomHalfHeight = 500.0f) {
    const float halfH = zoomHalfHeight;
    const float halfW = halfH * cam.aspectRatio;

    // Projection: map world rectangle -> clip
    glm::mat4 proj = glm::ortho(-halfW, +halfW,  // left, right
                                -halfH, +halfH,  // bottom, top
                                -1.0f, +1.0f);

    // View: move the world opposite the camera position
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-cam.pos, 0.0f));

    return proj * view;
}

void GLFWHandler::OSWindowResized(int width, int height) {
    cam.aspectRatio = (height > 0) ? (float)width / (float)height : 1.0f;
    println("resize event detected, new AR:{}", cam.aspectRatio);
}

void GLFWHandler::init() {
    glfwSetErrorCallback(error_callback_static);
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
    glfwSwapInterval(shared.cfg.ENABLE_VSYNC);
    gl.init();
    int width, height;
    glfwGetFramebufferSize(shared.p_viewport, &width, &height);
    OSWindowResized(width, height);
    gl.viewProj = makeViewProjection(width, height);

    glfwSetWindowUserPointer(shared.p_viewport, this);
    glfwSetKeyCallback(shared.p_viewport, key_callback_static);
    glfwSetFramebufferSizeCallback(shared.p_viewport, framebuffer_size_callback_static);

    const char* s = (const char*)glGetString(GL_VERSION);
    println("OPENGL VERSION:{}", s);
}

void GLFWHandler::render() {
    int width, height;
    glfwGetFramebufferSize(shared.p_viewport, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(COLOR(shared.bgColor));
    glClear(GL_COLOR_BUFFER_BIT);

    gl.viewProj = makeViewProjection(width, height);
    gl.drawCircle({0, 0}, 200, {1, 0, 0, 0});

    if (shared.graphExists) {
        drawGraph(shared.graphConfig.ptr.get());
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(shared.p_viewport);
}
// clang-format on
inline void GLFWHandler::drawNode(const Graph* G, Graph::Node u) {
    const auto& pos = G->nodePositions[u];
    const auto& nodeSize_px = shared.graphConfig.draw.NodeSize_px;
    const auto& nodeColor = G->nodeColors[u];
    gl.drawCircle(pos, nodeSize_px, nodeColor);
}

inline void GLFWHandler::drawEdge(const Graph* G, Graph::Edge edge) {
    const auto& upos = G->nodePositions[edge.u];
    const auto& vpos = G->nodePositions[edge.v];
    (void)upos;
    (void)vpos;
    //    drawLine(upos, vpos);
}
void GLFWHandler::drawGraph(const Graph* G) {
    for (size_t u = 0; u < G->init_cfg.V; u++) {
        drawNode(G, u);
    }
    for (size_t e = 0; e < G->init_cfg.E; e++) {
        drawEdge(G, G->edges[e]);
    }
}

glm::vec2 GLFWHandler::getGLFWWindowSize() {
    int windowWidth, windowHeight;
    glfwGetWindowSize(shared.p_viewport, &windowWidth, &windowHeight);
    return glm::vec2{windowWidth, windowHeight};
}
void GLFWHandler::destroy() {
    gl.cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void GLFWHandler::handleInputs() {
    glfwPollEvents();
}
void GLFWHandler::handleUIUpdates() {
    if (shared.uiRequestsGraphGeneration) {
        glm::vec2 window = getGLFWWindowSize();
        println("[{},{}]", window.x, window.y);
        float margin = shared.graphConfig.draw.edgeMargin;
        // 10% from the left and 10% from the right
        float minX = 0 + (margin * window.x);
        float minY = 0 + (margin * window.y);

        float maxX = 0 + ((1.0f - margin) * window.x);
        float maxY = 0 + ((1.0f - margin) * window.y);
        shared.graphInitConfig.minPos = glm::vec2{minX, minY};
        shared.graphInitConfig.maxPos = glm::vec2{maxX, maxY};
    }
}

bool GLFWHandler::shouldClose() {
    return glfwWindowShouldClose(shared.p_viewport);
}

const char* GLFWHandler::getGLSLVersion() {
#if defined(__APPLE__)
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
// exclusively callback
void GLFWHandler::error_callback_static(int error, const char* description) {
    LOG::err("GLFW Error {}: {}\n", error, description);
}

void GLFWHandler::mouseMoved(glm::vec2 nPos) {
}
void GLFWHandler::mousePressed(int button, int action, int mods) {
}
void GLFWHandler::mouseScrolled(double xoffset, double yoffset) {
}
void GLFWHandler::mouseEnteredOSWindow() {
}
void GLFWHandler::mouseLeftOSWindow() {
}
// clang-format off
void GLFWHandler::mouse_scrolled_callback_static(GLFWwindow* window, double xoffset, double yoffset) {
    auto* inst = (GLFWHandler*)glfwGetWindowUserPointer(window);
    if (inst) {
        inst->mouseScrolled(xoffset, yoffset);
    }
}
void GLFWHandler::mouse_entered_callback_static(GLFWwindow* window, int entered) {
    auto* inst = (GLFWHandler*)glfwGetWindowUserPointer(window);
    if (inst) {
        entered ? inst->mouseEnteredOSWindow() : inst->mouseLeftOSWindow();
    }
}
void GLFWHandler::mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods) {
    auto* inst = (GLFWHandler*)glfwGetWindowUserPointer(window);
    if (inst) {
        inst->mousePressed(button, action, mods);
    }
}
void GLFWHandler::mouse_move_callback(GLFWwindow* window, double x, double y) {
    auto* inst = (GLFWHandler*)glfwGetWindowUserPointer(window);
    if (inst) {
        inst->mouseMoved(glm::vec2(x, y));
    }
}

void GLFWHandler::key_callback_static(GLFWwindow* window, int key, int scancode, int action,
                                      int mods) {
    auto* inst = (GLFWHandler*)glfwGetWindowUserPointer(window);
    if (inst) {
        inst->keyPressed(key, scancode, action, mods);
    }
}

void GLFWHandler::framebuffer_size_callback_static(GLFWwindow* window, int width, int height) {
    auto* inst = (GLFWHandler*)glfwGetWindowUserPointer(window);
    if (inst) {
        inst->OSWindowResized(width, height);
    }
}
void GLFWHandler::keyPressed(int key, int scancode, int action, int mods) {
    if (shared.cfg.PRINT_KEY_EVENTS)
        println("Key:{}, scancode:{}, action:{}, mods:{}", key, scancode, action, mods);
    switch (key) {
    case 'C':
        if (mods == GLFW_MOD_CONTROL) {
            shared.app->exit(EXIT_SUCCESS);
        }
        break;

    default: break;
    }
}
