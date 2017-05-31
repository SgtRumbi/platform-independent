#if !defined(PLATFORM_H)

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

#define PLATFORM_H
#endif // PLATFORM_H
