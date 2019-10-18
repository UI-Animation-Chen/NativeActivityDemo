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
#include <GLES3/gl32.h>

#include "app_log.h"
#include "GLESEngine.h"
#include "ShaderUtils.h"

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

  int32_t width;
  int32_t height;

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

void gl_init();

/**
 * Process the next main command.
 */
static void on_handle_cmd(struct android_app *app, int32_t cmd) {
  struct context *context = (struct context *) app->userData;
  switch (cmd) {
    case APP_CMD_SAVE_STATE:
      // The system has asked us to save our current state.  Do so.
      app_log("cmd -- save state\n");
      context->app->savedState = malloc(sizeof(struct saved_state));
      *((struct saved_state *) context->app->savedState) = context->state;
      context->app->savedStateSize = sizeof(struct saved_state);
      break;
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      app_log("cmd -- init window\n");
      if (context->app->window != NULL) {
        GLESEngine_init(context->app->window);
        GLfloat color[4] = {1, 1, 1, 1};
        GLESEngine_draw_frame(color, NULL);

        context->width = GLESEngine_get_width();
        context->height = GLESEngine_get_height();

        gl_init();
//        renderByANativeWindowAPI(app->window);
      }
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      app_log("cmd -- destroy window\n");
      GLESEngine_destroy();
      break;
    case APP_CMD_GAINED_FOCUS:
      // When our app gains focus, we start monitoring the accelerometer.
      app_log("cmd -- gained focus\n");
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
      app_log("cmd -- lost focus\n");
      // This is to avoid consuming battery while not being used.
//      if (context->accelerometerSensor != NULL) {
//        ASensorEventQueue_disableSensor(context->sensorEventQueue,
//                                        context->accelerometerSensor);
//      }
      // Also stop animating.
//      context->animating = 0;
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

/** ------------------------- */

static char *triangleVert = "#version 320 es\n"
                            "layout(location = 0) in vec4 vPosition;\n"
                            "void main() {\n"
                            "  gl_Position = vPosition;\n"
                            "}\n";

static char *triangleFrag = "#version 320 es\n"
                            "precision mediump float;\n"
                            "out vec4 fColor;\n"
                            "void main() {\n"
                            "  fColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                            "}\n";

GLuint vaos[1]; // vertex array objects
GLuint buffers[1];

//GLfloat triangles[6][2] = {
//    {0, 0.7F}, {-.5F, 0}, {.5F, 0},
//    {-.5F, 0}, {0, -.7F}, {.5F, 0}
//};

GLfloat triangles[] = {
    0.0f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
};

GLuint program;

void gl_init() {
//  glGenVertexArrays(1, vaos);
//  glBindVertexArray(vaos[0]);

//  glGenBuffers(1, buffers);
//  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//  glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

  program = linkShader(get_compiled_shader_vert(triangleVert),
                              get_compiled_shader_frag(triangleFrag));
}

void gl_render_triangle() {
//  glGenVertexArrays(1, vaos);
//  glBindVertexArray(vaos[0]);
  glUseProgram(program);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, triangles);
  glEnableVertexAttribArray(0);

  glDrawArrays(GL_TRIANGLES, 0, 3);
//  glFlush();
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

      if (app->window != NULL) {
//        renderByANativeWindowAPI(app->window);

        GLfloat factor = context.state.y / context.height;
        GLfloat color[4] = {0, factor, 0, 1};
        app_log("y/h: %f\n", factor);
        GLESEngine_draw_frame(color, gl_render_triangle);

//        gl_render_triangle();
      }
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
