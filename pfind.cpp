/*
 * Platform-independent code.
 */

// Just for IDE-auto-correction-support in CLion... (I do not want to edit these files in AndroidStudio...)
#if (!defined(__ANDROID__)) || (!defined(ANDROID))
#include <GL/gl.h>
#else
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#endif

#include "platform.h"
#include "app_memory.h"

#include "render.cpp"

struct pong_game_state {
    v2 PaddleLeftP;
    v2 PaddleRightP;

    v2 BallP;
    v2 dBallP;
};

static void
LoopCall(loop_call *LoopCallInformation) {
    static bool32 Initialized = false;
    static render_queue RenderQueue;
    static pong_game_state PongGameState;
    static memory_area RenderMemoryArea = {};

    if(!Initialized) {
        Initialized = true;

        PongGameState.BallP = V2(0.0f);
        PongGameState.dBallP = V2(-1.0f, 0.0f);
        PongGameState.PaddleLeftP = V2(-0.9f, 0.0f);
        PongGameState.PaddleRightP = V2(0.9f, 0.0f);

        InitializeMemoryArea(&RenderMemoryArea, Megabytes(16), (uint8 *)LoopCallInformation->AppMemory.TransientMemory);
    }

    temporary_memory RenderMemory = BeginTemporaryMemory(&RenderMemoryArea);
    RenderQueue = GenerateRenderQueue(&RenderMemoryArea, Megabytes(4));

    if(LoopCallInformation->HardwareContextInformation.HardwareAcceleratedContextInitialized) {
        PushViewport(&RenderQueue, 0, 0, LoopCallInformation->WindowWidth, LoopCallInformation->WindowHeight);

        PushClear(&RenderQueue, 0.0f, 0.0f, 0.0f, 1.0f);

        PushRectangle(&RenderQueue, PongGameState.PaddleLeftP, V2(0.2f, 0.75f), V4(1.0f));
        PushRectangle(&RenderQueue, PongGameState.PaddleRightP, V2(0.2f, 0.75f), V4(1.0f));
        PushRectangle(&RenderQueue, PongGameState.BallP, V2(0.2f, 0.2f), V4(1.0f));

        ExecuteRenderCommands(&RenderQueue, &LoopCallInformation->HardwareContextInformation, &RenderMemoryArea);
    }

    EndTemporaryMemory(RenderMemory);
}
