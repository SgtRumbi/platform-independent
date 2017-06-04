/*
 * Platform-independent code.
 */

// Just for IDE-auto-correction-support in CLion... (I do not want to edit these files in AndroidStudio...)
#if (!defined(__ANDROID__)) || (!defined(ANDROID))
#include <GL/gl.h>
#else
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#endif

#include "platform.h"

#include "render.cpp"

struct rectangle {
    v2 StartP;
    v2 EndP;
};

struct draw_information {
    rectangle RectanglesToDraw[256];
    int32 RectangleCount;
    hardware_context_information HardwareContextInformation;
};

static void
OpenGLLegacyFallback(draw_information *DrawInformation) {
    static bool32 Initialized = false;
    static bool32 FatalErrorDuringInitialisation = false;
    static vertex_buffer VertexBuffer;

    if(!Initialized) {
        Initialized = true;

        VertexBuffer = CreateVertexBuffer(DrawInformation->RectangleCount*12*sizeof(real32));

        for(int32 RectangleIndex = 0;
            RectangleIndex < DrawInformation->RectangleCount;
            ++RectangleIndex) {
            PushRectangle(&VertexBuffer, DrawInformation->RectanglesToDraw[RectangleIndex].StartP,
                          DrawInformation->RectanglesToDraw[RectangleIndex].EndP);
        }

        PlatformLogInfo("Initialized legacy OpenGL graphics.");
    }

    if(Initialized && (!FatalErrorDuringInitialisation)) {
        glClearColor(0.3f, 0.1f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, VertexBuffer.Data);
        glDrawArrays(GL_TRIANGLES, 0, 12*DrawInformation->RectangleCount);
        glDisableClientState(GL_VERTEX_ARRAY);
    } else {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

static bool32
ModernOpenGLCheckShaderCompilation(uint32 ShaderHandle) {
    bool32 Result = false;

    bool32 IsCompiled;
    glGetShaderiv(ShaderHandle, GL_COMPILE_STATUS, &IsCompiled);
    if(!IsCompiled) {
        GLint InfoLogLength = 0;
        glGetShaderiv(ShaderHandle, GL_INFO_LOG_LENGTH, &InfoLogLength);
        char Buffer[InfoLogLength];
        glGetShaderInfoLog(ShaderHandle, InfoLogLength, &InfoLogLength, Buffer);
        PlatformLogInfo("Failed to compile shader. Log: (blen: %d)\n%s", InfoLogLength, Buffer);
    } else {
        Result = true;
    }

    return(Result);
}

static void
ModernOpenGLBuildVersionLine(hardware_context_information *HardwareContextInformation, char *Dest, int32 Length) {
    if(HardwareContextInformation->EmbeddedOpenGL
       && ((HardwareContextInformation->OpenGLMajorVersion == 1)
           || (HardwareContextInformation->OpenGLMajorVersion == 2))) {
        strncpy(Dest, "", Length);
    } else if(HardwareContextInformation->EmbeddedOpenGL
              && (HardwareContextInformation->OpenGLMajorVersion == 3)) {
        if(HardwareContextInformation->OpenGLMinorVersion == 0) {
            strncpy(Dest, "#version 300 es\n", Length);
        } else if(HardwareContextInformation->OpenGLMinorVersion == 1) {
            strncpy(Dest, "#version 310 es\n", Length);
        }
    } else if(!HardwareContextInformation->EmbeddedOpenGL) {
        strncpy(Dest, "#version 150 core\n", Length);
    }
}

static void
ModernOpenGL(draw_information *DrawInformation) {
    static bool32 Initialized = false;
    static bool32 FatalErrorDuringInitialisation = false;
    static uint32 ProgramHandle = 0;
    static uint32 BufferHandle = 1;
    static vertex_buffer VertexBuffer;
    if(!Initialized) {
        Initialized = true;

        char VersionLine[255];
        ModernOpenGLBuildVersionLine(&DrawInformation->HardwareContextInformation, VersionLine, 255);

        const char *Defines = R"foo(
        #define v2 vec2
        #define V2 vec2
        #define v3 vec3
        #define V3 vec3
        #define v4 vec4
        #define V4 vec4
        #define r32 float
        #define real32 float
        )foo";

        const char *VertexShaderCode = R"foo(
        attribute v2 Position;

        void main(void) {
            gl_Position = V4(Position, 0.0, 1.0);
        }
        )foo";

        const char *FragmentShaderHeader;
        if(DrawInformation->HardwareContextInformation.EmbeddedOpenGL
           && (DrawInformation->HardwareContextInformation.OpenGLMajorVersion < 3)) {
            FragmentShaderHeader = R"foo(
            #if defined(GL_ES)
            precision mediump float;
            #endif

            #define OutColor gl_FragColor
            )foo";
        } else {
            FragmentShaderHeader = R"foo(
            #if defined(GL_ES)
            precision mediump float;
            #endif

            out vec4 OutColor;
            )foo";
        }


        const char *FragmentShaderCode = R"foo(
        void main(void) {
            OutColor = V4(1.0, 0.0, 0.0, 1.0);
        }
        )foo";

        int32 VertexShaderCodeLength = strlen(VertexShaderCode);
        int32 FragmentShaderCodeLength = strlen(FragmentShaderCode);

        uint32 VertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
        uint32 FragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

        const char *VertexShaderSources[3] = {
                VersionLine,
                Defines,
                VertexShaderCode
        };
        const char *FragmentShaderSources[4] = {
                VersionLine,
                Defines,
                FragmentShaderHeader,
                FragmentShaderCode
        };
        glShaderSource(VertexShaderHandle, 3, VertexShaderSources, 0);
        glShaderSource(FragmentShaderHandle, 4, FragmentShaderSources, 0);
        
        glCompileShader(VertexShaderHandle);
        glCompileShader(FragmentShaderHandle);

        if(ModernOpenGLCheckShaderCompilation(VertexShaderHandle)
           && ModernOpenGLCheckShaderCompilation(FragmentShaderHandle)) {
            ProgramHandle = glCreateProgram();
            glAttachShader(ProgramHandle, VertexShaderHandle);
            glAttachShader(ProgramHandle, FragmentShaderHandle);

            glLinkProgram(ProgramHandle);
        } else {
            FatalErrorDuringInitialisation = true;
        }

        VertexBuffer = CreateVertexBuffer(DrawInformation->RectangleCount*12*sizeof(real32));

        for(int32 RectangleIndex = 0;
            RectangleIndex < DrawInformation->RectangleCount;
            ++RectangleIndex) {
            PushRectangle(&VertexBuffer, DrawInformation->RectanglesToDraw[RectangleIndex].StartP,
                          DrawInformation->RectanglesToDraw[RectangleIndex].EndP);
        }

        glBindBuffer(GL_ARRAY_BUFFER, BufferHandle);
        glBufferData(GL_ARRAY_BUFFER, VertexBuffer.Used, VertexBuffer.Data, GL_STATIC_DRAW);

        PlatformLogInfo("Initialized modern OpenGL graphics.");
    }

    if(Initialized && (!FatalErrorDuringInitialisation)) {
        glClearColor(0.3f, 0.1f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(ProgramHandle);

        int32 PositionAttributeHandle = glGetAttribLocation(ProgramHandle, "Position");
        glEnableVertexAttribArray(PositionAttributeHandle);
        glVertexAttribPointer(PositionAttributeHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, 12*DrawInformation->RectangleCount);

        glDisableVertexAttribArray(PositionAttributeHandle);
    } else {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

static void
CanvasRender(draw_information *DrawInformation) {

}

static void
LoopCall(loop_call *LoopCallInformation) {
    if(LoopCallInformation->HardwareContextInformation.HardwareAcceleratedContextInitialized) {
        draw_information DrawInformation;
        DrawInformation.HardwareContextInformation = LoopCallInformation->HardwareContextInformation;
        DrawInformation.RectangleCount = 256;
        for(int32 RectangleIndex = 0;
            RectangleIndex < DrawInformation.RectangleCount;
            ++RectangleIndex) {
            v2 *StartP = &DrawInformation.RectanglesToDraw[RectangleIndex].StartP;
            v2 *EndP = &DrawInformation.RectanglesToDraw[RectangleIndex].EndP;

            int32 X = RectangleIndex%16;
            int32 Y = RectangleIndex/16;

            StartP->x = ((real32)X/16.0f);
            StartP->y = ((real32)Y/16.0f);
            EndP->x = StartP->x + 0.03f;
            EndP->y = StartP->y + 0.03f;
        }

        switch(LoopCallInformation->RenderConfiguration) {
            case LoopCallRenderConfiguration_HardwareAcceleratedModern: {
                ModernOpenGL(&DrawInformation);
            } break;

            case LoopCallRenderConfiguration_HardwareAcceleratedLegacy: {
                OpenGLLegacyFallback(&DrawInformation);
            } break;

            case LoopCallRenderConfiguration_Buffer: {
                CanvasRender(&DrawInformation);
            } break;
        }
    }
}
