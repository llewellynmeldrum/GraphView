#include "SharedContext.hpp"
#include <GLContext.hpp>
#include <glad/glad.h>
// Minimal-ish: assumes you already created a GLFW window + GL context.
// You need a loader (glad/glew). Example uses glad.

#include <GLFW/glfw3.h>
#include <cstdio>
#include <string>
#include <vector>

const char* GLContext::vertexShaderSrc = R"GLSL( 
	#version 410 core
        layout (location = 0) in vec2 aPos; 
        out vec2 vPos;
        void main() { 
            vPos = aPos; gl_Position = vec4(aPos, 0.0, 1.0); 
        }
    )GLSL";
const char* GLContext::fragShaderSrc = R"GLSL(
        #version 410 core
        in vec2 vPos;
        out vec4 FragColor;

        uniform vec2  uCenter;
        uniform float uRadius;
        uniform vec4  uColor;

        void main() {
            float dist = length(vPos - uCenter);
            float edge = 0.002;
            float alpha = 1.0 - smoothstep(uRadius, uRadius + edge, dist);
            FragColor = vec4(uColor.rgb, uColor.a * alpha);
        }
    )GLSL";

GLuint GLContext::compileShader(GLenum type, const char* src) {
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);
    GLint ok = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(shaderID, len, nullptr, log.data());
        std::fprintf(stderr, "Shader compile error:\n%s\n", log.c_str());
    }
    return shaderID;
}

GLuint GLContext::initProgram(const char* vertexSource, const char* fragSource) {
    GLuint vtxShaderID = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragShaderID = compileShader(GL_FRAGMENT_SHADER, fragSource);
    GLuint progID = glCreateProgram();
    glAttachShader(progID, vtxShaderID);
    glAttachShader(progID, fragShaderID);
    glLinkProgram(progID);
    GLint ok = 0;
    glGetProgramiv(progID, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(progID, len, nullptr, log.data());
        std::fprintf(stderr, "Program link error:\n%s\n", log.c_str());
    }
    glDeleteShader(vtxShaderID);
    glDeleteShader(fragShaderID);
    return progID;
}

int GLContext::initCircle() {
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) return 1;

    programID = initProgram(vertexShaderSrc, fragShaderSrc);

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

void GLContext::drawCircle(Vec2 uCenter, Vec4 uLoc, float uRad) {
    glUseProgram(programID);
    glUniform2f(glGetUniformLocation(programID, "uCenter"), uCenter.x, uCenter.y);
    glUniform1f(glGetUniformLocation(programID, "uRadius"), uRad);
    glUniform4f(glGetUniformLocation(programID, "uColor"), uLoc.r, uLoc.g, uLoc.b, uLoc.a);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLContext::cleanupCircle() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}
