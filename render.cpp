#include "render.h"

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

render_queue
GenerateRenderQueue(memory_area *Area, uint64 RequestedSize) {
    render_queue Result;

    Result.BufferSizeUsed = 0;
    Result.Buffer = (uint8 *)PushSize(Area, RequestedSize);
    if(Result.Buffer) {
        Result.BufferSize = RequestedSize;
    } else {
        PlatformLogError("Out of Memory Error. (requested %d bytes)", RequestedSize);
        Result.BufferSize = 0;
    }

    return(Result);
}

#if 0
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
#endif

void
ExecuteRenderCommands(render_queue *Queue, hardware_context_information *HardwareContextInformation, memory_area *RenderMemory) {
    static bool32 ModernContextInitialized = false;
    static bool32 LegacyContextInitialized = false;

    if(HardwareContextInformation->ModernContext) {
        static vertex_buffer SingleRectangleVertexBuffer;
        static bool32 FatalErrorDuringInitialisation = false;
        static uint32 ProgramHandle;
        static uint32 BufferHandle = 1;

        if(!ModernContextInitialized) {
            ModernContextInitialized = true;
            LegacyContextInitialized = false;

            SingleRectangleVertexBuffer = CreateVertexBuffer(RenderMemory, 6*sizeof(real32));

            char VersionLine[255];
            ModernOpenGLBuildVersionLine(HardwareContextInformation, VersionLine, 255);

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
            if(HardwareContextInformation->EmbeddedOpenGL && (HardwareContextInformation->OpenGLMajorVersion < 3)) {
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
                OutColor = V4(1.0, 1.0, 1.0, 1.0);
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
        }

        if(ModernContextInitialized && (!FatalErrorDuringInitialisation)) {
            for(uint32 BaseAddress = 0;
                BaseAddress < Queue->BufferSizeUsed;
                ) {
                render_queue_entry_header *Header = (render_queue_entry_header *)(Queue->Buffer + BaseAddress);
                switch(Header->Type) {
                    case RenderQueueEntryType_render_entry_clear: {
                        render_entry_clear *Entry = (render_entry_clear *)Header;

                        glClearColor(Entry->R, Entry->G, Entry->B, Entry->A);
                        glClear(GL_COLOR_BUFFER_BIT);

                        BaseAddress += sizeof(*Entry);
                    } break;

                    case RenderQueueEntryType_render_entry_rectangle: {
                        render_entry_rectangle *Entry = (render_entry_rectangle *)Header;

                        SingleRectangleVertexBuffer.Used = 0;
                        PushRectangle(&SingleRectangleVertexBuffer, Entry->P, Entry->P + Entry->Dim);
                        glBindBuffer(GL_ARRAY_BUFFER, BufferHandle);
                        glBufferData(GL_ARRAY_BUFFER, SingleRectangleVertexBuffer.Used,
                                     SingleRectangleVertexBuffer.Data, GL_DYNAMIC_DRAW);

                        glUseProgram(ProgramHandle);

                        int32 PositionAttributeHandle = glGetAttribLocation(ProgramHandle, "Position");
                        glEnableVertexAttribArray(PositionAttributeHandle);
                        glVertexAttribPointer(PositionAttributeHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);

                        glDrawArrays(GL_TRIANGLES, 0, 6);

                        glDisableVertexAttribArray(PositionAttributeHandle);

                        BaseAddress += sizeof(*Entry);
                    } break;

                    case RenderQueueEntryType_render_entry_viewport: {
                        render_entry_viewport *Entry = (render_entry_viewport *)Header;

                        glViewport(Entry->BeginX, Entry->BeginY, Entry->ToX, Entry->ToY);

                        BaseAddress += sizeof(*Entry);
                    } break;

                    InvalidDefaultCase;
                }
            }
        }
    } else {
        static vertex_buffer SingleRectangleVertexBuffer;

        if(!LegacyContextInitialized) {
            LegacyContextInitialized = true;
            ModernContextInitialized = false;

            SingleRectangleVertexBuffer = CreateVertexBuffer(RenderMemory, 6*sizeof(real32));
        }

        if(LegacyContextInitialized) {
            for(uint32 BaseAddress = 0;
                BaseAddress < Queue->BufferSizeUsed;
                ) {
                render_queue_entry_header *Header = (render_queue_entry_header *)(Queue->Buffer + BaseAddress);
                switch(Header->Type) {
                    case RenderQueueEntryType_render_entry_clear: {
                        render_entry_clear *Entry = (render_entry_clear *)Header;

                        glClearColor(Entry->R, Entry->G, Entry->B, Entry->A);
                        glClear(GL_COLOR_BUFFER_BIT);

                        BaseAddress += sizeof(*Entry);
                    } break;

                    case RenderQueueEntryType_render_entry_rectangle: {
                        render_entry_rectangle *Entry = (render_entry_rectangle *)Header;

                        SingleRectangleVertexBuffer.Used = 0;
                        PushRectangle(&SingleRectangleVertexBuffer, Entry->P, Entry->P + Entry->Dim);

                        glColor4f(Entry->R, Entry->G, Entry->B, Entry->A);
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(2, GL_FLOAT, 0, SingleRectangleVertexBuffer.Data);
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                        glDisableClientState(GL_VERTEX_ARRAY);

                        BaseAddress += sizeof(*Entry);
                    } break;

                    case RenderQueueEntryType_render_entry_viewport: {
                        render_entry_viewport *Entry = (render_entry_viewport *)Header;

                        glViewport(Entry->BeginX, Entry->BeginY, Entry->ToX, Entry->ToY);

                        BaseAddress += sizeof(*Entry);
                    } break;

                    InvalidDefaultCase;
                }
            }
        }
    }
}
