/**
 * Native app entry file
 */

#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <errno.h>
#include <assert.h>

#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "app_log.h"

#include "GLESEngine.h"

/**
 * Our saved state data.
 */
struct saved_state {
  float angle;
  float x;
  float y;
};

/**
 * Shared state for our app.
 */
struct context {
  struct android_app *app;

  ASensorManager *sensorManager;
  const ASensor *accelerometerSensor;
  ASensorEventQueue *sensorEventQueue;

  int animating;
  struct saved_state state;
};

/**
 * Process the next input event.
 */
static int32_t on_handle_input(struct android_app *app, AInputEvent *event) {
  struct context *context = (struct context *) app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
//    context->animating = 1;
    context->state.x = AMotionEvent_getX(event, 0);
    context->state.y = AMotionEvent_getY(event, 0);
    app_log("x: %f, y: %f\n", context->state.x, context->state.y);
    return 1;
  }
  return 0;
}

static void renderByANativeWindowAPI(ANativeWindow *window) {
  ANativeWindow_Buffer buf;
  int result = ANativeWindow_lock(window, &buf, NULL);
  if (result == 0) {
    struct timespec ts;
    memset(&ts, 0, sizeof(ts));
    clock_gettime(CLOCK_MONOTONIC, &ts);
    app_log("ts.tv_sec: %ld, tv_nsec: %ld\n", ts.tv_sec, ts.tv_nsec);
    memset(buf.bits, 255, buf.stride * buf.height * sizeof(uint32_t));
    memset(&ts, 0, sizeof(ts));
    clock_gettime(CLOCK_MONOTONIC, &ts);
    app_log("ts.tv_sec: %ld, tv_nsec: %ld\n", ts.tv_sec, ts.tv_nsec);

    for (int i = 0; i < 500000; i++) {
      *((int32_t *)buf.bits + i + 400000) = 255;
    }

    ANativeWindow_unlockAndPost(window);
  } else {
    app_log("lock fail: %s\n", strerror(errno));
  }
}

/**
 * Process the next main command.
 */
static void on_handle_cmd(struct android_app *app, int32_t cmd) {
  struct context *context = (struct context *) app->userData;
  switch (cmd) {
    case APP_CMD_SAVE_STATE:
      // The system has asked us to save our current state.  Do so.
      context->app->savedState = malloc(sizeof(struct saved_state));
      *((struct saved_state *) context->app->savedState) = context->state;
      context->app->savedStateSize = sizeof(struct saved_state);
      break;
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      if (context->app->window != NULL) {
        GLESEngine_init(context->app->window);
        GLESEngine_draw_frame();

//          renderByANativeWindowAPI(app->window);
      }
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      GLESEngine_destroy();
      break;
    case APP_CMD_GAINED_FOCUS:
      // When our app gains focus, we start monitoring the accelerometer.
//      if (context->accelerometerSensor != NULL) {
//        ASensorEventQueue_enableSensor(context->sensorEventQueue,
//                                       context->accelerometerSensor);
//        // We'd like to get 60 events per second (in us).
//        ASensorEventQueue_setEventRate(context->sensorEventQueue,
//                                       context->accelerometerSensor,
//                                       (1000L / 60) * 1000);
//      }
      break;
    case APP_CMD_LOST_FOCUS:
      // When our app loses focus, we stop monitoring the accelerometer.
      // This is to avoid consuming battery while not being used.
//      if (context->accelerometerSensor != NULL) {
//        ASensorEventQueue_disableSensor(context->sensorEventQueue,
//                                        context->accelerometerSensor);
//      }
      // Also stop animating.
//      context->animating = 0;

      GLESEngine_destroy();
      break;
    default:
      break;
  }
}

/*
 * AcquireASensorManagerInstance(void)
 *    Workaround ASensorManager_getInstance() deprecation false alarm
 *    for Android-N and before, when compiling with NDK-r15
 */
#include <dlfcn.h>

ASensorManager *AcquireASensorManagerInstance(struct android_app *app) {

  if (!app)
    return NULL;

  typedef ASensorManager *(*PF_GETINSTANCEFORPACKAGE)(const char *);
  void *androidHandle = dlopen("libandroid.so", RTLD_NOW);
  PF_GETINSTANCEFORPACKAGE getInstanceForPackageFunc =
      (PF_GETINSTANCEFORPACKAGE) dlsym(androidHandle, "ASensorManager_getInstanceForPackage");
  if (getInstanceForPackageFunc) {
    JNIEnv *env = NULL;
    (*(app->activity->vm))->AttachCurrentThread(app->activity->vm, &env, NULL);

    jclass android_content_Context = (*env)->GetObjectClass(env, app->activity->clazz);
    jmethodID midGetPackageName = (*env)->GetMethodID(env, android_content_Context,
                                                      "getPackageName",
                                                      "()Ljava/lang/String;");
    jstring packageName = (jstring) (*env)->CallObjectMethod(env, app->activity->clazz,
                                                             midGetPackageName);

    const char *nativePackageName = (*env)->GetStringUTFChars(env, packageName, 0);
    ASensorManager *mgr = getInstanceForPackageFunc(nativePackageName);
    (*env)->ReleaseStringUTFChars(env, packageName, nativePackageName);
    (*(app->activity->vm))->DetachCurrentThread(app->activity->vm);
    if (mgr) {
      dlclose(androidHandle);
      return mgr;
    }
  }

  typedef ASensorManager *(*PF_GETINSTANCE)();
  PF_GETINSTANCE getInstanceFunc = (PF_GETINSTANCE) dlsym(androidHandle,
                                                          "ASensorManager_getInstance");
  // by all means at this point, ASensorManager_getInstance should be available
  assert(getInstanceFunc);
  dlclose(androidHandle);

  return getInstanceFunc();
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app *app) {
  struct context context;
  memset(&context, 0, sizeof(context));

  app->userData = &context;
  app->onAppCmd = on_handle_cmd;
  app->onInputEvent = on_handle_input;
  context.app = app;

  // Prepare to monitor accelerometer
//  context.sensorManager = AcquireASensorManagerInstance(app);
//  context.accelerometerSensor = ASensorManager_getDefaultSensor(context.sensorManager,
//                                                               ASENSOR_TYPE_ACCELEROMETER);
//  context.sensorEventQueue = ASensorManager_createEventQueue(context.sensorManager, app->looper,
//                                                            LOOPER_ID_USER, NULL, NULL);

  if (app->savedState != NULL) {
    // We are starting with a previous saved state; restore from it.
    context.state = *(struct saved_state *) app->savedState;
  }

  // loop waiting for stuff to do.

  while (1) {
    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source *source;

    // If not animating, we will block forever waiting for events.
    // If animating, we loop until all events are read, then continue
    // to draw the next frame of animation.
    while ((ident = ALooper_pollAll(/*context.animating ? 0 : */-1, NULL, &events,
                                    (void **) &source)) >= 0) {

      // Process this event.
      if (source != NULL) {
        source->process(app, source);
      }

      // If a sensor has data, process it now.
//      if (ident == LOOPER_ID_USER) {
//        if (context.accelerometerSensor != NULL) {
//          ASensorEvent event;
//          while (ASensorEventQueue_getEvents(context.sensorEventQueue, &event, 1) > 0) {
//            //app_log("accelerometer: x=%f y=%f z=%f\n", event.acceleration.x,
//            //       event.acceleration.y, event.acceleration.z);
//          }
//        }
//      }

      // Check if we are exiting.
      if (app->destroyRequested != 0) {
        GLESEngine_destroy();
        return;
      }

//      if (app->window != NULL) {
//        renderByANativeWindowAPI(app->window);
//      }
    }

//    if (context.animating) {
//      // Done with events; draw next animation frame.
//      context.state.angle += .01F;
//      if (context.state.angle > 1) {
//        context.state.angle = 0;
//      }
//
//      // Drawing is throttled to the screen update rate, so there
//      // is no need to do timing here.
//        GLESEngine_draw_frame();
//    }
  }
}
