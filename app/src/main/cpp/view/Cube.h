//
// Created by czf on 2019-10-23.
//

#ifndef NATIVEACTIVITYDEMO_CUBE_H
#define NATIVEACTIVITYDEMO_CUBE_H

#include "Shape.h"

class Cube: public Shape {
private:
  GLuint program;
  GLuint vertShader;
  GLuint fragShader;

  GLuint vao; // vertex array object
  GLuint buffer;

  void init_shaders();

public:
  Cube();
  virtual ~Cube();
  virtual void draw();

};

#endif //NATIVEACTIVITYDEMO_CUBE_H
