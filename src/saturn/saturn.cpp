#include "saturn.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <SDL2/SDL.h>

#include "data/dynos.cpp.h"
#include "saturn/imgui/saturn_imgui.h"
#include "saturn/imgui/saturn_imgui_machinima.h"
#include "libs/sdl2_scancode_to_dinput.h"
#include "pc/configfile.h"

bool mario_exists;

bool camera_frozen;
float camera_speed = 0.0f;
float camera_focus = 1.f;
bool camera_fov_smooth = false;

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

bool is_custom_anim;
bool using_chainer;
int chainer_index;
bool is_anim_playing = false;
enum MarioAnimID selected_animation = MARIO_ANIM_BREAKDANCE;
bool is_anim_looped = false;
float anim_speed = 1.0f;
int current_anim_frame;
int current_anim_id;
int current_anim_length;
bool is_anim_paused = false;
int paused_anim_frame;

bool limit_fps = true;

// discord
bool has_discord_init;

// private
bool is_chroma_keying = false;
bool prev_quicks[3];
int lastCourseNum = -1;

float* active_key_float_value = &camera_fov;
bool* active_key_bool_value;

s32 active_data_type = KEY_FLOAT;
bool keyframe_playing;
bool k_popout_open;
int mcam_timer = 0;
int k_current_frame = 0;

std::vector<uint32_t> k_frame_keys = {0};
std::vector<float> k_v_float_keys = {0.f};
std::vector<bool> k_v_bool_keys = {false};

int k_last_passed_index = 0;
int k_distance_between;
int k_current_distance;
float k_static_increase_value;
int k_last_placed_frame;
bool k_loop;
bool k_animating_camera;
std::vector<float> k_c_pos1_keys = {0.f};
std::vector<float> k_c_pos2_keys = {0.f};
std::vector<float> k_c_foc0_keys = {0.f};
std::vector<float> k_c_foc1_keys = {0.f};
std::vector<float> k_c_foc2_keys = {0.f};
std::vector<float> k_c_rot0_keys = {0.f};
std::vector<float> k_c_rot1_keys = {0.f};
std::vector<float> k_c_rot2_keys = {0.f};
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

std::string model_details;
std::string cc_details;
bool is_cc_editing;

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
u16 gChromaKeyBackground = 0;

int keyResetter;

extern void saturn_run_chainer();

float key_increase_val(std::vector<float> vecfloat) {
    float next_val = vecfloat.at(k_last_passed_index + 1);
    float this_val = vecfloat.at(k_last_passed_index);

    return (next_val - this_val) / k_distance_between;
}

// SATURN Machinima Functions

void saturn_update() {

    // Keybinds

    if (mario_exists) {
        if (gPlayer1Controller->buttonPressed & D_JPAD) {
            showMenu = !showMenu;
            if (!showMenu) accept_text_input = true;
        }
        if (keyResetter == 6) {
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F2]) {
                if (gMarioState->action == ACT_IDLE) set_mario_action(gMarioState, ACT_DEBUG_FREE_MOVE, 0);
                else set_mario_action(gMarioState, ACT_IDLE, 0);
                keyResetter = 0;
            }
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F3]) {
                saturn_play_keyframe(active_data_type);
                keyResetter = 0;
            }
        }
        if (accept_text_input) {
            if (gPlayer1Controller->buttonPressed & U_JPAD) camera_frozen = !camera_frozen;
            if (gPlayer1Controller->buttonPressed & L_JPAD) {
                if (!is_anim_playing) {
                    anim_play_button();
                } else {
                    is_anim_paused = !is_anim_paused;
                }
            }
            if (gPlayer1Controller->buttonPressed & R_JPAD) {
                is_anim_looped = !is_anim_looped;
            }
        }
    }

    if (keyResetter < 6)
        keyResetter += 1;

    // Machinima

    machinimaMode = (camera_frozen) ? 1 : 0;
    machinimaKeyframing = (keyframe_playing && active_data_type == KEY_CAMERA);

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
                // Stop from moving
                cameraMoveForward = 0;
                cameraMoveBackward = 0;
                cameraMoveLeft = 0;
                cameraMoveRight = 0;
            } else {
                cameraMoveForward = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Y] & accept_text_input;
                cameraMoveBackward = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_H] & accept_text_input;
                cameraMoveLeft = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_G] & accept_text_input;
                cameraMoveRight = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J] & accept_text_input;
                // Stop from rotating
                cameraRotateUp = 0;
                cameraRotateDown = 0;
                cameraRotateLeft = 0;
                cameraRotateRight = 0;
            }
            cameraMoveUp = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_T] & accept_text_input;
            cameraMoveDown = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_U] & accept_text_input;
        }
    }

    if (!keyframe_playing && !camera_frozen) {
        gLakituState.focHSpeed = camera_focus * 0.8f;
        gLakituState.focVSpeed = camera_focus * 0.3f;
        gLakituState.posHSpeed = camera_focus * 0.3f;
        gLakituState.posVSpeed = camera_focus * 0.3f;
    }

    camera_default_fov = camera_fov + 5.0f;

    //SDL_GetMouseState(&camera_view_move_x, &camera_view_move_y);

    if (configEditorAlwaysChroma || gCurrLevelNum == LEVEL_SA) {
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
    if (gCurrLevelNum != LEVEL_SA && !configEditorAlwaysChroma) {
        if (!is_chroma_keying) is_chroma_keying = false;
        // Called once when exiting Chroma Key Stage
        //enable_shadows = prev_quicks[0];
        //enable_dust_particles = prev_quicks[1];
        //configHUD = prev_quicks[2];
    }

    // Animations

    if (mario_exists) {
        if (is_anim_paused) {
            gMarioState->marioObj->header.gfx.unk38.animFrame = current_anim_frame;
            gMarioState->marioObj->header.gfx.unk38.animFrameAccelAssist = current_anim_frame;
        } else if (is_anim_playing) {
            if (is_anim_past_frame(gMarioState, (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08) || is_anim_at_end(gMarioState)) {
                if (is_anim_looped && !using_chainer) {
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
            if (selected_animation != gMarioState->marioObj->header.gfx.unk38.animID) {
                is_anim_playing = false;
                is_anim_paused = false;
            }

            current_anim_id = (int)gMarioState->marioObj->header.gfx.unk38.animID;
            if (gMarioState->action == ACT_IDLE || gMarioState->action == ACT_FIRST_PERSON || gMarioState->action == ACT_DEBUG_FREE_MOVE) {
                current_anim_frame = (int)gMarioState->marioObj->header.gfx.unk38.animFrame;
                current_anim_length = (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08 - 1;
            }

            if (anim_speed != 1.0f)
                gMarioState->marioObj->header.gfx.unk38.animAccel = anim_speed * 65535;

            if (using_chainer && is_anim_playing) saturn_run_chainer();
        }
    }

    // Keyframes

    if (keyframe_playing) {
        if (mcam_timer < (60 * 10)) {
            mcam_timer++;
        } else {
            mcam_timer = 0;
        }
    }

    if (keyframe_playing) {
        k_current_frame = (uint32_t)(mcam_timer / 10);

        // Prevents smoothing for sharper, more consistent panning
        gLakituState.focHSpeed = 10.f * camera_focus * 0.8f;
        gLakituState.focVSpeed = 10.f * camera_focus * 0.3f;
        gLakituState.posHSpeed = 10.f * camera_focus * 0.3f;
        gLakituState.posVSpeed = 10.f * camera_focus * 0.3f;

        auto it1 = std::find(k_frame_keys.begin(), k_frame_keys.end(), k_current_frame);
        if (it1 != k_frame_keys.end()) {
            // Runs when current frame is on a keyframe
            k_last_passed_index = (it1 - k_frame_keys.begin());
            if (active_data_type == KEY_BOOL)           *active_key_bool_value = k_v_bool_keys.at(k_last_passed_index);
            if (k_frame_keys.size() - 1 == k_last_passed_index) {
                // Sequencer reached the last keyframe
                if (k_loop) {
                    k_last_passed_index = 0;
                    mcam_timer = 0;
                    k_current_frame = 0;

                    if (active_data_type == KEY_FLOAT)      *active_key_float_value = k_v_float_keys[0];
                    if (active_data_type == KEY_CAMERA) {
                        *active_key_float_value = k_v_float_keys[0];
                        mCameraKeyPos[0] = k_v_float_keys[0];
                        mCameraKeyPos[1] = k_c_pos1_keys[0];
                        mCameraKeyPos[2] = k_c_pos2_keys[0];
                        mCameraKeyFoc[0] = k_c_foc0_keys[0];
                        mCameraKeyFoc[1] = k_c_foc1_keys[0];
                        mCameraKeyFoc[2] = k_c_foc2_keys[0];
                        mCameraKeyYaw = k_c_rot0_keys[0];
                        mCameraKeyPitch = k_c_rot1_keys[0];
                    }
                } else {
                    keyframe_playing = false;

                    if (active_data_type == KEY_FLOAT)      *active_key_float_value = k_v_float_keys.at(k_last_passed_index);
                    if (active_data_type == KEY_BOOL)       *active_key_bool_value = k_v_bool_keys.at(k_last_passed_index);
                }
            } else {
                k_current_distance = 0;
                k_distance_between = k_frame_keys.at(k_last_passed_index + 1) - k_current_frame;
                if (active_data_type == KEY_FLOAT)          k_static_increase_value = key_increase_val(k_v_float_keys);
                if (active_data_type == KEY_CAMERA) {
                    k_c_pos1_incr = key_increase_val(k_c_pos1_keys);
                    k_c_pos2_incr = key_increase_val(k_c_pos2_keys);
                    k_c_foc0_incr = key_increase_val(k_c_foc0_keys);
                    k_c_foc1_incr = key_increase_val(k_c_foc1_keys);
                    k_c_foc2_incr = key_increase_val(k_c_foc2_keys);
                    k_c_rot0_incr = key_increase_val(k_c_rot0_keys);
                    k_c_rot1_incr = key_increase_val(k_c_rot1_keys);
                    std::cout << "camera working" << std::endl;
                }
            }
        } else {
            // Runs when we are between keyframes
            k_current_distance = k_frame_keys.at(k_last_passed_index + 1) - k_current_frame;
        }

        if (active_data_type == KEY_CAMERA) {
            mCameraKeyPos[0] += k_static_increase_value / 10;
            mCameraKeyPos[1] += k_c_pos1_incr / 10;
            mCameraKeyPos[2] += k_c_pos2_incr / 10;
            mCameraKeyFoc[0] += k_c_foc0_incr / 10;
            mCameraKeyFoc[1] += k_c_foc1_incr / 10;
            mCameraKeyFoc[2] += k_c_foc2_incr / 10;
            mCameraKeyYaw += k_c_rot0_incr / 10;
            mCameraKeyPitch += k_c_rot1_incr / 10;
        } else {
            // Set the variable
            if (active_data_type == KEY_FLOAT)              *active_key_float_value += k_static_increase_value / 10;
        }
    }

    if (camera_frozen && k_current_frame == 0 && k_popout_open) {
        if (active_data_type == KEY_CAMERA) {
            k_v_float_keys[0] = gCamera->pos[0];
            k_c_pos1_keys[0] = gCamera->pos[1];
            k_c_pos2_keys[0] = gCamera->pos[2];
            k_c_foc0_keys[0] = gCamera->focus[0];
            k_c_foc1_keys[0] = gCamera->focus[1];
            k_c_foc2_keys[0] = gCamera->focus[2];
        }
    }

    //s16 pitch, yaw;
    //f32 thisDist;
    //vec3f_get_dist_and_angle(gCamera->focus, gCamera->pos, &thisDist, &pitch, &yaw);
    //camera_approach_f32_symmetric_bool(&thisDist, 150.f, 7.f);
    //vec3f_set_dist_and_angle(c->focus, c->pos, dist, pitch, yaw);
    //update_camera_yaw(c);

    if (is_spinning && mario_exists) {
        gMarioState->faceAngle[1] += (s16)(spin_mult * 15 * 182.04f);
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
}

// Play Animation

void saturn_play_animation(MarioAnimID anim) {
    set_mario_animation(gMarioState, anim);
    //set_mario_anim_with_accel(gMarioState, anim, anim_speed * 65535);
    is_anim_playing = true;
}

void saturn_play_keyframe(s32 data_type) {
    if (!keyframe_playing) {
        if (data_type == KEY_CAMERA) {
            *active_key_float_value = k_v_float_keys[0];
            mCameraKeyPos[0] = k_v_float_keys[0];
            mCameraKeyPos[1] = k_c_pos1_keys[0];
            mCameraKeyPos[2] = k_c_pos2_keys[0];
            mCameraKeyFoc[0] = k_c_foc0_keys[0];
            mCameraKeyFoc[1] = k_c_foc1_keys[0];
            mCameraKeyFoc[2] = k_c_foc2_keys[0];
            mCameraKeyYaw = k_c_rot0_keys[0];
            mCameraKeyPitch = k_c_rot1_keys[0];
        } else {
            if (data_type == KEY_FLOAT)      k_v_float_keys[0] = *active_key_float_value;
            if (data_type == KEY_BOOL)       k_v_bool_keys[0] = *active_key_bool_value;
        }
        k_last_passed_index = 0;
        k_distance_between = 0;
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
        pos_relative[0] = gCamera->pos[0] - gMarioState->pos[0];
        pos_relative[1] = gCamera->pos[1] - gMarioState->pos[1];
        pos_relative[2] = gCamera->pos[2] - gMarioState->pos[2];

        foc_relative[0] = gCamera->focus[0] - gMarioState->pos[0];
        foc_relative[1] = gCamera->focus[1] - gMarioState->pos[1];
        foc_relative[2] = gCamera->focus[2] - gMarioState->pos[2];
    } else {
        vec3f_copy(stored_camera_pos, gCamera->pos);
        vec3f_copy(stored_camera_focus, gCamera->focus);
    }
    was_relative = relative;
}

void saturn_paste_camera() {
    if (was_relative) {
        stored_camera_pos[0] = gMarioState->pos[0] + pos_relative[0];
        stored_camera_pos[1] = gMarioState->pos[1] + pos_relative[1];
        stored_camera_pos[2] = gMarioState->pos[2] + pos_relative[2];
        stored_camera_focus[0] = gMarioState->pos[0] + foc_relative[0];
        stored_camera_focus[1] = gMarioState->pos[1] + foc_relative[1];
        stored_camera_focus[2] = gMarioState->pos[2] + foc_relative[2];
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
