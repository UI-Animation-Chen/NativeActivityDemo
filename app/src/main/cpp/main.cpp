/**
 * Native app entry file
 */

#include <memory.h>
#include <cstdlib>
#include <cstring>
#include <jni.h>
#include <cerrno>
#include <cassert>

#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <GLES3/gl32.h>
#include <vector>

#include "app_log.h"
#include "gles/GLESEngine.h"
#include "view/Triangles.h"
#include "view/Shape.h"
#include "view/Cube.h"
#include "utils/CoordinatesUtils.h"
#include "utils/TouchEventHandler.h"
#include "utils/AndroidAssetUtils.h"
#include "view/ObjModel.h"
#include "view/SkyBox.h"

static const float NS_2_S = 1.0f / 1000000000.0f; // 将纳秒转成秒
static const float DEG_2_RADIAN = (float) M_PI / 180.0f;

static std::vector<Shape *> pShape;

static TouchEventHandler *touchEventHandler = NULL;

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
                CoordinatesUtils::screenS = context->width > context->height ? context->height : context->width;
                CoordinatesUtils::screenL = context->width > context->height ? context->width : context->height;
                CoordinatesUtils::glesViewportSize = GLESEngine_get_viewport_size();

                AndroidAssetUtils::init(app->activity->assetManager);

                BaseShader::getSingletonProgram();
                TextureUtils::loadSimpleTexture(); // 加载一些纯色的纹理，当颜色用

                // 深度测试的基准,注意1.0代表从近裁剪面到远裁剪面 这一段范围！！并不是指Z轴的1个单位
                // 深度，是一个normalized的值，范围是 0-1(不是z轴坐标)，对应Z轴是从近裁剪面到远裁剪面
                // 所以这里的 1.0f 指的是，深度缓冲区中远裁剪面以内全部清除
                glClearDepthf(1.0f);
                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // blend跟物体渲染顺序有关，需要后渲染半透明物体
//                pShape[0] = new Cube();
//                pShape[0] = new Triangles();
                auto mountain = new ObjModel("blenderObjs/mountain.png", "mountain.png");
                mountain->scaleBy(9, 1.5, 9); // 长宽放大10倍
                pShape.push_back(mountain);
                auto tower = new ObjModel("blenderObjs/tower.png", "tower.png");
                tower->moveBy(5.66, 3.51, -17.21);
                pShape.push_back(tower);
                auto moodhouse = new ObjModel("blenderObjs/moodhouse.png", "moodhouse.png");
                moodhouse->moveBy(0, mountain->getMapHeight(0, 18), 18);
                moodhouse->rotateBy(0, 0.5, 0);
                moodhouse->scaleBy(-0.8, -0.8, -0.8);
                pShape.push_back(moodhouse);
                auto moon = new ObjModel("blenderObjs/moon.png", "moon.png");
                moon->moveBy(12, 12, 30);
                pShape.push_back(moon);
                auto skybox = new SkyBox();
                skybox->scaleBy(40.0f, 40.0f, 40.0f);
                pShape.push_back(skybox);
                auto monkey = new ObjModel("blenderObjs/monkey.png", "brown.png");
                monkey->moveBy(0, 0.294f, 0); // 模型的-y为-0.98
                monkey->rotateBy(0, 3.14f, 0);
                monkey->scaleBy(-0.7f, -0.7f, -0.7f); // 缩小为原来的3/10
                pShape.push_back(monkey);
                // 测试高度准确性
//                auto cocacola = new ObjModel("blenderObjs/cocacola.png", "cocacola.png");
//                cocacola->moveBy(0, 0.24656f, 0); // 模型的-y为-1.232813
//                cocacola->scaleBy(-0.95f, -0.8f, -0.95f); // 高缩小为原来的2/10
//                pShape.push_back(cocacola);

                GLfloat initHeight = mountain->getMapHeight(0, 0);
                for (int i = 0; i < pShape.size(); i++) {
                    if (pShape[i]) {
                        if (i < pShape.size() - 1) {
                            pShape[i]->worldMoveYTo(initHeight);
                        }
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
            for (int i = 0; i < pShape.size(); i++) {
                if (pShape[i]) {
                    delete pShape[i];
                }
            }
            pShape.clear();

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
        float distance2radianFactor = M_PI / CoordinatesUtils::screenS; // 划过屏幕短边为一个PI，横竖一致，符合操作常理
        float rotateXradian = (float)(deltaY * distance2radianFactor);
        float rotateYradian = (float)(deltaX * distance2radianFactor);
        float transX = CoordinatesUtils::android2gles_distance(deltaX);
        float transY = CoordinatesUtils::android2gles_distance(deltaY);
        GLfloat height = 0;
        for (int i = 0; i < pShape.size()-1; i++) {
            if (pShape[i]) {
                if (fingers == 1) {
                    // 透视模式下乘5，视角是60度，观察者距离是10，感觉是10的一半
                    pShape[i]->worldMoveBy(transX*5, 0, -transY*5);

                    if (height == 0) {
                        GLfloat transXYZ[3];
                        pShape[0]->getTranslate(transXYZ);
                        height = pShape[0]->getMapHeight(transXYZ[0], transXYZ[2]);
//                        app_log("map location: x: %f, z: %f, y: %f, height: %f\n", transXYZ[0], transXYZ[2], transXYZ[1], height);
                    }
                    pShape[i]->worldMoveYTo(height);
                } else {
                    if (abs(deltaX) > abs(deltaY)) {
                        pShape[i]->worldRotateBy(0, -rotateYradian, 0); // 对于矩阵变换来说，轴正向朝向自己，顺时针转为正
                    } else {
                        pShape[i]->worldMoveBy(0, -transY*5, 0);
                    }
                }
            }
        }
        if (fingers == 1) {
            float directionYradian = atan2(deltaY, deltaX) - 1.57f;
            pShape[pShape.size()-1]->rotateYTo(directionYradian);
        }
    });
    touchEventHandler->setOnTouchCancel([](float cancelX, float cancelY, float cancelMillis) {
        app_log("cancel\n");
    });
    touchEventHandler->setOnTouchUp([](float upX, float upY, float upMillis) {
    });
    touchEventHandler->setOnScale(
            [](float scaleX1, float scaleY1, float scaleDistance, float currMillis) {
                float scale = scaleDistance / CoordinatesUtils::screenS;
                for (int i = 0; i < pShape.size()-1; i++) {
                    if (pShape[i]) {
//                        pShape[i]->worldScaleBy(scale, scale, scale);
                    }
                }
            });
    touchEventHandler->setOnRotate([](float rotateDeg, float currMillis) {
        float rotateZradian = rotateDeg * DEG_2_RADIAN;
        for (int i = 0; i < pShape.size(); i++) {
            if (pShape[i]) {
//                pShape[i]->rotateBy(0, 0, -rotateZradian); // 对于矩阵变换来说，轴正向朝向自己，顺时针转为正
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
                            float rotateXradian = event.data[0] * dT; // gyro返回的值单位是弧度/s
                            float rotateYradian = event.data[1] * dT;
                            float rotateZradian = event.data[2] * dT;
                            for (int i = 0; i < pShape.size()-1; i++) {
                                if (pShape[i]) {
                                    pShape[i]->worldRotateBy(rotateXradian, rotateYradian, -rotateZradian);
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

                for (int i = 0; i < pShape.size(); i++) {
                    if (i == 3 && pShape[i]) { // moon
                        pShape[i]->rotateBy(0.0f, 1.0f/60.0f, 0.0f);
                    }
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
