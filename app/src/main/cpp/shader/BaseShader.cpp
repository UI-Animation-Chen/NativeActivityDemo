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

                          "uniform int transformEnabled;\n" // transform开关
                          "uniform mat4 transformMat4;\n"

                          "out vec2 texCoord;\n" // send to next stage(frag shader)
                          "out vec3 modelNormal;\n"
                          "out vec3 modelVertex;\n"

                          "void main() {\n"
                          "    gl_Position = vPosition;\n"
                          "    modelNormal = normalize(vNormal);\n" // 法向量，进行归一化，片元中仍需要。

                          "    if (transformEnabled == 1) {\n" // 是否在shader里进行缩放、旋转、平移操作等
                          "        gl_Position = transformMat4 * vPosition;\n"
                          "        modelNormal = vec3(transformMat4 * vec4(modelNormal, 0.0));\n"
                          "    }\n"

                          "    modelVertex = vec3(gl_Position[0], gl_Position[1], gl_Position[2]);\n"
                          "    texCoord = vTexCoord;\n"
                          "}\n";

// for fragment shader, Specifying the precision is compulsory.
static const char *frag = "#version 300 es\n"
                          "precision mediump float;\n"

                          "in vec2 texCoord;\n" // 纹理坐标
                          "in vec3 modelNormal;\n" // 法向量
                          "in vec3 modelVertex;\n" // 变换之后的顶点坐标

                          "uniform sampler2D textureUnit;\n"
                          "uniform vec4 modelColorFactor;\n" // 物体本身颜色的乘法因子，实现控制物体的颜色和透明度
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

                          "    vec4 factor68 = vec4(0.68, 0.68, 0.68, 1.0);\n" // 透明度为1，物体本身提供100%的透明度
                          "    vec4 factor17 = vec4(0.17, 0.17, 0.17, 0.0);\n" // 透明度为0，光照不提供透明度通道
                          "    vec4 factor15 = vec4(0.15, 0.15, 0.15, 0.0);\n" // 透明度为0，光照不提供透明度通道
                          "    fColor = texture(textureUnit, texCoord) * modelColorFactor * factor68 + diffuse * factor17 + specular * factor15;\n"
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
