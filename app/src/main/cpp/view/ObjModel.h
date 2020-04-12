//
// Created by mtdp on 2020-04-11.
//

#ifndef NATIVEACTIVITYDEMO_OBJMODEL_H
#define NATIVEACTIVITYDEMO_OBJMODEL_H

#include "Shape.h"

class ObjModel: public Shape {
private:
    GLuint vao = 0; // vertex array object
    GLuint buffers[3] = {0};
    GLuint indexCount = 0;

public:
    ObjModel();
    virtual ~ObjModel();
    virtual void draw();
};

#endif //NATIVEACTIVITYDEMO_OBJMODEL_H
