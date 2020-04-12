//
// Created by mtdp on 2020-04-11.
//

#include "ObjHelper.h"
#include "../app_log.h"

static void readVertices(FILE *file, ObjHelper::ObjModel *pObjModel) {
    GLfloat x, y, z;
    fscanf(file, "%f %f %f\n", &x, &y, &z);
    pObjModel->vertices.push_back(x);
    pObjModel->vertices.push_back(y);
    pObjModel->vertices.push_back(z);
}

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
    fscanf(file, " %hd/%hd/%hd %hd/%hd/%hd %hd/%hd/%hd\n", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
    pObjModel->indeces.push_back({v1, t1, n1});
    pObjModel->indeces.push_back({v2, t2, n2});
    pObjModel->indeces.push_back({v3, t3, n3});
}

// 按照f索引，新建一套匹配的顶点，纹理和法向量坐标，由一套索引引用。
static void rearrangeVVtVns(ObjHelper::ObjModel *pObjModel) {
    std::vector<GLfloat>vs;
    std::vector<GLfloat>vts;
    std::vector<GLfloat>vns;
    for (GLushort i = 0; i < pObjModel->indeces.size(); i++) {
        // vertices
        vs.push_back(pObjModel->vertices.at(pObjModel->indeces.at(i).at(0) * (GLushort)3));
        vs.push_back(pObjModel->vertices.at(pObjModel->indeces.at(i).at(0) * (GLushort)3 + (GLushort)1));
        vs.push_back(pObjModel->vertices.at(pObjModel->indeces.at(i).at(0) * (GLushort)3 + (GLushort)2));
        // texCoords
        vts.push_back(pObjModel->texCoords.at(pObjModel->indeces.at(i).at(1) * (GLushort)2));
        vts.push_back(pObjModel->texCoords.at(pObjModel->indeces.at(i).at(1) * (GLushort)2 + (GLushort)1));
        // normals
        vns.push_back(pObjModel->normals.at(pObjModel->indeces.at(i).at(2) * (GLushort)3));
        vns.push_back(pObjModel->normals.at(pObjModel->indeces.at(i).at(2) * (GLushort)3 + (GLushort)1));
        vns.push_back(pObjModel->normals.at(pObjModel->indeces.at(i).at(2) * (GLushort)3 + (GLushort)2));
        // indeces
        pObjModel->indeces.at(i).at(0) = i; // 索引就是0, 1, 2, 3...
    }
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
