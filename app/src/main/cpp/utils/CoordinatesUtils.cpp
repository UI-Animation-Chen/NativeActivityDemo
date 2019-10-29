//
// Created by czf on 19-10-29.
//

#include "CoordinatesUtils.h"

// error: 'static' can only be specified inside the class definition
// static float CoordinatesUtils::screenW = 1;

float CoordinatesUtils::screenW = 1;
float CoordinatesUtils::screenH = 1;

float CoordinatesUtils::android2gles_x(float x) {
  return 2*x/screenW - 1;
}

float CoordinatesUtils::android2gles_y(float y) {
  // -(y - screenH/2)
  return (1 - 2*y/screenH);
}

float CoordinatesUtils::gles2android_x(float x) {
  return (x + 1) * screenW/2;
}

float CoordinatesUtils::gles2android_y(float y) {
  return screenH/2 * (1 - y);
}
