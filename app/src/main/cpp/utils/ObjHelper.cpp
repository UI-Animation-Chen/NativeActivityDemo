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
}
