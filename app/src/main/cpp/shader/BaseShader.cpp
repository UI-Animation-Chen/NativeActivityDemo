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

                          "uniform vec3 translate;\n" // vec is not array
                          "uniform vec3 scale;\n"
                          "uniform vec3 rotate;\n"

                          "out vec2 texCoord;\n" // send to next stage(frag shader)
                          "out vec3 modelNormal;\n"
                          "out vec3 modelVertex;\n"

                          "void main() {\n"
                          "    gl_Position = vPosition;\n"
                          "    modelNormal = normalize(vNormal);\n" // 法向量，进行归一化，片元中仍需要。

                          "    gl_Position[0] *= scale[0];\n"
                          "    gl_Position[1] *= scale[1];\n"
                          "    gl_Position[2] *= scale[2];\n"

                          "    float x = gl_Position[0];\n"
                          "    float y = gl_Position[1];\n"
                          "    float z = gl_Position[2];\n"
                          "    float nx = modelNormal[0];\n" // 法线坐标，进行旋转变化
                          "    float ny = modelNormal[1];\n"
                          "    float nz = modelNormal[2];\n"
                               // 绕x轴旋转
                          "    float xDeg = rotate[0];\n"
                          "    gl_Position[2] = z*cos(xDeg) - y*sin(xDeg);\n"
                          "    gl_Position[1] = z*sin(xDeg) + y*cos(xDeg);\n"
                          "    modelNormal[2] = nz*cos(xDeg) - ny*sin(xDeg);\n"
                          "    modelNormal[1] = nz*sin(xDeg) + ny*cos(xDeg);\n"
                               // 绕y轴旋转
                          "    z = gl_Position[2];\n"
                          "    nz = modelNormal[2];\n"
                          "    float yDeg = rotate[1];\n"
                          "    gl_Position[0] = x*cos(yDeg) - z*sin(yDeg);\n"
                          "    gl_Position[2] = x*sin(yDeg) + z*cos(yDeg);\n"
                          "    modelNormal[0] = nx*cos(yDeg) - nz*sin(yDeg);\n"
                          "    modelNormal[2] = nx*sin(yDeg) + nz*cos(yDeg);\n"
                               // 绕z轴旋转
                          "    x = gl_Position[0];\n"
                          "    y = gl_Position[1];\n"
                          "    nx = modelNormal[0];\n"
                          "    ny = modelNormal[1];\n"
                          "    float zDeg = rotate[2];\n"
                          "    gl_Position[0] = x*cos(zDeg) - y*sin(zDeg);\n"
                          "    gl_Position[1] = x*sin(zDeg) + y*cos(zDeg);\n"
                          "    modelNormal[0] = nx*cos(zDeg) - ny*sin(zDeg);\n"
                          "    modelNormal[1] = nx*sin(zDeg) + ny*cos(zDeg);\n"

                          "    gl_Position[0] += translate[0];\n"
                          "    gl_Position[1] += translate[1];\n"
                          "    gl_Position[2] += translate[2];\n"

                          "    modelVertex = vec3(gl_Position[0], gl_Position[1], gl_Position[2]);\n"
                          "    texCoord = vTexCoord;\n"
                          "}\n";

// for fragment shader, Specifying the precision is compulsory.
static const char *frag = "#version 300 es\n"
                          "precision mediump float;\n"

                          "in vec2 texCoord;\n" // 纹理坐标
                          "in vec3 modelNormal;\n" // 法向量
                          "in vec3 modelVertex;\n" // 变换之后的顶点坐标

                          "uniform sampler2D texture;\n"
                          "uniform vec4 ambient;\n"
                          "uniform vec3 lightPosition;\n" // 光源位置
                          "uniform vec3 lightColor;\n" // 光源颜色

                          "out vec4 fColor;\n"

                          "void main() {\n"
                          "    vec3 nNormal = normalize(modelNormal);\n"
                          "    vec3 nLight = normalize(lightPosition - modelVertex);\n"
                               // 漫反射光
                          "    float cosAngle = max(0.0, dot(nNormal, nLight));\n"
                          "    vec4 diffuse = vec4(cosAngle * lightColor, 1.0);\n"
                               // 镜面反射光
                          "    vec3 nViewerPosition = vec3(0.0, 0.0, -1.0);\n"
                          "    vec3 nH = normalize(nLight + nViewerPosition);\n"
                          "    float shininessFactor = 40.0;\n" // 确定高光区域大小，值越大区域越小，1-200
                          "    float sIntensity = pow(max(0.0, dot(nNormal, nH)), shininessFactor);\n"
                          "    vec4 specular = vec4(sIntensity * lightColor, 1.0);\n"

                          "    fColor = texture(texture, texCoord) * ambient * 0.7 + diffuse * 0.15 + specular * 0.15;\n"
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
