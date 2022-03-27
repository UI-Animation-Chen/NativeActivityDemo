//
// Created by mtdp on 2020-04-11.
//

#include "ObjHelper.h"
#include "../app_log.h"
#include "CoordinatesUtils.h"
#include <unordered_map>
#include <string>
#include "Utils.h"

#define SMOOTH_LIGHT
//#define NO_TEX_COOR

float ObjHelper::heightMapSampleFactor = 100.0f; // 表示取浮点数小数部分的位数，10表示1位，100表示两位等等。注意只能是整数。

static void findMinMaxVertex(GLfloat x, GLfloat y, GLfloat z, ObjHelper::ObjData *pObjData) {
    if (x <= 0) {
        if (x < pObjData->minVertex.at(0)) {
            pObjData->minVertex.at(0) = x;
        }
    } else {
        if (x > pObjData->maxVertex.at(0)) {
            pObjData->maxVertex.at(0) = x;
        }
    }
    if (y <= 0) {
        if (y < pObjData->minVertex.at(1)) {
            pObjData->minVertex.at(1) = y;
        }
    } else {
        if (y > pObjData->maxVertex.at(1)) {
            pObjData->maxVertex.at(1) = y;
        }
    }
    if (z <= 0) {
        if (z < pObjData->minVertex.at(2)) {
            pObjData->minVertex.at(2) = z;
        }
    } else {
        if (z > pObjData->maxVertex.at(2)) {
            pObjData->maxVertex.at(2) = z;
        }
    }
}

// 构建高度数据，用于地图使用。heightMapSampleFactor表示取浮点数小数部分的位数，10表示1位，100表示两位等等。
static void genHeightMap(ObjHelper::ObjData *pObjData, GLfloat x, GLfloat y, GLfloat z) {
    int fixedX = (int)(x * ObjHelper::heightMapSampleFactor);
    int fixedZ = (int)(z * ObjHelper::heightMapSampleFactor);
    if (pObjData->heightMap.count(fixedX) == 0) { // 不存在该元素
        pObjData->heightMap[fixedX] = std::unordered_map<int, GLfloat>();
        pObjData->heightMap[fixedX][fixedZ] = y;
//        app_log("不存在该元素-x: x:%d, z:%d, y:%f\n", fixedX, fixedZ, y);
    } else {
        if (pObjData->heightMap[fixedX].count(fixedZ) == 0) {
            pObjData->heightMap[fixedX][fixedZ] = y;
//            app_log("不存在该元素-z: x:%d, z:%d, y:%f\n", fixedX, fixedZ, y);
        } else {
            GLfloat currValue = pObjData->heightMap[fixedX][fixedZ];
            if (y > currValue) {
                pObjData->heightMap[fixedX][fixedZ] = y;
//                app_log("替换元素-y: x:%d, z:%d, y:%f\n", fixedX, fixedZ, y);
            }
        }
    }
}

static void readVertices(FILE *file, ObjHelper::ObjData *pObjData, int needGenHeightMap) {
    GLfloat x, y, z;
    fscanf(file, "%f %f %f\n", &x, &y, &z);
    pObjData->vertices.push_back(-x); // obj文件(导出设置z forward，y up)的x坐标是反的。
    pObjData->vertices.push_back(y);
    pObjData->vertices.push_back(z);

    findMinMaxVertex(-x, y, z, pObjData); // obj文件(导出设置z forward，y up)的x坐标是反的。

    // 构建高度数据，用于地图使用。x和z采用小数点后1位的精度当一个区间
    if (needGenHeightMap == 1) {
        genHeightMap(pObjData, -x, y, z);
    }
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
    vector<GLfloat> vs;
    vector<GLfloat> vts;
    vector<GLfloat> vns;

    GLuint vertIndex;
    GLuint texCoordsIndex;
    GLuint nomalIndex;

#ifdef SMOOTH_LIGHT
    unordered_map<string, vector<GLuint>> vert2indecesMap; // 一个顶点被几个索引用过。
#endif

    for (GLuint i = 0; i < pObjData->indeces.size(); i++) {
        // vertices。后面乘3的逻辑是：vertices中三个元素为一组顶点，并且是从索引3开始，前三个元素是无用的。下面tex和normal同理。
        // 例如，从indeces中取出的索引是1时，实际要从vertices的3开始
        vertIndex = pObjData->indeces.at(i).at(0) * (GLuint)3; // 乘法左边是GLushort，会自动提为int
        vs.push_back(pObjData->vertices.at(vertIndex));
        vs.push_back(pObjData->vertices.at(vertIndex + 1));
        vs.push_back(pObjData->vertices.at(vertIndex + 2));
#ifdef SMOOTH_LIGHT
        GLuint vsIndex = i * 3;
        string vertKey = to_string(vs.at(vsIndex)) +
                         to_string(vs.at(vsIndex + 1)) +
                         to_string(vs.at(vsIndex + 2)); // 将顶点的三个坐标拼接，生成一个顶点的标识
        vert2indecesMap[vertKey].push_back(i); // 在push_back之前，map会先创建该key下的一个vector
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
        unordered_map<string, vector<GLfloat>> uniqueVnMap; // 该顶点相同的法向量去重，只保留不同的
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
        for (GLuint i = 0; i < entry.second.size(); i++) { // 这些索引下的顶点，都变成同一个法向量
            GLuint vnsIndex = entry.second.at(i) * 3;
            vns.at(vnsIndex) = nx;
            vns.at(vnsIndex + 1) = ny;
            vns.at(vnsIndex + 2) = nz;
        }
    }
#endif
    pObjData->vertices = move(vs);
    pObjData->texCoords = move(vts);
    pObjData->normals = move(vns);
}

void ObjHelper::readObjFile(FILE *file, ObjHelper::ObjData *pObjData, int needGenHeightMap) {
    if (file == nullptr) return;

    long time0 = Utils::getCurrTimeUS();

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
                        readVertices(file, pObjData, needGenHeightMap);
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
    long time1 = Utils::getCurrTimeUS();
    rearrangeVVtVns(pObjData);
    app_log("parseTime: %ld(us), rearrangeTime: %ld(us)\n", time1 - time0, Utils::getCurrTimeUS() - time1);
}
