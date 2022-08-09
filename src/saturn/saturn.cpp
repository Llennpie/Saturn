#include "saturn/saturn.h"

#include <string>
#include <iostream>
#include <SDL2/SDL.h>

#include "data/dynos.cpp.h"
#include "saturn/imgui/saturn_imgui.h"

bool mario_exists;

bool camera_frozen;
float camera_speed = 0.0f;
float camera_fov = 50.0f;

bool camera_view_enabled;
bool camera_view_moving;
bool camera_view_zooming;
bool camera_view_rotating;
int camera_view_move_x;
int camera_view_move_y;

bool enable_head_rotations = false;
bool enable_shadows = true;
bool enable_dust_particles = false;
bool can_fall_asleep = false;
int saturnModelState = 0;

bool is_anim_playing = false;
enum MarioAnimID selected_animation = MARIO_ANIM_BREAKDANCE;
bool is_anim_looped = false;
int current_anim_frame;
int current_anim_id;
int current_anim_length;
bool is_anim_paused = false;
int paused_anim_frame;

bool limit_fps = true;

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
#include "pc/controller/controller_keyboard.h"
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

    if (camera_frozen) {
        if (configMCameraMode == 2) {
            camera_view_enabled = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT];
            camera_view_moving = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK;
            camera_view_zooming = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK;
            camera_view_rotating = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK;
        } else if (configMCameraMode == 0) {
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_R]) {
                cameraRotateUp = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Y] & accept_text_input;
                cameraRotateDown = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_H] & accept_text_input;
                cameraRotateLeft = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_G] & accept_text_input;
                cameraRotateRight = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J] & accept_text_input;
            } else {
                cameraMoveForward = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Y] & accept_text_input;
                cameraMoveBackward = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_H] & accept_text_input;
                cameraMoveLeft = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_G] & accept_text_input;
                cameraMoveRight = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J] & accept_text_input;
            }
            cameraMoveUp = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_T] & accept_text_input;
            cameraMoveDown = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_U] & accept_text_input;
        }
    }

    camera_default_fov = camera_fov + 5.0f;

    //SDL_GetMouseState(&camera_view_move_x, &camera_view_move_y);

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

    switch(saturnModelState) {
        case 0:     scrollModelState = 0;       break;
        case 1:     scrollModelState = 0x200;   break;  // Metal Cap
        case 2:     scrollModelState = 0x180;   break;  // Vanish Cap
        case 3:     scrollModelState = 0x380;   break;  // Both
        default:    scrollModelState = 0;       break;
    }
}

// Play Animation

void saturn_play_animation(MarioAnimID anim) {
    set_mario_animation(gMarioState, anim);
    is_anim_playing = true;
}

// Debug

void saturn_print(const char* text) {
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RSHIFT])
        printf(text);
}