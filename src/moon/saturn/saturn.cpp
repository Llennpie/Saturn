#include "saturn.h"
#include "moon/mod-engine/hooks/hook.h"

#include "moon/utils/moon-env.h"
#include "moon/fs/moonfs.h"
#include "moon/ui/screens/addons/addons-view.h"
#include "moon/mod-engine/engine.h"
#include "moon/imgui/imgui_impl.h"
#include "pc/configfile.h"
#include "pc/controller/controller_keyboard.h"

#include "saturn_colors.h"
#include "saturn_textures.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include <dirent.h>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

extern "C" {
#include "game/camera.h"
#include "game/level_update.h"
#include "engine/level_script.h"
#include "game/mario.h"
#include "game/area.h"
#include "sm64.h"
#include "game/behavior_actions.h"
#include "game/behaviors/yoshi.inc.h"
#include "pc/cheats.h"
#include <mario_animation_ids.h>
}

bool camera_frozen;
bool enable_head_rotations;
bool enable_shadows;
bool enable_god;
bool enable_dust_particles;
bool enable_fall_asleep;

bool show_menu_bar;

float camera_speed = 0.8f;
bool enable_night_skybox;
bool enable_yoshi;

enum MarioAnimID selected_animation = MARIO_ANIM_BREAKDANCE;
bool loop_animation;
float anim_speed = 1.0f;
bool is_anim_playing;
bool is_anim_paused;
int anim_paused_index = 0;
bool is_spazzing;

int cur_anim_index = 0;
int cur_anim_frame = 0;
int cur_anim_length = 0;

// Second Check

bool mario_exists;

bool has_changed_chroma_sky;
bool has_changed_yoshi;
bool has_changed_anim_pause_index;
bool has_changed_spaz;
int every_other;

namespace MoonInternal {

    // Machinima

    void freeze_camera() {
        camera_frozen = !camera_frozen;
        //camVelSpeed = 1.0f;
    }
    void cycle_eye_state(int cycle) {
        current_eye_state += cycle;
    }

    // Play Animation

    void saturn_play_animation(MarioAnimID anim) {
        if (is_anim_paused || !mario_exists) return;
        set_mario_animation(gMarioState, anim);
        is_anim_playing = true;
    }

    // Setup Module

    void setupSaturnModule(string status){
        if(status == "PreStartup"){

            Moon::registerHookListener({.hookName = WINDOW_API_INIT, .callback = [&](HookCall call){
                show_menu_bar = false;

                camera_frozen = false;
                enable_shadows = true;
                enable_god = true;
                enable_yoshi = false;

                Cheats.EnableCheats = true;
                Cheats.InfiniteLives = true;
                Cheats.ExitAnywhere = true;

                MoonInternal::load_cc_directory();
                MoonInternal::load_cc_file(cc_array[configColorCode]);
                apply_editor_from_cc();
                
                // custom textures
                current_eye_state = 0;
                saturn_load_eye_array();
                custom_eye_name = "saturn/eyes/" + eye_array[0];
                saturn_eye_swap();

                saturn_load_emblem_array();
                custom_emblem_name = "saturn/blank";
                saturn_emblem_swap();

                saturn_load_stache_array();
                saturn_load_button_array();
                saturn_load_sideburn_array();

                // addons
                if(texturePackList.empty()){
                    texturePackList.clear();
                    copy(Moon::addons.begin(), Moon::addons.end(), back_inserter(texturePackList));
                    reverse(texturePackList.begin(), texturePackList.end());
                }
                currentPack = NULL;
            }});

            Moon::registerHookListener({.hookName = WINDOW_API_HANDLE_EVENTS, .callback = [&](HookCall call){
                SDL_Event* ev = (SDL_Event*) call.baseArgs["event"];
                switch (ev->type){
                    case SDL_KEYDOWN:
                        if(ev->key.keysym.sym == SDLK_f){
                            if (accept_input)
                                freeze_camera();
                        }
                        if(ev->key.keysym.sym == SDLK_g){
                            if (accept_input)
                                saturn_play_animation(selected_animation);
                        }
                        if(ev->key.keysym.sym == SDLK_h){
                            if (accept_input && is_anim_playing)
                                is_anim_paused = !is_anim_paused;
                        }
                        if(ev->key.keysym.sym == SDLK_F1){
                            show_menu_bar = !show_menu_bar;
                        }
                    case SDL_CONTROLLERBUTTONDOWN:
                        if (ev->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
                            freeze_camera();
                        }
                        if(ev->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT){
                            saturn_play_animation(selected_animation);
                        }
                        if(ev->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT){
                            if (is_anim_playing)
                                is_anim_paused = !is_anim_paused;
                        }
                        if(ev->cbutton.button == SDL_CONTROLLER_BUTTON_BACK){
                            show_menu_bar = !show_menu_bar;
                        }

                    break;
                }
            }});

            Moon::registerHookListener({.hookName = GFX_PRE_START_FRAME, .callback = [&](HookCall call){
                // Machinima Camera

                machinimaMode = (camera_frozen) ? 1 : 0;
                camVelSpeed = camera_speed;

                /* OLD CAMERA...
                
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

                // Animations

                if (mario_exists) {
                    if (is_anim_paused) {
                        anim_speed = 1.0f;
                        gMarioState->marioObj->header.gfx.unk38.animFrame = cur_anim_frame;
                        gMarioState->marioObj->header.gfx.unk38.animFrameAccelAssist = cur_anim_frame;
                    } else if (is_anim_playing) {
                        if (anim_speed != 1.0f) {
                            gMarioState->marioObj->header.gfx.unk38.animAccel = anim_speed * 65535;
                        }

                        if (is_anim_at_end(gMarioState)) {
                            if (loop_animation) {
                                gMarioState->marioObj->header.gfx.unk38.animFrame = 0;
                                gMarioState->marioObj->header.gfx.unk38.animFrameAccelAssist = 0;
                            } else {
                                is_anim_playing = false;
                            }
                        }
                        if (selected_animation != gMarioState->marioObj->header.gfx.unk38.animID) {
                            is_anim_playing = false;
                        }

                        cur_anim_index = (int)gMarioState->marioObj->header.gfx.unk38.animID;
                        if (gMarioState->action == ACT_IDLE || gMarioState->action == ACT_FIRST_PERSON) {
                            cur_anim_frame = (int)gMarioState->marioObj->header.gfx.unk38.animFrame;
                            cur_anim_length = (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08 - 1;
                        }
                    }
                }

                if (is_spazzing && mario_exists) {
                    anim_speed = -1.0f;
                    loop_animation = true;
                    is_anim_paused = false;
                    saturn_play_animation(MARIO_ANIM_A_POSE);
                    has_changed_spaz = true;
                    if (gMarioState->marioObj->header.gfx.unk38.animFrame < -32) {
                        gMarioState->marioObj->header.gfx.unk38.animFrame = 1;
                        gMarioState->marioObj->header.gfx.unk38.animFrameAccelAssist = 1;
                    }
                }
                if (has_changed_spaz && !is_spazzing) {
                    has_changed_spaz = false;
                    anim_speed = 1.0f;
                }

                // Chroma Key

                if (gCurrLevelNum == LEVEL_SA && !has_changed_chroma_sky) {
                    has_changed_chroma_sky = true;
                    saturn_chroma_sky_swap();
                    defaultColorChromaKeyR = 0;
                    defaultColorChromaKeyG = 255;
                    defaultColorChromaKeyB = 0;
                }
                if (gCurrLevelNum != LEVEL_SA && has_changed_chroma_sky) {
                    has_changed_chroma_sky = false;
                    saturn_chroma_sky_swap();
                    defaultColorChromaKeyR = 255;
                    defaultColorChromaKeyG = 255;
                    defaultColorChromaKeyB = 255;
                }

                // Yoshi

                enableYoshi = (enable_yoshi) ? 1 : 0;

                // Check

                mario_exists = (gMarioState->action != ACT_UNINITIALIZED & sCurrPlayMode != 2 & mario_loaded);
            }});
        }
    }
}