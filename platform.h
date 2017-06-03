#if !defined(PLATFORM_H)

#include <memory>

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

union v2 {
    struct {
        real32 x, y;
    };
    struct {
        real32 u, v;
    };
    struct {
        real32 a, b;
    };
    real32 E[2];
};

inline v2
V2() {
    v2 Result = {};
    return(Result);
}

inline v2
V2(real32 Both) {
    v2 Result;
    Result.x = Both;
    Result.y = Both;
    return(Result);
}

inline v2
V2(real32 X, real32 Y) {
    v2 Result;
    Result.x = X;
    Result.y = Y;
    return(Result);
}

#define ZeroInstance(Instance) ZeroSize_((Instance), sizeof(Instance))
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

struct loop_call {
    bool32 HardwareAcceleratedContextInitialized;
    loop_call_render_configuration RenderConfiguration;
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

#define PLATFORM_H
#endif // PLATFORM_H
