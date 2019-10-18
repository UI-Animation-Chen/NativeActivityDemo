//
// Created by czf on 2019-10-18.
//

#include <GLES3/gl32.h>
#include <stdlib.h>

#include "app_log.h"
#include "ShaderUtils.h"

GLuint loadAndCompileShader(GLenum shaderType, const char *sourceCode) {
  // 1, Create the shader
  GLuint shader = glCreateShader(shaderType);
  if (shader) {
    // 2, Pass the shader source code
    glShaderSource(shader, 1, &sourceCode, NULL);

    // 3, Compile the shader source code
    glCompileShader(shader);

    // Check the status of compilation
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      // Get the info log for compilation failure
      GLint infoLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
      if (infoLen) {
        char *buf = (char *) malloc((size_t) infoLen);
        if (buf) {
          glGetShaderInfoLog(shader, infoLen, NULL, buf);
          app_log("Could not compile shader %s:\n", buf);
          free(buf);
        }

        // Delete the shader program
        glDeleteShader(shader);
        shader = 0;
      }
    }
  }
  return shader;
}

GLuint get_compiled_shader_vert(const char *sourceCode) {
  return loadAndCompileShader(GL_VERTEX_SHADER, sourceCode);
}

GLuint get_compiled_shader_frag(const char *sourceCode) {
  return loadAndCompileShader(GL_FRAGMENT_SHADER, sourceCode);
}

GLuint linkShader(GLuint vertShaderID, GLuint fragShaderID) {
  if (!vertShaderID || !fragShaderID) { // Fails! return
    return 0;
  }

  // Create an empty program object
  GLuint program = glCreateProgram();
  if (program) {
    // Attach vertex and fragment shader to it
    glAttachShader(program, vertShaderID);
    glAttachShader(program, fragShaderID);

    // Link the program
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus != GL_TRUE) {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if (bufLength) {
        char *buf = (char *) malloc((size_t)bufLength);
        if (buf) {
          glGetProgramInfoLog(program, bufLength, NULL, buf);
          app_log("Could not link program: %s\n", buf);
          free(buf);
        }
      }
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}
