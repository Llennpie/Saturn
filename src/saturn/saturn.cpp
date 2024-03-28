#include "saturn.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <thread>
#include <map>
#include <SDL2/SDL.h>

#include "data/dynos.cpp.h"
#include "saturn/imgui/saturn_imgui.h"
#include "saturn/imgui/saturn_imgui_machinima.h"
#include "saturn/imgui/saturn_imgui_chroma.h"
#include "libs/sdl2_scancode_to_dinput.h"
#include "pc/configfile.h"
#include "saturn/filesystem/saturn_projectfile.h"
#include "saturn/imgui/saturn_imgui_dynos.h"
#include "saturn/filesystem/saturn_locationfile.h"
#include "data/dynos.cpp.h"
#include "saturn/filesystem/saturn_registerfile.h"
#include "saturn/filesystem/saturn_animfile.h"
#include "saturn/cmd/saturn_cmd.h"
#include "saturn/saturn_rom_extract.h"
#include "saturn/saturn_timelines.h"

extern "C" {
#include "audio/external.h"
}

bool mario_exists;

bool camera_frozen = false;
float camera_speed = 0.0f;
float camera_focus = 1.f;
float camera_savestate_mult = 1.f;
bool camera_fov_smooth = false;
bool is_camera_moving;

bool camera_view_enabled;
bool camera_view_moving;
bool camera_view_zooming;
bool camera_view_rotating;
int camera_view_move_x;
int camera_view_move_y;

bool enable_head_rotations = false;
bool enable_shadows = false;
bool enable_dust_particles = false;
bool enable_torso_rotation = true;
float run_speed = 127.0f;
bool can_fall_asleep = false;
int saturnModelState = 0;
bool linkMarioScale = true;
bool is_spinning;
float spin_mult = 1.0f;

bool using_chainer;
int chainer_index;
enum MarioAnimID selected_animation = MARIO_ANIM_BREAKDANCE;
int current_anim_frame;
int current_anim_id;
int current_anim_length;
bool is_anim_playing = false;
bool is_anim_paused = false;
int paused_anim_frame;
struct AnimationState current_animation = {
    .custom = false,
    .hang = false,
    .loop = false,
    .speed = 1,
    .id = MarioAnimID::MARIO_ANIM_RUNNING,
};

float this_face_angle;

bool limit_fps = true;

// discord
bool has_discord_init;

// private
bool is_chroma_keying = false;
bool prev_quicks[3];
int lastCourseNum = -1;
int saturn_launch_timer;

float* active_key_float_value = &camera_fov;
bool* active_key_bool_value;

s32 active_data_type = KEY_FLOAT;
bool keyframe_playing;
bool k_popout_open;
bool k_popout_focused;
int mcam_timer = 0;
int k_current_frame = 0;
int k_previous_frame = 0;
int k_curr_curve_type = 0;

int k_current_anim = -1;

bool should_update_cam_from_keyframes = false;

std::map<std::string, std::pair<KeyframeTimeline, std::vector<Keyframe>>> k_frame_keys = {};

int k_last_passed_index = 0;
int k_distance_between;
int k_current_distance;
float k_static_increase_value;
int k_last_placed_frame;
bool k_loop;
bool k_animating_camera;
float k_c_pos0_incr;
float k_c_pos1_incr;
float k_c_pos2_incr;
float k_c_foc0_incr;
float k_c_foc1_incr;
float k_c_foc2_incr;
float k_c_rot0_incr;
float k_c_rot1_incr;
float k_c_rot2_incr;
bool has_set_initial_k_frames;

bool is_cc_editing;

bool autoChroma;
bool autoChromaLevel;
bool autoChromaObjects;

u8 activatedToads = 0;

f32 mario_headrot_yaw = 0;
f32 mario_headrot_pitch = 0;
f32 mario_headrot_speed = 10.0f;

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
#include "pc/cheats.h"
#include "game/save_file.h"
}

using namespace std;

#define FREEZE_CAMERA	    0x0800
#define CYCLE_EYE_STATE     0x0100
#define LOAD_ANIMATION      0x0200
#define TOGGLE_MENU         0x0400

unsigned int chromaKeyColorR = 0;
unsigned int chromaKeyColorG = 255;
unsigned int chromaKeyColorB = 0;

int autosaveDelay = -1;

u16 gChromaKeyColor = 0x07C1;
u16 gChromaKeyBackground = 0;

int keyResetter;

u8 godmode_temp_off = false;

bool extract_thread_began = false;
bool extraction_finished = false;
float extraction_progress = 1;

extern void saturn_run_chainer();

float key_increase_val(std::vector<float> vecfloat) {
    float next_val = vecfloat.at(k_last_passed_index + 1);
    float this_val = vecfloat.at(k_last_passed_index);

    return (next_val - this_val) / k_distance_between;
}

bool timeline_has_id(std::string id) {
    if (k_frame_keys.size() > 0) {
        for (auto& entry : k_frame_keys) {
            for (Keyframe keyframe : entry.second.second) {
                if (keyframe.timelineID == id)
                    return true;
            }
        }
    }

    return false;
}

// SATURN Machinima Functions

void saturn_update() {

    // Keybinds

    if (mario_exists) {
        if (gPlayer1Controller->buttonPressed & D_JPAD) {
            showMenu = !showMenu;
        }
        if (keyResetter == 6) {
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F2]) {
                if (gMarioState->action == ACT_DEBUG_FREE_MOVE) {
                    reset_camera(gCamera);
                    set_mario_action(gMarioState, ACT_IDLE, 0);
                }
                else set_mario_action(gMarioState, ACT_DEBUG_FREE_MOVE, 0);
                keyResetter = 0;
            }
        }
        if (!saturn_disable_sm64_input()) {
            if (gPlayer1Controller->buttonPressed & U_JPAD) camera_frozen = !camera_frozen;
            if (gPlayer1Controller->buttonPressed & L_JPAD) {
                if (!is_anim_playing) {
                    anim_play_button();
                } else {
                    is_anim_paused = !is_anim_paused;
                }
            }
            if (gPlayer1Controller->buttonPressed & R_JPAD) {
                current_animation.loop = !current_animation.loop;
            }
        }
    }

    if (keyResetter < 6)
        keyResetter += 1;

    // Machinima

    machinimaMode = (camera_frozen) ? 1 : 0;
    machinimaKeyframing = (keyframe_playing && active_data_type == KEY_CAMERA);

    if (camera_frozen && !saturn_disable_sm64_input()) {
        if (configMCameraMode == 2) {
            camera_view_enabled = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT];
            camera_view_moving = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK;
            camera_view_zooming = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK;
            camera_view_rotating = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK;
        } else if (configMCameraMode == 0) {
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_R]) {
                cameraRotateUp = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Y] & !saturn_disable_sm64_input();
                cameraRotateDown = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_H] & !saturn_disable_sm64_input();
                cameraRotateLeft = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_G] & !saturn_disable_sm64_input();
                cameraRotateRight = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J] & !saturn_disable_sm64_input();
                // Stop from moving
                cameraMoveForward = 0;
                cameraMoveBackward = 0;
                cameraMoveLeft = 0;
                cameraMoveRight = 0;
            } else {
                cameraMoveForward = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Y] & !saturn_disable_sm64_input();
                cameraMoveBackward = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_H] & !saturn_disable_sm64_input();
                cameraMoveLeft = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_G] & !saturn_disable_sm64_input();
                cameraMoveRight = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J] & !saturn_disable_sm64_input();
                // Stop from rotating
                cameraRotateUp = 0;
                cameraRotateDown = 0;
                cameraRotateLeft = 0;
                cameraRotateRight = 0;
            }
            cameraMoveUp = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_T] & !saturn_disable_sm64_input();
            cameraMoveDown = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_U] & !saturn_disable_sm64_input();
        }
        cameraRollLeft = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_V];
        cameraRollRight = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_B];
    }

    if (!keyframe_playing && !camera_frozen) {
        gLakituState.focHSpeed = camera_focus * camera_savestate_mult * 0.8f;
        gLakituState.focVSpeed = camera_focus * camera_savestate_mult * 0.3f;
        gLakituState.posHSpeed = camera_focus * camera_savestate_mult * 0.3f;
        gLakituState.posVSpeed = camera_focus * camera_savestate_mult * 0.3f;
    }

    camera_default_fov = camera_fov + 5.0f;

    //SDL_GetMouseState(&camera_view_move_x, &camera_view_move_y);

    if (gCurrLevelNum == LEVEL_SA || autoChroma) {
        if (!is_chroma_keying) is_chroma_keying = true;
    }

    //if (gCurrLevelNum == LEVEL_SA && !is_chroma_keying) {
        //is_chroma_keying = true;
        // Called once when entering Chroma Key Stage
        //prev_quicks[0] = enable_shadows;
        //prev_quicks[1] = enable_dust_particles;
        //prev_quicks[2] = configHUD;
        //enable_shadows = false;
        //enable_dust_particles = false;
        //configHUD = false;
    //}
    if (gCurrLevelNum != LEVEL_SA && !autoChroma) {
        if (!is_chroma_keying) is_chroma_keying = false;
        // Called once when exiting Chroma Key Stage
        //enable_shadows = prev_quicks[0];
        //enable_dust_particles = prev_quicks[1];
        //configHUD = prev_quicks[2];
    }

    if (splash_finished) saturn_launch_timer++;
    //std::cout << saturn_launch_timer << std::endl;
    if (gCurrLevelNum == LEVEL_SA && saturn_launch_timer <= 1 && splash_finished) {
        gMarioState->faceAngle[1] = 0;
        if (gCamera) { // i hate the sm64 camera system aaaaaaaaaaaaaaaaaa
            float dist = 0;
            s16 yaw, pitch;
            vec3f_set(gCamera->pos, 0.f, 192.f, 264.f);
            vec3f_set(gCamera->focus, 0.f, 181.f, 28.f);
            vec3f_copy(freezecamPos, gCamera->pos);
            vec3f_get_dist_and_angle(gCamera->pos, gCamera->focus, &dist, &pitch, &yaw);
            freezecamYaw = (float)yaw;
            freezecamPitch = (float)pitch;
            vec3f_copy(gLakituState.pos, gCamera->pos);
            vec3f_copy(gLakituState.focus, gCamera->focus);
            vec3f_copy(gLakituState.goalPos, gCamera->pos);
            vec3f_copy(gLakituState.goalFocus, gCamera->focus);
            gCamera->yaw = calculate_yaw(gCamera->focus, gCamera->pos);
            gLakituState.yaw = gCamera->yaw;
        }
    }

    // Keyframes

    if (!k_popout_open) k_popout_focused = false;
    if (keyframe_playing) {
        if (timeline_has_id("k_c_camera_pos0")) {
            // Prevents smoothing for sharper, more consistent panning
            gLakituState.focHSpeed = 15.f * camera_focus * 0.8f;
            gLakituState.focVSpeed = 15.f * camera_focus * 0.3f;
            gLakituState.posHSpeed = 15.f * camera_focus * 0.3f;
            gLakituState.posVSpeed = 15.f * camera_focus * 0.3f;
        }
        
        bool end = true;
        for (const auto& entry : k_frame_keys) {
            if (!saturn_keyframe_apply(entry.first, mcam_timer)) end = false;
        }
        if (end) {
            if (k_loop) mcam_timer = 0;
            else keyframe_playing = false;
        }

        if (timeline_has_id("k_angle"))
            gMarioState->faceAngle[1] = (s16)(this_face_angle * 182.04f);

        schroma_imgui_init();

        if (!end) {
            mcam_timer++;
            k_current_frame = (uint32_t)mcam_timer;
        }
    }

    if (camera_frozen && keyframe_playing) {
        should_update_cam_from_keyframes = false;
        vec3f_copy(gCamera->pos, freezecamPos);
        vec3f_set_dist_and_angle(gCamera->pos, gCamera->focus, 100, freezecamPitch, freezecamYaw);
        gLakituState.roll = freezecamRoll;
        vec3f_copy(gLakituState.pos, gCamera->pos);
        vec3f_copy(gLakituState.focus, gCamera->focus);
        vec3f_copy(gLakituState.goalPos, gCamera->pos);
        vec3f_copy(gLakituState.goalFocus, gCamera->focus);
        gCamera->yaw = calculate_yaw(gCamera->focus, gCamera->pos);
        gLakituState.yaw = gCamera->yaw;
    }

    // Animations

    if (mario_exists) {
        if (is_anim_paused) {
            gMarioState->marioObj->header.gfx.unk38.animFrame = current_anim_frame;
            gMarioState->marioObj->header.gfx.unk38.animFrameAccelAssist = current_anim_frame;
        } else if (is_anim_playing) {
            if (current_animation.hang) {
                if (is_anim_past_frame(gMarioState, (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08 - 1)) {
                    is_anim_paused = !is_anim_paused;
                }
            }

            if (is_anim_past_frame(gMarioState, (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08) || is_anim_at_end(gMarioState)) {
                if (current_animation.loop && !using_chainer) {
                    gMarioState->marioObj->header.gfx.unk38.animFrame = 0;
                    gMarioState->marioObj->header.gfx.unk38.animFrameAccelAssist = 0;
                } else {
                    if (using_chainer) {
                        chainer_index++;
                    } else {
                        if (gMarioState->action == ACT_DEBUG_FREE_MOVE)
                            set_mario_animation(gMarioState, MARIO_ANIM_A_POSE);
                        is_anim_playing = false;
                        is_anim_paused = false;
                    }
                }
            }

            /*if (selected_animation != gMarioState->marioObj->header.gfx.unk38.animID) {
                is_anim_playing = false;
                is_anim_paused = false;
            }*/

            current_anim_id = (int)gMarioState->marioObj->header.gfx.unk38.animID;
            if (gMarioState->action == ACT_IDLE || gMarioState->action == ACT_FIRST_PERSON || gMarioState->action == ACT_DEBUG_FREE_MOVE) {
                current_anim_frame = (int)gMarioState->marioObj->header.gfx.unk38.animFrame;
                current_anim_length = (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08 - 1;
            }

            if (current_animation.speed != 1.0f)
                gMarioState->marioObj->header.gfx.unk38.animAccel = current_animation.speed * 65535;

            if (using_chainer && is_anim_playing) saturn_run_chainer();
        }
    }

    // Misc

    mario_exists = (gMarioState->action != ACT_UNINITIALIZED & sCurrPlayMode != 2 & mario_loaded);

    if (!mario_exists) {
        is_anim_playing = false;
        is_anim_paused = false;
    }

    switch(saturnModelState) {
        case 0:     scrollModelState = 0;       break;
        case 1:     scrollModelState = 0x200;   break;  // Metal Cap
        case 2:     scrollModelState = 0x180;   break;  // Vanish Cap
        case 3:     scrollModelState = 0x380;   break;  // Both
        default:    scrollModelState = 0;       break;
    }

    if (linkMarioScale) {
        marioScaleSizeY = marioScaleSizeX;
        marioScaleSizeZ = marioScaleSizeX;
    }

    if (is_spinning && mario_exists) {
        gMarioState->faceAngle[1] += (s16)(spin_mult * 15 * 182.04f);
    }

    saturn_cmd_resume();

    // Autosave

    if (gCurrLevelNum != LEVEL_SA || gCurrAreaIndex != 3) {
        if (autosaveDelay <= 0) autosaveDelay = 30 * configAutosaveDelay;
        autosaveDelay--;
        if (autosaveDelay == 0) saturn_save_project("autosave.spj");
    }
}

float saturn_keyframe_setup_interpolation(std::string id, int frame, int* keyframe, bool* last) {
    KeyframeTimeline timeline = k_frame_keys[id].first;
    std::vector<Keyframe> keyframes = k_frame_keys[id].second;

    // Get the keyframe to interpolate from
    for (int i = 0; i < keyframes.size(); i++) {
        if (frame < keyframes[i].position) break;
        *keyframe = i;
    }

    // Stop/loop if reached the end
    *last = *keyframe + 1 == keyframes.size();
    if (*last) *keyframe -= 1; // Assign values from final keyframe

    // Interpolate, formulas from easings.net
    float x = (frame - keyframes[*keyframe].position) / (float)(keyframes[*keyframe + 1].position - keyframes[*keyframe].position);
    if (*last) x = 1;
    else if (keyframes[*keyframe].curve == InterpolationCurve::SLOW) x = x * x;
    else if (keyframes[*keyframe].curve == InterpolationCurve::FAST) x = 1 - (1 - x) * (1 - x);
    else if (keyframes[*keyframe].curve == InterpolationCurve::SMOOTH) x = x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
    else if (keyframes[*keyframe].curve == InterpolationCurve::WAIT) x = floor(x);

    return x;
}

// applies the values from keyframes to its destination, returns true if its the last frame, false if otherwise
bool saturn_keyframe_apply(std::string id, int frame) {
    KeyframeTimeline timeline = k_frame_keys[id].first;
    std::vector<Keyframe> keyframes = k_frame_keys[id].second;

    if (timeline.behavior == KFBEH_EVENT) {
        if (!keyframe_playing) return true;
        int idx = -1;
        for (int i = 0; i < keyframes.size(); i++) {
            if (keyframes[i].position == frame) {
                idx = i;
                break;
            }
        }
        if (idx == -1) return keyframes.size() == 0;
        if (timeline.type == KFTYPE_ANIM) {
            AnimationState* dest = (AnimationState*)timeline.dest;
            dest->custom = keyframes[idx].value[0] >= 1;
            dest->loop = keyframes[idx].value[1] >= 1;
            dest->hang = keyframes[idx].value[2] >= 1;
            dest->speed = keyframes[idx].value[3];
            dest->id = keyframes[idx].value[4];
            is_anim_playing = false;
            is_anim_paused = false;
            using_chainer = false;
            chainer_index = 0;
            anim_play_button();
        }
        if (timeline.type == KFTYPE_EXPRESSION) {
            Model* dest = (Model*)timeline.dest;
            for (int i = 0; i < keyframes[idx].value.size(); i++) {
                dest->Expressions[i].CurrentIndex = keyframes[idx].value[i];
            }
        }
        return idx + 1 == keyframes.size();
    }

    std::vector<float> values;
    bool last = true;
    if (keyframes.size() == 1) values = keyframes[0].value;
    else {
        int keyframe = 0;
        last = false;
        float x = saturn_keyframe_setup_interpolation(id, frame, &keyframe, &last);
        for (int i = 0; i < keyframes[keyframe].value.size(); i++) {
            values.push_back((keyframes[keyframe + 1].value[i] - keyframes[keyframe].value[i]) * x + keyframes[keyframe].value[i]);
        }    
    }
    if (timeline.type == KFTYPE_BOOL) *(bool*)timeline.dest = values[0] >= 1;
    if (timeline.type == KFTYPE_FLOAT) *(float*)timeline.dest = values[0];
    if (timeline.type == KFTYPE_COLOR) {
        ((float*)timeline.dest)[0] = values[0];
        ((float*)timeline.dest)[1] = values[1];
        ((float*)timeline.dest)[2] = values[2];
    }

    return last;
}

// returns true if the value is the same as if the keyframe was applied
bool saturn_keyframe_matches(std::string id, int frame) {
    KeyframeTimeline& timeline = k_frame_keys[id].first;
    std::vector<Keyframe> keyframes = k_frame_keys[id].second;

    if (timeline.behavior == KFBEH_EVENT) {
        bool place = timeline.eventPlace;
        timeline.eventPlace = false;
        return !place;
    }

    std::vector<float> expectedValues;
    if (keyframes.size() == 1) expectedValues = keyframes[0].value;
    else {
        int keyframe = 0;
        bool last = false;
        float x = saturn_keyframe_setup_interpolation(id, frame, &keyframe, &last);
        for (int i = 0; i < keyframes[keyframe].value.size(); i++) {
            expectedValues.push_back((keyframes[keyframe + 1].value[i] - keyframes[keyframe].value[i]) * x + keyframes[keyframe].value[i]);
        }
    }
    if (timeline.type == KFTYPE_BOOL) {
        if (*(bool*)timeline.dest != 0 != expectedValues[0] >= 1) return false;
        return true;
    }
    if (timeline.type == KFTYPE_FLOAT || timeline.type == KFTYPE_COLOR) {
        for (int i = 0; i < (timeline.type == KFTYPE_FLOAT ? 1 : 3); i++) {
            float value = ((float*)timeline.dest)[i];
            float distance = abs(value - expectedValues[i]);
            if (distance > pow(10, timeline.precision)) {
                if (id.find("cam") != string::npos) return !is_camera_moving;
                else return false;
            }
        }
    }

    return true;
}

// Play Animation

void saturn_play_animation(MarioAnimID anim) {
    force_set_mario_animation(gMarioState, anim);
    //set_mario_anim_with_accel(gMarioState, anim, anim_speed * 65535);
    is_anim_playing = true;
}

void saturn_play_keyframe() {
    if (k_frame_keys.size() <= 0) return;

    if (!keyframe_playing) {
        k_last_passed_index = 0;
        k_distance_between = 0;
        k_current_frame = 0;
        mcam_timer = 0;
        keyframe_playing = true;
    } else {
        if (k_current_frame > 0)
            keyframe_playing = false;
    }
}

// Copy

void saturn_copy_object(Vec3f from, Vec3f to) {
    vec3f_copy(from, to);
    vec3s_set(gMarioState->marioObj->header.gfx.angle, 0, gMarioState->faceAngle[1], 0);
}

Vec3f stored_mario_pos;
Vec3s stored_mario_angle;

void saturn_copy_mario() {
    vec3f_copy(stored_mario_pos, gMarioState->pos);
    vec3s_copy(stored_mario_angle, gMarioState->faceAngle);

    vec3f_copy(stored_camera_pos, gCamera->pos);
    vec3f_copy(stored_camera_focus, gCamera->focus);
}

void saturn_paste_mario() {
    if (machinimaCopying == 0) {
        vec3f_copy(gMarioState->pos, stored_mario_pos);
        vec3f_copy(gMarioState->marioObj->header.gfx.pos, stored_mario_pos);
        vec3s_copy(gMarioState->faceAngle, stored_mario_angle);
        vec3s_set(gMarioState->marioObj->header.gfx.angle, 0, stored_mario_angle[1], 0);
    }
    machinimaCopying = 1;
}

Vec3f pos_relative;
Vec3s foc_relative;
bool was_relative;

void saturn_copy_camera(bool relative) {
    if (relative) {
        pos_relative[0] = floor(gCamera->pos[0] - gMarioState->pos[0]);
        pos_relative[1] = floor(gCamera->pos[1] - gMarioState->pos[1]);
        pos_relative[2] = floor(gCamera->pos[2] - gMarioState->pos[2]);

        foc_relative[0] = floor(gCamera->focus[0] - gMarioState->pos[0]);
        foc_relative[1] = floor(gCamera->focus[1] - gMarioState->pos[1]);
        foc_relative[2] = floor(gCamera->focus[2] - gMarioState->pos[2]);
    } else {
        vec3f_copy(stored_camera_pos, gCamera->pos);
        vec3f_copy(stored_camera_focus, gCamera->focus);
    }
    was_relative = relative;
}

void saturn_paste_camera() {
    if (was_relative) {
        stored_camera_pos[0] = floor(gMarioState->pos[0] + pos_relative[0]);
        stored_camera_pos[1] = floor(gMarioState->pos[1] + pos_relative[1]);
        stored_camera_pos[2] = floor(gMarioState->pos[2] + pos_relative[2]);
        stored_camera_focus[0] = floor(gMarioState->pos[0] + foc_relative[0]);
        stored_camera_focus[1] = floor(gMarioState->pos[1] + foc_relative[1]);
        stored_camera_focus[2] = floor(gMarioState->pos[2] + foc_relative[2]);
    }
    machinimaCopying = 1;
}

// Debug

void saturn_print(const char* text) {
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RSHIFT])
        printf(text);
}

// Other

SDL_Scancode saturn_key_to_scancode(unsigned int configKey[]) {
    for (int i = 0; i < MAX_BINDS; i++) {
        unsigned int key = configKey[i];

        if (key >= 0 && key < 0xEF) {
            for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
                if(scanCodeToKeyNum[i] == key) return (SDL_Scancode)i;
            }
        }
        return SDL_SCANCODE_UNKNOWN;
    }
}

const char* saturn_get_stage_name(int courseNum) {
    switch(courseNum) {
        case LEVEL_SA: return "Chroma Key Stage"; break;
        case LEVEL_CASTLE: return "Peach's Castle"; break;
        case LEVEL_CASTLE_GROUNDS: return "Castle Grounds"; break;
        case LEVEL_CASTLE_COURTYARD: return "Castle Courtyard"; break;
        case LEVEL_BOB: return "Bob-omb Battlefield"; break;
        case LEVEL_CCM: return "Cool, Cool Mountain"; break;
        case LEVEL_WF: return "Whomp's Fortress"; break;
        case LEVEL_JRB: return "Jolly Roger Bay"; break;
        case LEVEL_PSS: return "Princess's Secret Slide"; break;
        case LEVEL_TOTWC: return "Tower of the Wing Cap"; break;
        case LEVEL_BITDW: return "Bowser in the Dark World"; break;
        case LEVEL_BBH: return "Big Boo's Haunt"; break;
        case LEVEL_HMC: return "Hazy Maze Cave"; break;
        case LEVEL_COTMC: return "Cavern of the Metal Cap"; break;
        case LEVEL_LLL: return "Lethal Lava Land"; break;
        case LEVEL_SSL: return "Shifting Sand Land"; break;
        case LEVEL_VCUTM: return "Vanish Cap Under the Moat"; break;
        case LEVEL_DDD: return "Dire, Dire Docks"; break;
        case LEVEL_BITFS: return "Bowser in the Fire Sea"; break;
        case LEVEL_SL: return "Snowman's Land"; break;
        case LEVEL_WDW: return "Wet-Dry World"; break;
        case LEVEL_TTM: return "Tall, Tall Mountain"; break;
        case LEVEL_THI: return "Tiny, Huge Island"; break;
        case LEVEL_TTC: return "Tick Tock Clock"; break;
        case LEVEL_WMOTR: return "Wing Mario Over the Rainbow"; break;
        case LEVEL_RR: return "Rainbow Ride"; break;
        case LEVEL_BITS: return "Bowser in the Sky"; break;

        default: return "Unknown"; break;
    }
}

std::thread extract_thread;

s32 saturn_begin_extract_rom_thread() {
    if (extract_thread_began) return extraction_finished;
    extract_thread_began = true;
    extraction_finished = false;
    extract_thread = std::thread([]() {
        saturn_extract_rom(EXTRACT_TYPE_ALL);
        extraction_finished = true;
    });
    return false;
}

void saturn_do_load() {
    if (!(save_file_get_flags() & SAVE_FLAG_TALKED_TO_ALL_TOADS)) DynOS_Gfx_GetPacks().Clear();
    DynOS_Opt_Init();
    //model_details = "" + std::to_string(DynOS_Gfx_GetPacks().Count()) + " model pack";
    //if (DynOS_Gfx_GetPacks().Count() != 1) model_details += "s";
    saturn_imgui_init();
    saturn_load_locations();
    saturn_launch_timer = 0;
    saturn_cmd_registers_load();
    saturn_load_favorite_anims();
    saturn_fill_data_table();
}
void saturn_on_splash_finish() {
    splash_finished = true;
}
s32 saturn_should_show_splash() {
    return configSaturnSplash;
}

bool saturn_timeline_exists(const char* name) {
    return k_frame_keys.find(name) != k_frame_keys.end();
}