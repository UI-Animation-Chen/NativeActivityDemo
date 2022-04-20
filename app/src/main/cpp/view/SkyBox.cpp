//
// Created by bytedance on 2022/3/21.
//

#include "SkyBox.h"

SkyBox::SkyBox(): Shape() {
    app_log("SkyBox constructor\n");
    TextureUtils::loadPNGTexture("skybox.png", &textureId);

    glGenVertexArrays(1, &vao);
    glGenBuffers(3, buffers);

    glBindVertexArray(vao); // 以下的操作都会记录在这个vao上，绑定成功后，会自动接触之前的绑定。

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]); // 绑定成功后，会自动接触之前的绑定。
    GLfloat boxWidth = 1.0f;
    GLfloat cubePoints[] = {
            -boxWidth,  boxWidth,  boxWidth, // 里 z轴正向为屏幕向里
            -boxWidth, -boxWidth,  boxWidth,
            boxWidth,  -boxWidth,  boxWidth,
            boxWidth,   boxWidth,  boxWidth,
            -boxWidth,  boxWidth, -boxWidth, // 上
            -boxWidth,  boxWidth,  boxWidth,
            boxWidth,   boxWidth,  boxWidth,
            boxWidth,   boxWidth, -boxWidth,
            -boxWidth, -boxWidth, -boxWidth, // 左
            -boxWidth, -boxWidth,  boxWidth,
            -boxWidth,  boxWidth,  boxWidth,
            -boxWidth,  boxWidth, -boxWidth,
            boxWidth,  -boxWidth, -boxWidth, // 下
            boxWidth,  -boxWidth,  boxWidth,
            -boxWidth,  -boxWidth, boxWidth,
            -boxWidth, -boxWidth, -boxWidth,
            boxWidth,   boxWidth, -boxWidth, // 右
            boxWidth,   boxWidth,  boxWidth,
            boxWidth,  -boxWidth,  boxWidth,
            boxWidth,  -boxWidth, -boxWidth,
            boxWidth,  boxWidth,  -boxWidth, // 外
            boxWidth,  -boxWidth, -boxWidth,
            -boxWidth, -boxWidth, -boxWidth,
            -boxWidth,  boxWidth, -boxWidth
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubePoints), cubePoints, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    GLushort cubeIndices[] = {
            0,  1,  2,  0,  2,  3,  // 里， 逆时针
            4,  5,  6,  4,  6,  7,  // 上， 逆时针
            8,  9,  10, 8,  10, 11, // 左， 逆时针
            12, 13, 14, 12, 14, 15, // 下， 逆时针
            16, 17, 18, 16, 18, 19, // 右， 逆时针
            20, 21, 22, 20, 22, 23  // 外， 逆时针
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    GLfloat texCoords[] = {
            0.25f, 0.66f, // 里
            0.25f, 0.34f,
            0.5f, 0.34f,
            0.5f, 0.66f,
            0.25f, 1.0f, // 上
            0.25f, 0.66f,
            0.5f, 0.66f,
            0.5f, 1.0f,
            0.0f, 0.34f, // 左
            0.25f, 0.34f,
            0.25f, 0.66f,
            0.0f, 0.66f,
            0.5f, 0.0f, // 下
            0.5f, 0.34f,
            0.25f, 0.34f,
            0.25f, 0.0f,
            0.75f, 0.66f, // 右
            0.5f, 0.66f,
            0.5f, 0.34f,
            0.75f, 0.34f,
            0.75f, 0.66f, // 外
            0.75f, 0.34f,
            1.0f, 0.34f,
            1.0f, 0.66f
    };
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

SkyBox::~SkyBox() {
    glDeleteBuffers(3, buffers);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &textureId);
    app_log("SkyBox destructor~~~\n");
}

void SkyBox::draw() {
    Shape::draw();

    glUniform1i(transformEnabledLocation, 1); // 开启shader中的transform
    modelColorFactorV4[0] = 1.5f;
    modelColorFactorV4[1] = 1.5f;
    modelColorFactorV4[2] = 1.5f;
    modelColorFactorV4[3] = 1.0f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glBindTexture(GL_TEXTURE_2D, textureId); // skybox texture
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

    // 包围盒
//    drawWrapBox3D();

    // wrapBox2D
//    drawWrapBox2D();
}
