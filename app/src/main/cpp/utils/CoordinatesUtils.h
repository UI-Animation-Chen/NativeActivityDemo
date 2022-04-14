//
// Created by czf on 19-10-29.
//

#ifndef NATIVEACTIVITYDEMO_COORDINATESUTILS_H
#define NATIVEACTIVITYDEMO_COORDINATESUTILS_H

#include <GLES3/gl32.h>
#include <unordered_map>
#include "../entity/MapLocInfo.h"

// 安卓坐标系和OpenGL ES坐标系的转换。
//
// 安卓，左上角是原点，x正轴向右，y正轴向下。x和y的取值范围是[0,屏幕的尺寸]
// -------------------->
// |0,0      |        |
// |------------------|
// v         |        |
// --------------------
//
// OpenGL ES，中间是原点，x正轴向右，y正轴向上。x和y的取值范围是[-1,1]
// ----------^---------
// |         | 0,0    |
// |----------------->|
// |         |        |
// --------------------
//
class CoordinatesUtils {
public:
    static void insertLinearValue(std::unordered_map<int, std::unordered_map<int, std::unique_ptr<MapLocInfo>>> &data,
                                  int minX, int minZ, int maxX, int maxZ);

    static float android2gles_x(float x);
    static float android2gles_y(float y);
    static float gles2android_x(float x);
    static float gles2android_y(float y);

    static float gles2android_distance(float glesDistance);
    static float android2gles_distance(float androidDistance);

    // 屏幕的尺寸，单位像素
    static float screenW; // 屏幕宽
    static float screenH; // 屏幕高
    static float screenL; // 屏幕长边像素
    static float screenS; // 屏幕短边像素
    // OpenGL ES视口的像素数
    static float glesViewportSize;
};

#endif //NATIVEACTIVITYDEMO_COORDINATESUTILS_H
