//
// Created by czf on 2019-10-23.
//

#include <cstring>
#include <cmath>
#include <GLES3/gl32.h>
#include "Shape.h"
#include "../utils/CoordinatesUtils.h"

void Shape::move(float offsetX, float offsetY, float offsetZ) {
    translateXYZ[0] = CoordinatesUtils::android2gles_x(offsetX);
    translateXYZ[1] = CoordinatesUtils::android2gles_y(offsetY);
    translateXYZ[2] = offsetZ;
    glUniform3fv(transLocation, 1, translateXYZ); // vec is not array, so the count is 1.
    updateWrapBoxTransform();
}

void Shape::rotate(float xRadian, float yRadian, float zRadian) {
    rotateXYZ[0] = xRadian;
    rotateXYZ[1] = yRadian;
    rotateXYZ[2] = zRadian;
    glUniform3fv(rotateLocation, 1, rotateXYZ);
    updateWrapBoxTransform();
}

void Shape::scale(float x, float y, float z) {
    scaleXYZ[0] = x;
    scaleXYZ[1] = y;
    scaleXYZ[2] = z;
    glUniform3fv(scaleLocation, 1, scaleXYZ);
    updateWrapBoxTransform();
}

void Shape::drawWrapBox2D() {
    modelColorFactorV4[3] = 0.34f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glUniform1i(transformEnabledLocation, 0); // 关闭shader中的transform
    glLineWidth(5.0f);
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[2]); // red texture
    glBindVertexArray(0); // break previous binding
    // 如果只有一个物体，初始化时设置一次即可。如果是多个物体，每次绘制前要设置用哪个顶点数据。
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, wrapBox2DVertices);
    glDrawArrays(GL_LINE_LOOP, 0, wrapBox2DVerticesSize/3);
}

void Shape::drawWrapBox3D() {
    modelColorFactorV4[3] = 0.34f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glUniform1i(transformEnabledLocation, 1); // 开启shader中的transform
    glLineWidth(5.0f);
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[1]); // green texture
    glBindVertexArray(vao[0]);
    glDrawElements(GL_LINE_STRIP, 16, GL_UNSIGNED_SHORT, 0);
//    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void Shape::initWrapBox(GLfloat minX, GLfloat minY, GLfloat minZ,
                        GLfloat maxX, GLfloat maxY, GLfloat maxZ) {
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    /**
     *        0 -------- 3 (max)
     *         /       /
     *       1 -------- 2
     *        4 -------- 7
     *         /       /
     * (min) 5 -------- 6
     */
    GLfloat wrapBoxVertices_[] = {
            minX, maxY, maxZ,
            minX, maxY, minZ,
            maxX, maxY, minZ,
            maxX, maxY, maxZ,
            minX, minY, maxZ,
            minX, minY, minZ,
            maxX, minY, minZ,
            maxX, minY, maxZ
    };
    memcpy(wrapBox3DVertices, wrapBoxVertices_, sizeof(wrapBoxVertices_));
    glBufferData(GL_ARRAY_BUFFER, sizeof(wrapBox3DVertices), wrapBox3DVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    // indeces
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    GLushort wrapBoxIndeces[] = { // 16
            0, 1, 2, 3, 0,
            4, 5, 6, 7, 4,
            6, 2, 7, 3, 1,
            5
    };
    GLushort cubeIndices[] = { // 36
            0, 1, 2, 0, 2, 3, // 上， 逆时针
            1, 5, 6, 1, 6, 2, // 前， 逆时针
            2, 6, 7, 2, 7, 3, // 右， 逆时针
            3, 7, 4, 3, 4, 0, // 里， 逆时针
            0, 4, 5, 0, 5, 1, // 左， 逆时针
            5, 4, 7, 5, 7, 6  // 下， 逆时针
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wrapBoxIndeces), wrapBoxIndeces, GL_STATIC_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // 2D包围框，不使用vao、vbo，直接用本地数据渲染
    glBindVertexArray(0); // break previous binding
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind previous binding
    GLfloat wrapBox2DVertices_[] = {
            minX, minY, 0.0f, // 左下
            maxX, minY, 0.0f, // 右下
            maxX, maxY, 0.0f, // 右上
            minX, maxY, 0.0f  // 左上
    };
    memcpy(wrapBox2DVertices, wrapBox2DVertices_, sizeof(wrapBox2DVertices_));
    // 如果只有一个物体，初始化时设置一次即可。如果是多个物体，每次绘制前要设置用哪个顶点数据。
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, wrapBox2DVertices);
    glEnableVertexAttribArray(0); // 如果其他地方有关闭操作，则要在每次绘制前开启。
}

// 仅wrapBox2D在使用
void Shape::updateWrapBoxTransform() {
    GLfloat minX = 2.0f, minY = 2.0f, minZ = 2.0f, maxX = -2.0f, maxY = -2.0f, maxZ = -2.0f; // 2.0是为了超出OpenGL屏幕范围，只要大于1即可
    GLfloat wrapBoxVerticesTmp[wrapBox3DVerticesSize];
    for (int i = 0; i < wrapBox3DVerticesSize; i+=3) {
        // --==-- scale
        wrapBoxVerticesTmp[i] = wrapBox3DVertices[i] * scaleXYZ[0];
        wrapBoxVerticesTmp[i+1] = wrapBox3DVertices[i + 1] * scaleXYZ[1];
        wrapBoxVerticesTmp[i+2] = wrapBox3DVertices[i + 2] * scaleXYZ[2];

        // --==-- rotate
        GLfloat cos_xDeg = cos(rotateXYZ[0]);
        GLfloat sin_xDeg = sin(rotateXYZ[0]);
        GLfloat cos_yDeg = cos(rotateXYZ[1]);
        GLfloat sin_yDeg = sin(rotateXYZ[1]);
        GLfloat cos_zDeg = cos(rotateXYZ[2]);
        GLfloat sin_zDeg = sin(rotateXYZ[2]);
        GLfloat x = wrapBoxVerticesTmp[i];
        GLfloat y = wrapBoxVerticesTmp[i+1];
        GLfloat z = wrapBoxVerticesTmp[i+2];
        // 绕x轴旋转
        wrapBoxVerticesTmp[i+2] = z * cos_xDeg - y * sin_xDeg;
        wrapBoxVerticesTmp[i+1] = z * sin_xDeg + y * cos_xDeg;
        // 绕y轴旋转
        z = wrapBoxVerticesTmp[i+2];
        wrapBoxVerticesTmp[i] = x * cos_yDeg - z * sin_yDeg;
        wrapBoxVerticesTmp[i+2] = x * sin_yDeg + z * cos_yDeg;
        // 绕z轴旋转
        x = wrapBoxVerticesTmp[i];
        y = wrapBoxVerticesTmp[i+1];
        wrapBoxVerticesTmp[i] = x * cos_zDeg - y * sin_zDeg;
        wrapBoxVerticesTmp[i+1] = x * sin_zDeg + y * cos_zDeg;

        // --==-- translate
        wrapBoxVerticesTmp[i] += translateXYZ[0];
        wrapBoxVerticesTmp[i+1] += translateXYZ[1];
        wrapBoxVerticesTmp[i+2] += translateXYZ[2];

        if (wrapBoxVerticesTmp[i] < minX) {
            minX = wrapBoxVerticesTmp[i];
        }
        if (wrapBoxVerticesTmp[i] > maxX) {
            maxX = wrapBoxVerticesTmp[i];
        }
        if (wrapBoxVerticesTmp[i+1] < minY) {
            minY = wrapBoxVerticesTmp[i+1];
        }
        if (wrapBoxVerticesTmp[i+1] > maxY) {
            maxY = wrapBoxVerticesTmp[i+1];
        }
        if (wrapBoxVerticesTmp[i+2] < minZ) {
            minZ = wrapBoxVerticesTmp[i+2];
        }
        if (wrapBoxVerticesTmp[i+2] > maxZ) {
            maxZ = wrapBoxVerticesTmp[i+2];
        }
    }

    GLfloat wrapBox2DVertices_[] = {
            minX, minY, 0.0f, // 左下
            maxX, minY, 0.0f, // 右下
            maxX, maxY, 0.0f, // 右上
            minX, maxY, 0.0f  // 左上
    };
    memcpy(wrapBox2DVertices, wrapBox2DVertices_, sizeof(wrapBox2DVertices_));
}
