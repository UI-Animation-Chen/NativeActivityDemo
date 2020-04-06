//
// Created by czf on 2019-10-23.
//

#ifndef NATIVEACTIVITYDEMO_CUBE_H
#define NATIVEACTIVITYDEMO_CUBE_H

#include "Shape.h"

class Cube: public Shape {
private:
    GLuint vao; // vertex array object
    GLuint buffers[3];

public:
    Cube();
    virtual ~Cube();
    virtual void draw();
};

#endif //NATIVEACTIVITYDEMO_CUBE_H
