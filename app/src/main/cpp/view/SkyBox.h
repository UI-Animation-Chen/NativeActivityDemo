//
// Created by bytedance on 2022/3/21.
//

#ifndef NATIVEACTIVITYDEMO_SKYBOX_H
#define NATIVEACTIVITYDEMO_SKYBOX_H

#include "Shape.h"

class SkyBox: public Shape {
private:
    GLuint vao = 0; // vertex array object
    GLuint buffers[3] = {0};
    GLuint textureId = 0;

public:
    SkyBox();
    virtual ~SkyBox();
    virtual void draw();
};


#endif //NATIVEACTIVITYDEMO_SKYBOX_H
