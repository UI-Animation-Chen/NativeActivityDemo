//
// Created by bytedance on 2022/3/27.
//

#include "Utils.h"
#include <cstring>
#include <ctime>
#include <cstdio>
#include <cstdlib>

long Utils::getCurrTimeUS() {
    struct timeval ts = {
            .tv_sec = 0,
            .tv_usec = 0
    };
    gettimeofday(&ts, nullptr);
    return ts.tv_usec + ts.tv_sec * 1000000;
}

// fixedNum支持0-9
float Utils::toFixedFloat(float origin, int fixedNum) {
    char formatStr[4+1] = "%.xf";
    char fixedNumChar = (char)(fixedNum + 48);
    memcpy(formatStr+2, &fixedNumChar, 1);

    char fixedFloatStr[10]; // 10 is enough
    memset(fixedFloatStr, 0, sizeof(fixedFloatStr));
    sprintf(fixedFloatStr, formatStr, origin);
    return strtof(fixedFloatStr, nullptr);
}

