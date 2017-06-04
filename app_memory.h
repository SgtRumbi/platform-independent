#if !defined(PFIND_APP_MEMORY_H)

typedef uint64 memory_index;

// NOTE(js): Provided by the platform layer
struct app_memory {
    uint64 TransientMemorySize;
    void *TransientMemory;

    uint64 PermanentMemorySize;
    void *PermanentMemory;

    bool32 IsInitialized;
};

struct memory_area {
    uint8 *BasePointer;
    memory_index Used;
    memory_index Size;

    uint32 TempCount;
};

struct temporary_memory {
    memory_area *Area;
    memory_index Used;
};

inline void
InitializeMemoryArea(memory_area *Area, memory_index Size, uint8 *BasePointer) {
    Area->Used = 0;
    Area->BasePointer = BasePointer;
    Area->Size = Size;
    Area->TempCount = 0;
}

inline temporary_memory
BeginTemporaryMemory(memory_area *Area) {
    temporary_memory Result;

    Result.Area = Area;
    Result.Used = Area->Used;

    ++Area->TempCount;

    return(Result);
}

inline void
EndTemporaryMemory(temporary_memory TempMemory) {
    memory_area *Area = TempMemory.Area;
    Assert(Area->Used >= TempMemory.Used);
    Area->Used = TempMemory.Used;
    Assert(Area->TempCount > 0);
    --Area->TempCount;
}

#define PushStruct(Area, type) (type *)PushSize_(Area, sizeof(type))
#define PushArray(Area, Count, type) (type **)PushSize_(Area, (Count)*sizeof(type))
#define PushSize(Area, Size) PushSize_(Area, Size)
inline void *
PushSize_(memory_area *Area, memory_index Size) {
    Assert((Area->Used + Size) <= Area->Size);
    void *Result = Area->BasePointer + Area->Used;
    Area->Used += Size;
    return(Area);
}

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

#define PFIND_APP_MEMORY_H
#endif // PFIND_APP_MEMORY_H
