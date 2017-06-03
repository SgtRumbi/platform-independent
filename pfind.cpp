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
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();
}

static void
ModernOpenGL() {
    static bool32 Initialized = false;
    static uint32 ProgramHandle = -1;
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
    }

    static int32 BufferHandle = 1;
    
    real32 Vertices[6] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, BufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(real32), Vertices, GL_STREAM_DRAW);

    glUseProgram(ProgramHandle);

    int32 PositionAttributeHandle = glGetAttribLocation(ProgramHandle, "Position");
    glEnableVertexAttribArray(PositionAttributeHandle);
    glVertexAttribPointer(PositionAttributeHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(PositionAttributeHandle);
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
