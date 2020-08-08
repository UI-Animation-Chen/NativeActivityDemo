//
// Created by mtdp on 2020-04-11.
//

#include "ObjModel.h"
#include "../utils/ObjHelper.h"
#include "../utils/AndroidAssetUtils.h"
#include <cstring>
#include <cerrno>

ObjModel::ObjModel(): Shape() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(4, buffers);

    glBindVertexArray(vao);

    // assets目录下，文件后缀是png才能读到，否则会报错: no such file or directory.
    // 原因是：assets目录下的文件会进行压缩，所以读不到。而png会被认为是压缩文件，不会再次压缩。
    int fd = AndroidAssetUtils::openFdFromAsset("blenderObjs/sphere.png");
    if (fd <= 0) {
        app_log("openFdFromAsset failed: err: %s\n", strerror(errno));
        return;
    }
    FILE *file = fdopen(fd, "r");

    // 从手机sd读取
//    FILE *file = fopen("/sdcard/sphere.obj", "r");
//    if (file == NULL) {
//        app_log("file is NULL, err: %s\n", strerror(errno));
//        return;
//    }

    auto pObjModel = new ObjHelper::ObjModel();
    ObjHelper::readObjFile(file, pObjModel);
    fclose(file);

    // vertex data
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    long verticesSize = sizeof(GLfloat) * pObjModel->vertices.size();
    auto vertices = (GLfloat *)malloc((size_t)verticesSize);
    auto tmpVertices = vertices;
    for (GLfloat value: pObjModel->vertices) {
        *tmpVertices++ = value;
    }
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
    free(vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // indeces
    indexCount = pObjModel->indeces.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    long indecesSize = sizeof(GLushort) * pObjModel->indeces.size();
    auto indeces = (GLushort *)malloc((size_t)indecesSize);
    auto tmpIndeces = indeces;
    for (std::vector<GLushort> value: pObjModel->indeces) {
        *tmpIndeces++ = value.at(0);
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indecesSize, indeces, GL_STATIC_DRAW);
    free(indeces);

    // texture coordinates data
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    long texCoordsSize = sizeof(GLfloat) * pObjModel->texCoords.size();
    auto texCoords = (GLfloat *)malloc((size_t)texCoordsSize);
    auto tmpTexCoords = texCoords;
    for (GLfloat value: pObjModel->texCoords) {
        *tmpTexCoords++ = value;
    }
    glBufferData(GL_ARRAY_BUFFER, texCoordsSize, texCoords, GL_STATIC_DRAW);
    free(texCoords);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // normals data
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    long normalsSize = sizeof(GLfloat) * pObjModel->normals.size();
    auto normals = (GLfloat *)malloc((size_t)normalsSize);
    auto tmpNormals = normals;
    for (GLfloat value: pObjModel->normals) {
        *tmpNormals++ = value;
    }
    glBufferData(GL_ARRAY_BUFFER, normalsSize, normals, GL_STATIC_DRAW);
    free(normals);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    delete pObjModel;

//    ambientV4[3] = 0.75f;
    glUniform3fv(lightPositionLocation, 1, lightPositionV3);
    glUniform3fv(lightColorLocation, 1, lightColorV3);
}

ObjModel::~ObjModel() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(4, buffers);
}

void ObjModel::draw() {
    glBindVertexArray(vao);
    glUniform4fv(ambientLocation, 1, ambientV4);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);
}
