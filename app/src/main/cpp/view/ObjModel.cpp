//
// Created by mtdp on 2020-04-11.
//

#include "ObjModel.h"
#include "../utils/ObjHelper.h"
#include "../utils/AndroidAssetUtils.h"
#include "../utils/CoordinatesUtils.h"
#include <cstring>
#include <cerrno>

ObjModel::ObjModel(): Shape() {
    // assets目录下，文件后缀是png才能读到，否则会报错: no such file or directory.
    // 原因是：assets目录下的文件会进行压缩，所以读不到。而png会被认为是压缩文件，不会再次压缩。
    const char *assetName = "blenderObjs/cocacola.png";
    int fd = AndroidAssetUtils::openFdFromAsset(assetName);
    if (fd <= 0) {
        app_log("openFdFromAsset \"%s\" failed: err: %s\n", assetName, strerror(errno));
        return;
    }
    FILE *file = fdopen(fd, "r");

    // 从手机sd读取
//    FILE *file = fopen("/sdcard/sphere.obj", "r");
//    if (file == NULL) {
//        app_log("file is NULL, err: %s\n", strerror(errno));
//        return;
//    }

    auto pObjData = new ObjHelper::ObjData();
    ObjHelper::readObjFile(file, pObjData);
    fclose(file);

    glGenVertexArrays(2, vao);
    glGenBuffers(6, buffers);

    // vertex data
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    size_t verticesSize = sizeof(GLfloat) * pObjData->vertices.size();
    auto vertices = (GLfloat *)malloc(verticesSize);
    auto tmpVertices = vertices;
    for (GLfloat value: pObjData->vertices) {
        *tmpVertices++ = value;
    }
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
    free(vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // indeces
    indexCount = pObjData->indeces.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    size_t indecesSize = sizeof(GLushort) * pObjData->indeces.size();
    auto indeces = (GLushort *)malloc(indecesSize);
    auto tmpIndeces = indeces;
    for (std::vector<GLushort> value: pObjData->indeces) {
        *tmpIndeces++ = value.at(0);
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indecesSize, indeces, GL_STATIC_DRAW);
    free(indeces);

    // texture coordinates data
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    size_t texCoordsSize = sizeof(GLfloat) * pObjData->texCoords.size();
    auto texCoords = (GLfloat *)malloc(texCoordsSize);
    auto tmpTexCoords = texCoords;
    for (GLfloat value: pObjData->texCoords) {
        *tmpTexCoords++ = value;
    }
    glBufferData(GL_ARRAY_BUFFER, texCoordsSize, texCoords, GL_STATIC_DRAW);
    free(texCoords);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // normals data
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    size_t normalsSize = sizeof(GLfloat) * pObjData->normals.size();
    auto normals = (GLfloat *)malloc(normalsSize);
    auto tmpNormals = normals;
    for (GLfloat value: pObjData->normals) {
        *tmpNormals++ = value;
    }
    glBufferData(GL_ARRAY_BUFFER, normalsSize, normals, GL_STATIC_DRAW);
    free(normals);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // 3D包围盒
    GLfloat minX = pObjData->minVertex.at(0);
    GLfloat minY = pObjData->minVertex.at(1);
    GLfloat minZ = pObjData->minVertex.at(2);
    GLfloat maxX = pObjData->maxVertex.at(0);
    GLfloat maxY = pObjData->maxVertex.at(1);
    GLfloat maxZ = pObjData->maxVertex.at(2);
    delete pObjData;
    app_log("min(x: %f, y: %f, z: %f), max(x: %f, y: %f, z: %f)\n", minX, minY, minZ, maxX, maxY, maxZ);
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
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
    memcpy(wrapBoxVertices, wrapBoxVertices_, sizeof(wrapBoxVertices_));
    glBufferData(GL_ARRAY_BUFFER, sizeof(wrapBoxVertices), wrapBoxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    // indeces
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[5]);
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
            maxX, minY, 0.0f, // b
            maxX, maxY, 0.0f, // 右上
            minX, maxY, 0.0f  // d
    };
    memcpy(wrapBox2DVertices, wrapBox2DVertices_, sizeof(wrapBox2DVertices_));
    // 如果只有一个物体，初始化时设置一次即可。如果是多个物体，每次绘制前要设置用哪个顶点数据。
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, wrapBox2DVertices);
    glEnableVertexAttribArray(0); // 如果其他地方有关闭操作，则要在每次绘制前开启。

//    modelColorFactorV4[3] = 0.75f;
    glUniform3fv(lightPositionLocation, 1, lightPositionV3);
    glUniform3fv(lightColorLocation, 1, lightColorV3);
}

ObjModel::~ObjModel() {
    glDeleteVertexArrays(2, vao);
    glDeleteBuffers(6, buffers);
}

void ObjModel::draw() {
    // obj
    glUniform1i(transformEnabledLocation, 1);
    modelColorFactorV4[3] = 1.0f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[0]); // img texture
    glBindVertexArray(vao[0]);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);

    // 包围盒
    modelColorFactorV4[3] = 0.34f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glLineWidth(5.0f);
    // wrapBox3D
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[1]); // green texture
    glBindVertexArray(vao[1]);
    glDrawElements(GL_LINE_STRIP, 16, GL_UNSIGNED_SHORT, 0);
//    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

    // wrapBox2D
    glUniform1i(transformEnabledLocation, 0);
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[2]); // red texture
    glBindVertexArray(0); // break previous binding
    // 如果只有一个物体，初始化时设置一次即可。如果是多个物体，每次绘制前要设置用哪个顶点数据。
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, wrapBox2DVertices);
    glDrawArrays(GL_LINE_LOOP, 0, wrapBox2DVerticesSize/3);
}

void ObjModel::scale(float x, float y, float z) {
    Shape::scale(x, y, z);
    scaleX = x;
    scaleY = y;
    scaleZ = z;
    updateTransform();
}

void ObjModel::move(float offsetX, float offsetY, float offsetZ) {
    Shape::move(offsetX, offsetY, offsetZ);
    transX = CoordinatesUtils::android2gles_x(offsetX);
    transY = CoordinatesUtils::android2gles_y(offsetY);
    transZ = offsetZ;
    updateTransform();
}

void ObjModel::rotate(float xRadian, float yRadian, float zRadian) {
    Shape::rotate(xRadian, yRadian, zRadian);
    rotateXradian = xRadian;
    rotateYradian = yRadian;
    rotateZradian = zRadian;
    updateTransform();
}

void ObjModel::updateTransform() {
    GLfloat minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
    GLfloat wrapBoxVerticesTmp[wrapBoxVerticesSize];
    for (int i = 0; i < wrapBoxVerticesSize; i+=3) {
        // --==-- scale
        wrapBoxVerticesTmp[i] = wrapBoxVertices[i] * scaleX;
        wrapBoxVerticesTmp[i+1] = wrapBoxVertices[i + 1] * scaleY;
        wrapBoxVerticesTmp[i+2] = wrapBoxVertices[i + 2] * scaleZ;

        // --==-- rotate
        GLfloat cos_xDeg = cos(rotateXradian);
        GLfloat sin_xDeg = sin(rotateXradian);
        GLfloat cos_yDeg = cos(rotateYradian);
        GLfloat sin_yDeg = sin(rotateYradian);
        GLfloat cos_zDeg = cos(rotateZradian);
        GLfloat sin_zDeg = sin(rotateZradian);
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
        wrapBoxVerticesTmp[i] += transX;
        wrapBoxVerticesTmp[i+1] += transY;
        wrapBoxVerticesTmp[i+2] += transZ;

        if (wrapBoxVerticesTmp[i] < minX) {
            minX = wrapBoxVerticesTmp[i];
        } else if (wrapBoxVerticesTmp[i] > maxX) {
            maxX = wrapBoxVerticesTmp[i];
        }
        if (wrapBoxVerticesTmp[i+1] < minY) {
            minY = wrapBoxVerticesTmp[i+1];
        } else if (wrapBoxVerticesTmp[i+1] > maxY) {
            maxY = wrapBoxVerticesTmp[i+1];
        }
        if (wrapBoxVerticesTmp[i+2] < minZ) {
            minZ = wrapBoxVerticesTmp[i+2];
        } else if (wrapBoxVerticesTmp[i+2] > maxZ) {
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
