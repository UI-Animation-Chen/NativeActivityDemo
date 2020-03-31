//
// Created by czf on 19-10-29.
//

#ifndef NATIVEACTIVITYDEMO_COORDINATESUTILS_H
#define NATIVEACTIVITYDEMO_COORDINATESUTILS_H


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
