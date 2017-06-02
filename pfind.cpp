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
        
        char *VertexShaderCode = "" 
        "#version 150 core\n"
        "attribute vec2 Position;"
        
        "void main(void) {"
        "    gl_Position = vec4(Position, 0.0, 1.0);"
        "}";
        char *FragmentShaderCode = ""
        "#version 150 core\n"
        
        "void main(void) {"
        "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
        "}";
        
        uint32 VertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
        uint32 FragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(VertexShaderHandle, VertexShaderCode);
        glShaderSource(FragmentShaderHandle, FragmentShaderHandle);
        
        glCompileShader(VertexShaderHandle);
        glCompileShader(FragmentShaderHandle);
        
        ProgramHandle = glCreateProgram();
        glAttachShader(ProgramHandle, VertexShaderHandle);
        glAttachShader(ProgramHandle, FragmentShaderHandle);
        
        glLinkProgram(ProgramHandle);
    }

    static int32 BufferHandle = 1;
    
    /* glEnable(GL_CALL);
    glCullFace */
    
    /* if(!BufferHandle) {
        glGenBuffers(1, &BufferHandle);
    } */
    
    /* typedef float real32;
    typedef double real64; */
    
    real32 Vertices[6] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };
    
#if 1
    glUseProgram(ProgramHandle);

    glBindBuffer(GL_ARRAY_BUFFER, BufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(real32),& Vertices[0], GL_STREAM_DRAW);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
#endif
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
