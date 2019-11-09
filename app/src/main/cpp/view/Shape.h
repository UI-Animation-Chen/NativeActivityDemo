//
// Created by czf on 2019-10-23.
//

#ifndef NATIVEACTIVITYDEMO_SHAPE_H
#define NATIVEACTIVITYDEMO_SHAPE_H

#include <GLES3/gl32.h>
#include "../app_log.h"
#include "../gles/BaseShader.h"

class Shape {
public:
  Shape() {
    program = BaseShader::getSingletonProgram();
    glUseProgram(program);
    transLocation = glGetUniformLocation(program, "translate");
    scaleLocation = glGetUniformLocation(program, "scale");
    rotateLocation = glGetUniformLocation(program, "rotate");
    app_log("Shape constructor");
  }

  virtual ~Shape() {
    app_log("Shape destructor");
  }

  virtual void draw() = 0;

  virtual void move(float offsetX, float offsetY, float offsetZ);

  virtual void rotate(float xDeg, float yDeg, float zDeg);

  virtual void scale(float x, float y, float z);

private:
  GLuint program;

  GLfloat translateXYZ[3];
  GLint transLocation;

  GLfloat scaleXYZ[3];
  GLint scaleLocation;

  GLfloat rotateXYZ[3];
  GLint rotateLocation;
};

#endif //NATIVEACTIVITYDEMO_SHAPE_H
