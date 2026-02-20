#include "SharedContext.hpp"
#include <GLContext.hpp>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

const char* GLContext::vertexShaderSource = R"GLSL( 
        #version 410 core

        layout (location = 0) in vec2 aLocal; 

        uniform mat4 uViewProj;
        uniform vec2 uCenterWorld;
        uniform float uRadiusWorld;

        out vec2 vLocal;

        void main() { 
            vLocal = aLocal;

            vec2 worldPos = uCenterWorld + aLocal * uRadiusWorld;

            gl_Position = uViewProj * vec4(worldPos, 0.0, 1.0); 
        }
    )GLSL";
const char* GLContext::fragShaderSource = R"GLSL(
        #version 410 core

        in vec2 vLocal;
        out vec4 FragColor;
        uniform vec4 uColor;

        void main() {
            float dist = length(vLocal);
            float width = fwidth(dist);
            float alpha = 1.0 - smoothstep(1.0-width,  1.0 + width, dist);
            FragColor = vec4(uColor.rgb, uColor.a * alpha);
        }
    )GLSL";

void GLContext::drawCircle(glm::vec2 c, float rad, glm::vec4 uCol) {
    glUseProgram(programID);

    // need:
    glUniformMatrix4fv(glGetUniformLocation(programID, "uViewProj"), 1, GL_FALSE,
                       glm::value_ptr(viewProj));
    glUniform2fv(glGetUniformLocation(programID, "uCenterWorld"), 1, glm::value_ptr(c));
    glUniform1f(glGetUniformLocation(programID, "uRadiusWorld"), rad);

    glUniform4fv(glGetUniformLocation(programID, "uColor"), 1, glm::value_ptr(uCol));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
GLuint GLContext::InitShader(GLenum type, const char* src) {
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);
    if (GLContext::checkShaderError(GL_COMPILE_STATUS, shaderID)) {
        GLContext::reportShaderError(stderr, shaderID);
    }
    return shaderID;
}

struct VertexAttributes {
    GLuint      idx;
    GLint       size;
    GLenum      type;
    GLboolean   normalized = GL_FALSE;
    GLsizei     stride = size * sizeof(type);
    const void* ptr = (void*)0;
};

void SetAndBindVertexAttributes(VertexAttributes atr) {
    glEnableVertexAttribArray(atr.idx);
    glVertexAttribPointer(atr.idx, atr.size, atr.type, atr.normalized, atr.stride, atr.ptr);
    glBindVertexArray(atr.idx);
}
int GLContext::init() {
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::println("Error! unable to load GLAD!");
        std::exit(EXIT_FAILURE);
    }
    GLuint vtxShaderID = InitShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragShaderID = InitShader(GL_FRAGMENT_SHADER, fragShaderSource);
    initProgram(vtxShaderID, fragShaderID);

    // assign our NDC quad to the VBO (vertex buffer object)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shared.temp.NDC_quad), shared.temp.NDC_quad.data(),
                 GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    SetAndBindVertexAttributes({
            .idx = 0,
            .size = 2,
            .type = GL_FLOAT,
            .normalized = GL_FALSE,
    });

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return 0;
}

/*




*/

void GLContext::cleanup() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(programID);
}

bool GLContext::checkProgError(GLuint STATUS_CODE) {
    GLint ok = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &ok);
    return !ok;
}
bool GLContext::checkShaderError(GLuint STATUS_CODE, GLuint shaderID) {
    GLint ok = 0;
    glGetShaderiv(shaderID, GL_LINK_STATUS, &ok);
    return !ok;
}
void GLContext::reportProgError(FILE* outstream, GLuint progID) {
    GLint len = 0;
    glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    glGetProgramInfoLog(progID, len, nullptr, log.data());
    std::fprintf(outstream, "Program link error:\n%s\n", log.c_str());
}
void GLContext::reportShaderError(FILE* outstream, GLuint shaderID) {
    GLint len = 0;
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    glGetShaderInfoLog(shaderID, len, nullptr, log.data());
    std::fprintf(outstream, "Program link error:\n%s\n", log.c_str());
}
GLuint GLContext::initProgram(GLuint vertexShaderID, GLuint fragShaderID) {
    this->programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragShaderID);
    glLinkProgram(programID);

    if (GLContext::checkProgError(GL_LINK_STATUS)) {
        GLContext::reportProgError(stderr, programID);
    }

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragShaderID);
    return programID;
}
