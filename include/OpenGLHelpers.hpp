#pragma once
#include <OpenGL/gltypes.h>
struct GLContext {
    GLuint        prog = 0;
    GLuint        vao = 0, vbo = 0;
    static GLuint compileShader(GLenum type, const char* src);
    GLuint        makeProgram(const char* vsSrc, const char* fsSrc);
    int           initCircle();
    void          drawCircle();
    void          cleanupCircle();
};  // namespace GL
