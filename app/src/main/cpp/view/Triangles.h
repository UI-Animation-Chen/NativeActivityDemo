//
// Created by czf on 2019-10-20.
//

#ifndef NATIVEACTIVITYDEMO_TRIANGLES_H
#define NATIVEACTIVITYDEMO_TRIANGLES_H

#include "Shape.h"

class Triangles: public Shape {
private:
  GLuint program;
  GLuint vertShader;
  GLuint fragShader;

  GLuint vaos[2]; // vertex array objects
  GLuint buffers[2];

  void init_shaders();

  void bind_buf0();
  void bind_buf1();

public:
  Triangles();
  virtual ~Triangles();
  virtual void draw();

};

#endif //NATIVEACTIVITYDEMO_TRIANGLES_H
