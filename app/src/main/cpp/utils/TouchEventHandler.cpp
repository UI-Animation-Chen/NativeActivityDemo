//
// Created by czf on 19-11-4.
//

#include "TouchEventHandler.h"

void TouchEventHandler::onTouchEvent(AInputEvent *event) {
  int action = AMotionEvent_getAction(event);
  switch (action) {
    case AMOTION_EVENT_ACTION_DOWN:
      break;
    case AMOTION_EVENT_ACTION_POINTER_DOWN:
      break;
    case AMOTION_EVENT_ACTION_MOVE:
      break;
    case AMOTION_EVENT_ACTION_POINTER_UP:
      break;
    case AMOTION_EVENT_ACTION_UP:
      break;
    case AMOTION_EVENT_ACTION_CANCEL:
      break;
    default:
      break;
  }
}

void TouchEventHandler::setOnTouchDown(std::function<void(float downX, float downY,
  float downMills)> onTouchDownFunc) {
  this->onTouchDownFunc = onTouchDownFunc;
}

void TouchEventHandler::setOnTouchMove(std::function<void(float deltaX, float deltaY, float currX,
  float currY, float currMills)> onTouchMoveFunc) {
  this->onTouchMoveFunc = onTouchMoveFunc;
}

void TouchEventHandler::setOnTouchCancel(std::function<void(float cancelX, float cancelY,
  float cancelMills)> onTouchCancelFunc) {
  this->onTouchCancelFunc = onTouchCancelFunc;
}

void TouchEventHandler::setOnTouchUp(std::function<void(float upX, float upY,
  float upMillis)> onTouchUpFunc) {
  this->onTouchUpFunc = onTouchUpFunc;
}

void TouchEventHandler::setOnScale(std::function<void(float scaleX, float scaleY,
  float currMillis)> onTouchScaleFunc) {
  this->onTouchScaleFunc = onTouchScaleFunc;
}

void TouchEventHandler::setOnRotate(std::function<void(float rotateDeg,
  float currMillis)> onTouchRotateFunc) {
  this->onTouchRotateFunc = onTouchRotateFunc;
}
