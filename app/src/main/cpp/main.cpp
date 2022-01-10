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
#include "gles/GLESEngine.h"
#include "view/Triangles.h"
#include "view/Shape.h"
#include "view/Cube.h"
#include "utils/CoordinatesUtils.h"
#include "utils/TouchEventHandler.h"
#include "utils/AndroidAssetUtils.h"
#include "view/ObjModel.h"

static const float NS_2_S = 1.0f / 1000000000.0f; // 将纳秒转成秒
static const float DEG_2_RADIAN = (float) M_PI / 180.0f;

static const int shape_len = 2;
static Shape *pShape[shape_len] = {0};

static TouchEventHandler *touchEventHandler = NULL;

static float transX = 0, transY = 0, transZ = 0;
static float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
static float rotateXradian = 0, rotateYradian = 0, rotateZradian = 0;

static float gyro_event_ts_s_old = -1;

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
    const ASensor *gyroscopeSensor;
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
//        context->animating = 1;
        context->state.x = AMotionEvent_getX(event, 0);
        context->state.y = AMotionEvent_getY(event, 0);

        if (touchEventHandler != NULL) touchEventHandler->onTouchEvent(event);
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
            if (i % 2 == 0)
                *((int32_t *) buf.bits + i + 400000) = 255 << 8; // ABGR各占8位，共32位
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
                context->width = GLESEngine_get_width();
                context->height = GLESEngine_get_height();

                CoordinatesUtils::screenW = context->width;
                CoordinatesUtils::screenH = context->height;

                AndroidAssetUtils::init(app->activity->assetManager);

                BaseShader::getSingletonProgram();
                TextureUtils::loadSimpleTexture("cocacola.png");

                // 深度测试的基准,注意1.0代表从近裁剪面到远裁剪面 这一段范围！！并不是指Z轴的1个单位
                // 深度，是一个normolized的值，范围是 0-1(不是z轴坐标)，对应Z轴是从近裁剪面到远裁剪面
                // 所以这里的 1.0f 指的是，深度缓冲区中远裁剪面以内全部清除
                glClearDepthf(1.0f);
                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                if (transX == 0) transX = context->width / 2;
                if (transY == 0) transY = context->height / 2;

                // blend跟物体渲染顺序有关，需要后渲染半透明物体
//                pShape[0] = new Cube();
//                pShape[1] = new Triangles();
                pShape[0] = new ObjModel();
                for (int i = 0; i < shape_len; i++) {
                    if (pShape[i]) {
                        pShape[i]->scale(scaleX, scaleY, scaleZ);
                        pShape[i]->rotate(rotateXradian, rotateYradian, -rotateZradian);
                        pShape[i]->move(transX, transY, transZ);
                        pShape[i]->draw();
                    }
                }

//                renderByANativeWindowAPI(app->window);

                GLESEngine_refresh();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            app_log("cmd -- destroy window\n");
            for (int i = 0; i < shape_len; i++) {
                if (pShape[i]) {
                    delete pShape[i];
                }
            }
            BaseShader::deleteSingletonProgram();
            TextureUtils::deleteSimpleTexture();
            GLESEngine_destroy();
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            app_log("cmd -- gained focus\n");
            if (context->gyroscopeSensor != NULL) {
                ASensorEventQueue_enableSensor(context->sensorEventQueue,
                                               context->gyroscopeSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(context->sensorEventQueue,
                                               context->gyroscopeSensor,
                                               (1000L / 60) * 1000); // 16667微秒，即16.667毫秒
                gyro_event_ts_s_old = -1;
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            app_log("cmd -- lost focus\n");
            // This is to avoid consuming battery while not being used.
            if (context->gyroscopeSensor != NULL) {
                ASensorEventQueue_disableSensor(context->sensorEventQueue,
                                                context->gyroscopeSensor);
            }
            // Also stop animating.
            context->animating = 0;
            break;
        default:
            break;
    }
}

/*
 * AcquireASensorManagerInstance(void)
 * Workaround ASensorManager_getInstance() deprecation false alarm
 * for Android-N and before, when compiling with NDK-r15
 */
#include <dlfcn.h>
#include <EGL/egl.h>

ASensorManager *AcquireASensorManagerInstance(struct android_app *app) {
    if (!app)
        return NULL;

    typedef ASensorManager *(*PF_GETINSTANCEFORPACKAGE)(const char *);
    void *androidHandle = dlopen("libandroid.so", RTLD_NOW);
    PF_GETINSTANCEFORPACKAGE getInstanceForPackageFunc =
            (PF_GETINSTANCEFORPACKAGE) dlsym(androidHandle, "ASensorManager_getInstanceForPackage");
    if (getInstanceForPackageFunc) {
        JNIEnv *env = NULL;
        app->activity->vm->AttachCurrentThread(&env, NULL);

        jclass android_content_Context = env->GetObjectClass(app->activity->clazz);
        jmethodID midGetPackageName = env->GetMethodID(android_content_Context, "getPackageName",
                                                       "()Ljava/lang/String;");
        jstring packageName = (jstring) env->CallObjectMethod(app->activity->clazz,
                                                              midGetPackageName);

        const char *nativePackageName = env->GetStringUTFChars(packageName, 0);
        ASensorManager *mgr = getInstanceForPackageFunc(nativePackageName);
        env->ReleaseStringUTFChars(packageName, nativePackageName);
        app->activity->vm->DetachCurrentThread();
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

void initTouchEventHandlerCallbacks() {
    touchEventHandler->setOnTouchDown([](float downX, float downY, float downMillis) {
    });
    touchEventHandler->setOnTouchMove([](float deltaX, float deltaY, float currX, float currY,
                                         float currMillis, int fingers) {
        if (fingers == 1) {
            rotateXradian += (deltaY * M_PI / CoordinatesUtils::screenH); // 划过屏幕为一个PI
            rotateYradian += (deltaX * M_PI / CoordinatesUtils::screenW);
        } else {
            transX += deltaX;
            transY += deltaY;
        }
        for (int i = 0; i < shape_len; i++) {
            if (pShape[i]) {
                if (fingers == 1) {
                    pShape[i]->rotate(rotateXradian, rotateYradian, -rotateZradian);
                } else {
                    pShape[i]->move(transX, transY, transZ);
                }
            }
        }
    });
    touchEventHandler->setOnTouchCancel([](float cancelX, float cancelY, float cancelMillis) {
        app_log("cancel\n");
    });
    touchEventHandler->setOnTouchUp([](float upX, float upY, float upMillis) {
    });
    touchEventHandler->setOnScale(
            [](float scaleX1, float scaleY1, float scaleDistance, float currMillis) {
//                transZ += (scaleDistance / CoordinatesUtils::screenH);
                float scale = scaleDistance / CoordinatesUtils::screenH;
                for (int i = 0; i < shape_len; i++) {
                    if (pShape[i]) {
                        pShape[i]->scale(scaleX += scale, scaleY += scale, scaleZ += scale);
                    }
                }
            });
    touchEventHandler->setOnRotate([](float rotateDeg, float currMillis) {
        rotateZradian += rotateDeg * DEG_2_RADIAN;
        for (int i = 0; i < shape_len; i++) {
            if (pShape[i]) {
                pShape[i]->rotate(rotateXradian, rotateYradian, -rotateZradian);
            }
        }
    });
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

    touchEventHandler = new TouchEventHandler();
    initTouchEventHandlerCallbacks();

    // Prepare to monitor accelerometer
    context.sensorManager = AcquireASensorManagerInstance(app);
    context.gyroscopeSensor = ASensorManager_getDefaultSensor(context.sensorManager,
                                                              ASENSOR_TYPE_GYROSCOPE);
    context.sensorEventQueue = ASensorManager_createEventQueue(context.sensorManager, app->looper,
                                                               LOOPER_ID_USER, NULL, NULL);

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
            if (ident == LOOPER_ID_USER) {
                if (context.gyroscopeSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(context.sensorEventQueue, &event, 1) > 0) {
                        float event_ts_s_now = event.timestamp * NS_2_S;
                        if (gyro_event_ts_s_old != -1) {
                            float dT = event_ts_s_now - gyro_event_ts_s_old;
                            rotateXradian -= event.data[0] * dT; // gyro返回的值单位是弧度/s
                            rotateYradian -= event.data[1] * dT;
                            rotateZradian += event.data[2] * dT;
                            for (int i = 0; i < shape_len; i++) {
                                if (pShape[i]) {
                                    pShape[i]->rotate(rotateXradian, rotateYradian, -rotateZradian);
                                }
                            }
                        }
                        gyro_event_ts_s_old = event_ts_s_now;
                    }
                }
            }

            // Check if we are exiting.
            if (app->destroyRequested != 0) {
                app_log("destroy requested\n");
                return;
            }

            if (app->window != NULL) {
//                renderByANativeWindowAPI(app->window);

                glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
                glClearDepthf(1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                for (int i = 0; i < shape_len; i++) {
                    if (pShape[i]) {
                        pShape[i]->draw();
                    }
                }

                GLESEngine_refresh();
            }
        }

//        if (context.animating) {
//          // Done with events; draw next animation frame.
//          context.state.angle += .01F;
//          if (context.state.angle > 1) {
//            context.state.angle = 0;
//          }
//
//          // Drawing is throttled to the screen update rate, so there
//          // is no need to do timing here.
//            GLESEngine_refresh();
//        }
    }
}
