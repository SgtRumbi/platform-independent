/*
 * Platform-independent code.
 */

// Just for IDE-auto-correction-support in CLion... (I do not want to edit these files in AndroidStudio...)
#if (!defined(__ANDROID__)) || (!defined(ANDROID))
#include <GL/gl.h>
#else
#include <GLES/gl.h>
#endif

#include "platform.h"

static void
OpenGLLegacyFallback() {

}

static void
ModernOpenGL() {

}

static void
CanvasRender() {

}

static void
LoopCall(loop_call *LoopCallInformation) {
    if(LoopCallInformation->HardwareAcceleratedContextInitialized) {
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        switch(LoopCallInformation->RenderConfiguration) {
            case LoopCallRenderConfiguration_HardwareAcceleratedModern: {
                ModernOpenGL();
            } break;

            case LoopCallRenderConfiguration_HardwareAcceleratedLegacy: {
                OpenGLLegacyFallback();
            } break;

            case LoopCallRenderConfiguration_Buffer: {
                CanvasRender();
            } break;

            default: {
                PlatformLogError("Unsupported render configuration!");
            } break;
        }
    }
}
