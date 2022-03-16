#include "saturn/saturn.h"

#include <string>
#include <iostream>
#include <SDL2/SDL.h>

#include "data/dynos.cpp.h"
#include "saturn/imgui/saturn_imgui.h"

bool mario_exists;

bool camera_frozen;
float camera_speed = 0.0f;

bool enable_head_rotations = false;
bool enable_shadows = true;
bool enable_dust_particles = false;

bool is_anim_playing = false;
enum MarioAnimID selected_animation = MARIO_ANIM_BREAKDANCE;
bool is_anim_looped = false;
int current_anim_frame;
int current_anim_id;
int current_anim_length;
bool is_anim_paused = false;
int paused_anim_frame;

// private
bool is_chroma_keying = false;
bool prev_quicks[3];
int lastCourseNum = -1;

extern "C" {
#include "game/camera.h"
#include "game/area.h"
#include "game/level_update.h"
#include "engine/level_script.h"
#include "game/game_init.h"
#include "data/dynos.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include <mario_animation_ids.h>
#include <sm64.h>
}

using namespace std;

#define FREEZE_CAMERA	    0x0800
#define CYCLE_EYE_STATE     0x0100
#define LOAD_ANIMATION      0x0200
#define TOGGLE_MENU         0x0400

unsigned int chromaKeyColorR = 0;
unsigned int chromaKeyColorG = 255;
unsigned int chromaKeyColorB = 0;

u16 gChromaKeyColor = 0x07C1;

// SATURN Machinima Functions

void saturn_update() {

    // Machinima

    machinimaMode = (camera_frozen) ? 1 : 0;

    if (gCurrLevelNum == LEVEL_SA && !is_chroma_keying) {
        is_chroma_keying = true;
        // Called once when entering Chroma Key Stage
        prev_quicks[0] = enable_shadows;
        prev_quicks[1] = enable_dust_particles;
        prev_quicks[2] = configHUD;
        enable_shadows = false;
        enable_dust_particles = false;
        configHUD = false;
    }
    if (is_chroma_keying && gCurrLevelNum != LEVEL_SA) {
        is_chroma_keying = false;
        // Called once when exiting Chroma Key Stage
        enable_shadows = prev_quicks[0];
        enable_dust_particles = prev_quicks[1];
        configHUD = prev_quicks[2];
    }

    // Animations

    if (mario_exists) {
        if (is_anim_paused) {
            gMarioState->marioObj->header.gfx.unk38.animFrame = current_anim_frame;
            gMarioState->marioObj->header.gfx.unk38.animFrameAccelAssist = current_anim_frame;
        } else if (is_anim_playing) {
            if (is_anim_at_end(gMarioState)) {
                if (is_anim_looped) {
                    gMarioState->marioObj->header.gfx.unk38.animFrame = 0;
                    gMarioState->marioObj->header.gfx.unk38.animFrameAccelAssist = 0;
                } else {
                    is_anim_playing = false;
                }
            }
            if (selected_animation != gMarioState->marioObj->header.gfx.unk38.animID) {
                is_anim_playing = false;
            }

            current_anim_id = (int)gMarioState->marioObj->header.gfx.unk38.animID;
            if (gMarioState->action == ACT_IDLE || gMarioState->action == ACT_FIRST_PERSON) {
                current_anim_frame = (int)gMarioState->marioObj->header.gfx.unk38.animFrame;
                current_anim_length = (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08 - 1;
            }
        }
    }

    // Misc

    mario_exists = (gMarioState->action != ACT_UNINITIALIZED & sCurrPlayMode != 2 & mario_loaded);
}

// Play Animation

void saturn_play_animation(MarioAnimID anim) {
    set_mario_animation(gMarioState, anim);
    is_anim_playing = true;
}

void saturn_warp_to(s16 destLevel, s16 destArea = 0x01, s16 destWarpNode = 0x0A) {
    if (!mario_exists)
        return;

    mario_loaded = false;
    initiate_warp(destLevel, destArea, destWarpNode, 0);
    fade_into_special_warp(0,0);
}