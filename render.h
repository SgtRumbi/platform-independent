#if !defined(PFIND_RENDER_H)

enum render_queue_entry_type {
    RenderQueueEntryType_render_entry_clear,
    RenderQueueEntryType_render_entry_rectangle
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

struct render_queue {
    uint8 *Buffer;
    uint32 BufferSize;
    uint32 BufferSizeUsed;
};

#define PushRenderElement(RenderQueue, type) (type *)PushRenderElement_(RenderQueue, sizeof(type), RenderQueueEntryType_##type)
inline render_queue_entry_header *
PushRenderElement_(render_queue *RenderQueue, uint32 Bytes, render_queue_entry_type Type) {
    render_queue_entry_header *Result = 0;
    if((RenderQueue->BufferSizeUsed + Bytes) < RenderQueue->BufferSize) {
        Result = (render_queue_entry_header *)(RenderQueue->Buffer + RenderQueue->BufferSizeUsed);
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

render_queue GenerateRenderQueue(memory_area *Area, uint64 RequestedSize);

void ExecuteRenderCommands(render_queue *Queue, hardware_context_information *HardwareContextInformation, memory_area *RenderMemory);

#define PFIND_RENDER_H
#endif // PFIND_RENDER_H
