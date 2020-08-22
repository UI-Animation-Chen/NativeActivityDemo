//
// Created by mtdp on 2020-04-11.
//

#ifndef NATIVEACTIVITYDEMO_OBJHELPER_H
#define NATIVEACTIVITYDEMO_OBJHELPER_H

#include <cstdio>
#include <vector>
#include <GLES3/gl32.h>

class ObjHelper {
public:
    class ObjData {
    public:
        std::vector<GLfloat> vertices; // 3个为一组
        std::vector<GLfloat> normals; // 3个为一组
        std::vector<GLfloat> texCoords; // 2个为一组
        std::vector<std::vector<GLushort>> indeces; // v vt vn的索引
        ObjData() {
            vertices.push_back(0); // obj文件中，索引是从1开始的，这里先存入索引0的无用数据。
            vertices.push_back(0);
            vertices.push_back(0);

            normals.push_back(0);
            normals.push_back(0);
            normals.push_back(0);

            texCoords.push_back(0);
            texCoords.push_back(0);
        }
    };
    static void readObjFile(FILE *file, ObjData *pObjData);
};

#endif //NATIVEACTIVITYDEMO_OBJHELPER_H
