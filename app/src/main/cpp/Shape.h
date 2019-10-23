//
// Created by czf on 2019-10-23.
//

#ifndef NATIVEACTIVITYDEMO_SHAPE_H
#define NATIVEACTIVITYDEMO_SHAPE_H

class Shape {
public:
  Shape() {}
  virtual ~Shape() {}
  virtual void draw() = 0;
};

#endif //NATIVEACTIVITYDEMO_SHAPE_H
