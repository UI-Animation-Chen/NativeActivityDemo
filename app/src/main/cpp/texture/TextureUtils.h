//
// Created by mtdp on 2020-04-04.
//

#ifndef NATIVEACTIVITYDEMO_TEXTUREUTILS_H
#define NATIVEACTIVITYDEMO_TEXTUREUTILS_H

#include <GLES3/gl32.h>

class TextureUtils {
public:
    static GLuint loadSimpleTexture(const char *pngFile);
    static void deleteSimpleTexture();
private:
    static GLuint textureId;
    static GLubyte pixels[];
};

#endif //NATIVEACTIVITYDEMO_TEXTUREUTILS_H
