//
// Created by mtdp on 2020-04-11.
//

#include "ObjModel.h"
#include "../utils/ObjHelper.h"
#include "../utils/AndroidAssetUtils.h"
#include <cstring>
#include <cerrno>

ObjModel::ObjModel(): Shape() {
    glGenVertexArrays(2, vao);
    glGenBuffers(6, buffers);

    glBindVertexArray(vao[0]);

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

    // vertex data
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    long verticesSize = sizeof(GLfloat) * pObjData->vertices.size();
    auto vertices = (GLfloat *)malloc((size_t)verticesSize);
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
    long indecesSize = sizeof(GLushort) * pObjData->indeces.size();
    auto indeces = (GLushort *)malloc((size_t)indecesSize);
    auto tmpIndeces = indeces;
    for (std::vector<GLushort> value: pObjData->indeces) {
        *tmpIndeces++ = value.at(0);
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indecesSize, indeces, GL_STATIC_DRAW);
    free(indeces);

    // texture coordinates data
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    long texCoordsSize = sizeof(GLfloat) * pObjData->texCoords.size();
    auto texCoords = (GLfloat *)malloc((size_t)texCoordsSize);
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
    long normalsSize = sizeof(GLfloat) * pObjData->normals.size();
    auto normals = (GLfloat *)malloc((size_t)normalsSize);
    auto tmpNormals = normals;
    for (GLfloat value: pObjData->normals) {
        *tmpNormals++ = value;
    }
    glBufferData(GL_ARRAY_BUFFER, normalsSize, normals, GL_STATIC_DRAW);
    free(normals);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // 包围盒
    glBindVertexArray(vao[1]);
    GLfloat minX = pObjData->minVertex.at(0);
    GLfloat minY = pObjData->minVertex.at(1);
    GLfloat minZ = pObjData->minVertex.at(2);
    GLfloat maxX = pObjData->maxVertex.at(0);
    GLfloat maxY = pObjData->maxVertex.at(1);
    GLfloat maxZ = pObjData->maxVertex.at(2);
    app_log("min(x: %f, y: %f, z: %f), max(x: %f, y: %f, z: %f)\n", minX, minY, minZ, maxX, maxY, maxZ);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
    /**
     *         a0 -------- d3 (max)
     *           /       /
     *        b1 -------- c2
     *         a`4-------- d`7
     *           /       /
     *  (min) b`5-------- c`6
     */
    GLfloat wrapBoxVertices[] = {
            minX, maxY, maxZ, // a
            minX, maxY, minZ, // b
            maxX, maxY, minZ, // c
            maxX, maxY, maxZ, // d
            minX, minY, maxZ, // a`
            minX, minY, minZ, // b`
            maxX, minY, minZ, // c`
            maxX, minY, maxZ  // d`
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(wrapBoxVertices), wrapBoxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    // indeces
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[5]);
    GLushort wrapBoxIndeces[] = {
        0, 1, 2, 3, 0,
        4, 5, 6, 7, 4,
        6, 2, 7, 3, 1,
        5
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wrapBoxIndeces), wrapBoxIndeces, GL_STATIC_DRAW);

    delete pObjData;

//    modelColorFactorV4[3] = 0.75f;
    glUniform3fv(lightPositionLocation, 1, lightPositionV3);
    glUniform3fv(lightColorLocation, 1, lightColorV3);
}

ObjModel::~ObjModel() {
    glDeleteVertexArrays(2, vao);
    glDeleteBuffers(6, buffers);
}

void ObjModel::draw() {
    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[0]);
    glBindVertexArray(vao[0]);
    modelColorFactorV4[3] = 1.0f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);

    glBindTexture(GL_TEXTURE_2D, TextureUtils::textureIds[1]);
    glBindVertexArray(vao[1]);
    modelColorFactorV4[3] = 0.34f;
    glUniform4fv(modelColorFactorLocation, 1, modelColorFactorV4);
    glLineWidth(5.0f);
    glDrawElements(GL_LINE_STRIP, 16, GL_UNSIGNED_SHORT, 0);
}
