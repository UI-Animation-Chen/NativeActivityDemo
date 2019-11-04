//
// Created by czf on 19-11-4.
//

#ifndef NATIVEACTIVITYDEMO_TOUCHEVENTHANDLER_H
#define NATIVEACTIVITYDEMO_TOUCHEVENTHANDLER_H

#include <android/input.h>
#include <functional>

class TouchEventHandler {
public:
  TouchEventHandler(): downX(-1), downY(-1) {}
  virtual ~TouchEventHandler() {}

  void onTouchEvent(AInputEvent *event);

  void setOnTouchDown(std::function<void(float downX, float downY,
    float downMills)> onTouchDownFunc);
  void setOnTouchMove(std::function<void(float deltaX, float deltaY, float currX, float currY,
    float currMills)> onTouchMoveFunc);
  void setOnTouchCancel(std::function<void(float cancelX, float cancelY,
    float cancelMills)> onTouchCancelFunc);
  void setOnTouchUp(std::function<void(float upX, float upY, float upMillis)> onTouchUpFunc);
  void setOnScale(std::function<void(float scaleX, float scaleY,
    float currMillis)> onTouchScaleFunc);
  void setOnRotate(std::function<void(float rotateDeg, float currMillis)> onTouchRotateFunc);

private:
  // callback functions
  std::function<void(float downX, float downY, float downMills)> onTouchDownFunc;
  std::function<void(float deltaX, float deltaY, float currX, float currY,
    float currMills)> onTouchMoveFunc;
  std::function<void(float cancelX, float cancelY, float cancelMills)> onTouchCancelFunc;
  std::function<void(float upX, float upY, float upMillis)> onTouchUpFunc;
  std::function<void(float scaleX, float scaleY, float currMillis)> onTouchScaleFunc;
  std::function<void(float rotateDeg, float currMillis)> onTouchRotateFunc;

  // private vars
  float downX, downY;
};

#endif //NATIVEACTIVITYDEMO_TOUCHEVENTHANDLER_H
