//
// Created by mtdp on 2020-04-04.
//

#ifndef NATIVEACTIVITYDEMO_TEXTUREUTILS_H
#define NATIVEACTIVITYDEMO_TEXTUREUTILS_H

#include <GLES3/gl32.h>

class TextureUtils {
public:
    static void loadPNGTexture(const char *pngFile, GLuint *textureId);
    static void loadSimpleTexture();
    static void deleteSimpleTexture();
    static GLuint textureIds[];
private:
    static GLubyte pixels[];
    static GLubyte pixelsGreen[];
    static GLubyte pixelsRed[];
};

#endif //NATIVEACTIVITYDEMO_TEXTUREUTILS_H
