//
// Created by czf on 2019-10-23.
//

#include <GLES3/gl32.h>

#include "../gles/ShaderUtils.h"
#include "Cube.h"

static const char *cubeVert = "#version 320 es\n"
                              "layout(location = 0) in vec4 vPosition;\n"
                              "void main() {\n"
                              "  gl_Position = vPosition;\n"
                              "}\n";

static const char *cubeFrag = "#version 320 es\n"
                              "precision mediump float;\n"
                              "out vec4 fColor;\n"
                              "void main() {\n"
                              "  fColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                              "}\n";

Cube::Cube() {
  init_shaders();

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &buffer);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  GLfloat cubePoints[] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f,  0.5f, 0.0f,
      0.5f, 0.5f, 0.0f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubePoints), cubePoints, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
}

Cube::~Cube() {
  glDeleteBuffers(1, &buffer);
  glDeleteVertexArrays(1, &vao);

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  glDeleteProgram(program);
}

void Cube::draw() {
  // 1->2->3, 2->3->4，先逆时针，后顺时针。
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Cube::init_shaders() {
  vertShader = get_compiled_shader_vert(cubeVert);
  fragShader = get_compiled_shader_frag(cubeFrag);
  program = linkShader(vertShader, fragShader);
  glUseProgram(program);
}
