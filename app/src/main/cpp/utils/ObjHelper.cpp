//
// Created by mtdp on 2020-04-11.
//

#include "ObjHelper.h"
#include "../app_log.h"
#include "CoordinatesUtils.h"
#include <map>
#include <string>

#define SMOOTH_LIGHT

static void readVertices(FILE *file, ObjHelper::ObjModel *pObjModel) {
    GLfloat x, y, z;
    fscanf(file, "%f %f %f\n", &x, &y, &z);
    if (CoordinatesUtils::screenW > CoordinatesUtils::screenH) {
        x = x * (GLfloat)CoordinatesUtils::screenH / (GLfloat)CoordinatesUtils::screenW;
    } else if (CoordinatesUtils::screenW < CoordinatesUtils::screenH) {
        y = y * (GLfloat)CoordinatesUtils::screenW / (GLfloat)CoordinatesUtils::screenH;
    }
    pObjModel->vertices.push_back(x);
    pObjModel->vertices.push_back(y);
    pObjModel->vertices.push_back(z);
}

// obj文件中每个顶点的法向量，其实是构成一个三角面片的面法向量，这三个顶点的法向量都相同。
// 同一个顶点，当其所处的三角面片不一样，就会具有不同的法向量。
static void readNormals(FILE *file, ObjHelper::ObjModel *pObjModel) {
    GLfloat x, y, z;
    fscanf(file, " %f %f %f\n", &x, &y, &z);
    pObjModel->normals.push_back(x);
    pObjModel->normals.push_back(y);
    pObjModel->normals.push_back(z);
}

static void readTexCoords(FILE *file, ObjHelper::ObjModel *pObjModel) {
    GLfloat u, v;
    fscanf(file, " %f %f\n", &u, &v);
    pObjModel->texCoords.push_back(u);
    pObjModel->texCoords.push_back(v);
}

static void readInfexInfo(FILE *file, ObjHelper::ObjModel *pObjModel) {
    GLushort v1, v2, v3, t1, t2, t3, n1, n2, n3;
    // %hd 短整型
    fscanf(file, " %hd/%hd/%hd %hd/%hd/%hd %hd/%hd/%hd\n", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
    pObjModel->indeces.push_back({v1, t1, n1});
    pObjModel->indeces.push_back({v2, t2, n2});
    pObjModel->indeces.push_back({v3, t3, n3});
}

// 按照obj文件格式读出来后，顶点，纹理和法向量坐标都有各自的索引数组。
// 现在新建一套匹配的顶点，纹理和法向量坐标，由同一个索引数组控制。
// 这可能会导致各坐标数组变大，包含重复的坐标数据，这是统一索引的代价。
static void rearrangeVVtVns(ObjHelper::ObjModel *pObjModel) {
    using namespace std;
    vector<GLfloat>vs;
    vector<GLfloat>vts;
    vector<GLfloat>vns;

    GLushort vertIndex;
    GLushort texCoordsIndex;
    GLushort nomalIndex;

#ifdef SMOOTH_LIGHT
    map<string, vector<GLushort>> vert2indeces; // 一个顶点被几个索引用过。
#endif

    for (GLushort i = 0; i < pObjModel->indeces.size(); i++) {
        // vertices
        vertIndex = pObjModel->indeces.at(i).at(0) * (GLushort)3;
        vs.push_back(pObjModel->vertices.at(vertIndex));
        vs.push_back(pObjModel->vertices.at(vertIndex + (GLushort)1));
        vs.push_back(pObjModel->vertices.at(vertIndex + (GLushort)2));
#ifdef SMOOTH_LIGHT
        GLushort vsIndex = i * (GLushort)3;
        string vertKey = to_string(vs.at(vsIndex)) + to_string(vs.at(vsIndex + 1)) + to_string(vs.at(vsIndex + 2));
        if (vert2indeces.count(vertKey) == 0) {
            vert2indeces[vertKey] = {i};
        } else {
            vert2indeces[vertKey].push_back(i);
        }
#endif
        // texCoords
        texCoordsIndex = pObjModel->indeces.at(i).at(1) * (GLushort)2;
        vts.push_back(pObjModel->texCoords.at(texCoordsIndex));
        vts.push_back(pObjModel->texCoords.at(texCoordsIndex + (GLushort)1));
        // normals
        nomalIndex = pObjModel->indeces.at(i).at(2) * (GLushort)3;
        vns.push_back(pObjModel->normals.at(nomalIndex));
        vns.push_back(pObjModel->normals.at(nomalIndex + (GLushort)1));
        vns.push_back(pObjModel->normals.at(nomalIndex + (GLushort)2));
        // indeces
        pObjModel->indeces.at(i).at(0) = i; // 索引就是0, 1, 2, 3... 索引里每一个值都对应一个坐标(xyz)。
    }
#ifdef SMOOTH_LIGHT
    for (auto const &entry: vert2indeces) {
        GLfloat nx = 0, ny = 0, nz = 0; // 将该顶点对应的所有面的法向量相加，在shader里进行归一化。
        for (GLushort i = 0; i < entry.second.size(); i++) {
            GLushort vnsIndex = entry.second.at(i) * (GLushort)3;
            nx += vns.at(vnsIndex);
            ny += vns.at(vnsIndex + (GLushort)1);
            nz += vns.at(vnsIndex + (GLushort)2);
        }
        for (GLushort i = 0; i < entry.second.size(); i++) {
            GLushort vnsIndex = entry.second.at(i) * (GLushort)3;
            vns.at(vnsIndex) = nx;
            vns.at(vnsIndex + (GLushort)1) = ny;
            vns.at(vnsIndex + (GLushort)2) = nz;
        }
    }
#endif
    pObjModel->vertices = vs;
    pObjModel->texCoords = vts;
    pObjModel->normals = vns;
}

void ObjHelper::readObjFile(FILE *file, ObjHelper::ObjModel *pObjModel) {
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
                        readVertices(file, pObjModel);
                        break;
                    case 'n':
                        readNormals(file, pObjModel);
                        break;
                    case 't':
                        readTexCoords(file, pObjModel);
                        break;
                    default:
                        app_log("case v, found some unkown chars!!! the char is: %c", c);
                        break;
                }
                break;
            case 'f': // face, index info
                readInfexInfo(file, pObjModel);
                break;
            default:
                shouldQuit = true;
                app_log("readObjFile, found some unkown lines!!! the char is: %c", c);
                break;
        }
    }
    rearrangeVVtVns(pObjModel);
}
