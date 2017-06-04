#if !defined(PFIND_RENDER_H)

enum render_queue_entry_type {
    RenderQueueEntryType_render_entry_clear,
    RenderQueueEntryType_render_entry_rectangle
};

struct render_queue_entry_header {
    render_queue_entry_type Type;
};

struct render_queue_entry {

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
    void *Buffer;
    uint32 BufferSize;
    uint32 BufferSizeUsed;
};

render_queue GenerateRenderQueue(uint32 RequestedSize);

void ExecuteRenderCommands(render_queue *Queue, hardware_context_information *HardwareContextInformation);

#define PFIND_RENDER_H
#endif // PFIND_RENDER_H
