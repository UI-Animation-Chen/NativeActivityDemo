//
// Created by czf on 2019-10-18.
//

#ifndef NATIVEACTIVITYDEMO_SHADERUTILS_H
#define NATIVEACTIVITYDEMO_SHADERUTILS_H

GLuint get_compiled_shader_vert(const char *sourceCode);
GLuint get_compiled_shader_frag(const char *sourceCode);

GLuint linkShader(GLuint vertShaderID, GLuint fragShaderID);

#endif //NATIVEACTIVITYDEMO_SHADERUTILS_H
