#include "render.h"

render_queue
GenerateRenderQueue(uint32 RequestedSize) {
    render_queue Result;

    Result.BufferSizeUsed = 0;
    Result.Buffer = malloc(RequestedSize);
    if(Result.Buffer) {
        Result.BufferSize = RequestedSize;
    } else {
        Result.BufferSize = 0;
    }
    return(Result);
}

void
ExecuteRenderCommands(render_queue *Queue, hardware_context_information *HardwareContextInformation) {
    if(HardwareContextInformation->EmbeddedOpenGL && HardwareContextInformation->ModernContext) {
        for(uint32 BaseAddress = 0;
            BaseAddress < Queue->BufferSize;
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


                    BaseAddress += sizeof(*Entry);
                } break;
            }
        }
    }
}
