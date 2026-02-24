#include "GLFWHandler.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <OpenGL/gl.h>
#include <cmath>
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
    shared.dpiScaling = ImGui_ImplGlfw_GetContentScaleForMonitor(primaryMonitor);
    println("mainScale = {}", shared.dpiScaling);
    int screenWidth = glfwGetVideoMode(primaryMonitor)->width;
    int screenHeight = glfwGetVideoMode(primaryMonitor)->height;

    shared.p_viewport =
            glfwCreateWindow((screenWidth / 2.0) * shared.dpiScaling,
                             screenHeight * shared.dpiScaling, "Viewport title", nullptr, nullptr);
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
    glfwSetWindowUserPointer(shared.p_viewport, this);

    glfwSetKeyCallback(shared.p_viewport, key_callback_static);
    glfwSetFramebufferSizeCallback(shared.p_viewport, framebuffer_size_callback_static);
    glfwSetCursorEnterCallback(shared.p_viewport, mouse_entered_callback_static);
    glfwSetMouseButtonCallback(shared.p_viewport, mouse_button_callback_static);
    glfwSetScrollCallback(shared.p_viewport, mouse_scrolled_callback_static);
    glfwSetCursorPosCallback(shared.p_viewport, mouse_move_callback);

    const char* opengl_ver_str = (const char*)glGetString(GL_VERSION);
    println("OPENGL VERSION:{}", opengl_ver_str);
}
void GLFWHandler::render() {
    glfwGetFramebufferSize(shared.p_viewport, &scrWidth_px, &scrHeight_px);
    glViewport(0, 0, scrWidth_px, scrHeight_px);
    glClearColor(COLOR(shared.bgColor));
    glClear(GL_COLOR_BUFFER_BIT);

    gl.viewProj = makeViewProjection(shared.cam.smoothPos, DEFAULT_ZOOM_AMOUNT / shared.cam.zoom);
    gl.invViewProj = glm::inverse(gl.viewProj);

    gl.raw_viewProj = makeViewProjection(shared.cam.truePos, DEFAULT_ZOOM_AMOUNT / shared.cam.zoom);
    gl.raw_invViewProj = glm::inverse(gl.raw_viewProj);

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
    if (shared.graphExists && !shared.graphConfig.isHidden) {
        drawGraph(shared.graphConfig.ptr.get());
    }
    if (shared.graphConfig.draw.showBounds) {
        drawGraphBounds(5.0f, {1, 0, 0, 0.5});
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(shared.p_viewport);
}
#define xy(var) var.x, var.y
#define xyz(var) var.x, var.y, var.z
#define xyzw(var) var.x, var.y, var.z, var.w

glm::vec2 GLFWHandler::screenToWorld(glm::vec2 scr2) {
    // always use the raw_InvViewProj

    auto ndc2 = scr2;
    // 1. convert screen space to ndc
    ndc2.x = 2.0f * scr2.x / scrWidth_px - 1.0f;
    ndc2.y = 1.0f - 2.0f * scr2.y / scrHeight_px;  // flip for tl origin

    auto ndc_z = +1.0f;  // far plane
    // float ndc_z = +1;  // near plane

    // 2. convert ndc to clip
    auto clip4 = glm::vec4{xy(ndc2), ndc_z, 1.0f};  // w=1 means coord rather than vector

    // 3. convert clip->world ==> worldPos = inverse(viewProj) * clipPos
    auto world4 = gl.raw_invViewProj * clip4;
    auto world3 = glm::vec3{xyz(world4)};
    world3 / world4.w;
    // println("s:[{},{}]->w:[{},{}]", scr2.x, scr2.y, world3.x, world3.y);
    return glm::vec2{world3.x, world3.y};
}
void GLFWHandler::drawGrid(int scrWidth, int scrHeight) {
    gl.beginLinesPass();
    struct Grid {
        float     gap;
        float     thick;
        glm::vec4 color;
    };

    auto light = Grid{.gap = 10.0f, .thick = 1.0f, .color = {LIGREY}};

    constexpr float MIN_X = -10'000;
    constexpr float MIN_Y = -10'000;
    constexpr float MAX_X = +10'000;
    constexpr float MAX_Y = +10'000;

    // draw a grid which is 1.5x bigger than the screen.
    for (float x = MIN_X; x < MAX_X; x += light.gap) {
        gl.drawLine({x, MIN_Y}, {x, MAX_Y}, light.thick, light.color);
    }
    for (int y = MIN_Y; y < MAX_Y; y += light.gap) {
        gl.drawLine({MIN_X, y}, {MAX_X, y}, light.thick, light.color);
    }

    /*
    for (int x = x0; x < x1; x += darkGap) {
        gl.drawLine({x, y0}, {x, y1}, darkThickness, DARKGREY);
    }
    for (int y = y0; y < y1; y += darkThickness) {
        gl.drawLine({x0, y}, {x1, y}, darkThickness, DARKGREY);
    }
    */
    gl.endPass();
}
void GLFWHandler::drawGraphBounds(float thick, glm::vec4 color) {
    gl.beginLinesPass();
    auto& xbounds = shared.graphInitConfig.xBounds;
    auto& ybounds = shared.graphInitConfig.yBounds;
    auto  bl = glm::vec2{xbounds[0], ybounds[0]};
    auto  br = glm::vec2{xbounds[1], ybounds[0]};
    auto  tl = glm::vec2{xbounds[0], ybounds[1]};
    auto  tr = glm::vec2{xbounds[1], ybounds[1]};

    // tl-->tr
    // ^    |
    // |    V
    // bl<--br
    gl.drawLine(tl, tr, thick, color);
    gl.drawLine(tr, br, thick, color);
    gl.drawLine(br, bl, thick, color);
    gl.drawLine(bl, tl, thick, color);
    gl.endPass();
}

// clang-format on
inline void GLFWHandler::drawNode(const Graph* G, Graph::Node u) {
    const auto& draw = shared.graphConfig.draw;
    const auto& pos = G->nodePositions[u];
    const auto& nodeSize_px = draw.nodeSizeWorld + G->degreeFactor[u];
    const auto  isNodeColored = static_cast<bool>(G->isNodeColored[u]);
    const auto& nodeColor =
            (isNodeColored) ? G->nodeColors[u] : shared.graphConfig.draw.baseNodeColor;
    gl.drawCircle(pos, nodeSize_px, nodeColor);
}

inline void GLFWHandler::drawEdge(const Graph* G, Graph::Edge edge) {
    const auto& draw = shared.graphConfig.draw;
    const auto& upos = G->nodePositions[edge.u];
    const auto& vpos = G->nodePositions[edge.v];
    const auto& uScale = static_cast<float>(draw.edgeTaperOutgoing + G->degreeFactor[edge.u]);
    const auto& vScale = static_cast<float>(draw.edgeTaperIncoming);  //* G->degreeFactor[edge.v]);

    gl.drawTaperedLine(upos, vpos, uScale, vScale, draw.edgeColor);
}
void GLFWHandler::drawGraph(const Graph* G) {
    gl.beginLinesPass();
    for (size_t e = 0; e < G->init_cfg.E; e++) {
        drawEdge(G, G->edges[e]);
    }
    gl.endPass();

    gl.beginCirclePass();
    for (size_t u = 0; u < G->init_cfg.V; u++) {
        drawNode(G, u);
    }
    gl.endPass();
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
glm::vec2 lerp2(glm::vec2 a, glm::vec2 b, double alpha) {
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
    if (tprev == -1) tprev = glfwGetTime();
    double tnow = glfwGetTime();
    double dT = tnow - tprev;
    tprev = tnow;
    applyCameraSmoothing(dT);
    if (shared.graphExists) shared.graphConfig.ptr->update(dT);

    auto& io = IG::GetIO();
    shared.ignoreMouseInput = io.WantCaptureMouse;
    shared.ignoreKeyboardInput = io.WantCaptureKeyboard;
    glfwPollEvents();
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

glm::vec2 GLFWHandler::getMousePosWorld() {
    return screenToWorld(getMousePosScreen());
}
glm::vec2 GLFWHandler::getMousePosScreen() {
    double x0, y0;
    glfwGetCursorPos(shared.p_viewport, &x0, &y0);
    glm::vec2 cursorPos = {static_cast<float>(x0), static_cast<float>(y0)};
    return cursorPos;
}
void GLFWHandler::mouseMoved(glm::vec2 nPos) {
    if (shared.ignoreMouseInput) {
        glfwSetInputMode(shared.p_viewport, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        input.dragging = false;
        return;
    }
    if (input.dragging) {
        const glm::vec2 nowWorld = getMousePosWorld();
        const glm::vec2 delta = input.grabScreen - nowWorld;
        shared.cam.truePos += delta;
    }
}
void GLFWHandler::mousePressed(int button, int action, int mods) {
    if (shared.ignoreMouseInput) {
        glfwSetInputMode(shared.p_viewport, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        input.dragging = false;
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_1) {
        if (action == GLFW_PRESS) {
            if (!input.dragging) {
                //                glfwSetInputMode(shared.p_viewport, GLFW_CURSOR,
                //                GLFW_CURSOR_DISABLED);
            }
            input.dragging = true;
            input.grabScreen = getMousePosWorld();
        }
        if (action == GLFW_RELEASE) {
            glfwSetInputMode(shared.p_viewport, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            input.dragging = false;
            // move cursor to the place it now exists in
        }
    }
    // button 1 = right, 0 = left
    // action 1 = press, 0 = unpress
}

static float zoom = 1.0f;

void GLFWHandler::mouseScrolled([[maybe_unused]] double xoffset, double yoffset) {
    float delta = shared.cam.zoom * (yoffset / 10.0);
    if (shared.cam.zoom + delta > shared.cam.MAX_ZOOM) {
        shared.cam.zoom = shared.cam.MAX_ZOOM;
    } else if (shared.cam.zoom + delta < shared.cam.MIN_ZOOM) {
        shared.cam.zoom = shared.cam.MIN_ZOOM;
    } else {
        shared.cam.zoom += delta;
    }
}
void GLFWHandler::mouseEnteredOSWindow() {
}
void GLFWHandler::mouseLeftOSWindow() {
}
void GLFWHandler::keyPressed(int key, int scancode, int action, int mods) {
    auto& update = shared.graphConfig.update;
    if (shared.cfg.PRINT_KEY_EVENTS)
        println("Key:{}, scancode:{}, action:{}, mods:{}", key, scancode, action, mods);
    if (action == GLFW_PRESS) {
        switch (key) {
        case 'C':
            if (mods == GLFW_MOD_CONTROL) {
                shared.app->exit(EXIT_SUCCESS);
            }
            break;
        case ' ':
            {
                if (update.isPaused) {
                    update.isPaused = false;
                    update.timeScale = 1.0f;
                } else {
                    update.isPaused = true;
                    update.timeScale = 0.0f;
                }
                break;
            }
        case 'R': shared.uiRequestsGraphGeneration = true; break;
        case 'F':
            shared.graphConfig.update.isForceDirected = !shared.graphConfig.update.isForceDirected;
            break;

        default: break;
        }
    }
}
void GLFWHandler::OSWindowResized(int width, int height) {
    this->scrWidth_px = width;
    this->scrHeight_px = height;
    shared.cam.aspectRatio = (height > 0) ? (float)width / (float)height : 1.0f;
}

glm::mat4 GLFWHandler::makeViewProjection(glm::vec2 pos, float zoomHalfHeight) {
    const float halfH = zoomHalfHeight;
    const float halfW = halfH * shared.cam.aspectRatio;

    // Projection: map world rectangle -> clip
    glm::mat4 proj = glm::ortho(-halfW, +halfW,  // left, right
                                -halfH, +halfH,  // bottom, top
                                -1.0f, +1.0f);

    // View: move the world opposite the camera position
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-pos, 0.0f));

    return proj * view;
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
