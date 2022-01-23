#ifndef GAME_INIT_H
#define GAME_INIT_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include <PR/os_cont.h>
#include <PR/os_message.h>

#include "types.h"
#include "memory.h"

#define GFX_POOL_SIZE 6400

struct GfxPool {
    Gfx buffer[GFX_POOL_SIZE];
    struct SPTask spTask;
};

struct DemoInput
{
    u8 timer; // time until next input. if this value is 0, it means the demo is over
    s8 rawStickX;
    s8 rawStickY;
    u8 buttonMask;
};

extern struct Controller gControllers[3];
extern OSContStatus gControllerStatuses[4];
extern OSContPad gControllerPads[4];
extern OSMesgQueue gGameVblankQueue;
extern OSMesgQueue D_80339CB8;
extern OSMesg D_80339CD0;
extern OSMesg D_80339CD4;
extern struct VblankHandler gGameVblankHandler;
extern uintptr_t gPhysicalFrameBuffers[3];
extern uintptr_t gPhysicalZBuffer;
extern void *mario_anims_pool;
extern void *demo_anims_pool;
extern struct SPTask *gGfxSPTask;
extern struct AllocOnlyPool *gGfxAllocOnlyPool;
extern Gfx *gDisplayListHeadInChunk;
extern Gfx *gDisplayListEndInChunk;
extern struct GfxPool *gGfxPool;
extern u8 gControllerBits;
extern s8 gEepromProbe;

extern void (*D_8032C6A0)(void);
extern struct Controller *gPlayer1Controller;
extern struct Controller *gPlayer2Controller;
extern struct Controller *gPlayer3Controller;
extern struct DemoInput *gCurrDemoInput;
extern u16 gDemoInputListID;
extern struct DemoInput gRecordedDemoInput;

// this area is the demo input + the header. when the demo is loaded in, there is a header the size
// of a single word next to the input list. this word is the current ID count.
extern struct MarioAnimation mario_anims_table;
extern struct MarioAnimation gDemo;

extern u8* gDemoInputs[];

extern u16 frameBufferIndex;
extern u32 gGlobalTimer;

void setup_game_memory(void);
void thread5_game_loop(UNUSED void *arg);
void clear_frame_buffer(s32 color);
void clear_viewport(Vp *viewport, s32 color);
void make_viewport_clip_rect(Vp *viewport);
void init_render_image(void);
void end_master_display_list(void);
void rendering_init(void);
void config_gfx_pool(void);
void display_and_vsync(void);

Gfx **alloc_next_dl(void);
#define gDisplayListHead (*(gDisplayListEndInChunk - gDisplayListHeadInChunk >= 2 ? &gDisplayListHeadInChunk : alloc_next_dl()))

#endif // GAME_INIT_H
