#if !defined(PFIND_RENDER_H)

enum render_queue_entry_type {
    RenderQueueEntryType_render_entry_clear,
    RenderQueueEntryType_render_entry_rectangle,
    RenderQueueEntryType_render_entry_viewport,
};

struct render_queue_entry_header {
    render_queue_entry_type Type;
};

struct render_queue_entry {
    render_queue_entry_header Header;
};

struct render_entry_clear {
    render_queue_entry_header Header;
    real32 R, G, B, A;
};

struct render_entry_rectangle {
    render_queue_entry_header Header;
    v2 P;
    v2 Dim;
    real32 R, G, B, A;
};

struct render_entry_viewport {
    render_queue_entry_header Header;
    uint32 BeginX;
    uint32 BeginY;
    uint32 ToX;
    uint32 ToY;
};

struct render_queue {
    uint32 BufferSize;
    uint32 BufferSizeUsed;
    uint8 *Buffer;
};

#define PushRenderElement(RenderQueue, type) (type *)PushRenderElement_(RenderQueue, sizeof(type), RenderQueueEntryType_##type)
inline render_queue_entry_header *
PushRenderElement_(render_queue *RenderQueue, uint32 Bytes, render_queue_entry_type Type) {
    render_queue_entry_header *Result = 0;
    if((RenderQueue->BufferSizeUsed + Bytes) < RenderQueue->BufferSize) {
        uint64 NewPosition = (uint64)(RenderQueue->Buffer + RenderQueue->BufferSizeUsed);
        Result = (render_queue_entry_header *)NewPosition;
        Result->Type = Type;
        RenderQueue->BufferSizeUsed += Bytes;
    } else {
        InvalidCodePath;
    }
    return(Result);
}

inline void
PushRectangle(render_queue *RenderQueue, v2 P, v2 Dim, v4 Color) {
    render_entry_rectangle *Entry = PushRenderElement(RenderQueue, render_entry_rectangle);
    if(Entry) {
        Entry->P = P;
        Entry->Dim = Dim;
        Entry->R = Color.R;
        Entry->G = Color.G;
        Entry->B = Color.B;
        Entry->A = Color.A;
    }
}

inline void
PushClear(render_queue *RenderQueue, real32 R, real32 G, real32 B, real32 A) {
    render_entry_clear *Entry = PushRenderElement(RenderQueue, render_entry_clear);
    if(Entry) {
        Entry->R = R;
        Entry->G = G;
        Entry->B = B;
        Entry->A = A;
    }
}

inline void
PushViewport(render_queue *RenderQueue, uint32 BeginX, uint32 BeginY, uint32 ToX, uint32 ToY) {
    render_entry_viewport *Entry = PushRenderElement(RenderQueue, render_entry_viewport);
    if(Entry) {
        Entry->BeginX = BeginX;
        Entry->BeginY = BeginY;
        Entry->ToX = ToX;
        Entry->ToY = ToY;
    }
}

render_queue GenerateRenderQueue(memory_area *Area, uint64 RequestedSize);

void ExecuteRenderCommands(render_queue *Queue, hardware_context_information *HardwareContextInformation, memory_area *RenderMemory);

#define PFIND_RENDER_H
#endif // PFIND_RENDER_H
