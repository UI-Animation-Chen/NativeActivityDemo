//
// Created by mtdp on 2020-04-04.
//

#include <GLES3/gl32.h>
#include "TextureUtils.h"
#include "../utils/libpng1_6_29/png.h"
#include "../app_log.h"
#include "../utils/AndroidAssetUtils.h"
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

static void loadPng(uint32_t *w, uint32_t *h, void **image, const char *pngFile) {
    int fd = AndroidAssetUtils::openFdFromAsset(pngFile);
    if (fd <= 0) {
        app_log("openFdFromAsset failed: err: %s\n", strerror(errno));
        return;
    }

    FILE *file = fdopen(fd, "r"); // When the stream is closed via fclose(3), fildes is closed also.
    if (file == NULL) {
        close(fd);
        app_log("open file failed: err: %s\n", strerror(errno));
        return;
    }

    png_structp pngStructp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop pngInfop = png_create_info_struct(pngStructp);

    png_init_io(pngStructp, file);
    // 内部已经调用read_info，read_end，外面再调会崩溃
    png_read_png(pngStructp, pngInfop, PNG_TRANSFORM_EXPAND, NULL);

    fclose(file);

    int bit_depth, color_type;
    png_get_IHDR(pngStructp, pngInfop, w, h, &bit_depth, &color_type, NULL, NULL, NULL);

    *image = malloc((*w) * (*h) * 4);
    png_bytepp rows = png_get_rows(pngStructp, pngInfop);
    uint32_t rowbytes = png_get_rowbytes(pngStructp, pngInfop);
    for (int row = 0; row < *h; row++) {
        // texture接收的图片像素值是上下倒置的，注意「不是」旋转180度。
        memcpy((png_bytep)*image + (*h - 1 - row) * rowbytes, rows[row], rowbytes);
    }

    png_destroy_read_struct(&pngStructp, &pngInfop, NULL);
}

/* 3 x 3 Image,  R G B A Channels RAW Format. */
GLubyte TextureUtils::pixels[9 * 4] = {
        18,  140, 171, 255, /* Some Colour Bottom Left. */
        143, 143, 143, 255, /* Some Colour Bottom Middle. */
        255, 255, 255, 255, /* White Bottom Right. */

        255, 255, 0,   255, /* Yellow Middle Left. */
        0,   255, 255, 255, /* Some Colour Middle. */
        255, 0,   255, 255, /* Some Colour Middle Right. */

        255, 0,   0,   255, /* Red Top Left. */
        0,   255, 0,   255, /* Green Top Middle. */
        0,   0,   255, 255  /* Blue Top Right. */
};

GLubyte TextureUtils::pixelsGreen[1 * 4] = {
        0, 255, 0, 255
};
GLubyte TextureUtils::pixelsRed[1 * 4] = {
        255, 0, 0, 255
};

GLuint TextureUtils::textureIds[2] = {0};

void TextureUtils::loadSimpleTexture() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glActiveTexture(GL_TEXTURE0); // 激活纹理单元（texure unit），对应frag shader中的sampler2D变量。

    glGenTextures(2, textureIds); // 生成纹理对象id

    glBindTexture(GL_TEXTURE_2D, textureIds[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsGreen);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textureIds[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsRed);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TextureUtils::deleteSimpleTexture() {
    glDeleteTextures(2, textureIds);
}

void TextureUtils::loadPNGTexture(const char *pngFile, GLuint *textureId) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glActiveTexture(GL_TEXTURE0); // 激活纹理单元（texure unit），对应frag shader中的sampler2D变量。

    glGenTextures(1, textureId); // 生成纹理对象id

    glBindTexture(GL_TEXTURE_2D, *textureId); // 对于一个纹理单元只能绑定同一种target类型：GL_TEXTURE_2D, GL_TEXTURE_3D等
    uint32_t w, h;
    void *image;
    loadPng(&w, &h, &image, pngFile);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
