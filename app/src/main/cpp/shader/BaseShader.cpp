//
// Created by czf on 19-11-9.
//

#include "BaseShader.h"
#include "../utils/ShaderUtils.h"

// three types of the precision: lowp, mediump and highp.
// for vertex, if the precision is not specified, it is consider to be highest (highp).
static const char *vert = "#version 300 es\n"
                          "layout(location = 0) in vec4 vPosition;\n"
                          "layout(location = 1) in vec2 vTexCoord;\n"
                          "layout(location = 2) in vec3 vNormal;\n"

                          "uniform vec3 lightPosition;\n" // 光源位置
                          "uniform vec3 lightColor;\n" // 光源颜色
                          "uniform vec3 translate;\n" // vec is not array
                          "uniform vec3 scale;\n"
                          "uniform vec3 rotate;\n"

                          "out vec2 texCoord;\n" // send to next stage(frag shader)
                          "out vec4 diffuse;\n" // send to next stage(frag shader)

                          "void main() {\n"
                          "    gl_Position = vPosition;\n"
                          "    vec3 normal = vNormal;\n" // 法向量，已经归一化，只随旋转而变

                          "    gl_Position[0] *= scale[0];\n"
                          "    gl_Position[1] *= scale[1];\n"
                          "    gl_Position[2] *= scale[2];\n"

                          "    float x = gl_Position[0];\n"
                          "    float y = gl_Position[1];\n"
                          "    float z = gl_Position[2];\n"
                          "    float nx = normal[0];\n" // 法线坐标，进行旋转变化
                          "    float ny = normal[1];\n"
                          "    float nz = normal[2];\n"
                          "    // 绕x轴旋转\n"
                          "    float xDeg = rotate[0];\n"
                          "    gl_Position[2] = z*cos(xDeg) - y*sin(xDeg);\n"
                          "    gl_Position[1] = z*sin(xDeg) + y*cos(xDeg);\n"
                          "    normal[2] = nz*cos(xDeg) - ny*sin(xDeg);\n"
                          "    normal[1] = nz*sin(xDeg) + ny*cos(xDeg);\n"

                          "    // 绕y轴旋转\n"
                          "    z = gl_Position[2];\n"
                          "    nz = normal[2];\n"
                          "    float yDeg = rotate[1];\n"
                          "    gl_Position[0] = x*cos(yDeg) - z*sin(yDeg);\n"
                          "    gl_Position[2] = x*sin(yDeg) + z*cos(yDeg);\n"
                          "    normal[0] = nx*cos(yDeg) - nz*sin(yDeg);\n"
                          "    normal[2] = nx*sin(yDeg) + nz*cos(yDeg);\n"

                          "    // 绕z轴旋转\n"
                          "    x = gl_Position[0];\n"
                          "    y = gl_Position[1];\n"
                          "    nx = normal[0];\n"
                          "    ny = normal[1];\n"
                          "    float zDeg = rotate[2];\n"
                          "    gl_Position[0] = x*cos(zDeg) - y*sin(zDeg);\n"
                          "    gl_Position[1] = x*sin(zDeg) + y*cos(zDeg);\n"
                          "    normal[0] = nx*cos(zDeg) - ny*sin(zDeg);\n"
                          "    normal[1] = nx*sin(zDeg) + ny*cos(zDeg);\n"

                          "    gl_Position[0] += translate[0];\n"
                          "    gl_Position[1] += translate[1];\n"
                          "    gl_Position[2] += translate[2];\n"

                          "    // 计算漫反射光\n"
                          "    vec3 modelVertex = vec3(gl_Position[0], gl_Position[1], gl_Position[2]);\n"
                          "    vec3 nLight = normalize(lightPosition - modelVertex);\n"
                          "    float cosAngle = max(0.0, dot(normal, nLight));\n"
                          "    diffuse = vec4(cosAngle * lightColor, 1.0);\n"

                          "    texCoord = vTexCoord;\n"
                          "}\n";

// for fragment shader, Specifying the precision is compulsory.
static const char *frag = "#version 300 es\n"
                          "precision mediump float;\n"

                          "in vec2 texCoord;\n" // receive from previous stage(vert shader)
                          "in vec4 diffuse;\n" // receive from previous stage(vert shader)

                          "uniform sampler2D texture;\n"

                          "uniform vec4 ambient;\n"

                          "out vec4 fColor;\n"

                          "void main() {\n"
                          "    fColor = texture(texture, texCoord) * ambient * diffuse;\n"
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
