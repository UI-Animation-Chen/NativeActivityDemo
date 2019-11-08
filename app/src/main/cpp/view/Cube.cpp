//
// Created by czf on 2019-10-23.
//

#include <GLES3/gl32.h>

#include "../utils/ShaderUtils.h"
#include "Cube.h"
#include "../utils/CoordinatesUtils.h"

// three types of the precision: lowp, mediump and highp.

// for vertex, if the precision is not specified, it is consider to be highest (highp).
static const char *cubeVert = "#version 300 es\n"
                              "layout(location = 0) in vec4 vPositionCube;\n"

                              "uniform vec3 translate;\n" // vec is not array
                              "uniform vec3 scale;\n" // vec is not array
                              "uniform vec3 rotate;\n" // vec is not array

                              "out vec4 myColor;\n" // send to next stage(frag shader)

                              "void main() {\n"
                              "  gl_Position = vPositionCube;\n"

                              "  gl_Position[0] *= scale[0];\n"
                              "  gl_Position[1] *= scale[1];\n"
                              "  gl_Position[2] *= scale[2];\n"

                              "  float x = gl_Position[0];\n"
                              "  float y = gl_Position[1];\n"
                              "  float z = gl_Position[2];\n"
                              "  // 绕x轴旋转\n"
                              "  float xDeg = rotate[0];\n"
                              "  gl_Position[2] = z*cos(xDeg) - y*sin(xDeg);\n"
                              "  gl_Position[1] = z*sin(xDeg) + y*cos(xDeg);\n"

                              "  // 绕y轴旋转\n"
                              "  z = gl_Position[2];\n"
                              "  float yDeg = rotate[1];\n"
                              "  gl_Position[0] = x*cos(yDeg) - z*sin(yDeg);\n"
                              "  gl_Position[2] = x*sin(yDeg) + z*cos(yDeg);\n"

                              "  // 绕z轴旋转\n"
                              "  x = gl_Position[0];\n"
                              "  y = gl_Position[1];\n"
                              "  float zDeg = rotate[2];\n"
                              "  gl_Position[0] = x*cos(zDeg) - y*sin(zDeg);\n"
                              "  gl_Position[1] = x*sin(zDeg) + y*cos(zDeg);\n"

                              "  gl_Position[0] += translate[0];\n"
                              "  gl_Position[1] += translate[1];\n"
                              "  gl_Position[2] += translate[2];\n"

                              "  float c;\n"
                              "  if (gl_Position[2] < 0.0) {\n"
                              "    c = 0.5 - gl_Position[2]/1.5;\n"
                              "  } else {\n"
                              "    c = 0.5 - gl_Position[2]/4.5;\n"
                              "  }\n"
                              "  myColor = vec4(c, c, c, 1.0);\n"
                              "}\n";

// for fragment shader, Specifying the precision is compulsory.
static const char *cubeFrag = "#version 300 es\n"
                              "precision mediump float;\n"
                              "in vec4 myColor;\n" // receive from previous stage(vert shader)
                              "out vec4 fColorCube;\n"
                              "void main() {\n"
                              "  fColorCube = myColor;\n"
                              "}\n";

Cube::Cube() {
  init_shaders();

  glGenVertexArrays(1, &vao);
  glGenBuffers(2, buffers);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//  GLfloat cubePoints[] = {
//      -0.5f, -0.5f, 0.0f,
//      0.5f, -0.5f, 0.0f,
//      -0.5f,  0.5f, 0.0f,
//      0.5f, 0.5f, 0.0f
//  };
  GLfloat cubePoints[] = {
    -0.2f, 0.2f, 0.5f, // z轴正向为屏幕向里
    -0.2f, -0.2f, 0.5f,
    0.2f, -0.2f, 0.5f,
    0.2f, 0.2f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, 0.5f, -0.5f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubePoints), cubePoints, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
  GLushort cubeIndices[] = {
    0, 1, 2, 0, 2, 3, // 里， 逆时针
    0, 3, 4, 4, 3, 7, // 上， 逆时针
    0, 4, 5, 0, 5, 1, // 左， 逆时针
    5, 6, 1, 1, 6, 2, // 下， 逆时针
    3, 2, 6, 3, 6, 7, // 右， 逆时针
    4, 6, 5, 7, 6, 4 // 外，顺时针，cull face，被剔除了
  };
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
}

Cube::~Cube() {
  glDeleteBuffers(2, buffers);
  glDeleteVertexArrays(1, &vao);

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  glDeleteProgram(program);
}

void Cube::move(float offsetX, float offsetY, float offsetZ) {
  translateXYZ[0] = CoordinatesUtils::android2gles_x(offsetX);
  translateXYZ[1] = CoordinatesUtils::android2gles_y(offsetY);
  translateXYZ[2] = offsetZ;
  glUseProgram(program);
  glUniform3fv(transLocation, 1, translateXYZ); // vec is not array, so the count is 1.
}

void Cube::rotate(float xDeg, float yDeg, float zDeg) {
  rotateXYZ[0] = xDeg;
  rotateXYZ[1] = yDeg;
  rotateXYZ[2] = zDeg;
  glUseProgram(program);
  glUniform3fv(rotateLocation, 1, rotateXYZ);
}

void Cube::scale(float x, float y, float z) {
  scaleXYZ[0] = x;
  scaleXYZ[1] = y;
  scaleXYZ[2] = z;
  glUseProgram(program);
  glUniform3fv(scaleLocation, 1, scaleXYZ);
}

void Cube::draw() {
  glUseProgram(program);
  glBindVertexArray(vao);
  // 1->2->3, 2->3->4，先逆时针，后顺时针。
//  glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void Cube::init_shaders() {
  vertShader = get_compiled_shader_vert(cubeVert);
  fragShader = get_compiled_shader_frag(cubeFrag);
  program = linkShader(vertShader, fragShader);
  transLocation = glGetUniformLocation(program, "translate");
  scaleLocation = glGetUniformLocation(program, "scale");
  rotateLocation = glGetUniformLocation(program, "rotate");
}
