#pragma once
#include "Vectors.hpp"
#include <OpenGL/gltypes.h>
struct GLContext {
    // try to make the public interface as narrow as possible
    int  initCircle();
    void drawCircle(Vec2 uPos, Vec4 uLoc, float uRad);
    void cleanupCircle();

 private:
    GLuint programID = 0;
    GLuint vao = 0, vbo = 0;
    GLuint initProgram(const char* vsSrc, const char* fsSrc);
    GLuint compileShader(GLenum type, const char* src);

    static const char* vertexShaderSrc;

    static const char* fragShaderSrc;
};
