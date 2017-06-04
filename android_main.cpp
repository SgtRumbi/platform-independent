/*
 * Platform-specific code for platform Android.
 */

#include <android/log.h>

#include <android_native_app_glue.h>
#include <EGL/egl.h>
// Just for IDE-auto-correction-support in CLion... (I do not want to edit these files in AndroidStudio...)
#if (!defined(__ANDROID__)) || (!defined(ANDROID))
#include <GL/gl.h>
#else
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#endif

#include "platform.h"

// typedef void (*gl_buffer_data)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
// typedef void (*gl_bind_buffer)(GLenum, uint32);
/* typedef uint32 (*gl_create_shader)(GLenum);
typedef void (*gl_shader_source)(uint32, GLsizei, const GLchar * const *, const GLint *);
typedef void (*gl_compile_shader)(uint32);
typedef uint32 (*gl_create_program)(void);
typedef void (*gl_attach_shader)(uint32, uint32);
typedef void (*gl_link_program)(uint32);
typedef void (*gl_use_program)(uint32);
typedef void (*gl_vertex_attrib_pointer)(uint32, int32, GLenum, GLboolean, GLsizei, const GLvoid *);
typedef int32 (*gl_get_attrib_location)(uint32, const char *);
typedef void (*gl_enable_vertex_attrib_array)(uint32);
typedef void (*gl_disable_vertex_attrib_array)(uint32);
typedef void (*gl_bind_framebuffer)(GLenum, uint32);
typedef void (*gl_framebuffer_texture)(GLenum, GLenum, uint32, int32);

// static gl_buffer_data glBufferData;
// static gl_bind_buffer glBindBuffer;
static gl_create_shader glCreateShader;
static gl_shader_source glShaderSource;
static gl_compile_shader glCompileShader;
static gl_attach_shader glAttachShader;
static gl_create_program glCreateProgram;
static gl_link_program glLinkProgram;
static gl_use_program glUseProgram;
static gl_vertex_attrib_pointer glVertexAttribPointer;
static gl_get_attrib_location glGetAttribLocation;
static gl_enable_vertex_attrib_array glEnableVertexAttribArray;
static gl_disable_vertex_attrib_array glDisableVertexAttribArray;
static gl_bind_framebuffer glBindFramebuffer;
static gl_framebuffer_texture glFramebufferTexture; */

#define AndroidLogInfo(...) ((void)__android_log_print(ANDROID_LOG_INFO, "demo-app", __VA_ARGS__))
#define AndroidLogWarn(...) ((void)__android_log_print(ANDROID_LOG_WARN, "demo-app", __VA_ARGS__))
#define AndroidLogError(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "demo-app", __VA_ARGS__))

#define PlatformLogInfo(...) AndroidLogInfo(__VA_ARGS__)
#define PlatformLogWarn(...) AndroidLogWarn(__VA_ARGS__)
#define PlatformLogError(...) AndroidLogError(__VA_ARGS__)

// Unity-Build!
#include "pfind.cpp"

struct android_app_saved_state {
    int32 SurfaceWidth;
    int32 SurfaceHeight;
    bool32 OpenGLInitialized;
    bool32 ModernOpenGLContext;
    bool32 OpenGLSupportsFrameBufferObjects;
    uint32 OpenGLMajorVersion;
    uint32 OpenGLMinorVersion;
};

// TODO(js): Make these NOT globals bc globals are evil.
static EGLDisplay GlobalHardwareAcceleratedDisplay;
static EGLSurface GlobalHardwareAcceleratedSurface;
static EGLContext GlobalHardwareAcceleratedContext;

static void
AndroidLogOpenGLESContextInformation() {
    AndroidLogInfo("Loaded graphics driver %s (%s, by %s).\nShading Language Version: %s",
                   glGetString(GL_VERSION), glGetString(GL_RENDERER), glGetString(GL_VENDOR),
                   glGetString(GL_SHADING_LANGUAGE_VERSION));
}

static void
InitHardwareAcceleratedContext(ANativeWindow *AndroidWindow, android_app_saved_state *SavedState) {
    AndroidLogInfo("Trying to connect to EGL-display.");

    int32 BlueBits = 8;
    int32 GreenBits = 8;
    int32 RedBits = 8;

    const EGLint Attributes[9] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, BlueBits,
            EGL_GREEN_SIZE, GreenBits,
            EGL_RED_SIZE, RedBits,
            EGL_NONE
    };

    const EGLint ContextAttributeList[3] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    EGLint Width;
    EGLint Height;
    EGLint Format;
    EGLint ConfigsCount;
    EGLConfig Configuration = {0};

    EGLint CurrentError = 0;

    // TODO(js): Maybe choose best display?
    GlobalHardwareAcceleratedDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if((CurrentError = eglGetError())) {
        eglInitialize(GlobalHardwareAcceleratedDisplay, 0, 0);

        if((CurrentError = eglGetError())) {
            // Query for configurations...
            eglChooseConfig(GlobalHardwareAcceleratedDisplay, Attributes, 0, 0, &ConfigsCount);

            if(ConfigsCount) {
                // TODO(js): Make on heap? -> 'unique-ish'
                EGLConfig SupportedConfigs[ConfigsCount];

                eglChooseConfig(GlobalHardwareAcceleratedDisplay, Attributes, SupportedConfigs, ConfigsCount,
                                &ConfigsCount);

                // Check if configs support the requested values
                int32 ConfigIndex;
                for(ConfigIndex = 0;
                    ConfigIndex < ConfigsCount;
                    ++ConfigIndex) {
                    EGLConfig ConfigurationToCheck = SupportedConfigs[ConfigIndex];
                    // INFO(js): Depth-Query is not necessary -> Can be removed.
                    EGLint QueryR, QueryG, QueryB, QueryDepth;
                    if(eglGetConfigAttrib(GlobalHardwareAcceleratedDisplay, ConfigurationToCheck, EGL_RED_SIZE, &QueryR)
                       && (QueryR == RedBits)
                       && eglGetConfigAttrib(GlobalHardwareAcceleratedDisplay, ConfigurationToCheck, EGL_GREEN_SIZE, &QueryG)
                       && (QueryG == GreenBits)
                       && eglGetConfigAttrib(GlobalHardwareAcceleratedDisplay, ConfigurationToCheck, EGL_BLUE_SIZE, &QueryB)
                       && (QueryB == BlueBits)
                       && eglGetConfigAttrib(GlobalHardwareAcceleratedDisplay, ConfigurationToCheck, EGL_DEPTH_SIZE, &QueryDepth)
                       && (QueryDepth == 0)) {
                        Configuration = ConfigurationToCheck;
                        // If a valid configuration was found -> Break.
                        break;
                    }
                }
                // 'No (suited) configuration found' -> Take first
                if(ConfigIndex == ConfigsCount) {
                    Configuration = SupportedConfigs[0];
                }

                eglGetConfigAttrib(GlobalHardwareAcceleratedDisplay, Configuration, EGL_NATIVE_VISUAL_ID, &Format);
                GlobalHardwareAcceleratedSurface = eglCreateWindowSurface(GlobalHardwareAcceleratedDisplay, Configuration, AndroidWindow, 0);
                GlobalHardwareAcceleratedContext = eglCreateContext(GlobalHardwareAcceleratedDisplay, Configuration, 0, ContextAttributeList);
                const char *OpenGLExtensionsString = "GL_ARB_vertex_buffer_object GL_ARB_shader_objects GL_ARB_vertex_program GL_ARB_vertex_shader GL_ARB_framebuffer_object";

                bool32 ModernContextSupported = true;
                bool32 SupportsFrameBufferObjects = false;

                /* if(IsExtensionSupported(OpenGLExtensionsString, "GL_ARB_vertex_buffer_object")) {
                    // glBufferData = (gl_buffer_data)eglGetProcAddress((const GLubyte *)"glBufferData");
                    // glBindBuffer = (gl_bind_buffer)eglGetProcAddress((const GLubyte *)"glBindBuffer");
                } else {
                    ModernContextSupported = false;
                    PlatformLogError("GL_ARB_vertex_buffer_object is not supported.");
                }

                glCreateShader = (gl_create_shader)eglGetProcAddress("glCreateShader");
                glShaderSource = (gl_shader_source)eglGetProcAddress("glShaderSource");
                glCompileShader = (gl_compile_shader)eglGetProcAddress("glCompileShader");
                glCreateProgram = (gl_create_program)eglGetProcAddress("glCreateProgram");
                glAttachShader = (gl_attach_shader)eglGetProcAddress("glAttachShader");
                glLinkProgram = (gl_link_program)eglGetProcAddress("glLinkProgram");
                glUseProgram = (gl_use_program)eglGetProcAddress("glUseProgram");

                if(IsExtensionSupported(OpenGLExtensionsString, "GL_ARB_shader_objects")) {
                } else {
                    ModernContextSupported = false;
                    PlatformLogError("GL_ARB_shader_objects is not supported.");
                }

                glVertexAttribPointer = (gl_vertex_attrib_pointer)eglGetProcAddress("glVertexAttribPointer");

                if(IsExtensionSupported(OpenGLExtensionsString, "GL_ARB_vertex_program")) {
                } else {
                    ModernContextSupported = false;
                    PlatformLogError("GL_ARB_vertex_program is not supported.");
                }

                glGetAttribLocation = (gl_get_attrib_location)eglGetProcAddress("glGetAttribLocation");
                glEnableVertexAttribArray = (gl_enable_vertex_attrib_array)eglGetProcAddress("glEnableVertexAttribArray");
                glDisableVertexAttribArray = (gl_disable_vertex_attrib_array)eglGetProcAddress("glDisableVertexAttribArray");

                if(IsExtensionSupported(OpenGLExtensionsString, "GL_ARB_vertex_shader")) {
                } else {
                    ModernContextSupported = false;
                    PlatformLogError("GL_ARB_vertex_shader is not supported.");
                }

                if(IsExtensionSupported(OpenGLExtensionsString, "GL_ARB_framebuffer_object")) {
                    SupportsFrameBufferObjects = true;

                    glBindFramebuffer = (gl_bind_framebuffer)eglGetProcAddress("glBindFramebuffer");
                    glFramebufferTexture = (gl_framebuffer_texture)eglGetProcAddress("glFramebufferTexture");
                } */

                // TODO(js): Interesting: Read-buffer == Write-buffer!? Canvas-like?
                if(eglMakeCurrent(GlobalHardwareAcceleratedDisplay, GlobalHardwareAcceleratedSurface,
                                   GlobalHardwareAcceleratedSurface, GlobalHardwareAcceleratedContext)) {
                    // OpenGL (ES) is now available since make current has been called.

                    eglQuerySurface(GlobalHardwareAcceleratedDisplay, GlobalHardwareAcceleratedSurface, EGL_WIDTH, &Width);
                    eglQuerySurface(GlobalHardwareAcceleratedDisplay, GlobalHardwareAcceleratedSurface, EGL_WIDTH, &Height);

                    // Print/Query OpenGL information
                    // Important:
                    //   Queries can be used to decide whether using modern OpenGL or legacy fallback!
                    /* uint8 *VendorName = (uint8 *)glGetString(GL_VENDOR);
                    uint8 *RendererName = (uint8 *)glGetString(GL_RENDERER);
                    uint8 *VersionString = (uint8 *)glGetString(GL_VERSION); */

                    // TODO(js): Make this work...
#if 0
                    uint8 *ExtensionsString = (uint8 *)glGetString(GL_EXTENSIONS);
                    int32 CurrentStart = 0;
                    int32 CurrentEnd = 0;
                    for(int32 ByteIndex = 0;
                        ByteIndex < strlen((const char *)ExtensionsString);
                        ++ByteIndex) {
                        if(ExtensionsString[ByteIndex] == ' ') {
                            CurrentEnd = ByteIndex - 1;
                        }

                        uint8 String[CurrentEnd - CurrentStart];
                        for(int32 TransferIndex = 0;
                            TransferIndex < CurrentEnd - CurrentStart;
                            ++TransferIndex) {
                            String[TransferIndex] = ExtensionsString[CurrentStart + TransferIndex];
                        }
                        AndroidLogInfo("Extension: %s", String);

                        CurrentStart = ByteIndex;
                    }
#endif

                    // For legacy OpenGL and 3D in legacy:
                    // GL_PERSPECTIVE_CORRECTION_HINT tends to fall back to a linear color interpolation...
#if 0
                    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
                    glShadeModel(GL_SMOOTH);

                    // Legacy performance...
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);

                    // TODO(js): Check: if there is a depth-buffer...
                    glDisable(GL_DEPTH_TEST);
#endif

                    AndroidLogOpenGLESContextInformation();

                    // const char *OpenGLExtensionsString = (const char *)glGetString(GL_EXTENSIONS);

                    SavedState->SurfaceWidth = Width;
                    SavedState->SurfaceHeight = Height;

                    SavedState->OpenGLInitialized = true;
                    SavedState->ModernOpenGLContext = ModernContextSupported;
                    SavedState->OpenGLSupportsFrameBufferObjects = SupportsFrameBufferObjects;
                    // TODO(js): Make un-hardcoded.
                    SavedState->OpenGLMajorVersion = 2;
                    SavedState->OpenGLMinorVersion = 0;
                } else {
                    CurrentError = eglGetError();
                    AndroidLogError("Failed to make OpenGL (ES) context current! (Possible error: 0x%x)", CurrentError);
                }
            } else {
                AndroidLogError("No EGL-configurations available!");
            }
        } else {
            AndroidLogError("Failed to initialize EGL context. (Error-Code: 0x%x)", CurrentError);
        }
    } else {
        AndroidLogError("Failed to connect to EGL display. (Error-Code: 0x%x)", CurrentError);
    }
}

static int32
PlatformAndroidHandleInputEvent(struct android_app *AppState, AInputEvent *InputEvent) {
    // Handle screen input in here
    return(0);
}

static void
PlatformAndroidHandleCommand(struct android_app *AppState, int32 Command) {
    // Handle commands in here
    switch(Command) {
        case APP_CMD_INIT_WINDOW: {
            InitHardwareAcceleratedContext(AppState->window,
                                           (android_app_saved_state *)AppState->userData);
        } break;

        default: {
        } break;
    }
}

void
android_main(struct android_app *AppState) {
    // Check if app-glue is linked with the app. (MUST be bc .a-linked)
    app_dummy();

    android_app_saved_state UserSavedState;
    ZeroInstance(&UserSavedState);

    // Cache your data here.
    AppState->onAppCmd = PlatformAndroidHandleCommand;
    AppState->onInputEvent = PlatformAndroidHandleInputEvent;
    AppState->userData = &UserSavedState;

    loop_call LoopCallInfo = {};

    while(1) {
        int32 LooperID;
        int32 Events;

        struct android_poll_source *EventSource;

        while((LooperID = ALooper_pollAll(
                0, // Timeout
                0,
                &Events,
                (void **)&EventSource // Event data
            )) >= 0) {
            // Process event internal
            if(EventSource) {
                EventSource->process(AppState, EventSource);
            }

            if(LooperID == LOOPER_ID_USER) {
                // Something has an event for us -> Process it here (GPS, Gyroscope, ...)
                AndroidLogInfo("Event should be handled.");
            }

            if(AppState->destroyRequested) {
                return;
            }
        }

        LoopCallInfo.HardwareContextInformation.HardwareAcceleratedContextInitialized = UserSavedState.OpenGLInitialized;
        LoopCallInfo.HardwareContextInformation.ModernContext = UserSavedState.ModernOpenGLContext;
        LoopCallInfo.RenderConfiguration = UserSavedState.ModernOpenGLContext
                                           ? LoopCallRenderConfiguration_HardwareAcceleratedModern
                                           : LoopCallRenderConfiguration_HardwareAcceleratedLegacy;
        LoopCallInfo.HardwareContextInformation.SupportsFrameBufferObjects = UserSavedState.OpenGLSupportsFrameBufferObjects;
        LoopCallInfo.HardwareContextInformation.EmbeddedOpenGL = true;
        LoopCallInfo.HardwareContextInformation.OpenGLMajorVersion = UserSavedState.OpenGLMajorVersion;
        LoopCallInfo.HardwareContextInformation.OpenGLMinorVersion = UserSavedState.OpenGLMinorVersion;

        LoopCallInfo.WindowWidth = UserSavedState.SurfaceWidth;
        LoopCallInfo.WindowHeight = UserSavedState.SurfaceHeight;

        // Draw frame / flush back buffer
        LoopCall(&LoopCallInfo);

        if(UserSavedState.OpenGLInitialized) {
            eglSwapBuffers(GlobalHardwareAcceleratedDisplay, GlobalHardwareAcceleratedSurface);
        }
    }
}