//
// Created by mtdp on 2020-04-11.
//

#ifndef NATIVEACTIVITYDEMO_OBJMODEL_H
#define NATIVEACTIVITYDEMO_OBJMODEL_H

#include <map>
#include "Shape.h"

class ObjModel: public Shape {
private:
    GLuint vao[1] = {0}; // vertex array object
    GLuint buffers[4] = {0}; // vertex buffer object
    GLuint textureId = 0;
    GLuint indexCount = 0;
    std::map<GLfloat, std::map<GLfloat, GLfloat>> heightMap;

public:
    ObjModel(const char *assetObjName, const char *assetPngName);
    virtual ~ObjModel();

    void draw();
    GLfloat getMapHeight(GLfloat x, GLfloat z);
};

#endif //NATIVEACTIVITYDEMO_OBJMODEL_H
