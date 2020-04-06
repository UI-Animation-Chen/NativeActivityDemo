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
        glUseProgram(BaseShader::getSingletonProgram());
        glUniform1i(textureLocation, 0);
        app_log("Shape constructor");
    }

    virtual ~Shape() {
        app_log("Shape destructor");
    }

    virtual void draw() = 0;

    virtual void move(float offsetX, float offsetY, float offsetZ);

    virtual void rotate(float xDeg, float yDeg, float zDeg);

    virtual void scale(float x, float y, float z);

protected:
    GLint ambientLocation;
    GLfloat ambientV4[4] = {1.0f, 1.0f, 1.0f, 1.0f};

private:
    GLfloat translateXYZ[3];
    GLint transLocation;

    GLfloat scaleXYZ[3];
    GLint scaleLocation;

    GLfloat rotateXYZ[3];
    GLint rotateLocation;

    GLint textureLocation;
};

#endif //NATIVEACTIVITYDEMO_SHAPE_H
