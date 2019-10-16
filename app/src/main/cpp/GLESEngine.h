//
// Created by czf on 2019-10-16.
//

#ifndef NATIVEACTIVITYDEMO_GLESENGINE_H
#define NATIVEACTIVITYDEMO_GLESENGINE_H

#include <android/native_window.h>

int GLESEngine_init(ANativeWindow *);

void GLESEngine_draw_frame();

void GLESEngine_destroy();

#endif //NATIVEACTIVITYDEMO_GLESENGINE_H
