//
// Created by mtdp on 2020-04-04.
//

#include <GLES3/gl32.h>
#include "TextureUtils.h"
#include "../utils/libpng1_6_29/png.h"
#include "../app_log.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int openFdFromAsset(AAssetManager *assetMgr, const char *fileName) {
    AAsset *asset = AAssetManager_open(assetMgr, fileName, AASSET_MODE_BUFFER);

    off_t start = 0, len = 0;
    int fd = AAsset_openFileDescriptor(asset, &start, &len);

    AAsset_close(asset);

    if (fd > 0) {
        lseek(fd, start, SEEK_CUR);// 通过asset方式打开的文件，流里的[start, start+len)区间属于该文件。
    }

    return fd;
}

static void loadPng(uint32_t *w, uint32_t *h, void **image, AAssetManager *assetMgr) {
    int fd = openFdFromAsset(assetMgr, "mt.png");
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
    png_infop  pngInfop = png_create_info_struct(pngStructp);

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
        // texture接收的图片像素值是上下颠倒的。
        memcpy(((png_bytep)*image + (*h - 1 - row) * rowbytes), rows[row], rowbytes);
    }

    png_destroy_read_struct(&pngStructp, &pngInfop, NULL);
}

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

GLuint TextureUtils::loadSimpleTexture(AAssetManager *assetMgr) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0); // 在绑定之前先激活
    glBindTexture(GL_TEXTURE_2D, textureId);

    uint32_t w, h;
    void *image;
    loadPng(&w, &h, &image, assetMgr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureId;
}

void TextureUtils::deleteSimpleTexture() {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
    }
}
