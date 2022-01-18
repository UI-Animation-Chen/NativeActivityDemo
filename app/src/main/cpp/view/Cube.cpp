//
// Created by czf on 2019-10-23.
//

#include <GLES3/gl32.h>

#include "Cube.h"
#include "../utils/CoordinatesUtils.h"

Cube::Cube(): Shape() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(3, buffers);

    glBindVertexArray(vao); // 以下的操作都会记录在这个vao上，绑定成功后，会自动接触之前的绑定。

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]); // 绑定成功后，会自动接触之前的绑定。
    GLfloat cubePoints[] = {
            -0.2f,  0.2f,  0.5f, // 里 z轴正向为屏幕向里
            -0.2f, -0.2f,  0.5f,
             0.2f, -0.2f,  0.5f,
             0.2f,  0.2f,  0.5f,
            -0.5f,  0.5f, -0.5f, // 上
            -0.2f,  0.2f,  0.5f,
             0.2f,  0.2f,  0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f, // 左
            -0.2f, -0.2f,  0.5f,
            -0.2f,  0.2f,  0.5f,
            -0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f, // 下
             0.2f, -0.2f,  0.5f,
            -0.2f, -0.2f,  0.5f,
            -0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f, // 右
             0.2f,  0.2f,  0.5f,
             0.2f, -0.2f,  0.5f,
             0.5f, -0.5f, -0.5f
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
            16, 17, 18, 16, 18, 19  // 右， 逆时针
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    GLfloat texCoords[] = {
            0.0f, 1.0f, // 里
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f, // 上
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f, // 左
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f, // 下
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f, // 右
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    initWrapBox(-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f);
    float left = CoordinatesUtils::screenW / 2 * -0.5f;
    float top = CoordinatesUtils::screenH / 2 * 0.5f;
    app_log("cube: left: %f, top: %f\n", left, top);
    move(-left, top, 0); // 物体左上角对齐窗口左上角
}

Cube::~Cube() {
    glDeleteBuffers(3, buffers);
    glDeleteVertexArrays(1, &vao);
}

void Cube::draw() {
    Shape::draw();

    glUniform1i(transformEnabledLocation, 1); // 开启shader中的transform
    modelColorFactorV4[3] = 1.0f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[0]); // img texture
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_SHORT, 0);

    // 包围盒
    drawWrapBox3D();

    // wrapBox2D
    drawWrapBox2D();
}
