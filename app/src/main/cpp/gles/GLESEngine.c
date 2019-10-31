//
// Created by czf on 2019-10-16.
//

/**
 * OpenGL is designed to translate function calls into graphics commands that
 * can be sent to underlying graphics hardware. Because this underlying hardware
 * is dedicated to processing graphics commands, OpenGL drawing is typically very fast.
 */

#include <EGL/egl.h>
#include <GLES3/gl32.h> // sdk18以上才支持GLESv3版本

#include <assert.h>

#include "GLESEngine.h"
#include "../app_log.h"

struct GLESEngine {
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  int32_t width;
  int32_t height;
} engine;

/**
 * Initialize an EGL context for the current display.
 */
int GLESEngine_init(ANativeWindow *window) {
  // initialize OpenGL ES and EGL

  GLint major, minor;
  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(display, &major, &minor);
  app_log("egl, major: %d, minor: %d\n", major, minor);

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
      EGL_ALPHA_SIZE, 8,
      EGL_DEPTH_SIZE, 24,
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
    EGLint r, g, b, a, d;
    if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
        eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
        eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
        eglGetConfigAttrib(display, cfg, EGL_ALPHA_SIZE, &a) &&
        eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
        r == 8 && g == 8 && b == 8 && a == 8 && d == 24) {

      config = supportedConfigs[i];
      app_log("r: %d, g: %d, b: %d, a: %d, d: %d\n", r, g, b, a, d);
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

  EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);

  EGLint contextAttribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 3,
      EGL_NONE
  };
  EGLContext context = eglCreateContext(display, config, NULL, contextAttribs);

  if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    app_log("Unable to eglMakeCurrent\n");
    return -1;
  }

  EGLint w, h;
  eglQuerySurface(display, surface, EGL_WIDTH, &w);
  eglQuerySurface(display, surface, EGL_HEIGHT, &h);

  engine.display = display;
  engine.context = context;
  engine.surface = surface;
  engine.width = w;
  engine.height = h;

  // Check openGL on the system
  GLenum opengl_info[] = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
  for (int j = 0; j < 4; j++) {
    const GLubyte *info = glGetString(opengl_info[j]);
    app_log("OpenGL Info[%d]: %s\n", j, info);
  }
  // Initialize GL state.
//  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return 0;
}

/**
 * Just the current frame in the display.
 */
void GLESEngine_refresh() {
  if (engine.display == NULL) {
    // No display.
    return;
  }
  eglSwapBuffers(engine.display, engine.surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void GLESEngine_destroy() {
  if (engine.display != EGL_NO_DISPLAY) {
    eglMakeCurrent(engine.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (engine.context != EGL_NO_CONTEXT) {
      eglDestroyContext(engine.display, engine.context);
    }
    if (engine.surface != EGL_NO_SURFACE) {
      eglDestroySurface(engine.display, engine.surface);
    }
    eglTerminate(engine.display);
  }
  engine.display = EGL_NO_DISPLAY;
  engine.context = EGL_NO_CONTEXT;
  engine.surface = EGL_NO_SURFACE;
}

int32_t GLESEngine_get_width() {
  return engine.width;
}

int32_t GLESEngine_get_height() {
  return engine.height;
}
