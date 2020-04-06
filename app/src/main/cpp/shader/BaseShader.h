//
// Created by czf on 19-11-9.
//

#ifndef NATIVEACTIVITYDEMO_BASESHADER_H
#define NATIVEACTIVITYDEMO_BASESHADER_H

#include <GLES3/gl32.h>

class BaseShader {
public:
    static GLuint getSingletonProgram();
    static void deleteSingletonProgram();
private:
    static GLuint program;
    static GLuint vertShader;
    static GLuint fragShader;
};

#endif //NATIVEACTIVITYDEMO_BASESHADER_H
