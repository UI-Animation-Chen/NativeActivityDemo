//
// Created by bytedance on 2022/4/14.
//

#ifndef NATIVEACTIVITYDEMO_MAPLOCINFO_H
#define NATIVEACTIVITYDEMO_MAPLOCINFO_H

#include <GLES3/gl32.h>
#include <vector>

struct MapLocInfo {
    GLfloat height;
    std::vector<GLfloat> normal = std::vector<GLfloat>(3);
};

#endif //NATIVEACTIVITYDEMO_MAPLOCINFO_H
