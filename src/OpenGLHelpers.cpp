#include "SharedContext.hpp"
#include <OpenGLHelpers.hpp>
#include <glad/glad.h>
// Minimal-ish: assumes you already created a GLFW window + GL context.
// You need a loader (glad/glew). Example uses glad.

#include <GLFW/glfw3.h>
#include <cstdio>
#include <string>
#include <vector>
GLuint GLContext::compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(s, len, nullptr, log.data());
        std::fprintf(stderr, "Shader compile error:\n%s\n", log.c_str());
    }
    return s;
}

GLuint GLContext::makeProgram(const char* vsSrc, const char* fsSrc) {
    GLuint vtxShader = compileShader(GL_VERTEX_SHADER, vsSrc);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fsSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vtxShader);
    glAttachShader(prog, fragShader);
    glLinkProgram(prog);
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        std::fprintf(stderr, "Program link error:\n%s\n", log.c_str());
    }
    glDeleteShader(vtxShader);
    glDeleteShader(fragShader);
    return prog;
}

int GLContext::initCircle() {
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) return 1;

    const char* vertexShaderSrc = R"GLSL(
        #version 410 core
        layout (location = 0) in vec2 aPos;
        out vec2 vPos;
        void main() { vPos = aPos; gl_Position = vec4(aPos, 0.0, 1.0); }
    )GLSL";

    const char* fragShaderSrc = R"GLSL(
        #version 410 core
        in vec2 vPos;
        out vec4 FragColor;
        uniform vec2  uCenter;
        uniform float uRadius;
        uniform vec4  uColor;
        void main() {
            float d = length(vPos - uCenter);
            float edge = 0.002;
            float alpha = 1.0 - smoothstep(uRadius, uRadius + edge, d);
            FragColor = vec4(uColor.rgb, uColor.a * alpha);
        }
    )GLSL";

    prog = makeProgram(vertexShaderSrc, fragShaderSrc);

    // 2 triangles forming a quad in NDC
    float quad[] = {-1.f, -1.f, 1.f, -1.f, 1.f, 1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return 0;
}

void GLContext::drawCircle() {
    glUseProgram(prog);
    glUniform2f(glGetUniformLocation(prog, "uCenter"), 0.0f, 0.0f);
    glUniform1f(glGetUniformLocation(prog, "uRadius"), 0.35f);
    glUniform4f(glGetUniformLocation(prog, "uColor"), 0.2f, 0.8f, 1.0f, 1.0f);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLContext::cleanupCircle() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(prog);
}
