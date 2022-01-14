//
// Created by czf on 2019-10-23.
//

#ifndef NATIVEACTIVITYDEMO_SHAPE_H
#define NATIVEACTIVITYDEMO_SHAPE_H

#include <GLES3/gl32.h>
#include "../app_log.h"
#include "../shader/BaseShader.h"
#include "../texture/TextureUtils.h"

class Shape {
public:
    Shape() {
        transformEnabledLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "transformEnabled");
        transLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "translate");
        scaleLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "scale");
        rotateLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "rotate");
        textureUnitLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "textureUnit");
        modelColorFactorLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "modelColorFactor");
        lightPositionLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "lightPosition");
        lightColorLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "lightColor");
        glUseProgram(BaseShader::getSingletonProgram());
        glUniform1i(textureUnitLocation, 0);
        move(0, 0, 0); // 初始化。
        rotate(0, 0, 0);
        scale(1.0f, 1.0f, 1.0f);

        glGenVertexArrays(1, vao);
        glGenBuffers(2, vbo);

        app_log("Shape constructor");
    }

    virtual ~Shape() {
        app_log("Shape destructor");
        glDeleteVertexArrays(1, vao);
        glDeleteBuffers(2, vbo);
    }

    virtual void draw() = 0;

    virtual void move(float offsetX, float offsetY, float offsetZ);

    virtual void rotate(float xRadian, float yRadian, float zRadian);

    virtual void scale(float x, float y, float z);

    void initWrapBox(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat maxX, GLfloat maxY, GLfloat maxZ);

    void drawWrapBox2D();
    void drawWrapBox3D();

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
    const static GLint wrapBox3DVerticesSize = 24;
    const static GLint wrapBox2DVerticesSize = 12;
    GLfloat wrapBox3DVertices[wrapBox3DVerticesSize] = {0};
    GLfloat wrapBox2DVertices[wrapBox2DVerticesSize] = {0};

    GLfloat translateXYZ[3];
    GLint transLocation;

    GLfloat scaleXYZ[3];
    GLint scaleLocation;

    GLfloat rotateXYZ[3];
    GLint rotateLocation;

    GLint textureUnitLocation;

    void updateWrapBoxTransform();
    void updateBounds(GLfloat minX, GLfloat minY, GLfloat maxX, GLfloat maxY);
};

#endif //NATIVEACTIVITYDEMO_SHAPE_H
