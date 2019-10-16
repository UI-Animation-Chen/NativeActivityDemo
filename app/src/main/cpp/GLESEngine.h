//
// Created by czf on 2019-10-16.
//

#ifndef NATIVEACTIVITYDEMO_GLESENGINE_H
#define NATIVEACTIVITYDEMO_GLESENGINE_H

#include <android/native_window.h>

int GLESEngine_init(ANativeWindow *);

void GLESEngine_draw_frame(GLfloat *);

void GLESEngine_destroy();

int32_t GLESEngine_get_width();

int32_t GLESEngine_get_height();

#endif //NATIVEACTIVITYDEMO_GLESENGINE_H
