#include "saturn/saturn.h"

#include <string>
#include <iostream>
#include <SDL2/SDL.h>

#include "data/dynos.cpp.h"
#include "saturn/imgui/saturn_imgui.h"

bool camera_frozen;
float camera_speed = 0.0f;

bool enable_head_rotations = false;
bool enable_shadows = true;
bool enable_dust_particles = false;

bool is_anim_playing = false;
enum MarioAnimID selected_animation = MARIO_ANIM_BREAKDANCE;
bool is_anim_looped = false;

// private
bool is_chroma_keying = false;
bool prev_quicks[3];

extern "C" {
#include "game/camera.h"
#include "game/area.h"
#include "game/level_update.h"
#include "game/game_init.h"
#include "data/dynos.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include <mario_animation_ids.h>
}

using namespace std;

#define FREEZE_CAMERA	    0x0800
#define CYCLE_EYE_STATE     0x0100
#define LOAD_ANIMATION      0x0200
#define TOGGLE_MENU         0x0400



// SATURN Machinima Functions

void saturn_update() {

    /*
    if (camera_frozen == true) {
        if (set_cam_angle(0) != CAM_ANGLE_MARIO) {
            gLakituState.focVSpeed = camera_speed;
            gLakituState.focHSpeed = camera_speed;
        }
        gLakituState.posVSpeed = camera_speed;
        gLakituState.posHSpeed = camera_speed;
        gCamera->nextYaw = calculate_yaw(gLakituState.focus, gLakituState.pos);
        gCamera->yaw = gCamera->nextYaw;
        gCameraMovementFlags &= ~CAM_MOVE_FIX_IN_PLACE;
    }
    */
    machinimaMode = (camera_frozen) ? 1 : 0;

    if (is_anim_playing && is_anim_at_end(gMarioState)) {
        if (is_anim_looped) {
            gMarioState->marioObj->header.gfx.unk38.animFrame = 0;
        } else {
            is_anim_playing = false;
        }
    }
    if (is_anim_playing && selected_animation != gMarioState->marioObj->header.gfx.unk38.animID) {
        is_anim_playing = false;
    }

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

    // Keybinds

    if (gPlayer1Controller->buttonPressed & FREEZE_CAMERA) {
        camera_frozen = !camera_frozen;
    }

    if (gPlayer1Controller->buttonPressed & CYCLE_EYE_STATE) {
        if (scrollEyeState != 8) {
            scrollEyeState += 1;
        } else {
            scrollEyeState = 0;
        }
    }
    if (gPlayer1Controller->buttonPressed & LOAD_ANIMATION) {
        saturn_play_animation(selected_animation);
    }
}

// Play Animation

void saturn_play_animation(MarioAnimID anim) {
    set_mario_animation(gMarioState, anim);
    is_anim_playing = true;
}