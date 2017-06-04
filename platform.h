#if !defined(PLATFORM_H)

#include <memory>
#include <cstring>

#define Assert(Expression) if(!(Expression)) {PlatformLogError("Expression: '%s' failed!", #Expression); __builtin_trap();}

typedef unsigned char uint8;
typedef signed char int8;

typedef unsigned short uint16;
typedef signed short int16;

typedef unsigned int uint32;
typedef signed int int32;

typedef unsigned long uint64;
typedef signed long int64;

typedef int32 bool32;

typedef size_t size;

typedef float real32;
typedef double real64;

#include "math.h"

#define ZeroInstance(Instance) ZeroSize_((size)(Instance), sizeof(*(Instance)))
#define ZeroArray(Array, ArrayCount) ZeroSize_((Array), (ArrayCount)*sizeof((Array)[0]))
#define ZeroSize(Pointer, Size) ZeroSize_(Pointer, Size)
inline void
ZeroSize_(size Pointer, size Size) {
    uint8 *BytePointer = (uint8 *)Pointer;
    while(Size--) {
        *BytePointer++ = 0;
    }
}

enum loop_call_render_configuration {
    LoopCallRenderConfiguration_Buffer,
    LoopCallRenderConfiguration_HardwareAcceleratedModern,
    LoopCallRenderConfiguration_HardwareAcceleratedLegacy
};

// Dummy...
union device_input_information {
    struct {
        uint32 MaxTouchInputs;
    } Handheld;
    struct {
        uint32 ScreenCount;
    } Desktop;
};

struct device_information {
    bool32 Handheld;
    device_input_information InputInformation;
};

struct hardware_context_information {
    bool32 HardwareAcceleratedContextInitialized;
    bool32 SupportsFrameBufferObjects;
    bool32 EmbeddedOpenGL;
    bool32 ModernContext;
    uint32 OpenGLMajorVersion;
    uint32 OpenGLMinorVersion;
};

struct loop_call {
    device_information DeviceInformation;
    loop_call_render_configuration RenderConfiguration;
    hardware_context_information HardwareContextInformation;

    int32 WindowWidth;
    int32 WindowHeight;
};

struct vertex_buffer {
    uint8 *Data;
    uint32 Size;
    uint32 Used;
};

inline vertex_buffer
CreateVertexBuffer(uint32 RequestedSizeBytes) {
    vertex_buffer Result;
    Result.Data = (uint8 *)malloc(RequestedSizeBytes);
    if(Result.Data) {
        Result.Size = RequestedSizeBytes;
    } else {
        Result.Size = 0;
    }
    Result.Used = 0;
    return(Result);
}

#define PushV2(Buffer, Vector) PushElement_(Buffer, (void *)Vector.E, sizeof(v2))
#define PushReal32(Buffer, Real) PushElement_(Buffer, (void *)&Real, sizeof(real32))
inline void
PushElement_(vertex_buffer *Buffer, void *Data, int32 Size) {
    uint8 *Pointer = (uint8 *)Data;
    while(Size--) {
        Buffer->Data[Buffer->Used++] = *Pointer++;
    }
}

inline void
PushRectangle(vertex_buffer *VertexBuffer, v2 StartP, v2 EndP) {
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

inline bool32
IsExtensionSupported(const char *ExtensionsList, const char *Extension) {
    bool32 Result = false;

    if(ExtensionsList && Extension) {
        const char *Start;
        const char *Where;
        const char *Terminator;

        Where = strchr(Extension, ' ');
        if (!Where && (*Extension != '\0')) {
            for (Start = ExtensionsList;;
                    ) {
                Where = strstr(Start, Extension);

                if (Where) {
                    Terminator = Where + strlen(Extension);

                    if ((Where == Start) || (*(Where - 1) == ' ')) {
                        if ((*Terminator == ' ') || (*Terminator == '\0')) {
                            Result = true;
                            break;
                        }
                    }

                    Start = Terminator;
                } else {
                    break;
                }
            }
        } else {
            Result = false;
        }
    }

    return(Result);
}

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif

#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

#ifndef GL_STREAM_DRAW
#define GL_STREAM_DRAW 0x88E0
#endif

// NOTE(js): Just pre-definition. Should be 'implemented' in platform-layer.
#define PlatformLogInfo(...)
#define PlatformLogWarn(...)
#define PlatformLogError(...)

#define InvalidCodePath Assert(!("Invalid code path."))
#define InvalidDefaultCase default: {InvalidCodePath;} break

#define PLATFORM_H
#endif // PLATFORM_H
