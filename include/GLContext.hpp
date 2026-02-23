#pragma once
#include "SharedContext.hpp"
#include "Vectors.hpp"
#include <OpenGL/gltypes.h>
#include <glm/glm.hpp>
#include <string>
using ProgramID = GLuint;
using ShaderID = GLuint;
using glID = GLuint;
struct GLContext {
    // try to make the public interface as narrow as possible
    // TODO: Remove! this is just for testing
    SharedContext& shared;
    GLContext(auto& _s) : shared(_s) {}

    int  init();
    void drawCircle(glm::vec2 centreWorld, float radiusWorld, glm::vec4 color);
    void drawLine(glm::vec2 s, glm::vec2 t, GLfloat w, glm::vec4 col);
    void drawTaperedLine(glm::vec2 s, glm::vec2 t, GLfloat w0, GLfloat w1, glm::vec4 col);
    void drawLine(glm::vec2 s, glm::vec2 t, float th);

    void beginLinesPass();
    void beginCirclePass();
    void endPass();
    void cleanup();

    glm::mat4 viewProj;  // set by GLFW once per frame via makeViewProjection()
    glm::mat4 invViewProj;

    glm::mat4 raw_viewProj;  // set by GLFW once per frame via makeViewProjection()
    glm::mat4 raw_invViewProj;

 private:
    struct LineRenderer {
        ProgramID prog;
        GLuint    VAO;
        GLuint    VBO;

        struct AttributeLocations {
            GLuint uViewProj;
            GLuint uA;
            GLuint uB;
            GLuint uW0;
            GLuint uW1;
            GLuint uColor;
        } attr;
    } lines;

    struct CircleRenderer {
        ProgramID prog;
        GLuint    VAO;
        GLuint    quadVBO;

        struct Uniforms {
            GLuint aaRadius;
        } uniforms;
        struct AttributeLocations {
            GLuint uViewProj;
            GLuint uCentreWorld;
            GLuint uRadiusWorld;
            GLuint uColor;
        } attr;

        void* uniform_data;
    } circles;
    void setCircleAttributeLocations();
    void setLineAttributeLocations();
    void setLineMesh();
    void createCircleProgram();
    void createLineProgram();
};
