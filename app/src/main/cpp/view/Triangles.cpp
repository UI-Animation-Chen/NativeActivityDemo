//
// Created by czf on 2019-10-20.
//

#include <GLES3/gl32.h>
#include "Triangles.h"
#include "../gles/ShaderUtils.h"

static const char *triangleVert = "#version 320 es\n"
                            "layout(location = 0) in vec4 vPosition;\n"
                            "void main() {\n"
                            "  gl_Position = vPosition;\n"
                            "}\n";

static const char *triangleFrag = "#version 320 es\n"
                            "precision mediump float;\n"
                            "out vec4 fColor;\n"
                            "void main() {\n"
                            "  fColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                            "}\n";

Triangles::Triangles(): Shape() {
  init_shaders();

  glGenVertexArrays(2, vaos);
  glGenBuffers(2, buffers);

  glBindVertexArray(vaos[0]); // 记录所有的状态数据，包括attrib的开启与否。
  bind_buf0();
  // glVertexAttribPointer操作的是[当前绑定]到GL_ARRAY_BUFFER上的VBO
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(vaos[1]);
  bind_buf1();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
}

Triangles::~Triangles() {
  glDeleteBuffers(2, buffers);
  glDeleteVertexArrays(2, vaos);

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  glDeleteProgram(program);
}

void Triangles::init_shaders() {
  vertShader = get_compiled_shader_vert(triangleVert);
  fragShader = get_compiled_shader_frag(triangleFrag);
  program = linkShader(vertShader, fragShader);
  glUseProgram(program);
}

void Triangles::bind_buf0() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  GLfloat triangles[] = {
      0.0f,  0.75f, 0.0f,
      -0.25f, -0.0f, 0.0f,
      0.0f, -0.75f, 0.0f,
      0.25f, 0.0f, 0.0f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
}

void Triangles::bind_buf1() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  GLfloat triangles[] = {
      0.0f,  0.5f, 0.0f,
      -0.5f, 0.0f, 0.0f,
      0.0f, -0.5f, 0.0f,
      0.5f, 0.0f, 0.0f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
}

int state = 0;

void Triangles::draw() {
  if (state == 0) {
    glBindVertexArray(vaos[1]);
    state = 1;
  } else {
    glBindVertexArray(vaos[0]);
    state = 0;
  }
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}