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

struct rectangle {
    v2 StartP;
    v2 EndP;
};

struct draw_information {
    rectangle RectanglesToDraw[256];
    int32 RectangleCount;
};

static void
OpenGLLegacyFallback(draw_information *DrawInformation) {
    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLES);
    for(int32 RectangleIndex = 0;
        RectangleIndex < DrawInformation->RectangleCount;
        ++RectangleIndex) {
        rectangle Rectangle = DrawInformation->RectanglesToDraw[RectangleIndex];

        v2 LeftLowerCornerP = Rectangle.StartP;
        v2 LeftUpperCornerP = V2(Rectangle.StartP.x, Rectangle.EndP.y);
        v2 RightLowerCornerP = V2(Rectangle.EndP.x, Rectangle.StartP.y);
        v2 RightUpperCornerP = Rectangle.EndP;

        glVertex2fv(LeftLowerCornerP.E);
        glVertex2fv(RightUpperCornerP.E);
        glVertex2fv(RightLowerCornerP.E);

        glVertex2fv(LeftLowerCornerP.E);
        glVertex2fv(RightUpperCornerP.E);
        glVertex2fv(LeftUpperCornerP.E);
    }
    glEnd();
}

static void
ModernOpenGLPushRectangle(vertex_buffer *VertexBuffer, v2 StartP, v2 EndP) {
    v2 LeftLowerCornerP = StartP;
    v2 LeftUpperCornerP = V2(StartP.x, EndP.y);
    v2 RightLowerCornerP = V2(EndP.x, StartP.y);
    v2 RightUpperCornerP = EndP;

    PushV2(VertexBuffer, LeftLowerCornerP);
    PushV2(VertexBuffer, RightUpperCornerP);
    PushV2(VertexBuffer, RightLowerCornerP);

    PushV2(VertexBuffer, LeftLowerCornerP);
    PushV2(VertexBuffer, RightUpperCornerP);
    PushV2(VertexBuffer, LeftUpperCornerP);
}

static void
ModernOpenGL(draw_information *DrawInformation) {
    static bool32 Initialized = false;
    static uint32 ProgramHandle = -1;
    static vertex_buffer VertexBuffer;
    if(!Initialized) {
        Initialized = true;

        const char *VertexShaderCode = ""
        "#version 150 core\n"
        "in vec2 Position;"
        
        "void main(void) {"
        "    gl_Position = vec4(Position, 0.0, 1.0);"
        "}";
        const char *FragmentShaderCode = ""
        "#version 150 core\n"
        // "out vec4 Color;"

        "void main(void) {"
        "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
        "}";
        int32 VertexShaderCodeLength = strlen(VertexShaderCode);
        int32 FragmentShaderCodeLength = strlen(FragmentShaderCode);

        uint32 VertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
        uint32 FragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(VertexShaderHandle, 1, &VertexShaderCode, &VertexShaderCodeLength);
        glShaderSource(FragmentShaderHandle, 1, &FragmentShaderCode, &FragmentShaderCodeLength);
        
        glCompileShader(VertexShaderHandle);
        glCompileShader(FragmentShaderHandle);
        
        ProgramHandle = glCreateProgram();
        glAttachShader(ProgramHandle, VertexShaderHandle);
        glAttachShader(ProgramHandle, FragmentShaderHandle);
        
        glLinkProgram(ProgramHandle);

        VertexBuffer = CreateVertexBuffer(DrawInformation->RectangleCount*12*sizeof(real32));

        for(int32 RectangleIndex = 0;
            RectangleIndex < DrawInformation->RectangleCount;
            ++RectangleIndex) {
            ModernOpenGLPushRectangle(&VertexBuffer, DrawInformation->RectanglesToDraw[RectangleIndex].StartP,
                                      DrawInformation->RectanglesToDraw[RectangleIndex].EndP);
        }
    }

    static int32 BufferHandle = 1;

    glBindBuffer(GL_ARRAY_BUFFER, BufferHandle);
    glBufferData(GL_ARRAY_BUFFER, VertexBuffer.Used, VertexBuffer.Data, GL_STREAM_DRAW);

    glUseProgram(ProgramHandle);

    int32 PositionAttributeHandle = glGetAttribLocation(ProgramHandle, "Position");
    glEnableVertexAttribArray(PositionAttributeHandle);
    glVertexAttribPointer(PositionAttributeHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 12*DrawInformation->RectangleCount);

    glDisableVertexAttribArray(PositionAttributeHandle);
}

static void
CanvasRender(draw_information *DrawInformation) {

}

static void
LoopCall(loop_call *LoopCallInformation) {
    if(LoopCallInformation->HardwareAcceleratedContextInitialized) {
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_information DrawInformation;
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

            default: {
                PlatformLogError("Unsupported render configuration!");
            } break;
        }
    }
}
