#pragma once
#include "GLContext.hpp"
#include "SharedContext.hpp"
#include <OpenGL/gl.h>
#include <OpenGL/gltypes.h>
#include <string>
struct VertexAttributes {
    GLuint      idx;
    GLint       size;
    GLenum      type;
    GLboolean   normalized = GL_FALSE;
    GLsizei     stride = size * sizeof(type);
    const void* ptr = (void*)0;
};
static inline std::array<float, 12> NDC_quad = {-1.f, -1.f, 1.f, -1.f, 1.f,  1.f,
                                                -1.f, -1.f, 1.f, 1.f,  -1.f, 1.f};
template <class Fn>
GLuint error(Fn&& fn, GLuint id, GLenum en) {
    GLint ok = 0;
    fn(id, en, &ok);
    return ok;
}

template <class Fn1, class Fn2>
void reportError(Fn1&& a, Fn2&& b, GLuint id) {
    GLint len = 0;
    a(id, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    b(id, len, nullptr, log.data());
    LOG::err("error:\n{}\n", log);
}

ProgramID initProgram(GLuint vertexShaderID, GLuint fragShaderID);
GLuint    initShader(GLenum type, const char* src, const char* file);
void      bindVAO(VertexAttributes atr);
