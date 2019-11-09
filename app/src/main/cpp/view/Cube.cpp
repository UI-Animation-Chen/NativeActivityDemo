//
// Created by czf on 2019-10-23.
//

#include <GLES3/gl32.h>

#include "Cube.h"

Cube::Cube() : Shape() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(2, buffers);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
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
    3, 2, 6, 3, 6, 7 // 右， 逆时针
//    4, 6, 5, 7, 6, 4 // 外，顺时针，cull face，被剔除了
  };
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
}

Cube::~Cube() {
  glDeleteBuffers(2, buffers);
  glDeleteVertexArrays(1, &vao);
}

void Cube::draw() {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_SHORT, 0);
}
