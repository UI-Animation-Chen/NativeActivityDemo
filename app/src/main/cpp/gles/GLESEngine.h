//
// Created by czf on 2019-10-16.
//

#ifndef NATIVEACTIVITYDEMO_GLESENGINE_H
#define NATIVEACTIVITYDEMO_GLESENGINE_H

#include <android/native_window.h>

#ifdef __cplusplus
extern "C" {
#endif

int GLESEngine_init(ANativeWindow *);

void GLESEngine_refresh();

void GLESEngine_destroy();

int32_t GLESEngine_get_width();

int32_t GLESEngine_get_height();

int32_t GLESEngine_get_viewport_size();

#ifdef __cplusplus
}
#endif

#endif //NATIVEACTIVITYDEMO_GLESENGINE_H
