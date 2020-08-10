//
// Created by czf on 2019-10-23.
//

#include "Shape.h"
#include "../utils/CoordinatesUtils.h"

void Shape::move(float offsetX, float offsetY, float offsetZ) {
    translateXYZ[0] = CoordinatesUtils::android2gles_x(offsetX);
    translateXYZ[1] = CoordinatesUtils::android2gles_y(offsetY);
    translateXYZ[2] = offsetZ;
    glUniform3fv(transLocation, 1, translateXYZ); // vec is not array, so the count is 1.
}

void Shape::rotate(float xRadian, float yRadian, float zRadian) {
    rotateXYZ[0] = xRadian;
    rotateXYZ[1] = yRadian;
    rotateXYZ[2] = zRadian;
    glUniform3fv(rotateLocation, 1, rotateXYZ);
}

void Shape::scale(float x, float y, float z) {
    scaleXYZ[0] = x;
    scaleXYZ[1] = y;
    scaleXYZ[2] = z;
    glUniform3fv(scaleLocation, 1, scaleXYZ);
}
