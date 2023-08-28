#include <fstream>
#include <vector>
#include <cstring>
#include <iostream>
#include <string>
#include <map>
#include "saturn_format.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/saturn_textures.h"
#include "saturn/imgui/saturn_imgui.h"
#include "saturn/imgui/saturn_imgui_chroma.h"
#include "saturn/imgui/saturn_imgui_machinima.h"
#include "saturn/imgui/saturn_imgui_dynos.h"
extern "C" {
#include "engine/geo_layout.h"
#include "engine/math_util.h"
#include "include/sm64.h"
#include "include/types.h"
#include "game/level_update.h"
#include "game/interaction.h"
#include "game/envfx_snow.h"
#include "game/camera.h"
#include "game/mario.h"
#include "pc/configfile.h"
#include "pc/gfx/gfx_pc.h"
#include "pc/cheats.h"
#include "game/object_list_processor.h"
}

#define SATURN_PROJECT_VERSION 1

#define SATURN_PROJECT_IDENTIFIER           "STPJ"
#define SATURN_PROJECT_GAME_IDENTIFIER      "GAME"
#define SATURN_PROJECT_CHROMAKEY_IDENTIFIER "CKEY"
#define SATURN_PROJECT_TIMELINE_IDENTIFIER  "TMLN"
#define SATURN_PROJECT_KEYFRAME_IDENTIFIER  "KEFR"
#define SATURN_PROJECT_CAMERA_IDENTIFIER    "CMRA"
#define SATURN_PROJECT_DONE_IDENTIFIER      "DONE"

#define SATURN_PROJECT_FLAG_CAMERA_FROZEN        (1 << 15)
#define SATURN_PROJECT_FLAG_CAMERA_SMOOTH        (1 << 14)
#define SATURN_PROJECT_FLAG_SCALE_LINKED         (1 << 13)
#define SATURN_PROJECT_FLAG_MARIO_HEAD_ROTATIONS (1 << 12)
#define SATURN_PROJECT_FLAG_DUST_PARTICLES       (1 << 11)
#define SATURN_PROJECT_FLAG_TORSO_ROTATIONS      (1 << 10)
#define SATURN_PROJECT_FLAG_M_CAP_EMBLEM         (1 << 9)
#define SATURN_PROJECT_FLAG_MARIO_SPINNING       (1 << 8)
#define SATURN_PROJECT_FLAG_FLY_MODE             (1 << 7)
#define SATURN_PROJECT_FLAG_TIMELINE_SHOWN       (1 << 6)
#define SATURN_PROJECT_FLAG_SHADOWS              (1 << 5)
#define SATURN_PROJECT_FLAG_INVULNERABILITY      (1 << 4)
#define SATURN_PROJECT_FLAG_NPC_DIALOG           (1 << 3)
#define SATURN_PROJECT_FLAG_LOOP_ANIMATION       (1 << 2)
#define SATURN_PROJECT_FLAG_LOOP_TIMELINE        (1 << 1)
#define SATURN_PROJECT_ENV_BIT_1                 (1)
#define SATURN_PROJECT_ENV_BIT_2                 (1 << 7)
#define SATURN_PROJECT_WALKPOINT_MASK            0x7F

#define SATURN_KFENTRY_BOOL(name, variable, id) { id, { { nullptr, (bool*)&variable }, name } }
#define SATURN_KFENTRY_FLOAT(name, variable, id) { id, { { (float*)&variable, nullptr }, name } }
#define SATURN_KFENTRY_COLOR(name, variable, id) \
    { (std::string(id) + "_r").c_str(), { { (float*)&variable.x, nullptr }, (std::string(name) + " R").c_str() } },\
    { (std::string(id) + "_g").c_str(), { { (float*)&variable.y, nullptr }, (std::string(name) + " G").c_str() } },\
    { (std::string(id) + "_b").c_str(), { { (float*)&variable.z, nullptr }, (std::string(name) + " B").c_str() } }
#define SATURN_KFENTRY_COLOR_VEC3F(name, variable, id) \
    { (std::string(id) + "_r").c_str(), { { (float*)&variable[0], nullptr }, (std::string(name) + " R").c_str() } },\
    { (std::string(id) + "_g").c_str(), { { (float*)&variable[1], nullptr }, (std::string(name) + " G").c_str() } },\
    { (std::string(id) + "_b").c_str(), { { (float*)&variable[2], nullptr }, (std::string(name) + " B").c_str() } }

std::map<std::string, std::pair<std::pair<float*, bool*>, std::string>> timelineDataTable = {
    SATURN_KFENTRY_BOOL("k_skybox_mode", use_color_background, "Skybox Mode"),
    SATURN_KFENTRY_BOOL("k_shadows", enable_shadows, "Shadows"),
    SATURN_KFENTRY_FLOAT("k_shade_x", world_light_dir1, "Mario Shade X"),
    SATURN_KFENTRY_FLOAT("k_shade_y", world_light_dir2, "Mario Shade Y"),
    SATURN_KFENTRY_FLOAT("k_shade_z", world_light_dir3, "Mario Shade Z"),
    SATURN_KFENTRY_FLOAT("k_shade_t", world_light_dir4, "Mario Shade Tex"),
    SATURN_KFENTRY_FLOAT("k_scale", marioScaleSizeX, "Mario Scale"),
    SATURN_KFENTRY_FLOAT("k_scale_x", marioScaleSizeX, "Mario Scale X"),
    SATURN_KFENTRY_FLOAT("k_scale_y", marioScaleSizeY, "Mario Scale Y"),
    SATURN_KFENTRY_FLOAT("k_scale_z", marioScaleSizeZ, "Mario Scale Z"),
    SATURN_KFENTRY_BOOL("k_head_rot", enable_head_rotations, "Head Rotations"),
    SATURN_KFENTRY_BOOL("k_v_cap_emblem", show_vmario_emblem, "M Cap Emblem"),
    SATURN_KFENTRY_FLOAT("k_angle", gMarioState->faceAngle[1], "Mario Angle"),
    SATURN_KFENTRY_BOOL("k_hud", configHUD, "HUD"),
    SATURN_KFENTRY_FLOAT("k_fov", camera_fov, "FOV"),
    SATURN_KFENTRY_FLOAT("k_focus", camera_focus, "Follow"),
    SATURN_KFENTRY_FLOAT("k_c_camera_pos0", freezecamPos[0], "Camera Pos X"),
    SATURN_KFENTRY_FLOAT("k_c_camera_pos1", freezecamPos[1], "Camera Pos Y"),
    SATURN_KFENTRY_FLOAT("k_c_camera_pos2", freezecamPos[2], "Camera Pos Z"),
    SATURN_KFENTRY_FLOAT("k_c_camera_yaw", freezecamYaw, "Camera Yaw"),
    SATURN_KFENTRY_FLOAT("k_c_camera_pitch", freezecamPitch, "Camera Pitch"),
    SATURN_KFENTRY_FLOAT("k_c_camera_roll", freezecamRoll, "Camera Roll"),
    SATURN_KFENTRY_COLOR_VEC3F("k_light_col", gLightingColor, "Light Color"),
    SATURN_KFENTRY_COLOR("k_color", uiChromaColor, "Skybox Color"),
    SATURN_KFENTRY_COLOR("k_1/2###hat_half_1", uiHatColor, "Hat, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###hat_half_2", uiHatShadeColor, "Hat, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###overalls_half_1", uiOverallsColor, "Overalls, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###overalls_half_2", uiOverallsShadeColor, "Overalls, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###gloves_half_1", uiGlovesColor, "Gloves, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###gloves_half_2", uiGlovesShadeColor, "Gloves, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###shoes_half_1", uiShoesColor, "Shoes, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###shoes_half_2", uiShoesShadeColor, "Shoes, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###skin_half_1", uiSkinColor, "Skin, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###skin_half_2", uiSkinShadeColor, "Skin, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###hair_half_1", uiHairColor, "Hair, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###hair_half_2", uiHairShadeColor, "Hair, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###shirt_half_1", uiShirtColor, "Shirt, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###shirt_half_2", uiShirtShadeColor, "Shirt, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###shoulders_half_1", uiShouldersColor, "Shoulders, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###shoulders_half_2", uiShouldersShadeColor, "Shoulders, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###arms_half_1", uiArmsColor, "Arms, Main"),
    SATURN_KFENTRY_COLOR("k_1/2###arms_half_2", uiArmsShadeColor, "Arms, Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###overalls_bottom_half_1", uiOverallsBottomColor, "Overalls (Bottom), Main"),
    SATURN_KFENTRY_COLOR("k_1/2###overalls_bottom_half_2", uiOverallsBottomShadeColor, "Overalls (Bottom), Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###leg_top_half_1", uiLegTopColor, "Leg (Top), Main"),
    SATURN_KFENTRY_COLOR("k_1/2###leg_top_half_2", uiLegTopShadeColor, "Leg (Top), Shade"),
    SATURN_KFENTRY_COLOR("k_1/2###leg_bottom_half_1", uiLegBottomColor, "Leg (Bottom), Main"),
    SATURN_KFENTRY_COLOR("k_1/2###leg_bottom_half_2", uiLegBottomShadeColor, "Leg (Bottom), Shade"),
};

std::string full_file_path(char* filename) {
    return std::string("dynos/projects/") + filename;
}

void saturn_project_game_handler(SaturnFormatStream* stream, int version) {
    u16 flags = saturn_format_read_int16(stream);
    u8 walkpoint = saturn_format_read_int8(stream);
    camera_frozen = flags & SATURN_PROJECT_FLAG_CAMERA_FROZEN;
    camera_fov_smooth = flags & SATURN_PROJECT_FLAG_CAMERA_SMOOTH;
    linkMarioScale = flags & SATURN_PROJECT_FLAG_SCALE_LINKED;
    enable_head_rotations = flags & SATURN_PROJECT_FLAG_MARIO_HEAD_ROTATIONS;
    enable_dust_particles = flags & SATURN_PROJECT_FLAG_DUST_PARTICLES;
    enable_torso_rotation = flags & SATURN_PROJECT_FLAG_TORSO_ROTATIONS;
    show_vmario_emblem = flags & SATURN_PROJECT_FLAG_M_CAP_EMBLEM;
    is_spinning = flags & SATURN_PROJECT_FLAG_MARIO_SPINNING;
    gMarioState->action = (flags & SATURN_PROJECT_FLAG_FLY_MODE) ? ACT_DEBUG_FREE_MOVE : ACT_IDLE;
    k_popout_open = flags & SATURN_PROJECT_FLAG_TIMELINE_SHOWN;
    enable_shadows = flags & SATURN_PROJECT_FLAG_SHADOWS;
    enable_immunity = flags & SATURN_PROJECT_FLAG_INVULNERABILITY;
    enable_dialogue = flags & SATURN_PROJECT_FLAG_NPC_DIALOG;
    is_anim_looped = flags & SATURN_PROJECT_FLAG_LOOP_ANIMATION;
    k_loop = flags & SATURN_PROJECT_FLAG_LOOP_TIMELINE;
    gLevelEnv = ((flags & SATURN_PROJECT_ENV_BIT_1) << 1) | (((walkpoint & SATURN_PROJECT_ENV_BIT_2) >> 7) & 1);
    run_speed = walkpoint & SATURN_PROJECT_WALKPOINT_MASK;
    u8 level = saturn_format_read_int8(stream);
    spin_mult = saturn_format_read_float(stream);
    camVelSpeed = saturn_format_read_float(stream);
    camVelRSpeed = saturn_format_read_float(stream);
    camera_fov = saturn_format_read_float(stream);
    camera_focus = saturn_format_read_float(stream);
    current_eye_state = saturn_format_read_int8(stream);
    scrollHandState = saturn_format_read_int8(stream);
    scrollCapState = saturn_format_read_int8(stream);
    saturnModelState = saturn_format_read_int8(stream);
    gMarioState->faceAngle[1] = saturn_format_read_float(stream);
    gMarioState->pos[0] = saturn_format_read_float(stream);
    gMarioState->pos[1] = saturn_format_read_float(stream);
    gMarioState->pos[2] = saturn_format_read_float(stream);
    vec3f_copy(overriden_mario_pos, gMarioState->pos);
    overriden_mario_angle = gMarioState->faceAngle[1];
    world_light_dir4 = saturn_format_read_float(stream);
    world_light_dir1 = saturn_format_read_float(stream);
    world_light_dir2 = saturn_format_read_float(stream);
    world_light_dir3 = saturn_format_read_float(stream);
    gLightingColor[0] = saturn_format_read_int8(stream) / 255.0f;
    gLightingColor[1] = saturn_format_read_int8(stream) / 255.0f;
    gLightingColor[2] = saturn_format_read_int8(stream) / 255.0f;
    saturn_format_read_int8(stream); // filler byte, align to 4 bytes
    marioScaleSizeX = saturn_format_read_float(stream);
    marioScaleSizeY = saturn_format_read_float(stream);
    marioScaleSizeZ = saturn_format_read_float(stream);
    endFrame = saturn_format_read_int32(stream);
    endFrameText = endFrame;
    int act = 0;
    k_current_frame = saturn_format_read_int32(stream);
    gMarioState->action = saturn_format_read_int32(stream);
    gMarioState->actionState = saturn_format_read_int32(stream);
    gMarioState->actionTimer = saturn_format_read_int32(stream);
    gMarioState->actionArg = saturn_format_read_int32(stream);
    act = saturn_format_read_int8(stream);
    k_previous_frame = -1;
    u8 lvlID = (level >> 2) & 63;
    if (lvlID != get_saturn_level_id(gCurrLevelNum) || (level & 3) != gCurrAreaIndex - 1) {
        warp_to_level(lvlID, (level & 3) + 1, act);
        if (lvlID == 0) dynos_override_mario = 1;
        override_mario = 3; // We override for 3 frames otherwise it breaks :/
    }
}

void saturn_project_chromakey_handler(SaturnFormatStream* stream, int version) {
    u8 skybox = saturn_format_read_int8(stream);
    if (skybox == 0xFF || skybox == 0xFE) {
        use_color_background = true;
        renderFloor = skybox & 1;
    }
    else gChromaKeyBackground = skybox;
    u8 r = saturn_format_read_int8(stream);
    u8 g = saturn_format_read_int8(stream);
    u8 b = saturn_format_read_int8(stream);
    uiChromaColor.x = r / 255.0f;
    uiChromaColor.y = g / 255.0f;
    uiChromaColor.z = b / 255.0f;
    uiChromaColor.w = 1.0f;
    autoChroma = true;
}

void saturn_project_timeline_handler(SaturnFormatStream* stream, int version) {
    KeyframeTimeline timeline = KeyframeTimeline();
    timeline.precision = (char)saturn_format_read_int8(stream);
    timeline.autoCreation = saturn_format_read_int8(stream);
    char id[257];
    saturn_format_read_string(stream, id);
    id[256] = 0;
    auto timelineConfig = timelineDataTable[id];
    timeline.fdest = timelineConfig.first.first;
    timeline.bdest = timelineConfig.first.second;
    timeline.name = timelineConfig.second;
    k_frame_keys.insert({ std::string(id), { timeline, {} } });
}

void saturn_project_keyframe_handler(SaturnFormatStream* stream, int version) {
    float value = saturn_format_read_float(stream);
    u32 position = saturn_format_read_int8(stream);
    InterpolationCurve curve = InterpolationCurve(saturn_format_read_int8(stream));
    Keyframe keyframe = Keyframe((int)position, curve);
    keyframe.value = value;
    char id[257];
    saturn_format_read_string(stream, id);
    id[256] = 0;
    keyframe.timelineID = id;
    k_frame_keys[id].second.push_back(keyframe);
}

void saturn_project_camera_handler(SaturnFormatStream* stream, int version) {
    gCamera->areaCenX = saturn_format_read_int32(stream);
    gCamera->areaCenY = saturn_format_read_int32(stream);
    gCamera->areaCenZ = saturn_format_read_int32(stream);
    gCamera->cutscene = saturn_format_read_int8(stream);
    gCamera->defMode = saturn_format_read_int8(stream);
    gCamera->doorStatus = saturn_format_read_int8(stream);
    gCamera->focus[0] = saturn_format_read_float(stream);
    gCamera->focus[1] = saturn_format_read_float(stream);
    gCamera->focus[2] = saturn_format_read_float(stream);
    gCamera->mode = saturn_format_read_int8(stream);
    gCamera->nextYaw = saturn_format_read_int16(stream);
    gCamera->pos[0] = saturn_format_read_float(stream);
    gCamera->pos[1] = saturn_format_read_float(stream);
    gCamera->pos[2] = saturn_format_read_float(stream);
    gCamera->yaw = saturn_format_read_int16(stream);
    gLakituState.curFocus[0] = saturn_format_read_float(stream);
    gLakituState.curFocus[1] = saturn_format_read_float(stream);
    gLakituState.curFocus[2] = saturn_format_read_float(stream);
    gLakituState.curPos[0] = saturn_format_read_float(stream);
    gLakituState.curPos[1] = saturn_format_read_float(stream);
    gLakituState.curPos[2] = saturn_format_read_float(stream);
    gLakituState.defMode = saturn_format_read_int8(stream);
    gLakituState.focHSpeed = saturn_format_read_float(stream);
    gLakituState.focus[0] = saturn_format_read_float(stream);
    gLakituState.focus[1] = saturn_format_read_float(stream);
    gLakituState.focus[2] = saturn_format_read_float(stream);
    gLakituState.focusDistance = saturn_format_read_float(stream);
    gLakituState.focVSpeed = saturn_format_read_float(stream);
    gLakituState.goalFocus[0] = saturn_format_read_float(stream);
    gLakituState.goalFocus[1] = saturn_format_read_float(stream);
    gLakituState.goalFocus[2] = saturn_format_read_float(stream);
    gLakituState.goalPos[0] = saturn_format_read_float(stream);
    gLakituState.goalPos[1] = saturn_format_read_float(stream);
    gLakituState.goalPos[2] = saturn_format_read_float(stream);
    gLakituState.keyDanceRoll = saturn_format_read_int16(stream);
    gLakituState.lastFrameAction = saturn_format_read_int32(stream);
    gLakituState.mode = saturn_format_read_int8(stream);
    gLakituState.nextYaw = saturn_format_read_int16(stream);
    gLakituState.oldPitch = saturn_format_read_int16(stream);
    gLakituState.oldRoll = saturn_format_read_int16(stream);
    gLakituState.oldYaw = saturn_format_read_int16(stream);
    gLakituState.pos[0] = saturn_format_read_float(stream);
    gLakituState.pos[1] = saturn_format_read_float(stream);
    gLakituState.pos[2] = saturn_format_read_float(stream);
    gLakituState.posHSpeed = saturn_format_read_float(stream);
    gLakituState.posVSpeed = saturn_format_read_float(stream);
    gLakituState.roll = saturn_format_read_int16(stream);
    gLakituState.shakeMagnitude[0] = saturn_format_read_int16(stream);
    gLakituState.shakeMagnitude[1] = saturn_format_read_int16(stream);
    gLakituState.shakeMagnitude[2] = saturn_format_read_int16(stream);
    gLakituState.shakePitchDecay = saturn_format_read_int16(stream);
    gLakituState.shakePitchPhase = saturn_format_read_int16(stream);
    gLakituState.shakePitchVel = saturn_format_read_int16(stream);
    gLakituState.shakeRollDecay = saturn_format_read_int16(stream);
    gLakituState.shakeRollPhase = saturn_format_read_int16(stream);
    gLakituState.shakeRollVel = saturn_format_read_int16(stream);
    gLakituState.shakeYawDecay = saturn_format_read_int16(stream);
    gLakituState.shakeYawPhase = saturn_format_read_int16(stream);
    gLakituState.shakeYawVel = saturn_format_read_int16(stream);
    gLakituState.skipCameraInterpolationTimestamp = saturn_format_read_int32(stream);
    gLakituState.yaw = saturn_format_read_int16(stream);
}

void saturn_load_project(char* filename) {
    k_frame_keys.clear();
    saturn_format_input((char*)full_file_path(filename).c_str(), SATURN_PROJECT_IDENTIFIER, {
        { SATURN_PROJECT_GAME_IDENTIFIER, saturn_project_game_handler },
        { SATURN_PROJECT_CHROMAKEY_IDENTIFIER, saturn_project_chromakey_handler },
        { SATURN_PROJECT_TIMELINE_IDENTIFIER, saturn_project_timeline_handler },
        { SATURN_PROJECT_KEYFRAME_IDENTIFIER, saturn_project_keyframe_handler },
        { SATURN_PROJECT_CAMERA_IDENTIFIER, saturn_project_camera_handler },
    });
}
void saturn_save_project(char* filename) {
    SaturnFormatStream stream = saturn_format_output(SATURN_PROJECT_IDENTIFIER, SATURN_PROJECT_VERSION);
    if (autoChroma) {
        saturn_format_new_section(&stream, SATURN_PROJECT_CHROMAKEY_IDENTIFIER);
        u8 skybox = 0;
        if (use_color_background) {
            if (renderFloor) skybox = 0xFF;
            else skybox = 0xFE;
        }
        else skybox = gChromaKeyBackground;
        saturn_format_write_int8(&stream, skybox);
        saturn_format_write_int8(&stream, chromaColor.red[0]);
        saturn_format_write_int8(&stream, chromaColor.green[0]);
        saturn_format_write_int8(&stream, chromaColor.blue[0]);
        saturn_format_close_section(&stream);
    }
    saturn_format_new_section(&stream, SATURN_PROJECT_GAME_IDENTIFIER);
    u16 flags = 0;
    u8 walkpoint = run_speed;
    if (camera_frozen) flags |= SATURN_PROJECT_FLAG_CAMERA_FROZEN;
    if (camera_fov_smooth) flags |= SATURN_PROJECT_FLAG_CAMERA_SMOOTH;
    if (linkMarioScale) flags |= SATURN_PROJECT_FLAG_SCALE_LINKED;
    if (enable_head_rotations) flags |= SATURN_PROJECT_FLAG_MARIO_HEAD_ROTATIONS;
    if (enable_dust_particles) flags |= SATURN_PROJECT_FLAG_DUST_PARTICLES;
    if (enable_torso_rotation) flags |= SATURN_PROJECT_FLAG_TORSO_ROTATIONS;
    if (show_vmario_emblem) flags |= SATURN_PROJECT_FLAG_M_CAP_EMBLEM;
    if (is_spinning) flags |= SATURN_PROJECT_FLAG_MARIO_SPINNING;
    if (gMarioState->action == ACT_DEBUG_FREE_MOVE) flags |= SATURN_PROJECT_FLAG_FLY_MODE;
    if (k_popout_open) flags |= SATURN_PROJECT_FLAG_TIMELINE_SHOWN;
    if (enable_shadows) flags |= SATURN_PROJECT_FLAG_SHADOWS;
    if (enable_immunity) flags |= SATURN_PROJECT_FLAG_INVULNERABILITY;
    if (enable_dialogue) flags |= SATURN_PROJECT_FLAG_NPC_DIALOG;
    if (is_anim_looped) flags |= SATURN_PROJECT_FLAG_LOOP_ANIMATION;
    if (k_loop) flags |= SATURN_PROJECT_FLAG_LOOP_TIMELINE;
    if (gLevelEnv & 2) flags |= SATURN_PROJECT_ENV_BIT_1;
    if (gLevelEnv & 1) walkpoint |= SATURN_PROJECT_ENV_BIT_2;
    flags |= (gLevelEnv >> 1) & 1;
    walkpoint |= (gLevelEnv & 1) << 7;
    saturn_format_write_int16(&stream, flags);
    saturn_format_write_int8(&stream, walkpoint);
    saturn_format_write_int8(&stream, (get_saturn_level_id(gCurrLevelNum) << 2) | (gCurrAreaIndex - 1));
    saturn_format_write_float(&stream, spin_mult);
    saturn_format_write_float(&stream, camVelSpeed);
    saturn_format_write_float(&stream, camVelRSpeed);
    saturn_format_write_float(&stream, camera_fov);
    saturn_format_write_float(&stream, camera_focus);
    saturn_format_write_int8(&stream, current_eye_state);
    saturn_format_write_int8(&stream, scrollHandState);
    saturn_format_write_int8(&stream, scrollCapState);
    saturn_format_write_int8(&stream, saturnModelState);
    saturn_format_write_float(&stream, gMarioState->faceAngle[1]);
    saturn_format_write_float(&stream, gMarioState->pos[0]);
    saturn_format_write_float(&stream, gMarioState->pos[1]);
    saturn_format_write_float(&stream, gMarioState->pos[2]);
    saturn_format_write_float(&stream, world_light_dir4);
    saturn_format_write_float(&stream, world_light_dir1);
    saturn_format_write_float(&stream, world_light_dir2);
    saturn_format_write_float(&stream, world_light_dir3);
    saturn_format_write_int8(&stream, gLightingColor[0] * 255);
    saturn_format_write_int8(&stream, gLightingColor[1] * 255);
    saturn_format_write_int8(&stream, gLightingColor[2] * 255);
    saturn_format_write_int8(&stream, 255);
    saturn_format_write_float(&stream, marioScaleSizeX);
    saturn_format_write_float(&stream, marioScaleSizeY);
    saturn_format_write_float(&stream, marioScaleSizeZ);
    saturn_format_write_int32(&stream, endFrame);
    saturn_format_write_int32(&stream, k_current_frame);
    saturn_format_write_int32(&stream, gMarioState->action);
    saturn_format_write_int32(&stream, gMarioState->actionState);
    saturn_format_write_int32(&stream, gMarioState->actionTimer);
    saturn_format_write_int32(&stream, gMarioState->actionArg);
    saturn_format_write_int8(&stream, gCurrActNum);
    saturn_format_close_section(&stream);
    saturn_format_new_section(&stream, SATURN_PROJECT_CAMERA_IDENTIFIER);
    saturn_format_write_float(&stream, gCamera->areaCenX);
    saturn_format_write_float(&stream, gCamera->areaCenY);
    saturn_format_write_float(&stream, gCamera->areaCenZ);
    saturn_format_write_int8(&stream, gCamera->cutscene);
    saturn_format_write_int8(&stream, gCamera->defMode);
    saturn_format_write_int8(&stream, gCamera->doorStatus);
    saturn_format_write_float(&stream, gCamera->focus[0]);
    saturn_format_write_float(&stream, gCamera->focus[1]);
    saturn_format_write_float(&stream, gCamera->focus[2]);
    saturn_format_write_int8(&stream, gCamera->mode);
    saturn_format_write_int16(&stream, gCamera->nextYaw);
    saturn_format_write_float(&stream, gCamera->pos[0]);
    saturn_format_write_float(&stream, gCamera->pos[1]);
    saturn_format_write_float(&stream, gCamera->pos[2]);
    saturn_format_write_int16(&stream, gCamera->yaw);
    saturn_format_write_float(&stream, gLakituState.curFocus[0]);
    saturn_format_write_float(&stream, gLakituState.curFocus[1]);
    saturn_format_write_float(&stream, gLakituState.curFocus[2]);
    saturn_format_write_float(&stream, gLakituState.curPos[0]);
    saturn_format_write_float(&stream, gLakituState.curPos[1]);
    saturn_format_write_float(&stream, gLakituState.curPos[2]);
    saturn_format_write_int8(&stream, gLakituState.defMode);
    saturn_format_write_float(&stream, gLakituState.focHSpeed);
    saturn_format_write_float(&stream, gLakituState.focus[0]);
    saturn_format_write_float(&stream, gLakituState.focus[1]);
    saturn_format_write_float(&stream, gLakituState.focus[2]);
    saturn_format_write_float(&stream, gLakituState.focusDistance);
    saturn_format_write_float(&stream, gLakituState.focVSpeed);
    saturn_format_write_float(&stream, gLakituState.goalFocus[0]);
    saturn_format_write_float(&stream, gLakituState.goalFocus[1]);
    saturn_format_write_float(&stream, gLakituState.goalFocus[2]);
    saturn_format_write_float(&stream, gLakituState.goalPos[0]);
    saturn_format_write_float(&stream, gLakituState.goalPos[1]);
    saturn_format_write_float(&stream, gLakituState.goalPos[2]);
    saturn_format_write_int16(&stream, gLakituState.keyDanceRoll);
    saturn_format_write_int32(&stream, gLakituState.lastFrameAction);
    saturn_format_write_int8(&stream, gLakituState.mode);
    saturn_format_write_int16(&stream, gLakituState.nextYaw);
    saturn_format_write_int16(&stream, gLakituState.oldPitch);
    saturn_format_write_int16(&stream, gLakituState.oldRoll);
    saturn_format_write_int16(&stream, gLakituState.oldYaw);
    saturn_format_write_float(&stream, gLakituState.pos[0]);
    saturn_format_write_float(&stream, gLakituState.pos[1]);
    saturn_format_write_float(&stream, gLakituState.pos[2]);
    saturn_format_write_float(&stream, gLakituState.posHSpeed);
    saturn_format_write_float(&stream, gLakituState.posVSpeed);
    saturn_format_write_int16(&stream, gLakituState.roll);
    saturn_format_write_int16(&stream, gLakituState.shakeMagnitude[0]);
    saturn_format_write_int16(&stream, gLakituState.shakeMagnitude[1]);
    saturn_format_write_int16(&stream, gLakituState.shakeMagnitude[2]);
    saturn_format_write_int16(&stream, gLakituState.shakePitchDecay);
    saturn_format_write_int16(&stream, gLakituState.shakePitchPhase);
    saturn_format_write_int16(&stream, gLakituState.shakePitchVel);
    saturn_format_write_int16(&stream, gLakituState.shakeRollDecay);
    saturn_format_write_int16(&stream, gLakituState.shakeRollPhase);
    saturn_format_write_int16(&stream, gLakituState.shakeRollVel);
    saturn_format_write_int16(&stream, gLakituState.shakeYawDecay);
    saturn_format_write_int16(&stream, gLakituState.shakeYawPhase);
    saturn_format_write_int16(&stream, gLakituState.shakeYawVel);
    saturn_format_write_int32(&stream, gLakituState.skipCameraInterpolationTimestamp);
    saturn_format_write_int16(&stream, gLakituState.yaw);
    saturn_format_close_section(&stream);
    for (auto& entry : k_frame_keys) {
        saturn_format_new_section(&stream, SATURN_PROJECT_TIMELINE_IDENTIFIER);
        saturn_format_write_int8(&stream, entry.second.first.precision);
        saturn_format_write_bool(&stream, entry.second.first.autoCreation);
        saturn_format_write_string(&stream, (char*)entry.first.c_str());
        saturn_format_close_section(&stream);
    }
    for (auto& entry : k_frame_keys) {
        char* name = (char*)entry.first.c_str();
        for (Keyframe keyframe : entry.second.second) {
            saturn_format_new_section(&stream, SATURN_PROJECT_TIMELINE_IDENTIFIER);
            saturn_format_write_float(&stream, keyframe.value);
            saturn_format_write_int32(&stream, keyframe.position);
            saturn_format_write_int8(&stream, keyframe.curve);
            saturn_format_write_string(&stream, name);
            saturn_format_close_section(&stream);
        }
    }
    saturn_format_write((char*)full_file_path(filename).c_str(), &stream);
    std::cout << "Saved project " << filename << std::endl;
}