//
// Created by czf on 2019-10-23.
//

#ifndef NATIVEACTIVITYDEMO_SHAPE_H
#define NATIVEACTIVITYDEMO_SHAPE_H

#include "../app_log.h"

class Shape {
public:
  Shape() {
    app_log("Shape constructor");
  }
  virtual ~Shape() {
    app_log("Shape destructor");
  }
  virtual void draw() = 0;
  virtual void move(float offsetX, float offsetY, float offsetZ) = 0;
};

#endif //NATIVEACTIVITYDEMO_SHAPE_H
