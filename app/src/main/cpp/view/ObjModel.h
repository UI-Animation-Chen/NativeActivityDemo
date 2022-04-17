//
// Created by mtdp on 2020-04-11.
//

#ifndef NATIVEACTIVITYDEMO_OBJMODEL_H
#define NATIVEACTIVITYDEMO_OBJMODEL_H

#include <unordered_map>
#include "Shape.h"
#include "../entity/MapLocInfo.h"

class ObjModel: public Shape {
private:
    GLuint vao[1] = {0}; // vertex array object
    GLuint buffers[4] = {0}; // vertex buffer object
    GLuint textureId = 0;
    GLuint indexCount = 0;
    std::unordered_map<int, std::unordered_map<int, std::unique_ptr<MapLocInfo>>> mapLocInfos;

public:
    ObjModel(const char *assetObjName, const char *assetPngName, bool needGenHeightMap,
             bool hasTexCoords, bool isSmoothLight);
    virtual ~ObjModel();

    void draw();
    GLfloat getMapHeight(GLfloat x, GLfloat z);
    void getMapNormal(GLfloat x, GLfloat z, glm::vec3 &outVec3);
};

#endif //NATIVEACTIVITYDEMO_OBJMODEL_H
