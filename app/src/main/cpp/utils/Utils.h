//
// Created by bytedance on 2022/3/27.
//

#ifndef NATIVEACTIVITYDEMO_UTILS_H
#define NATIVEACTIVITYDEMO_UTILS_H


class Utils {
public:
    static long getCurrTimeUS();
    // fixedNum支持0-9
    static float toFixedFloat(float origin, int fixedNum);
};


#endif //NATIVEACTIVITYDEMO_UTILS_H
