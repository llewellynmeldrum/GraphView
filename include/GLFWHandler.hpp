#pragma once
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "GLContext.hpp"
#include "SharedContext.hpp"
#include "imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
struct GLFWHandler {
    SharedContext& shared;
    GLContext      gl;  // opengl context
    const char*    getGLSLVersion();

    GLFWHandler(auto& _shared) : shared(_shared), gl(_shared) {}
    //    GLFWHandler(auto& _shared) : shared(_shared){}

    void handleInputs();
    void handleUIUpdates();
    void render();
    bool shouldClose();
    void destroy();

    // init glfw, create the OS window,
    void init();

 private:
    glm::mat4 makeViewProjection(int width, int height, float zoomHalfHeight);
    glm::vec2 getGLFWWindowSize();

    void keyPressed(int key, int scancode, int action, int mods);
    void OSWindowResized(int width, int height);
    void mouseMoved(glm::vec2 nPos);
    void mousePressed(int button, int action, int mods);
    void mouseScrolled(double xoffset, double yoffset);

    void mouseEnteredOSWindow();
    void mouseLeftOSWindow();

    void renderUI();
    void drawGraph(const Graph* G);
    void drawEdge(const Graph* G, Graph::Edge edge);
    void drawNode(const Graph* G, Graph::Node u);

    static void error_callback_static(int error, const char* description);
    static void framebuffer_size_callback_static(GLFWwindow* window, int width, int height);
    static void key_callback_static(GLFWwindow* win, int key, int scancode, int action, int mods);

    static void mouse_scrolled_callback_static(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_entered_callback_static(GLFWwindow* window, int entered);
    static void mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods);
    static void mouse_move_callback(GLFWwindow* window, double x, double y);
};
