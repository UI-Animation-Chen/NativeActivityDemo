//
// Created by czf on 19-11-4.
//

#ifndef NATIVEACTIVITYDEMO_TOUCHEVENTHANDLER_H
#define NATIVEACTIVITYDEMO_TOUCHEVENTHANDLER_H

#include <android/input.h>
#include <functional>

class TouchEventHandler {
public:
  TouchEventHandler() : oldX(-1), oldY(-1), moreThan2Fingers(false), oldTanDeg(0),
                        oldScaledX(0), oldScaledY(0), old2FingersDistance(0) {}

  virtual ~TouchEventHandler() {}

  void onTouchEvent(AInputEvent *event);

  void setOnTouchDown(std::function<void(float downX, float downY,
                                         float downMillis)> onTouchDownFunc);

  void setOnTouchMove(std::function<void(float deltaX, float deltaY, float currX, float currY,
                                         float currMillis, int fingers)> onTouchMoveFunc);

  void setOnTouchCancel(std::function<void(float cancelX, float cancelY,
                                           float cancelMillis)> onTouchCancelFunc);

  void setOnTouchUp(std::function<void(float upX, float upY, float upMillis)> onTouchUpFunc);

  void setOnScale(std::function<void(float scaledX, float scaledY, float scaledDinstance,
                                     float currMillis)> onTouchScaleFunc);

  void setOnRotate(std::function<void(float rotateDeg, float currMillis)> onTouchRotateFunc);

private:
  // callback functions
  std::function<void(float downX, float downY, float downMillis)> onTouchDownFunc;
  std::function<void(float deltaX, float deltaY, float currX, float currY,
                     float currMillis, int fingers)> onTouchMoveFunc;
  std::function<void(float cancelX, float cancelY, float cancelMillis)> onTouchCancelFunc;
  std::function<void(float upX, float upY, float upMillis)> onTouchUpFunc;
  std::function<void(float scaledX, float scaledY, float scaledDistance,
                     float currMillis)> onTouchScaleFunc;
  std::function<void(float rotateDeg, float currMillis)> onTouchRotateFunc;

  // private vars
  float oldX, oldY;
  bool moreThan2Fingers;

  float oldTanDeg;

  float oldScaledX, oldScaledY, old2FingersDistance;

  float getRotatedDegBetween2Events(AInputEvent *event);

  float getDeltaScaledXBetween2Events(AInputEvent *event);

  float getDeltaScaledYBetween2Events(AInputEvent *event);

  float getScaledDistanceBetween2Events(AInputEvent *event);
};

#endif //NATIVEACTIVITYDEMO_TOUCHEVENTHANDLER_H
