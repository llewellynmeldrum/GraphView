
#include <cmath>
#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

#include <OpenGL/gl.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Application.hpp"
#include "GLFWHandler.hpp"
#include "SharedContext.hpp"
#include "glm_wrapper.hpp"

#include "GLContext.hpp"

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
    shared.dpiScaling = getDPIScaling(primaryMonitor);
    vec2 displaySize = getMainDisplaySize(primaryMonitor);

    float scale = shared.dpiScaling;
    int   w = (displaySize.x / (WINDOW_W_SCALE)) * scale;
    int   h = displaySize.y;
    shared.OSWindow = glfwCreateWindow(w, h, "GraphView", nullptr, nullptr);
    if (!shared.OSWindow) {
        LOG::err("Failed to initialize glfw - window ptr is null.");
        std::exit(EXIT_FAILURE);
    }
    glfwSetWindowPos(shared.OSWindow, 0, 0);
    glfwMakeContextCurrent(shared.OSWindow);
    glfwSwapInterval(shared.cfg.ENABLE_VSYNC);

    if (gl.init()) {
        LOG::err("Error initializing openGL.");
    }
    int width, height;
    glfwGetFramebufferSize(shared.OSWindow, &width, &height);
    OSWindowResized(width, height);
    glfwSetWindowUserPointer(shared.OSWindow, this);

    setCallbacks();

    const char* opengl_ver_str = (const char*)glGetString(GL_VERSION);
    println("OPENGL VERSION:{}", opengl_ver_str);
}
void GLFWHandler::setCallbacks() {
    glfwSetKeyCallback(shared.OSWindow, key_callback_static);
    glfwSetFramebufferSizeCallback(shared.OSWindow, framebuffer_size_callback_static);
    glfwSetCursorEnterCallback(shared.OSWindow, mouse_entered_callback_static);
    glfwSetMouseButtonCallback(shared.OSWindow, mouse_button_callback_static);
    glfwSetScrollCallback(shared.OSWindow, mouse_scrolled_callback_static);
    glfwSetCursorPosCallback(shared.OSWindow, mouse_move_callback);
}

void GLFWHandler::render() {
    for (int u = 0; u < shared.graphs.ptr->init_cfg.V; u++) {
        shared.graphs.ptr->screenPos[u] = worldToScreen(shared.graphs.ptr->worldPos[u]);
    }
    glfwGetFramebufferSize(shared.OSWindow, &scrWidth_px, &scrHeight_px);
    glViewport(0, 0, scrWidth_px, scrHeight_px);
    glClearColor(COLOR(shared.bgColor));
    glClear(GL_COLOR_BUFFER_BIT);

    gl.viewProj = makeViewProjection(shared.cam.smoothPos, DEFAULT_ZOOM_AMOUNT / shared.cam.zoom);
    gl.invViewProj = inverse(gl.viewProj);

    gl.raw_viewProj = makeViewProjection(shared.cam.truePos, DEFAULT_ZOOM_AMOUNT / shared.cam.zoom);
    gl.raw_invViewProj = inverse(gl.raw_viewProj);

    {  // debug visuals to test camera smoothing
        /*
        gl.beginLinesPass();
        gl.drawTaperedLine(shared.cam.truePos, shared.cam.smoothPos, 2.0f, 2.0f, {1, 1, 1, 1});
        gl.endPass();
        gl.beginCirclePass();
        gl.drawCircle(shared.cam.truePos, 15.0f, {0, 1, 0, 1});
        gl.drawCircle(shared.cam.smoothPos, 15.0f, {0, 1, 0, 0.1});
        gl.endPass();
        */
    }
    if (shared.graphExists && !shared.graphs.isHidden) {
        drawGraph(shared.graphs.ptr.get());
    }
    if (shared.graphs.draw.showBounds) {
        drawGraphBounds(5.0f, {1, 0, 0, 0.5});
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(shared.OSWindow);
}
#define xy(var) var.x, var.y
#define xyz(var) var.x, var.y, var.z
#define xyzw(var) var.x, var.y, var.z, var.w

vec2 GLFWHandler::worldToScreen(vec2 w2) {
    auto world4 = vec4{w2.x, w2.y, 0.0f, 1.0f};
    auto clip4 = gl.raw_viewProj * world4;
    auto ndc2 = vec2{clip4.x, clip4.y} / clip4.w;
    return vec2{(ndc2.x * 0.5f + 0.5f) * scrWidth_px, (-ndc2.y * 0.5f + 0.5f) * scrHeight_px};
}
vec2 GLFWHandler::screenToWorld(vec2 scr2) {
    // always use the raw_InvViewProj

    auto ndc2 = scr2;
    // 1. convert screen space to ndc
    ndc2.x = 2.0f * scr2.x / scrWidth_px - 1.0f;
    ndc2.y = 1.0f - 2.0f * scr2.y / scrHeight_px;  // flip for tl origin

    auto ndc_z = +1.0f;  // far plane
    // float ndc_z = +1;  // near plane

    // 2. convert ndc to clip
    auto clip4 = vec4{xy(ndc2), ndc_z, 1.0f};  // w=1 means coord rather than vector

    // 3. convert clip->world ==> worldPos = inverse(viewProj) * clipPos
    auto world4 = gl.raw_invViewProj * clip4;
    auto world3 = vec3{xyz(world4)};
    world3 /= world4.w;
    // println("s:[{},{}]->w:[{},{}]", scr2.x, scr2.y, world3.x, world3.y);
    return vec2{world3.x, world3.y};
}

void GLFWHandler::drawGraphBounds(float lineThickness, vec4 boundsColor) {
    gl.beginLinesPass();
    auto& xbounds = shared.graphInitConfig.xBounds;
    auto& ybounds = shared.graphInitConfig.yBounds;
    auto  bl = vec2{xbounds[0], ybounds[0]};
    auto  br = vec2{xbounds[1], ybounds[0]};
    auto  tl = vec2{xbounds[0], ybounds[1]};
    auto  tr = vec2{xbounds[1], ybounds[1]};

    // tl-->tr
    // ^    |
    // |    V
    // bl<--br
    gl.drawLine(tl, tr, lineThickness, boundsColor);
    gl.drawLine(tr, br, lineThickness, boundsColor);
    gl.drawLine(br, bl, lineThickness, boundsColor);
    gl.drawLine(bl, tl, lineThickness, boundsColor);
    gl.endPass();
}

inline void GLFWHandler::drawNode(const Graph* G, Graph::Node u) {
    const auto& draw = shared.graphs.draw;

    const auto& pos = G->worldPos[u];
    const auto  isNodeColored = static_cast<bool>(G->isNodeColored[u]);
    const auto& nodeColor = (isNodeColored) ? G->nodeColor[u] : shared.graphs.draw.baseNodeColor;

    gl.drawCircle(pos, draw.nodeSizeWorld, nodeColor);
}

inline void GLFWHandler::drawEdge(const Graph* G, Graph::Edge edge) {
    const auto& draw = shared.graphs.draw;

    const auto& uPos = G->worldPos[edge.u];
    const auto& uScale = draw.edgeTaperOutgoing;

    const auto& vPos = G->worldPos[edge.v];
    const auto& vScale = draw.edgeTaperIncoming;
    // u-->v
    gl.drawTaperedLine(uPos, vPos, uScale, vScale, draw.edgeColor);
}
void GLFWHandler::drawGraph(const Graph* G) {
    gl.beginLinesPass();
    for (size_t edgeIdx = 0; edgeIdx < G->init_cfg.E; edgeIdx++) {
        drawEdge(G, G->edges[edgeIdx]);
    }
    gl.endPass();

    gl.beginCirclePass();
    for (size_t nodeIdx = 0; nodeIdx < G->init_cfg.V; nodeIdx++) {
        drawNode(G, nodeIdx);
    }
    gl.endPass();
}

vec2 GLFWHandler::getOSWindowSize() {
    int windowWidth, windowHeight;
    glfwGetWindowSize(shared.OSWindow, &windowWidth, &windowHeight);
    return vec2{windowWidth, windowHeight};
}

float GLFWHandler::getDPIScaling(GLFWmonitor* monitor) {
    return ImGui_ImplGlfw_GetContentScaleForMonitor(monitor);
}

vec2 GLFWHandler::getMainDisplaySize(GLFWmonitor* monitor) {
    int displayWidth = glfwGetVideoMode(monitor)->width;
    int displayHeight = glfwGetVideoMode(monitor)->height;
    return vec2{displayWidth, displayHeight};
}
void GLFWHandler::destroy() {
    gl.cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
vec2 lerp2(vec2 a, vec2 b, double alpha) {
    a.x = std::lerp(a.x, b.x, alpha);
    a.y = std::lerp(a.y, b.y, alpha);
    return a;
}
void GLFWHandler::applyCameraSmoothing(double dT) {
    // yo i got no clue what this line does like not even the slightest
    double alpha = 1.0f - std::exp(-(10.0f / shared.cam.smoothFactor) * dT);
    shared.cam.smoothPos = lerp2(shared.cam.smoothPos, shared.cam.truePos, alpha);
}

void GLFWHandler::handleInputs() {
    if (tLastFrame == -1) tLastFrame = glfwGetTime();
    double tCurrent = glfwGetTime();
    double dT = tCurrent - tLastFrame;
    tLastFrame = tCurrent;

    applyCameraSmoothing(dT);
    if (shared.graphExists) {
        shared.graphs.ptr->updateGraph(dT);
    }

    auto& io = IG::GetIO();
    shared.ignoreMouseInput = io.WantCaptureMouse;
    shared.ignoreKeyboardInput = io.WantCaptureKeyboard;
    glfwPollEvents();
}

bool GLFWHandler::shouldClose() {
    return glfwWindowShouldClose(shared.OSWindow);
}

// exclusively callback

vec2 GLFWHandler::getMousePosWorld() {
    return screenToWorld(getMousePosScreen());
}
vec2 GLFWHandler::getMousePosScreen() {
    double x0, y0;
    glfwGetCursorPos(shared.OSWindow, &x0, &y0);
    vec2 cursorPos = {static_cast<float>(x0), static_cast<float>(y0)};
    return cursorPos;
}
void GLFWHandler::OSWindowResized(int width, int height) {
    this->scrWidth_px = width;
    this->scrHeight_px = height;
    shared.cam.aspectRatio = (height > 0) ? (float)width / (float)height : 1.0f;
}

mat4 GLFWHandler::makeViewProjection(vec2 pos, float zoomHalfHeight) {
    const float halfH = zoomHalfHeight;
    const float halfW = halfH * shared.cam.aspectRatio;

    // Projection: map world rectangle -> clip
    mat4 proj = ortho(-halfW, +halfW,  // left, right
                      -halfH, +halfH,  // bottom, top
                      -1.0f, +1.0f);

    // View: move the world opposite the camera position
    mat4 view = translate(mat4(1.0f), vec3(-pos, 0.0f));

    return proj * view;
}

/*
Everything below is a static callback or a mostly useless but long winded function.

















*/
// clang-format off
void GLFWHandler::error_callback_static(int error, const char* description) {
    LOG::err("GLFW Error {}: {}\n", error, description);
}
void GLFWHandler::mouse_scrolled_callback_static(GLFWwindow* window, double xoffset, double yoffset) {
    auto* inst = static_cast<GLFWHandler*>(glfwGetWindowUserPointer(window));
    if (inst) {
        inst->mouseScrolled(xoffset, yoffset);
    }
}
void GLFWHandler::mouse_entered_callback_static(GLFWwindow* window, int entered) {
    auto* inst = static_cast<GLFWHandler*>(glfwGetWindowUserPointer(window));
    if (inst) {
        entered ? inst->mouseEnteredOSWindow() : inst->mouseLeftOSWindow();
    }
}
void GLFWHandler::mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods) {
    auto* inst = static_cast<GLFWHandler*>(glfwGetWindowUserPointer(window));
    if (inst) {
        inst->mousePressed(button, action, mods);
    }
}
void GLFWHandler::mouse_move_callback(GLFWwindow* window, double x, double y) {
    auto* inst = static_cast<GLFWHandler*>(glfwGetWindowUserPointer(window));
    if (inst) {
        inst->mouseMoved(vec2(x, y));
    }
}

void GLFWHandler::key_callback_static(GLFWwindow* window, int key, int scancode, int action,
                                      int mods) {
    auto* inst = static_cast<GLFWHandler*>(glfwGetWindowUserPointer(window));
    if (inst) {
        inst->keyPressed(key, scancode, action, mods);
    }
}

void GLFWHandler::framebuffer_size_callback_static(GLFWwindow* window, int width, int height) {
    auto* inst = static_cast<GLFWHandler*>(glfwGetWindowUserPointer(window));
    if (inst) {
        inst->OSWindowResized(width, height);
    }
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
