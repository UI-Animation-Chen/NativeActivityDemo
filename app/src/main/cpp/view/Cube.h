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
  GLuint buffers[2];

  GLfloat translateXYZ[3];
  GLint transLocation;

  GLfloat scaleXYZ[3];
  GLint scaleLocation;

  void init_shaders();

public:
  Cube();
  virtual ~Cube();
  virtual void draw();
  virtual void move(float offsetX, float offsetY, float offsetZ);
  virtual void rotate(float xDeg, float yDeg, float zDeg);
  virtual void scale(float x, float y, float z);
};

#endif //NATIVEACTIVITYDEMO_CUBE_H
