#pragma once
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "GLContext.hpp"
#include "Graph.hpp"
#include "SharedContext.hpp"
#include "imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#ifndef DEBUG
// debug mode has the window take up the left half of the screen
constexpr float WINDOW_W_SCALE = 2.0f;
#else
constexpr float WINDOW_W_SCALE = 1.0f;
#endif
struct GLFWHandler {
    SharedContext& shared;
    OpenGLHandler  gl;

    GLFWHandler(SharedContext& _shared) : shared(_shared), gl(_shared) {}
    //    GLFWHandler(auto& _shared) : shared(_shared){}

    void handleInputs();
    void updateGraph();
    void render();
    bool shouldClose();
    void destroy();

    // init glfw, create the OS window,
    void init();

    vec2 worldToScreen(vec2 w2);

    struct Input {
        bool dragging = false;
        vec2 grabScreen{0, 0};
        vec2 grabWorld{0, 0};
        bool clickedThisFrame = false;
    } input;

 private:
    void renderUI();
    void drawGraph(const Graph* G);
    void drawEdge(const Graph* G, Graph::Edge edge);
    void drawNode(const Graph* G, Graph::Node u);
    void drawGraphBounds(float thick, vec4 color);

    double tLastFrame = -1;
    vec2   getMousePosScreen();
    vec2   getMousePosWorld();
    vec2   screenToWorld(vec2 scr2);

    // = how many world units fits in the height of the screen at zoom=1.0f
    static constexpr float DEFAULT_ZOOM_AMOUNT = 500.0f;

    int  scrWidth_px;
    int  scrHeight_px;
    mat4 makeViewProjection(vec2 pos, float zoomHalfHeight);
    void applyCameraSmoothing(double dT);

    vec2         getOSWindowSize();
    static float getDPIScaling(GLFWmonitor* monitor);
    static vec2  getMainDisplaySize(GLFWmonitor* monitor);

    void keyPressed(int key, int scancode, int action, int mods);
    void OSWindowResized(int width, int height);
    void mouseMoved(vec2 nPos);
    void mousePressed(int button, int action, int mods);
    void mouseScrolled(double xoffset, double yoffset);
    void mouseEnteredOSWindow();
    void mouseLeftOSWindow();

    void setCallbacks();
    // ----
    static void error_callback_static(int error, const char* description);
    static void framebuffer_size_callback_static(GLFWwindow* window, int width, int height);
    static void key_callback_static(GLFWwindow* win, int key, int scancode, int action, int mods);
    static void mouse_scrolled_callback_static(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_entered_callback_static(GLFWwindow* window, int entered);
    static void mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods);
    static void mouse_move_callback(GLFWwindow* window, double x, double y);

    const char* getGLSLVersion();
};
