//
// Created by czf on 19-10-12.
//

#include <android/log.h>

#include "config.h"

#ifdef APP_DEBUG
  #define app_log(...) __android_log_print(ANDROID_LOG_DEBUG, "--==--", __VA_ARGS__)
#else
  #define app_log(...)
#endif
