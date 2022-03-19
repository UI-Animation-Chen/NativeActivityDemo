//
// Created by czf on 2019-10-23.
//

#ifndef NATIVEACTIVITYDEMO_SHAPE_H
#define NATIVEACTIVITYDEMO_SHAPE_H

#include <GLES3/gl32.h>
#include "../app_log.h"
#include "../shader/BaseShader.h"
#include "../texture/TextureUtils.h"
#include "../utils/libglm0_9_6_3/glm/glm.hpp"

class Shape {
public:
    Shape() {
        transformEnabledLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "transformEnabled");
        textureUnitLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "textureUnit");
        modelColorFactorLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "modelColorFactor");
        lightPositionLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "lightPosition");
        lightColorLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "lightColor");
        transformMat4Location = glGetUniformLocation(BaseShader::getSingletonProgram(), "transformMat4");
        glUseProgram(BaseShader::getSingletonProgram());
        glUniform1i(textureUnitLocation, 0);

        glGenVertexArrays(1, vao);
        glGenBuffers(2, vbo);

        app_log("Shape constructor");
    }

    virtual ~Shape() {
        app_log("Shape destructor");
        glDeleteVertexArrays(1, vao);
        glDeleteBuffers(2, vbo);
    }

    virtual void draw();

    virtual void moveBy(float offsetX, float offsetY, float offsetZ);

    virtual void rotateBy(float xRadian, float yRadian, float zRadian);

    virtual void scaleBy(float x, float y, float z);

    virtual void worldMoveBy(float offsetX, float offsetY, float offsetZ);

    virtual void worldRotateBy(float xRadian, float yRadian, float zRadian);

    virtual void worldScaleBy(float x, float y, float z);

    void initWrapBox(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ);

    void drawWrapBox2D();
    void drawWrapBox3D();

    const GLfloat *getScale();

    const int *getBounds();

protected: // 子类可以按需进行修改
    GLint transformEnabledLocation;

    GLint modelColorFactorLocation;
    GLfloat modelColorFactorV4[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    GLint lightPositionLocation;
    GLfloat lightPositionV3[3] = {0.0f, 0.0f, -2.0f};

    GLint lightColorLocation;
    GLfloat lightColorV3[3] = {1.0f, 1.0f, 1.0f};

private:
    int bounds[4]; // [l, t, r, b]，屏幕尺寸值，不是GL ES的归一化值。

    GLuint vao[1];
    GLuint vbo[2];
    const static GLint wrapBox3DVerticesSize = 24; // shader里计算，3分量即可
    const static GLint wrapBox2DVerticesSize = 16; // 在cpu侧计算，需要w分量
    GLfloat wrapBox3DVertices[wrapBox3DVerticesSize] = {0};
    GLfloat wrapBox2DVertices[wrapBox2DVerticesSize] = {0};

    float _offsetX = 0;
    float _offsetY = 0;
    GLfloat translateXYZ[3] = {0}; // model
    GLfloat scaleXYZ[3] = {1.0f, 1.0f, 1.0f}; // model
    GLfloat rotateXYZ[3] = {0}; // model
    GLfloat worldTranslateXYZ[3] = {0}; // world
    GLfloat worldScaleXYZ[3] = {1.0f, 1.0f, 1.0f}; // world
    GLfloat worldRotateXYZ[3] = {0}; // world

    glm::mat4 modelMat4 = glm::mat4(1);
    GLint transformMat4Location;

    GLint textureUnitLocation;

    void updateWrapBoxTransform();
    void updateBounds(GLfloat minX, GLfloat minY, GLfloat maxX, GLfloat maxY);
    void updateModelMat4();
};

#endif //NATIVEACTIVITYDEMO_SHAPE_H
