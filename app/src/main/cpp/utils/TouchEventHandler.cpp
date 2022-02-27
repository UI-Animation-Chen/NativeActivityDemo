//
// 简单的手势处理，只考虑2指。
// Created by czf on 19-11-4.
//

#include "TouchEventHandler.h"
#include <math.h>

#define MAX_DEGREES_IN_TWO_MOVE_EVENTS 180.0f
#define REFERENCE_DEGREES (360.0f - MAX_DEGREES_IN_TWO_MOVE_EVENTS)  // 权衡考虑，定为180
#define RADIAN_TO_DEGREE ((float) (180.0f / M_PI))
#define NS_2_MS (1.0f / 1000000.0f)

/**
 * 面向屏幕看，顺时针转为正，逆时针转为负。
 * Math.atan2(x, y): 左侧的-180和180交界需要额外处理，其余区域都是连续的。
 *
 *              | y
 *              | -90
 *          ----|----
 *  -180  /     |     \   0
 *  -----|------+------|-----> x
 *   180  \     |     /   0
 *          ----|----
 *              | 90
 * 如果连续两次move事件的转动穿过了左侧-180到180，分2种情况：
 *  实际转动角度超过180，则计算的结果是360减去该角度，即小于实际转动效果；
 *      比如逆时针从-80转到80，实际转动为200，但计算结果是80 - (-80) = 160，小于实际效果。
 *  实际转动角度不超过180，则忽略这次转动。
 *      比如逆时针从-100转到100，实际转动为160，但计算结果为200，将返回0，忽略这次转动。
 */
inline float TouchEventHandler::getRotatedDegBetween2Events(AInputEvent *event) {
    float spanX = AMotionEvent_getX(event, 1) - AMotionEvent_getX(event, 0);
    float spanY = AMotionEvent_getY(event, 1) - AMotionEvent_getY(event, 0);
    float tanDeg = atan2(spanY, spanX) * RADIAN_TO_DEGREE;
    if (oldTanDeg == 0
        || (tanDeg >= 0 && oldTanDeg <= 0 && tanDeg - oldTanDeg > REFERENCE_DEGREES)
        || (tanDeg <= 0 && oldTanDeg >= 0 && oldTanDeg - tanDeg > REFERENCE_DEGREES)) {

        oldTanDeg = tanDeg;
        return 0; // 忽略这次转动（如果是转动的话）。
    } else {
        float deltaDeg = tanDeg - oldTanDeg;
        oldTanDeg = tanDeg;
        return deltaDeg;
    }
}

inline float TouchEventHandler::getDeltaScaledXBetween2Events(AInputEvent *event) {
    float newScaledX = abs(AMotionEvent_getX(event, 1) - AMotionEvent_getX(event, 0));
    if (oldScaledX == 0) {
        oldScaledX = newScaledX;
        return 0;
    } else {
        float deltaScaledX = newScaledX - oldScaledX;
        oldScaledX = newScaledX;
        return deltaScaledX;
    }
}

inline float TouchEventHandler::getDeltaScaledYBetween2Events(AInputEvent *event) {
    float newScaledY = abs(AMotionEvent_getY(event, 1) - AMotionEvent_getY(event, 0));
    if (oldScaledY == 0) {
        oldScaledY = newScaledY;
        return 0;
    } else {
        float deltaScaledY = newScaledY - oldScaledY;
        oldScaledY = newScaledY;
        return deltaScaledY;
    }
}

inline float TouchEventHandler::getScaledDistanceBetween2Events(AInputEvent *event) {
    float newScaledX = AMotionEvent_getX(event, 1) - AMotionEvent_getX(event, 0),
            newScaledY = AMotionEvent_getY(event, 1) - AMotionEvent_getY(event, 0);
    float new2FingerDistance = sqrt((newScaledX * newScaledX) + (newScaledY * newScaledY));
    if (old2FingersDistance == 0) {
        old2FingersDistance = new2FingerDistance;
        return 0;
    } else {
        float deltaDistance = new2FingerDistance - old2FingersDistance;
        old2FingersDistance = new2FingerDistance;
        return deltaDistance;
    }
}

void TouchEventHandler::resetOldValues() {
    oldScaledX = 0;
    oldScaledY = 0;
    oldTanDeg = 0;
    old2FingersDistance = 0;
}

// 关于java.lang.System.nanoTime()，表示从虚拟机实例开始运行的时延值。跨度需小于292年（2的63次方纳秒）。
void TouchEventHandler::onTouchEvent(AInputEvent *event) {
    int action = AMotionEvent_getAction(event);
    switch (action & AMOTION_EVENT_ACTION_MASK) {
        case AMOTION_EVENT_ACTION_DOWN: {
            resetOldValues();
            oldX = AMotionEvent_getX(event, 0);
            oldY = AMotionEvent_getY(event, 0);
            if (onTouchDownFunc)
                onTouchDownFunc(oldX, oldX, AMotionEvent_getEventTime(event) * NS_2_MS);
            break;
        }
        case AMOTION_EVENT_ACTION_POINTER_DOWN: {
//            int pointerIndex = action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK;
//            app_log("pointer down -- %d\n", pointerIndex >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);

            // 在pointer_down事件之后，手指数。
            if (!moreThan2Fingers && AMotionEvent_getPointerCount(event) > 2) {
                if (onTouchCancelFunc)
                    onTouchCancelFunc(oldX, oldY, AMotionEvent_getEventTime(event) * NS_2_MS);
                moreThan2Fingers = true;
                return;
            } else {
                resetOldValues(); // 防止双指触控时，一指抬起再按下导致跳动。
                oldX = (AMotionEvent_getX(event, 0) + AMotionEvent_getX(event, 1)) / 2.0f;
                oldY = (AMotionEvent_getY(event, 0) + AMotionEvent_getY(event, 1)) / 2.0f;
            }
            break;
        }
        case AMOTION_EVENT_ACTION_MOVE: {
            if (moreThan2Fingers) return;

            float newX, newY;
            int fingers;
            if ((fingers = AMotionEvent_getPointerCount(event)) == 2) {
                // handle rotate
                float currDeltaRotatedDeg = getRotatedDegBetween2Events(event);
                if (onTouchRotateFunc)
                    onTouchRotateFunc(currDeltaRotatedDeg, AMotionEvent_getEventTime(event) * NS_2_MS);
                // handle scale
                float deltaScaledX = getDeltaScaledXBetween2Events(event);
                float deltaScaledY = getDeltaScaledYBetween2Events(event);
                float deltaScaledDistance = getScaledDistanceBetween2Events(event);
                if (onTouchScaleFunc)
                    onTouchScaleFunc(deltaScaledX, deltaScaledY, deltaScaledDistance,
                                     AMotionEvent_getEventTime(event) * NS_2_MS);

                newX = (AMotionEvent_getX(event, 0) + AMotionEvent_getX(event, 1)) / 2.0f;
                newY = (AMotionEvent_getY(event, 0) + AMotionEvent_getY(event, 1)) / 2.0f;
            } else {
                newX = AMotionEvent_getX(event, 0);
                newY = AMotionEvent_getY(event, 0);
            }

            float currDeltaMovedX = newX - oldX;
            float currDeltaMovedY = newY - oldY;
            oldX = newX;
            oldY = newY;

            if (onTouchMoveFunc)
                onTouchMoveFunc(currDeltaMovedX, currDeltaMovedY, newX, newY,
                                AMotionEvent_getEventTime(event) * NS_2_MS, fingers);
            break;
        }
        case AMOTION_EVENT_ACTION_POINTER_UP: {
            // 在pointer_up分支获取手指数，是pointer_up事件之前的手指数。
            if (moreThan2Fingers) return;

            int pointerIndex = action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK;
            pointerIndex >>= AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            if (pointerIndex == 1) {
                oldX = AMotionEvent_getX(event, 0);
                oldY = AMotionEvent_getY(event, 0);
            } else { // pointerIndex == 0
                oldX = AMotionEvent_getX(event, 1);
                oldY = AMotionEvent_getY(event, 1);
            }
            break;
        }
        case AMOTION_EVENT_ACTION_CANCEL: {
            if (!moreThan2Fingers && onTouchCancelFunc)
                onTouchCancelFunc(oldX, oldY, AMotionEvent_getEventTime(event) * NS_2_MS);
            break;
        }
        case AMOTION_EVENT_ACTION_UP: {
            if (!moreThan2Fingers && onTouchUpFunc)
                onTouchUpFunc(oldX, oldX, AMotionEvent_getEventTime(event) * NS_2_MS);
            moreThan2Fingers = false;
            break;
        }
        default:
            break;
    }
}

void TouchEventHandler::setOnTouchDown(std::function<void(float downX, float downY,
                                                          float downMillis)> onTouchDownFunc) {
    this->onTouchDownFunc = onTouchDownFunc;
}

void TouchEventHandler::setOnTouchMove(std::function<void(float deltaX, float deltaY, float currX,
                                                          float currY, float currMillis,
                                                          int fingers)> onTouchMoveFunc) {
    this->onTouchMoveFunc = onTouchMoveFunc;
}

void TouchEventHandler::setOnTouchCancel(std::function<void(float cancelX, float cancelY,
                                                            float cancelMillis)> onTouchCancelFunc) {
    this->onTouchCancelFunc = onTouchCancelFunc;
}

void TouchEventHandler::setOnTouchUp(std::function<void(float upX, float upY,
                                                        float upMillis)> onTouchUpFunc) {
    this->onTouchUpFunc = onTouchUpFunc;
}

void
TouchEventHandler::setOnScale(std::function<void(float scaledX, float scaledY, float scaledDistance,
                                                 float currMillis)> onTouchScaleFunc) {
    this->onTouchScaleFunc = onTouchScaleFunc;
}

void TouchEventHandler::setOnRotate(std::function<void(float rotateDeg,
                                                       float currMillis)> onTouchRotateFunc) {
    this->onTouchRotateFunc = onTouchRotateFunc;
}
