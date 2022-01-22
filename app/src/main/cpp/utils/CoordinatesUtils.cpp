//
// Created by czf on 19-10-29.
//

#include "CoordinatesUtils.h"

// error: 'static' can only be specified inside the class definition
// static float CoordinatesUtils::screenW = 1;

float CoordinatesUtils::screenW = 1;
float CoordinatesUtils::screenH = 1;
float CoordinatesUtils::screenS = 1;
float CoordinatesUtils::screenL = 1;

// 将安卓的坐标值转换为OpenGL ES的坐标值。
float CoordinatesUtils::android2gles_x(float x) {
    return 2*x/screenL - 1;
}

float CoordinatesUtils::android2gles_y(float y) {
    // -(y - screenH/2)
    return (1 - 2*y/screenL);
}

// 将OpenGL ES的坐标值转换为安卓的坐标值。
float CoordinatesUtils::gles2android_x(float x) {
    return (x + 1) * screenL/2;
}

float CoordinatesUtils::gles2android_y(float y) {
    return screenL/2 * (1 - y);
}

// androidDistance/screenL = glesDistance/2
float CoordinatesUtils::gles2android_distance(float glesDistance) {
    return glesDistance * screenL / 2;
}

float CoordinatesUtils::android2gles_distance(float androidDistance) {
    return androidDistance * 2 / screenL;
}
