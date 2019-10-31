//
// Created by czf on 2019-10-23.
//

#include <GLES3/gl32.h>

#include "../gles/ShaderUtils.h"
#include "Cube.h"
#include "../utils/CoordinatesUtils.h"

// three types of the precision: lowp, mediump and highp.

// for vertex, if the precision is not specified, it is consider to be highest (highp).
static const char *cubeVert = "#version 300 es\n"
                              "layout(location = 0) in vec4 vPositionCube;\n"
                              "uniform float dx;\n"
                              "uniform float dy;\n"
                              "out vec4 myColor;\n" // send to next stage(frag shader)
                              "void main() {\n"
                              "  gl_Position = vPositionCube;\n"
                              "  gl_Position[0] += dx;\n"
                              "  gl_Position[1] += dy;\n"
                              "  myColor = vec4(dx, dx - dy, dy, 1.0);\n"
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
    0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f,  -0.5f, 0.0f,
    0.5f, 0.5f, 0.0f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubePoints), cubePoints, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
  GLushort cubeIndices[] = {
    0, 1, 2, 0, 2, 3, 0, 3, 4
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
  GLfloat offsets[4] = {0};
  offsets[0] = CoordinatesUtils::android2gles_x(offsetX);
  offsets[1] = CoordinatesUtils::android2gles_y(offsetY);
  glUseProgram(program);
  glUniform1f(dxLocation, offsets[0]);
  glUniform1f(dyLocation, offsets[1]);
}

void Cube::draw() {
  glUseProgram(program);
  glBindVertexArray(vao);
  // 1->2->3, 2->3->4，先逆时针，后顺时针。
//  glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
  glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, 0);
}

void Cube::init_shaders() {
  vertShader = get_compiled_shader_vert(cubeVert);
  fragShader = get_compiled_shader_frag(cubeFrag);
  program = linkShader(vertShader, fragShader);
  dxLocation = glGetUniformLocation(program, "dx");
  dyLocation = glGetUniformLocation(program, "dy");
}
