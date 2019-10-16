/**
 * Native app entry file
 */

#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <errno.h>
#include <assert.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "app_log.h"

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
struct engine {
  struct android_app *app;

  ASensorManager *sensorManager;
  const ASensor *accelerometerSensor;
  ASensorEventQueue *sensorEventQueue;

  int animating;
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  int32_t width;
  int32_t height;
  struct saved_state state;
};

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine *engine) {
  // initialize OpenGL ES and EGL

  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(display, 0, 0);

  /*
   * Here specify the attributes of the desired configuration.
   * Below, we select an EGLConfig with at least 8 bits per color
   * component compatible with on-screen windows
   */
  const EGLint attribs[] = {
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_RED_SIZE, 8,
      EGL_NONE
  };
  EGLint numConfigs = 0;
  EGLConfig config = NULL;

  /* Here, the application chooses the configuration it desires.
   * find the best match if possible, otherwise use the very first one
   */
  eglChooseConfig(display, attribs, NULL, 0, &numConfigs);
  EGLConfig supportedConfigs[numConfigs];
  eglChooseConfig(display, attribs, supportedConfigs, numConfigs, &numConfigs);
  assert(numConfigs);
  int i = 0;
  for (; i < numConfigs; i++) {
    EGLConfig cfg = supportedConfigs[i];
    EGLint r, g, b, d;
    if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
        eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
        eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
        eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
        r == 8 && g == 8 && b == 8 && d == 0) {

      config = supportedConfigs[i];
      break;
    }
  }
  if (i == numConfigs) {
    config = supportedConfigs[0];
  }

  /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
   * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
   * As soon as we picked a EGLConfig, we can safely reconfigure the
   * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
  EGLint format;
  eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

  EGLSurface surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
  EGLContext context = eglCreateContext(display, config, NULL, NULL);

  if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    app_log("Unable to eglMakeCurrent\n");
    return -1;
  }

  EGLint w, h;
  eglQuerySurface(display, surface, EGL_WIDTH, &w);
  eglQuerySurface(display, surface, EGL_HEIGHT, &h);

  engine->display = display;
  engine->context = context;
  engine->surface = surface;
  engine->width = w;
  engine->height = h;
  engine->state.angle = 0;

  // Check openGL on the system
  GLenum opengl_info[] = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
  for (int j = 0; j < 4; j++) {
    const GLubyte *info = glGetString(opengl_info[j]);
    app_log("OpenGL Info[%d]: %s\n", j, info);
  }
  // Initialize GL state.
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);
  glDisable(GL_DEPTH_TEST);

  return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine *engine) {
  if (engine->display == NULL) {
    // No display.
    return;
  }

  // Just fill the screen with a color.
  glClearColor((engine->state.x) / engine->width, engine->state.angle,
               (engine->state.y) / engine->height, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine *engine) {
  if (engine->display != EGL_NO_DISPLAY) {
    eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (engine->context != EGL_NO_CONTEXT) {
      eglDestroyContext(engine->display, engine->context);
    }
    if (engine->surface != EGL_NO_SURFACE) {
      eglDestroySurface(engine->display, engine->surface);
    }
    eglTerminate(engine->display);
  }
  engine->animating = 0;
  engine->display = EGL_NO_DISPLAY;
  engine->context = EGL_NO_CONTEXT;
  engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app *app, AInputEvent *event) {
  struct engine *engine = (struct engine *) app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
//    engine->animating = 1;
    engine->state.x = AMotionEvent_getX(event, 0);
    engine->state.y = AMotionEvent_getY(event, 0);
    app_log("x: %f, y: %f\n", engine->state.x, engine->state.y);
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
static void engine_handle_cmd(struct android_app *app, int32_t cmd) {
  struct engine *engine = (struct engine *) app->userData;
  switch (cmd) {
    case APP_CMD_SAVE_STATE:
      // The system has asked us to save our current state.  Do so.
      engine->app->savedState = malloc(sizeof(struct saved_state));
      *((struct saved_state *) engine->app->savedState) = engine->state;
      engine->app->savedStateSize = sizeof(struct saved_state);
      break;
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      if (engine->app->window != NULL) {
//        engine_init_display(engine);
//        engine_draw_frame(engine);

          renderByANativeWindowAPI(app->window);
      }
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      //engine_term_display(engine);
      break;
    case APP_CMD_GAINED_FOCUS:
      // When our app gains focus, we start monitoring the accelerometer.
//      if (engine->accelerometerSensor != NULL) {
//        ASensorEventQueue_enableSensor(engine->sensorEventQueue,
//                                       engine->accelerometerSensor);
//        // We'd like to get 60 events per second (in us).
//        ASensorEventQueue_setEventRate(engine->sensorEventQueue,
//                                       engine->accelerometerSensor,
//                                       (1000L / 60) * 1000);
//      }
      break;
    case APP_CMD_LOST_FOCUS:
      // When our app loses focus, we stop monitoring the accelerometer.
      // This is to avoid consuming battery while not being used.
//      if (engine->accelerometerSensor != NULL) {
//        ASensorEventQueue_disableSensor(engine->sensorEventQueue,
//                                        engine->accelerometerSensor);
//      }
      // Also stop animating.
//      engine->animating = 0;
//      engine_draw_frame(engine);
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
  struct engine engine;
  memset(&engine, 0, sizeof(engine));

  app->userData = &engine;
  app->onAppCmd = engine_handle_cmd;
  app->onInputEvent = engine_handle_input;
  engine.app = app;

  // Prepare to monitor accelerometer
//  engine.sensorManager = AcquireASensorManagerInstance(app);
//  engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
//                                                               ASENSOR_TYPE_ACCELEROMETER);
//  engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager, app->looper,
//                                                            LOOPER_ID_USER, NULL, NULL);

  if (app->savedState != NULL) {
    // We are starting with a previous saved state; restore from it.
    engine.state = *(struct saved_state *) app->savedState;
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
    while ((ident = ALooper_pollAll(/*engine.animating ? 0 : */-1, NULL, &events,
                                    (void **) &source)) >= 0) {

      // Process this event.
      if (source != NULL) {
        source->process(app, source);
      }

      // If a sensor has data, process it now.
//      if (ident == LOOPER_ID_USER) {
//        if (engine.accelerometerSensor != NULL) {
//          ASensorEvent event;
//          while (ASensorEventQueue_getEvents(engine.sensorEventQueue, &event, 1) > 0) {
//            //app_log("accelerometer: x=%f y=%f z=%f\n", event.acceleration.x,
//            //       event.acceleration.y, event.acceleration.z);
//          }
//        }
//      }

      // Check if we are exiting.
      if (app->destroyRequested != 0) {
        //engine_term_display(&engine);
        return;
      }

      if (app->window != NULL) {
        renderByANativeWindowAPI(app->window);
      }
    }

//    if (engine.animating) {
//      // Done with events; draw next animation frame.
//      engine.state.angle += .01F;
//      if (engine.state.angle > 1) {
//        engine.state.angle = 0;
//      }
//
//      // Drawing is throttled to the screen update rate, so there
//      // is no need to do timing here.
//      engine_draw_frame(&engine);
//    }
  }
}
