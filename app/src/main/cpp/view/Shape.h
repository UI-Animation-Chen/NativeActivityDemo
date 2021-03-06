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
        transLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "translate");
        scaleLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "scale");
        rotateLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "rotate");
        textureLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "texture");
        ambientLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "ambient");
        lightPositionLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "lightPosition");
        lightColorLocation = glGetUniformLocation(BaseShader::getSingletonProgram(), "lightColor");
        glUseProgram(BaseShader::getSingletonProgram());
        glUniform1i(textureLocation, 0);
        move(0, 0, 0); // 初始化。
        rotate(0, 0, 0);
        scale(1.0f, 1.0f, 1.0f);
        app_log("Shape constructor");
    }

    virtual ~Shape() {
        app_log("Shape destructor");
    }

    virtual void draw() = 0;

    virtual void move(float offsetX, float offsetY, float offsetZ);

    virtual void rotate(float xRadian, float yRadian, float zRadian);

    virtual void scale(float x, float y, float z);

protected: // 子类可以按需进行修改
    GLint ambientLocation;
    GLfloat ambientV4[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    GLint lightPositionLocation;
    GLfloat lightPositionV3[3] = {0.0f, 0.0f, -2.0f};

    GLint lightColorLocation;
    GLfloat lightColorV3[3] = {1.0f, 1.0f, 1.0f};

private:
    GLint bounds[4]; // [x, y, w, h]

    GLfloat translateXYZ[3];
    GLint transLocation;

    GLfloat scaleXYZ[3];
    GLint scaleLocation;

    GLfloat rotateXYZ[3];
    GLint rotateLocation;

    GLint textureLocation;
};

#endif //NATIVEACTIVITYDEMO_SHAPE_H
