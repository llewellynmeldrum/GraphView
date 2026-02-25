// glad must come first or it gets pissy
#include <glad/glad.h>
// glad must come first or it gets pissy

#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <utility>
#include <vector>

#include "GLContext.hpp"
#include "GLContext_Private.hpp"
#include "SharedContext.hpp"
// QUAD DRAWING
const char* line_vtx_shader_src = R"GLSL( 
    #version 410 core
    layout (location = 0) in float aT;      // 0 at start, 1 at end
    layout (location = 1) in float aSide;   // -1 left, +1 right

    uniform mat4  uViewProj;
    uniform vec2  uA;        // start in world
    uniform vec2  uB;        // end in world
    uniform float uW0;       // thickness at start (world units)
    uniform float uW1;       // thickness at end   (world units)

    out float vSide;         // handy for AA / caps if you want later
    out float vT;            // along-line coordinate in [0,1]

    void main()
    {
        vT    = aT;
        vSide = aSide;
        vec2 AB = uB - uA;
        vec2 dir = normalize(AB);
        vec2 n = vec2(-dir.y, dir.x);
        vec2 P = mix(uA, uB, aT);
        float w = mix(uW0, uW1, aT);
        vec2 worldPos = P + n * aSide * (w*0.5);
        gl_Position = uViewProj * vec4(worldPos, 0.0, 1.0);
    }

    )GLSL";

const char* line_frag_shader_src = R"GLSL(
    #version 410 core
    in float vSide;
    in float vT;
    
    out vec4 FragColor;
    
    uniform vec4 uColor;
    uniform bool enableGreenToRedBlend;
    // enable dynamic toggles
    
    void main()
    {
        // We want alpha = 1 inside the quad, and fade out at the side edges.
        // vSide should interpolate from -1 at left edge to +1 at right edge,
        // and be ~0 near the centreline.
    
        float d = abs(vSide);     // 0 at centre, 1 at edges
    
        float aa = fwidth(d);
    
        // TODO: fade out around d==1 (the edges)
        // hint: alpha = 1 - smoothstep(1-aa, 1+aa, d)
        float alpha = 1-smoothstep(1-aa,1+aa,d);
    
        
    // Fade from green to red
    /*
        FragColor = vec4(uColor.rgb, uColor.a * alpha);
        FragColor.g=vT;
        FragColor.r=(1-vT);
    */
        FragColor = vec4(uColor.rgb, uColor.a * alpha);
    // Fade from opaque to transparent along vT
    /*
        FragColor*=vT;
    */
    
    }
    )GLSL";

// CIRCLE DRAWING
const char* circle_vtx_shader_src = R"GLSL( 
        #version 410 core
        layout (location = 0) in vec2 aLocal;   // -1 left, +1 right

        uniform mat4 uViewProj;
        uniform vec2 uCentreWorld;
        uniform float uRadiusWorld;

        out vec2 vLocal;

        void main() { 
            
            vLocal = aLocal; // set the local vertex pos as the out vec

            vec2 worldPos = uCentreWorld + aLocal * uRadiusWorld;
            gl_Position = uViewProj * vec4(worldPos, 0.0, 1.0); 
        }
    )GLSL";

const char* circle_frag_shader_src = R"GLSL(
        #version 410 core
        in vec2 vLocal;
        uniform vec4 uColor;

        out vec4 FragColor;

        void main() {
            float dist = length(vLocal);
            float width = fwidth(dist);
            float alpha = 1.0 - smoothstep(1.0-width,  1.0 + width, dist);
            FragColor = vec4(uColor.rgb, uColor.a * alpha);
        }
    )GLSL";

void OpenGLHandler::beginLinesPass() {
    glUseProgram(lines.prog);
    glBindVertexArray(lines.VAO);
    glUniformMatrix4fv(lines.attr.uViewProj, 1, GL_FALSE, glm::value_ptr(viewProj));
}

void OpenGLHandler::beginCirclePass() {
    glUseProgram(circles.prog);
    glBindVertexArray(circles.VAO);
    glUniformMatrix4fv(circles.attr.uViewProj, 1, GL_FALSE, glm::value_ptr(viewProj));
}

void OpenGLHandler::endPass() {
    glBindVertexArray(0);
    glUseProgram(0);
}
void OpenGLHandler::drawCircle(glm::vec2 centre, float radius, glm::vec4 col) {
    // relatively slow, uses uniforms and shit instead of buffers because im lazy
    glUseProgram(circles.prog);
    glUniform2fv(circles.attr.uCentreWorld, 1, glm::value_ptr(centre));
    glUniform1f(circles.attr.uRadiusWorld, radius);
    glUniform4fv(circles.attr.uColor, 1, glm::value_ptr(col));

    glBindVertexArray(circles.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void OpenGLHandler::drawLine(glm::vec2 s, glm::vec2 t, GLfloat w, glm::vec4 col) {
    drawTaperedLine(s, t, w, w, col);
}
void OpenGLHandler::drawTaperedLine(glm::vec2 s, glm::vec2 t, GLfloat w0, GLfloat w1,
                                    glm::vec4 col) {
    glUniform4fv(lines.attr.uColor, 1, glm::value_ptr(col));
    glUniform2fv(lines.attr.uA, 1, glm::value_ptr(s));
    glUniform2fv(lines.attr.uB, 1, glm::value_ptr(t));
    glUniform1f(lines.attr.uW0, w0);
    glUniform1f(lines.attr.uW1, w1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void OpenGLHandler::createLineProgram() {
    ShaderID vtx = initShader(GL_VERTEX_SHADER, line_vtx_shader_src, "Line:Vertex");
    ShaderID frag = initShader(GL_FRAGMENT_SHADER, line_frag_shader_src, "Line:Frag");
    lines.prog = initProgram(vtx, frag);
    setLineAttributeLocations();

    struct V {
        float t, side;
    };
    const V verts[6] = {
            {0.f, -1.f}, {1.f, -1.f}, {1.f, +1.f}, {0.f, -1.f}, {1.f, +1.f}, {0.f, +1.f},
    };

    glGenVertexArrays(1, &lines.VAO);
    glGenBuffers(1, &lines.VBO);

    glBindVertexArray(lines.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, lines.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // location 0: aT
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V, t));

    // location 1: aSide
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V, side));
    glBindVertexArray(0);
}

void OpenGLHandler::createCircleProgram() {
    ShaderID vtx = initShader(GL_VERTEX_SHADER, circle_vtx_shader_src, "Circle::vertex");
    ShaderID frag = initShader(GL_FRAGMENT_SHADER, circle_frag_shader_src, "Circle::frag");
    circles.prog = initProgram(vtx, frag);
    setCircleAttributeLocations();

    glGenVertexArrays(1, &circles.VAO);
    glGenBuffers(1, &circles.quadVBO);

    glBindVertexArray(circles.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, circles.quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(NDC_quad), NDC_quad.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,  // location
                          2,  // size
                          GL_FLOAT, GL_FALSE,
                          sizeof(float) * 2,  // tightly packed vec2
                          (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLHandler::cleanup() {
    glDeleteBuffers(1, &circles.quadVBO);
    glDeleteVertexArrays(1, &circles.VAO);
    glDeleteProgram(circles.prog);
}
// GENERIC
int OpenGLHandler::init() {
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::println("Error! unable to load GLAD!");
        return -1;
    }
    // init circle program
    createCircleProgram();
    createLineProgram();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return 0;
}

GLuint initProgram(GLuint vertexShaderID, GLuint fragShaderID) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShaderID);
    glAttachShader(program, fragShaderID);
    glLinkProgram(program);

    if (error(glGetProgramiv, program, GL_LINK_STATUS)) {
        print("Program link ");
        reportError(glGetProgramiv, glGetProgramInfoLog, program);
    }

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragShaderID);
    return program;
}

GLuint initShader(GLenum type, const char* src, const char* file) {
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);

    if (error(glGetShaderiv, GL_COMPILE_STATUS, shaderID)) {
        print("Shader ({})->", file);
        reportError(glGetShaderiv, glad_glGetShaderInfoLog, shaderID);
    }
    return shaderID;
}
void OpenGLHandler::setCircleAttributeLocations() {
    circles.attr.uViewProj = glGetUniformLocation(circles.prog, "uViewProj");
    circles.attr.uCentreWorld = glGetUniformLocation(circles.prog, "uCentreWorld");
    circles.attr.uRadiusWorld = glGetUniformLocation(circles.prog, "uRadiusWorld");
    circles.attr.uColor = glGetUniformLocation(circles.prog, "uColor");
}
void OpenGLHandler::setLineAttributeLocations() {
    lines.attr.uViewProj = glGetUniformLocation(lines.prog, "uViewProj");
    lines.attr.uA = glGetUniformLocation(lines.prog, "uA");
    lines.attr.uB = glGetUniformLocation(lines.prog, "uB");
    lines.attr.uW0 = glGetUniformLocation(lines.prog, "uW0");
    lines.attr.uW1 = glGetUniformLocation(lines.prog, "uW1");
    lines.attr.uColor = glGetUniformLocation(lines.prog, "uColor");
}
