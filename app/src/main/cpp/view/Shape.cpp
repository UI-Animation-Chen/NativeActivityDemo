//
// Created by czf on 2019-10-23.
//

#include <cstring>
#include <cmath>
#include <GLES3/gl32.h>
#include "Shape.h"
#include "../utils/CoordinatesUtils.h"
#include "../utils/libglm0_9_6_3/glm/glm.hpp"
#include "../utils/libglm0_9_6_3/glm/gtc/matrix_transform.hpp"
#include "../utils/libglm0_9_6_3/glm/ext.hpp"

void printMat(const glm::mat4 &Mat0)
{
    app_log("mat4(\n");
    app_log("\tvec4(%2.9f, %2.9f, %2.9f, %2.9f)\n", Mat0[0][0], Mat0[0][1], Mat0[0][2], Mat0[0][3]);
    app_log("\tvec4(%2.9f, %2.9f, %2.9f, %2.9f)\n", Mat0[1][0], Mat0[1][1], Mat0[1][2], Mat0[1][3]);
    app_log("\tvec4(%2.9f, %2.9f, %2.9f, %2.9f)\n", Mat0[2][0], Mat0[2][1], Mat0[2][2], Mat0[2][3]);
    app_log("\tvec4(%2.9f, %2.9f, %2.9f, %2.9f))\n\n", Mat0[3][0], Mat0[3][1], Mat0[3][2], Mat0[3][3]);
}

void Shape::moveBy(float offsetX, float offsetY, float offsetZ) {
    translateXYZ[0] += offsetX;
    translateXYZ[1] += offsetY;
    translateXYZ[2] += offsetZ;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::moveXTo(float x) {

}
void Shape::moveYTo(float y) {

}
void Shape::moveZTo(float z) {

}

void Shape::worldMoveBy(float offsetX, float offsetY, float offsetZ) {
    // 这里worldRotateXYZ[1]的正负号是试出来的，跟viewMat4的rotate里的worldRotateXYZ[1]的正负号配合使用。
    GLfloat transZ = offsetZ * cos(-worldRotateXYZ[1]) + offsetX * sin(worldRotateXYZ[1]);
    GLfloat transX = offsetZ * sin(-worldRotateXYZ[1]) + offsetX * cos(worldRotateXYZ[1]);
    worldTranslateXYZ[0] += transX;
    worldTranslateXYZ[1] += offsetY;
    worldTranslateXYZ[2] += transZ;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::worldMoveXTo(float x) {

}
void Shape::worldMoveYTo(float y) {
    worldTranslateXYZ[1] = y;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::worldMoveZTo(float z) {

}

void Shape::rotateBy(float xRadian, float yRadian, float zRadian) {
    rotateXYZ[0] += xRadian;
    rotateXYZ[1] += yRadian;
    rotateXYZ[2] += zRadian;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::rotateXTo(float xRadian) {
    rotateXYZ[0] = xRadian;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::rotateYTo(float yRadian) {
    rotateXYZ[1] = yRadian;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::rotateZTo(float zRadian) {
    rotateXYZ[2] = zRadian;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::worldRotateBy(float xRadian, float yRadian, float zRadian) {
    worldRotateXYZ[0] += xRadian;
    worldRotateXYZ[1] += yRadian;
    worldRotateXYZ[2] += zRadian;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::worldRotateXTo(float xRadian) {
    worldRotateXYZ[0] = xRadian;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::worldRotateYTo(float yRadian) {
    worldRotateXYZ[1] = yRadian;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::worldRotateZTo(float zRadian) {
    worldRotateXYZ[2] = zRadian;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::scaleBy(float x, float y, float z) {
    scaleXYZ[0] += x;
    scaleXYZ[1] += y;
    scaleXYZ[2] += z;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::scaleXTo(float x) {
    scaleXYZ[0] = x;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::scaleYTo(float y) {
    scaleXYZ[1] = y;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::scaleZTo(float z) {
    scaleXYZ[2] = z;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::worldScaleBy(float x, float y, float z) {
    worldScaleXYZ[0] += x;
    worldScaleXYZ[1] += y;
    worldScaleXYZ[2] += z;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::worldScaleXTo(float x) {
    worldScaleXYZ[0] = x;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::worldScaleYTo(float y) {
    worldScaleXYZ[1] = y;
    updateModelMat4();
    updateWrapBoxTransform();
}
void Shape::worldScaleZTo(float z) {
    worldScaleXYZ[2] = z;
    updateModelMat4();
    updateWrapBoxTransform();
}

void Shape::draw() {
    glUniformMatrix4fv(transformMat4Location, 1, GL_FALSE, glm::value_ptr(modelMat4));
}

void Shape::drawWrapBox2D() {
    modelColorFactorV4[3] = 0.34f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glUniform1i(transformEnabledLocation, 0); // 关闭shader中的transform
    glLineWidth(5.0f);
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[1]); // red texture
    glBindVertexArray(0); // break previous binding
    // 如果只有一个物体，初始化时设置一次即可。如果是多个物体，每次绘制前要设置用哪个顶点数据。
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, wrapBox2DVertices); // 需要w分量
    glDrawArrays(GL_LINE_LOOP, 0, wrapBox2DVerticesSize/4);
}

void Shape::drawWrapBox3D() {
    modelColorFactorV4[3] = 0.34f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glUniform1i(transformEnabledLocation, 1); // 开启shader中的transform
    glLineWidth(5.0f);
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[0]); // green texture
    glBindVertexArray(vao[0]);
    glDrawElements(GL_LINE_STRIP, 16, GL_UNSIGNED_SHORT, 0);
//    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void Shape::initWrapBox(GLfloat minX, GLfloat minY, GLfloat minZ,
                        GLfloat maxX, GLfloat maxY, GLfloat maxZ) {
    // init bounds
    updateBounds(minX, minY, maxX, maxY);

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
    GLfloat wrapBox2DVertices_[] = { // 需要w分量
            minX, minY, 0.0f, 1.0f, // 左下
            maxX, minY, 0.0f, 1.0f, // 右下
            maxX, maxY, 0.0f, 1.0f, // 右上
            minX, maxY, 0.0f, 1.0f  // 左上
    };
    memcpy(wrapBox2DVertices, wrapBox2DVertices_, sizeof(wrapBox2DVertices_));
    // 如果只有一个物体，初始化时设置一次即可。如果是多个物体，每次绘制前要设置用哪个顶点数据。
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, wrapBox2DVertices);// 需要w分量
    glEnableVertexAttribArray(0); // 如果其他地方有关闭操作，则要在每次绘制前开启。
}

// 仅wrapBox2D在使用
void Shape::updateWrapBoxTransform() {
    GLfloat minX = 0.0f, minY = 0.0f, minZ = 0.0f, maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f, w = 0.0f;
    glm::vec4 wrapBoxVerticesTmp(1.0f);
    for (int i = 0; i < wrapBox3DVerticesSize; i+=3) {
        wrapBoxVerticesTmp[0] = wrapBox3DVertices[i];
        wrapBoxVerticesTmp[1] = wrapBox3DVertices[i+1];
        wrapBoxVerticesTmp[2] = wrapBox3DVertices[i+2];
        wrapBoxVerticesTmp[3] = 1;
//        app_log("vec4 before--------------: %f, %f, %f, %f\n", wrapBoxVerticesTmp[0], wrapBoxVerticesTmp[1], wrapBoxVerticesTmp[2], wrapBoxVerticesTmp[3]);
        wrapBoxVerticesTmp = modelMat4 * wrapBoxVerticesTmp;
//        app_log("vec4 after---: %f, %f, %f, %f\n", wrapBoxVerticesTmp[0], wrapBoxVerticesTmp[1], wrapBoxVerticesTmp[2], wrapBoxVerticesTmp[3]);

        if (i == 0) { // 首次先全部赋值，要在本次循环中的所有值中取出min和max
            minX = maxX = wrapBoxVerticesTmp[0];
            minY = maxY = wrapBoxVerticesTmp[1];
            minZ = maxZ = wrapBoxVerticesTmp[2];
        } else {
            if (wrapBoxVerticesTmp[0] < minX) {
                minX = wrapBoxVerticesTmp[0];
            }
            if (wrapBoxVerticesTmp[0] > maxX) {
                maxX = wrapBoxVerticesTmp[0];
            }
            if (wrapBoxVerticesTmp[1] < minY) {
                minY = wrapBoxVerticesTmp[1];
            }
            if (wrapBoxVerticesTmp[1] > maxY) {
                maxY = wrapBoxVerticesTmp[1];
            }
            if (wrapBoxVerticesTmp[2] < minZ) {
                minZ = wrapBoxVerticesTmp[2];
            }
            if (wrapBoxVerticesTmp[2] > maxZ) {
                maxZ = wrapBoxVerticesTmp[2];
            }
        }
        w += wrapBoxVerticesTmp[3]; // w取平均值
    }
    w = w / (wrapBox3DVerticesSize / 3.0f);

    GLfloat wrapBox2DVertices_[] = {
            minX, minY, 0.0f, w, // 左下
            maxX, minY, 0.0f, w, // 右下
            maxX, maxY, 0.0f, w, // 右上
            minX, maxY, 0.0f, w  // 左上
    };
//    app_log("wrapBox2DVertices: minX: %f, minY: %f, minZ: %f, maxX: %f, maxY: %f, maxZ: %f, w: %f\n", minX, minY, minZ, maxX, maxY, maxZ, w);
    memcpy(wrapBox2DVertices, wrapBox2DVertices_, sizeof(wrapBox2DVertices_));
    updateBounds(minX, minY, maxX, maxY);
}

/**
 * GLM是基于（GLSL）规范的图形软件的仅头文件C++数学库。该库可与OpenGL完美配合。
 * 1、glm::mat4在内存中存储是列优先的。
 * 2、glm::vec4是列向量，应当左边乘以矩阵。但是当右边乘以矩阵时，也能看做行向量。
 * 3、glm::mat4.length() 返回矩阵的列数，glm::mat4[i][j]返回的是第i列第j行的元素。
 * 4、glm::mat3x4表示3列4行的矩阵。注意不是3行4列！
 *
 * 关于坐标系：
 * normalized device coordinates are in a left-handed coordinate system, while by convention,
 * when we use a projection matrix, we work in a right-handed coordinate system.
 * x向右，y向上，left-handed的z向屏幕里，right-handed的z向外。
 */
void Shape::updateModelMat4() {
    // model自身变换
    modelMat4 = glm::mat4(1);
    // translate
    modelMat4 = glm::translate(modelMat4, glm::vec3(translateXYZ[0], translateXYZ[1], translateXYZ[2]));
    // rotate 注意：x，y，z的先后顺序不同，旋转的效果不同
    modelMat4 = glm::rotate(modelMat4, rotateXYZ[0], glm::vec3(1, 0, 0)); // x轴
    modelMat4 = glm::rotate(modelMat4, rotateXYZ[1], glm::vec3(0, 1, 0)); // y轴
    modelMat4 = glm::rotate(modelMat4, rotateXYZ[2], glm::vec3(0, 0, 1)); // z轴
    // scale
    modelMat4 = glm::scale(modelMat4, glm::vec3(scaleXYZ[0], scaleXYZ[1], scaleXYZ[2]));

    // view变换
    glm::mat4 viewMat4 = glm::lookAt(glm::vec3(0, 3, -10), glm::vec3(0), glm::vec3(0, 1, 0));
    viewMat4 = glm::scale(viewMat4, glm::vec3(-1, 1, 1)); // lookAt返回的矩阵，需要x取反一下效果才是对的
    // 对viewMat4的平移、旋转操作，相当于left-handed，即z正向屏幕里的坐标系，直接操作model的结果。而不是操作的camera。
    viewMat4 = glm::scale(viewMat4, glm::vec3(worldScaleXYZ[0], worldScaleXYZ[1], worldScaleXYZ[2]));
    viewMat4 = glm::rotate(viewMat4, worldRotateXYZ[0], glm::vec3(1, 0, 0));
    viewMat4 = glm::rotate(viewMat4, worldRotateXYZ[1], glm::vec3(0, 1, 0)); // 这里用负值时，worldMove里的cos和sin就得用正值，两者相反
    viewMat4 = glm::rotate(viewMat4, worldRotateXYZ[2], glm::vec3(0, 0, 1));
    viewMat4 = glm::translate(viewMat4, glm::vec3(-worldTranslateXYZ[0], -worldTranslateXYZ[1], -worldTranslateXYZ[2]));

    // 透视投影变换
    glm::mat4 projectMat4 = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);

    modelMat4 = projectMat4 * viewMat4 * modelMat4; // 最先发生的变换矩阵，往后放
}

void Shape::updateBounds(GLfloat minX, GLfloat minY, GLfloat maxX, GLfloat maxY) {
    bounds[0] = lround(CoordinatesUtils::gles2android_x(minX));
    bounds[1] = lround(CoordinatesUtils::gles2android_y(maxY));
    bounds[2] = lround(CoordinatesUtils::gles2android_x(maxX));
    bounds[3] = lround(CoordinatesUtils::gles2android_y(minY));
//    app_log("bounds: l: %d, t: %d, r: %d, b: %d\n", bounds[0], bounds[1], bounds[2], bounds[3]);
}

void Shape::getScale(GLfloat *scaleXYZarr) {
    if (scaleXYZarr) {
        scaleXYZarr[0] = scaleXYZ[0] + worldScaleXYZ[0];
        scaleXYZarr[1] = scaleXYZ[1] + worldScaleXYZ[1];
        scaleXYZarr[2] = scaleXYZ[2] + worldScaleXYZ[2];
    }
}

void Shape::getTranslate(GLfloat *translateXYZarr) {
    if (translateXYZarr) {
        translateXYZarr[0] = translateXYZ[0] + worldTranslateXYZ[0];
        translateXYZarr[1] = translateXYZ[1] + worldTranslateXYZ[1];
        translateXYZarr[2] = translateXYZ[2] + worldTranslateXYZ[2];
    }
}

void Shape::getRotate(GLfloat *rotateXYZarr) {
    if (rotateXYZarr) {
        rotateXYZarr[0] = rotateXYZ[0] + worldRotateXYZ[0];
        rotateXYZarr[1] = rotateXYZ[1] + worldRotateXYZ[1];
        rotateXYZarr[2] = rotateXYZ[2] + worldRotateXYZ[2];
    }
}

GLfloat Shape::getMapHeight(GLfloat x, GLfloat z) {
    return translateXYZ[1];
}
