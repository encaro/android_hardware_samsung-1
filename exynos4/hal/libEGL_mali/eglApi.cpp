/*
 * Copyright (C) 2019 The Android Open Source Project
 * Copyright (C) 2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <log/log.h>
#include <system/graphics.h>
#include <system/window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

extern "C" EGLBoolean shim_eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,
        EGLint attribute, EGLint *value);

extern "C" EGLSurface shim_eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
                                    NativeWindowType window,
                                    const EGLint *attrib_list);

extern "C" EGLBoolean shim_eglMakeCurrent(  EGLDisplay dpy, EGLSurface draw,
                            EGLSurface read, EGLContext ctx);

extern "C" EGLBoolean shim_eglDestroySurface(EGLDisplay dpy, EGLSurface surface);

extern "C" EGLSurface shim_eglGetCurrentSurface(EGLint readdraw);

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,
        EGLint attribute, EGLint *value) {
     return shim_eglGetConfigAttrib(dpy, config, attribute, value);
}

EGLSurface eglCreateWindowSurface(  EGLDisplay dpy, EGLConfig config,
                                    NativeWindowType window,
                                    const EGLint *attrib_list) {
#ifdef NEEDS_NATIVE_WINDOW_FORMAT_FIX
     int format, err;
     window->query(window, NATIVE_WINDOW_FORMAT, &format);

     if (format == (int)HAL_PIXEL_FORMAT_RGBA_8888) {
         format = (int)HAL_PIXEL_FORMAT_BGRA_8888;
         err = window->perform(window, NATIVE_WINDOW_SET_BUFFERS_FORMAT, format);
     }

#endif
     return shim_eglCreateWindowSurface(dpy, config, window, attrib_list);
}

EGLBoolean eglMakeCurrent(  EGLDisplay dpy, EGLSurface draw,
                            EGLSurface read, EGLContext ctx) {
#ifdef DESTROY_SURFACE_AFTER_DETACH
    if (draw == EGL_NO_SURFACE &&
            read == EGL_NO_SURFACE &&
            ctx == EGL_NO_CONTEXT)
        ALOGE("%s: draw, read, ctx == NULL", __func__);
    else
        ALOGE("%s: init", __func__);
#endif
   EGLSurface curRead = shim_eglGetCurrentSurface(EGL_READ);
   EGLSurface curDraw = shim_eglGetCurrentSurface(EGL_DRAW);
   ALOGE("%s: before makeCurrent: curRead=%s, curDraw=%s", __func__, (curRead == EGL_NO_SURFACE ? "NULL" : "ok"), (curDraw == EGL_NO_SURFACE ? "NULL" : "ok"));

   EGLBoolean res = shim_eglMakeCurrent(dpy, draw, read, ctx);

   curRead = shim_eglGetCurrentSurface(EGL_READ);
   curDraw = shim_eglGetCurrentSurface(EGL_DRAW);
   ALOGE("%s: after makeCurrent: curRead=%s, curDraw=%s", __func__, (curRead == EGL_NO_SURFACE ? "NULL" : "ok"), (curDraw == EGL_NO_SURFACE ? "NULL" : "ok"));

   return res;
}

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface) {
#ifdef DESTROY_SURFACE_AFTER_DETACH
    //ALOGE("%s: detaching surface before destroy", __func__);
    //bool res = shim_eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
#endif
   EGLSurface curRead = shim_eglGetCurrentSurface(EGL_READ);
   EGLSurface curDraw = shim_eglGetCurrentSurface(EGL_DRAW);
   ALOGE("%s: before DestroySurface: curRead=%s, curDraw=%s", __func__, (curRead == EGL_NO_SURFACE ? "NULL" : "ok"), (curDraw == EGL_NO_SURFACE ? "NULL" : "ok"));
   EGLBoolean res = shim_eglDestroySurface(dpy, surface);
   curRead = shim_eglGetCurrentSurface(EGL_READ);
   curDraw = shim_eglGetCurrentSurface(EGL_DRAW);
   ALOGE("%s: after DestroySurface: curRead=%s, curDraw=%s", __func__, (curRead == EGL_NO_SURFACE ? "NULL" : "ok"), (curDraw == EGL_NO_SURFACE ? "NULL" : "ok"));
   return res;
}
