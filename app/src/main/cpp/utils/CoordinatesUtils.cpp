//
// Created by czf on 19-10-29.
//

#include "CoordinatesUtils.h"
#include "../app_log.h"
#include "Utils.h"
#include "../entity/MapLocInfo.h"
#include "./libglm0_9_6_3/glm/glm.hpp"

// error: 'static' can only be specified inside the class definition
// static float CoordinatesUtils::screenW = 1;

float CoordinatesUtils::screenW = 1;
float CoordinatesUtils::screenH = 1;
float CoordinatesUtils::screenS = 1;
float CoordinatesUtils::screenL = 1;
float CoordinatesUtils::glesViewportSize = 1;

// 将安卓的坐标值转换为OpenGL ES的坐标值。
float CoordinatesUtils::android2gles_x(float x) {
    return 2 * x / glesViewportSize - 1;
}

float CoordinatesUtils::android2gles_y(float y) {
    // -(y - screenH/2)
    return (1 - 2 * y / glesViewportSize);
}

// 将OpenGL ES的坐标值转换为安卓的坐标值。
float CoordinatesUtils::gles2android_x(float x) {
    return (x + 1) * glesViewportSize / 2;
}

float CoordinatesUtils::gles2android_y(float y) {
    return glesViewportSize / 2 * (1 - y);
}

// androidDistance/glesViewportSize = glesDistance/2
float CoordinatesUtils::gles2android_distance(float glesDistance) {
    return glesDistance * glesViewportSize / 2;
}

float CoordinatesUtils::android2gles_distance(float androidDistance) {
    return androidDistance * 2 / glesViewportSize;
}

static int findExistIndex(std::unordered_map<int, std::unique_ptr<MapLocInfo>> &ztoy, int start, int end) {
    for (int i = start; i <= end; i++) {
        if (ztoy.count(i)) {
            return i;
        }
    }
    return start;
}

static int findExistIndex2(std::unordered_map<int, std::unordered_map<int, std::unique_ptr<MapLocInfo>>> &data, int start, int end, int z) {
    for (int i = start; i <= end; i++) {
        if (data.count(i) && data[i].count(z)) {
            return i;
        }
    }
    return start;
}

// 找出两个数之间的中间值，之间的数为偶数个时，找出偏左的那个。正负数通用。
static int findMiddleSmallerInt(int small, int large) {
    if ((large - small) % 2 != 0) { // 间隔是偶数个时，需要区分正负处理，正的浮点数截断取整时偏小，负的反而会偏大。
        float middle_f = (float)(small + large) / 2.0f;
        if (middle_f < 0.0f) { // 负数的浮点数截断取整后偏大，需要减1
            return (int)middle_f - 1;
        } else {
            return (int)middle_f;
        }
    } else {
        return (small + large)/2;
    }
}

static void getMiddleNormal(glm::vec3 &outVec3, std::vector<GLfloat> &vec1, std::vector<GLfloat> &vec2) {
    outVec3[0] = vec1[0] + vec2[0];
    outVec3[1] = vec1[1] + vec2[1];
    outVec3[2] = vec1[2] + vec2[2];
    outVec3 = glm::normalize(outVec3);
}

// 对二维map数据，按行和列分别进行插值。依次找两个存在的值，这两个值之间有空隙则插入线性值。如果找不到这样的两个值则跳过。
static void insertLinearValueInner(std::unordered_map<int, std::unordered_map<int, std::unique_ptr<MapLocInfo>>> &data,
                                   int minX, int minZ, int maxX, int maxZ) {
    // x方向过一遍
    for (int x = minX; x <= maxX; x++) {
//        app_log("inser linear value: x: %d\n", x);
        if (data.count(x)) {
            std::unordered_map<int, std::unique_ptr<MapLocInfo>> &ztoy = data[x]; // 注意得是引用
            for (int z = minZ; z < maxZ; z++) { // 不需要<=maxZ，下面find中有+1
                int z1 = findExistIndex(ztoy, z, maxZ);
                int z2 = findExistIndex(ztoy, z1 + 1, maxZ);
//                app_log("find: z1: %d, z2: %d\n", z1, z2);
                if (z2 - z1 > 1) {
                    int z_in = findMiddleSmallerInt(z1, z2);
                    GLfloat y1 = ztoy[z1]->height;
                    GLfloat y2 = ztoy[z2]->height;
                    ztoy[z_in] = std::make_unique<MapLocInfo>();
                    ztoy[z_in]->height = ((z_in-z2)*y1+(z1-z_in)*y2)/(z1-z2);
//                    app_log("insert: x: %d, z: %d, y: %f\n", x, z_in, ztoy[z_in]);
                    glm::vec3 normal;
                    getMiddleNormal(normal, ztoy[z1]->normal, ztoy[z2]->normal);
                    ztoy[z_in]->normal[0] = normal[0];
                    ztoy[z_in]->normal[1] = normal[1];
                    ztoy[z_in]->normal[2] = normal[2];
//                    app_log("insert normal x { %f, %f, %f }\n", normal[0], normal[1], normal[2]);
                    if (z2 - z1 > 3) {
                        z--; // 间隔大于2个时，还需要再走一遍当前值。
//                        app_log("间隔大于3，再走一遍\n");
                    }
                }
            }
        }
    }
    // z方向过一遍
    for (int z = minZ; z <= maxZ; z++) {
//        app_log("inser linear value: z: %d\n", z);
        for (int x = minX; x < maxX; x++) { // 不需要<=maxX，下面find中有+1
            int x1 = findExistIndex2(data, x, maxX, z);
            int x2 = findExistIndex2(data, x1 + 1, maxX, z);
//            app_log("find: x1: %d, x2: %d\n", x1, x2);
            if (x2 - x1 > 1) {
                int x_in = findMiddleSmallerInt(x1, x2);
                if (data.count(x_in) == 0) { // x_in位置可能不存在
                    data[x_in] = std::unordered_map<int, std::unique_ptr<MapLocInfo>>();
                }
                GLfloat y1 = data[x1][z]->height;
                GLfloat y2 = data[x2][z]->height;
                data[x_in][z] = std::make_unique<MapLocInfo>();
                data[x_in][z]->height = ((x_in-x2)*y1+(x1-x_in)*y2)/(x1-x2);
//                app_log("insert: x: %d, z: %d, y: %f\n", x_in, z, data[x_in][z]);
                glm::vec3 normal;
                getMiddleNormal(normal, data[x1][z]->normal, data[x2][z]->normal);
                data[x_in][z]->normal[0] = normal[0];
                data[x_in][z]->normal[1] = normal[1];
                data[x_in][z]->normal[2] = normal[2];
//                app_log("insert normal z { %f, %f, %f }\n", normal[0], normal[1], normal[2]);
                if (x2 - x1 > 3) {
                    x--;
//                    app_log("间隔大于3，再走一遍\n");
                }
            }
        }
    }
}

// 进行线性插值算法
void CoordinatesUtils::insertLinearValue(std::unordered_map<int, std::unordered_map<int, std::unique_ptr<MapLocInfo>>> &data,
                                         int minX, int minZ, int maxX, int maxZ) {
    long time0 = Utils::getCurrTimeUS();
    // 先将空隙按线性算法补上
    insertLinearValueInner(data, minX, minZ, maxX, maxZ);
    // 在补完之后，边缘仍然有空隙，则将四个边缘的空隙补为0
    for (int x = minX; x <= maxX; x++) {
        if (data.count(x) == 0) {
            data[x] = std::unordered_map<int, std::unique_ptr<MapLocInfo>>();
        }
        if (x == minX || x == maxX) {
            for (int z = minZ; z <= maxZ; z++) {
                if (data[x].count(z) == 0) {
                    data[x][z] = std::make_unique<MapLocInfo>();
                    data[x][z]->height = 0.0f;
//                    app_log("边缘空隙补充0: x: %d, z: %d\n", x, z);
                }
            }
        } else {
            if (data[x].count(minZ) == 0) {
                data[x][minZ] = std::make_unique<MapLocInfo>();
                data[x][minZ]->height = 0.0f;
//                app_log("边缘空隙补充0: x: %d, minZ: %d\n", x, minZ);
            }
            if (data[x].count(maxZ) == 0) {
                data[x][maxZ] = std::make_unique<MapLocInfo>();
                data[x][maxZ]->height = 0.0f;
//                app_log("边缘空隙补充0: x: %d, maxZ: %d\n", x, maxZ);
            }
        }
    }
    // 再进行一遍空隙补偿
    insertLinearValueInner(data, minX, minZ, maxX, maxZ);
    app_log("linear interpolateTime: %ld(us)\n", Utils::getCurrTimeUS() - time0);
}
