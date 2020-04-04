//
// Created by czf on 19-11-9.
//

#include "BaseShader.h"
#include "../utils/ShaderUtils.h"

// three types of the precision: lowp, mediump and highp.
// for vertex, if the precision is not specified, it is consider to be highest (highp).
static const char *vert = "#version 300 es\n"
                          "layout(location = 0) in vec4 vPosition;\n"

                          "uniform vec3 translate;\n" // vec is not array
                          "uniform vec3 scale;\n" // vec is not array
                          "uniform vec3 rotate;\n" // vec is not array

                          "out vec4 myColor;\n" // send to next stage(frag shader)

                          "void main() {\n"
                          "    gl_Position = vPosition;\n"

                          "    gl_Position[0] *= scale[0];\n"
                          "    gl_Position[1] *= scale[1];\n"
                          "    gl_Position[2] *= scale[2];\n"

                          "    float x = gl_Position[0];\n"
                          "    float y = gl_Position[1];\n"
                          "    float z = gl_Position[2];\n"
                          "    // 绕x轴旋转\n"
                          "    float xDeg = rotate[0];\n"
                          "    gl_Position[2] = z*cos(xDeg) - y*sin(xDeg);\n"
                          "    gl_Position[1] = z*sin(xDeg) + y*cos(xDeg);\n"

                          "    // 绕y轴旋转\n"
                          "    z = gl_Position[2];\n"
                          "    float yDeg = rotate[1];\n"
                          "    gl_Position[0] = x*cos(yDeg) - z*sin(yDeg);\n"
                          "    gl_Position[2] = x*sin(yDeg) + z*cos(yDeg);\n"

                          "    // 绕z轴旋转\n"
                          "    x = gl_Position[0];\n"
                          "    y = gl_Position[1];\n"
                          "    float zDeg = rotate[2];\n"
                          "    gl_Position[0] = x*cos(zDeg) - y*sin(zDeg);\n"
                          "    gl_Position[1] = x*sin(zDeg) + y*cos(zDeg);\n"

                          "    gl_Position[0] += translate[0];\n"
                          "    gl_Position[1] += translate[1];\n"
                          "    gl_Position[2] += translate[2];\n"

                          "    float c;\n"
                          "    if (gl_Position[2] < 0.0) {\n"
                          "        c = 0.5 - gl_Position[2]/1.3;\n"
                          "    } else {\n"
                          "        c = 0.5 - gl_Position[2]/4.0;\n"
                          "    }\n"
                          "    myColor = vec4(c, c, c, 1.0);\n"
                          "}\n";

// for fragment shader, Specifying the precision is compulsory.
static const char *frag = "#version 300 es\n"
                          "precision mediump float;\n"
                          "in vec4 myColor;\n" // receive from previous stage(vert shader)
                          "out vec4 fColor;\n"
                          "void main() {\n"
                          "    fColor = myColor;\n"
                          "}\n";

GLuint BaseShader::vertShader = 0;
GLuint BaseShader::fragShader = 0;
GLuint BaseShader::program = 0;

GLuint BaseShader::getSingletonProgram() {
    if (program == 0) {
        vertShader = get_compiled_shader_vert(vert);
        fragShader = get_compiled_shader_frag(frag);
        program = linkShader(vertShader, fragShader);
    }
    return program;
}

void BaseShader::deleteSingletonProgram() {
    if (program != 0) {
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        glDeleteProgram(program);
        vertShader = 0;
        fragShader = 0;
        program = 0;
    }
}
