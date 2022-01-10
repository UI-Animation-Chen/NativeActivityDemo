//
// Created by mtdp on 2020-04-11.
//

#ifndef NATIVEACTIVITYDEMO_OBJMODEL_H
#define NATIVEACTIVITYDEMO_OBJMODEL_H

#include "Shape.h"

class ObjModel: public Shape {
private:
    GLuint vao[3] = {0}; // vertex array object
    GLuint buffers[7] = {0};
    GLuint indexCount = 0;

    const static GLint wrapBoxVerticesSize = 24;
    const static GLint wrapBox2DVerticesSize = 12;
    GLfloat wrapBoxVertices[wrapBoxVerticesSize];
    GLfloat wrapBox2DVertices[wrapBox2DVerticesSize];

    float transX = 0, transY = 0, transZ = 0;
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
    float rotateXradian = 0, rotateYradian = 0, rotateZradian = 0;

    void updateTransform();

public:
    ObjModel();
    virtual ~ObjModel();
    virtual void draw();
    virtual void move(float offsetX, float offsetY, float offsetZ);
    virtual void rotate(float xRadian, float yRadian, float zRadian);
    virtual void scale(float x, float y, float z);
};

#endif //NATIVEACTIVITYDEMO_OBJMODEL_H
