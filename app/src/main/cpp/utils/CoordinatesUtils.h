//
// Created by czf on 19-10-29.
//

#ifndef NATIVEACTIVITYDEMO_COORDINATESUTILS_H
#define NATIVEACTIVITYDEMO_COORDINATESUTILS_H

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
    static float android2gles_x(float x);
    static float android2gles_y(float y);
    static float gles2android_x(float x);
    static float gles2android_y(float y);

    static float screenW;
    static float screenH;
};

#endif //NATIVEACTIVITYDEMO_COORDINATESUTILS_H
