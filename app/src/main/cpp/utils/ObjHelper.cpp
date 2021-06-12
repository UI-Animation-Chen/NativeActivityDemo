//
// Created by mtdp on 2020-04-11.
//

#include "ObjHelper.h"
#include "../app_log.h"
#include "CoordinatesUtils.h"
#include <map>
#include <string>

#define SMOOTH_LIGHT
//#define NO_TEX_COOR

static void readVertices(FILE *file, ObjHelper::ObjData *pObjData) {
    GLfloat x, y, z;
    fscanf(file, "%f %f %f\n", &x, &y, &z);
    pObjData->vertices.push_back(-x); // obj文件(导出设置z forward，y up)的x坐标是反的。
    pObjData->vertices.push_back(y);
    pObjData->vertices.push_back(z);
}

// obj文件中每个顶点的法向量，其实是构成一个三角面片的面法向量，这三个顶点的法向量都相同。
// 同一个顶点，当其所处的三角面片不一样，就会具有不同的法向量。
static void readNormals(FILE *file, ObjHelper::ObjData *pObjData) {
    GLfloat x, y, z;
    fscanf(file, " %f %f %f\n", &x, &y, &z);
    pObjData->normals.push_back(-x); // obj文件(导出设置z forward，y up)的x坐标是反的。
    pObjData->normals.push_back(y);
    pObjData->normals.push_back(z);
}

static void readTexCoords(FILE *file, ObjHelper::ObjData *pObjData) {
    GLfloat u, v;
    fscanf(file, " %f %f\n", &u, &v);
    pObjData->texCoords.push_back(u);
    pObjData->texCoords.push_back(v);
}

static void readIndexInfo(FILE *file, ObjHelper::ObjData *pObjData) {
    GLushort v1, v2, v3, t1, t2, t3, n1, n2, n3;
    // %hd 短整型
#ifdef NO_TEX_COOR
    fscanf(file, " %hd//%hd %hd//%hd %hd//%hd\n", &v1, &n1, &v2, &n2, &v3, &n3);
#else
    fscanf(file, " %hd/%hd/%hd %hd/%hd/%hd %hd/%hd/%hd\n",
                   &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
#endif
#ifdef NO_TEX_COOR
    pObjData->indeces.push_back({v1, 1, n1});
    pObjData->indeces.push_back({v2, 1, n2});
    pObjData->indeces.push_back({v3, 1, n3});
#else
    pObjData->indeces.push_back({v1, t1, n1});
    pObjData->indeces.push_back({v2, t2, n2});
    pObjData->indeces.push_back({v3, t3, n3});
#endif
}

// 按照obj文件格式读出来后，顶点，纹理和法向量坐标都有各自的索引数组。
// 现在新建一套匹配的顶点，纹理和法向量坐标，由同一个索引数组控制。
// 这可能会导致各坐标数组变大，包含重复的坐标数据，这是统一索引的代价。
static void rearrangeVVtVns(ObjHelper::ObjData *pObjData) {
    using namespace std;
    vector<GLfloat>vs;
    vector<GLfloat>vts;
    vector<GLfloat>vns;

    GLuint vertIndex;
    GLuint texCoordsIndex;
    GLuint nomalIndex;

#ifdef SMOOTH_LIGHT
    map<string, vector<GLuint>> vert2indecesMap; // 一个顶点被几个索引用过。
#endif

    for (GLuint i = 0; i < pObjData->indeces.size(); i++) {
        // vertices
        vertIndex = pObjData->indeces.at(i).at(0) * (GLuint)3; // 乘法左边是GLushort，会自动提为int
        vs.push_back(pObjData->vertices.at(vertIndex));
        vs.push_back(pObjData->vertices.at(vertIndex + 1));
        vs.push_back(pObjData->vertices.at(vertIndex + 2));
#ifdef SMOOTH_LIGHT
        GLuint vsIndex = i * 3;
        string vertKey = to_string(vs.at(vsIndex)) +
                         to_string(vs.at(vsIndex + 1)) +
                         to_string(vs.at(vsIndex + 2));
        vert2indecesMap[vertKey].push_back(i);
#endif
        // texCoords
        texCoordsIndex = pObjData->indeces.at(i).at(1) * (GLuint)2;
        vts.push_back(pObjData->texCoords.at(texCoordsIndex));
        vts.push_back(pObjData->texCoords.at(texCoordsIndex + 1));
        // normals
        nomalIndex = pObjData->indeces.at(i).at(2) * (GLuint)3;
        vns.push_back(pObjData->normals.at(nomalIndex));
        vns.push_back(pObjData->normals.at(nomalIndex + 1));
        vns.push_back(pObjData->normals.at(nomalIndex + 2));
        // indeces
        pObjData->indeces.at(i).at(0) = (GLushort)i; // 索引就是0,1,2... 每个索引都对应一个坐标(xyz)。
    }
#ifdef SMOOTH_LIGHT
    for (auto const &entry: vert2indecesMap) {
        GLfloat nx = 0, ny = 0, nz = 0; // 将该顶点对应的所有面的法向量相加，在shader里进行归一化。
        map<string, vector<GLfloat>> uniqueVnMap; // 该顶点相同的法向量去重，只保留不同的
        for (GLuint i = 0; i < entry.second.size(); i++) {
            GLuint vnsIndex = entry.second.at(i) * 3;
            string vnKey = to_string(vns.at(vnsIndex)) +
                           to_string(vns.at(vnsIndex + 1)) +
                           to_string(vns.at(vnsIndex + 2));
            if (uniqueVnMap.count(vnKey) == 0) {
                uniqueVnMap[vnKey].push_back(vns.at(vnsIndex));
                uniqueVnMap[vnKey].push_back(vns.at(vnsIndex + 1));
                uniqueVnMap[vnKey].push_back(vns.at(vnsIndex + 2));
            }
        }
        for (auto const &vnEntry: uniqueVnMap) {
            nx += vnEntry.second.at(0);
            ny += vnEntry.second.at(1);
            nz += vnEntry.second.at(2);
        }
        for (GLuint i = 0; i < entry.second.size(); i++) {
            GLuint vnsIndex = entry.second.at(i) * 3;
            vns.at(vnsIndex) = nx;
            vns.at(vnsIndex + 1) = ny;
            vns.at(vnsIndex + 2) = nz;
        }
    }
#endif
    pObjData->vertices = vs;
    pObjData->texCoords = vts;
    pObjData->normals = vns;
}

void ObjHelper::readObjFile(FILE *file, ObjHelper::ObjData *pObjData) {
    if (file == NULL) return;
    bool shouldQuit = false;
    int c;
    while ((c = fgetc(file)) != EOF && !shouldQuit) {
        switch (c) {
            case '#': // comments
            case 'o': // o ObjModel
            case 's': // s off
            case 'g': // grouping
                while (fgetc(file) != '\n'); // skip this line in these cases.
                break;
            case 'v': // v vn vt
                c = fgetc(file);
                switch (c) {
                    case ' ':
                        readVertices(file, pObjData);
                        break;
                    case 'n':
                        readNormals(file, pObjData);
                        break;
                    case 't':
                        readTexCoords(file, pObjData);
                        break;
                    default:
                        app_log("case v, found some unkown chars!!! the char is: %c", c);
                        break;
                }
                break;
            case 'f': // face, index info
                readIndexInfo(file, pObjData);
                break;
            default:
                shouldQuit = true;
                app_log("readObjFile, found some unkown lines!!! the char is: %c", c);
                break;
        }
    }
#ifdef NO_TEX_COOR
    pObjData->texCoords.push_back(0.5f); // 如果没有生成纹理坐标，就创建一个坐标，使用纹理的中心点颜色
    pObjData->texCoords.push_back(0.5f);
#endif
    rearrangeVVtVns(pObjData);
}
