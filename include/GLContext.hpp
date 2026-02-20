#pragma once
#include "SharedContext.hpp"
#include "Vectors.hpp"
#include <OpenGL/gltypes.h>
#include <string>
struct GLContext {
    // try to make the public interface as narrow as possible
    // TODO: Remove! this is just for testing
    SharedContext& shared;
    GLContext(auto& _s) : shared(_s) {}
    int  init();
    void drawCircle(Vec2 uPos, float uRad, Vec4 uCol);
    void cleanup();

 private:
    bool   checkProgError(GLuint STATUS_CODE);
    bool   checkShaderError(GLuint STATUS_CODE, GLuint shaderID);
    void   reportProgError(FILE* outstream, GLuint progID);
    void   reportShaderError(FILE* outstream, GLuint shaderID);
    GLuint programID = 0;
    GLuint VAO = 0, VBO = 0;
    GLuint initProgram(GLuint vertexShaderID, GLuint fragShaderID);
    GLuint InitShader(GLenum type, const char* src);

    static const char* vertexShaderSource;

    static const char* fragShaderSource;
};
