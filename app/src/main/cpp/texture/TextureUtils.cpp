//
// Created by mtdp on 2020-04-04.
//

#include <GLES3/gl32.h>
#include "TextureUtils.h"

/* 3 x 3 Image,  R G B A Channels RAW Format. */
GLubyte TextureUtils::pixels[9 * 4] = {
        18,  140, 171, 255, /* Some Colour Bottom Left. */
        143, 143, 143, 255, /* Some Colour Bottom Middle. */
        255, 255, 255, 255, /* Some Colour Bottom Right. */

        255, 255, 0,   255, /* Yellow Middle Left. */
        0,   255, 255, 255, /* Some Colour Middle. */
        255, 0,   255, 255, /* Some Colour Middle Right. */

        255, 0,   0,   255, /* Red Top Left. */
        0,   255, 0,   255, /* Green Top Middle. */
        0,   0,   255, 255, /* Blue Top Right. */
};

GLuint TextureUtils::textureId = 0;

GLuint TextureUtils::loadSimpleTexture() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0); // 在绑定之前先激活
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureId;
}

void TextureUtils::deleteSimpleTexture() {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
    }
}
