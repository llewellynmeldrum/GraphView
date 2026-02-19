#pragma once
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "GLContext.hpp"
#include "SharedContext.hpp"
#include "imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
void        key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
static void error_callback(int error, const char* description) {
    LOG::err("GLFW Error {}: {}\n", error, description);
}
struct GLFWHandler {
    GLContext      gl;  // opengl context
    SharedContext& shared;

    const char* getGLSLVersion();

    GLFWHandler(auto& _shared) : shared(_shared) {}

    void handleInputs();
    void handleUIUpdates();
    void render();
    bool shouldClose();
    void destroy();

    // init glfw, create the OS window,
    void init(const AppConfig& cfg);

 private:
    Vec2 getGLFWWindowSize();
    void renderUI();
    void drawGraph(const Graph* G);
    void drawEdge(const Graph* G, Graph::Edge edge);
    void drawNode(const Graph* G, Graph::Node u);

    void drawLine(const Vec2& s, const Vec2& t);
    void drawCircle(const Vec2& s, float extent);
};
