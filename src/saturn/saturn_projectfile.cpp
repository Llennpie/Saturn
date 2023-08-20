#include <fstream>
#include <vector>
#include <cstring>
#include <iostream>
#include <string>
#include <map>
#include "saturn.h"
#include "saturn_colors.h"
#include "saturn_textures.h"
#include "imgui/saturn_imgui.h"
#include "imgui/saturn_imgui_chroma.h"
#include "imgui/saturn_imgui_machinima.h"
extern "C" {
#include "engine/geo_layout.h"
#include "engine/math_util.h"
#include "include/sm64.h"
#include "game/level_update.h"
#include "game/interaction.h"
#include "game/envfx_snow.h"
#include "game/camera.h"
#include "game/mario.h"
#include "pc/configfile.h"
#include "pc/gfx/gfx_pc.h"
#include "pc/cheats.h"
}

#define SATURN_PROJECT_VERSION           1
#define SATURN_PROJECT_BLOCK_SIZE        0x10
#define SATURN_PROJECT_IDENTIFIER_LENGTH 0x04

#define SATURN_PROJECT_HEADER_SIZE          0x10     // 16 bytes
#define SATURN_PROJECT_MAX_CONTENT_SIZE     0x200000 // 2 MB, max 131072 blocks

#define SATURN_PROJECT_IDENTIFIER             "STPJ"
#define SATURN_PROJECT_GAME_IDENTIFIER        "GAME"
#define SATURN_PROJECT_CHROMAKEY_IDENTIFIER   "CKEY"
#define SATURN_PROJECT_TIMELINE_IDENTIFIER    "TMLN"
#define SATURN_PROJECT_KEYFRAME_IDENTIFIER    "KEFR"
#define SATURN_PROJECT_DONE_IDENTIFIER "DONE"

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

typedef unsigned char uchar;

std::map<std::string, std::pair<std::pair<float*, bool*>, std::string>> timelineDataTable = {
    { "k_skybox_mode", { { nullptr, &use_color_background }, "Skybox Mode" } },
    { "k_shadows", { { nullptr, &enable_shadows }, "Shadows" } },
    { "k_shade_x", { { &world_light_dir1, nullptr }, "Mario Shade X" } },
    { "k_shade_y", { { &world_light_dir2, nullptr }, "Mario Shade Y" } },
    { "k_shade_z", { { &world_light_dir3, nullptr }, "Mario Shade Z" } },
    { "k_shade_t", { { &world_light_dir4, nullptr }, "Mario Shade Tex" } },
    { "k_scale", { { &marioScaleSizeX, nullptr }, "Mario Scale" } },
    { "k_scale_x", { { &marioScaleSizeX, nullptr }, "Mario Scale X" } },
    { "k_scale_y", { { &marioScaleSizeY, nullptr }, "Mario Scale Y" } },
    { "k_scale_z", { { &marioScaleSizeZ, nullptr }, "Mario Scale Z" } },
    { "k_head_rot", { { nullptr, &enable_head_rotations }, "Head Rotations" } },
    { "k_v_cap_emblem", { { nullptr, &show_vmario_emblem }, "M Cap Emblem" } },
    { "k_angle", { { (float*)&gMarioState->faceAngle[1], nullptr }, "Mario Angle" } },
    { "k_hud", { { nullptr, &configHUD }, "HUD" } },
    { "k_fov", { { &camera_fov, nullptr }, "FOV" } },
    { "k_focus", { { &camera_focus, nullptr }, "Follow" } },
    { "k_c_camera_pos0", { { &freezecamPos[0], nullptr }, "Camera Pos X" } },
    { "k_c_camera_pos1", { { &freezecamPos[1], nullptr }, "Camera Pos Y" } },
    { "k_c_camera_pos2", { { &freezecamPos[2], nullptr }, "Camera Pos Z" } },
    { "k_c_camera_yaw", { { &freezecamYaw, nullptr }, "Camera Yaw" } },
    { "k_c_camera_pitch", { { &freezecamPitch, nullptr }, "Camera Pitch" } },
    { "k_c_camera_roll", { { &freezecamRoll, nullptr }, "Camera Roll" } }
};

uchar get_int8(char* data, int*offset) {
    union {
        char x;
        uchar y;
    } u;
    u.x = data[*offset];
    *offset += 1;
    return u.y;
}
uint get_int32(char* data, int* offset) {
    uint value = (get_int8(data, offset) << 24) | (get_int8(data, offset) << 16) | (get_int8(data, offset) << 8) | get_int8(data, offset);
    return value;
}
ushort get_int16(char* data, int* offset) {
    ushort value = (get_int8(data, offset) << 8) | get_int8(data, offset);
    return value;
}
void get_string(char* data, int* offset, char* dest, int length) {
    int i = 0;
    do {
        dest[i] = data[*offset + i];
        i++;
    }
    while (data[*offset + i] != 0 && i < length);
    dest[i] = 0;
    *offset += i;
}
float get_floating_point(char* data, int* offset) {
    union {
        uint x;
        float y;
    } u;
    u.x = get_int32(data, offset);
    return u.y;
}
bool get_bool(char* data, int* offset) {
    bool value = data[*offset] != 0;
    *offset += 1;
    return value;
}
void identifier(char* data, int* offset, char* identifier) {
    get_string(data, offset, identifier, SATURN_PROJECT_IDENTIFIER_LENGTH);
}

void put_int32(char* data, int* offset, uint value) {
    data[*offset] = (value >> 24) & 0xFF;
    data[*offset + 1] = (value >> 16) & 0xFF;
    data[*offset + 2] = (value >> 8) & 0xFF;
    data[*offset + 3] = value & 0xFF;
    *offset += 4;
}
void put_int16(char* data, int* offset, ushort value) {
    data[*offset] = (value >> 8) & 0xFF;
    data[*offset + 1] = value & 0xFF;
    *offset += 2;
}
void put_int8(char* data, int* offset, uchar value) {
    data[*offset] = value;
    *offset += 1;
}
void put_string(char* data, int* offset, char* string) {
    int i = 0;
    do {
        data[*offset + i] = string[i];
        i++;
    }
    while (string[i] != 0);
    *offset += i + 1;
}
void put_floating_point(char* data, int* offset, float value) {
    union {
        float x;
        uint y;
    } u;
    u.x = value;
    put_int32(data, offset, u.y);
}
void put_bool(char* data, int* offset, bool value) {
    put_int8(data, offset, value ? 1 : 0);
}
void put_identifier(char* data, int* offset, char* identifier) {
    for (int i = 0; i < SATURN_PROJECT_IDENTIFIER_LENGTH; i++) {
        data[*offset + i] = identifier[i];
    }
    *offset += SATURN_PROJECT_IDENTIFIER_LENGTH;
}
void pad(char* data, int* offset, int alignment) {
    if (*offset % alignment == 0) return;
    int padding = alignment - *offset % alignment;
    for (int i = 0; i < padding; i++) {
        put_int8(data, offset, rand() % 256);
    }
}

void align(int* offset, int alignment) {
    if (*offset % alignment == 0) return;
    *offset += alignment - *offset % alignment;
}

uint hash(char* data, int length, int offset = 0) { // See Java's java.lang.String#hashCode() method
    uint hash = 0;
    for (int i = 0; i < length; i++) {
        hash += 31 * hash + (int)(uchar)(data[i + offset]);
    }
    return hash;
}

void saturn_load_project(char* filename) {
    int pointer = 0;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return;
    char* headerData = (char*)malloc(SATURN_PROJECT_HEADER_SIZE);
    file.read(headerData, SATURN_PROJECT_HEADER_SIZE);
    char fileIdentifier[5];
    identifier(headerData, &pointer, fileIdentifier);
    if (std::strcmp(fileIdentifier, SATURN_PROJECT_IDENTIFIER) != 0) {
        std::cout << "Identifier doesn't match " SATURN_PROJECT_IDENTIFIER << ". Ignoring." << std::endl;
    }
    uint blocks = get_int32(headerData, &pointer);
    uint checksum = get_int32(headerData, &pointer);
    uint version = get_int32(headerData, &pointer);
    char* data = (char*)malloc(SATURN_PROJECT_HEADER_SIZE + blocks * SATURN_PROJECT_BLOCK_SIZE);
    file.seekg(0);
    file.read(data, SATURN_PROJECT_HEADER_SIZE + blocks * SATURN_PROJECT_BLOCK_SIZE);
    uint dataChecksum = hash(data, blocks * SATURN_PROJECT_BLOCK_SIZE, SATURN_PROJECT_HEADER_SIZE);
    if (checksum != dataChecksum) {
        std::cout << "Checksum failed: should be " << checksum << " but is " << dataChecksum << ", corruptions or crash might happen!" << std::endl;
    }
    k_frame_keys.clear();
    while (true) {
        int beginning = pointer;
        char sectionIdentifier[5];
        identifier(data, &pointer, sectionIdentifier);
        uint size = get_int32(data, &pointer);
        align(&pointer, SATURN_PROJECT_BLOCK_SIZE);
        if (std::strcmp(sectionIdentifier, SATURN_PROJECT_DONE_IDENTIFIER) == 0) break;
        if (std::strcmp(sectionIdentifier, SATURN_PROJECT_GAME_IDENTIFIER) == 0) {
            ushort flags = get_int16(data, &pointer);
            uchar walkpoint = get_int8(data, &pointer);
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
            uchar level = get_int8(data, &pointer);
            warp_to_level((level >> 2) & 63, level & 3);
            override_mario_and_camera = 1;
            spin_mult = get_floating_point(data, &pointer);
            gCamera->pos[0] = get_floating_point(data, &pointer);
            gCamera->pos[1] = get_floating_point(data, &pointer);
            gCamera->pos[2] = get_floating_point(data, &pointer);
            float yaw = get_floating_point(data, &pointer);
            float pitch = get_floating_point(data, &pointer);
            vec3f_set_dist_and_angle(gCamera->pos, gCamera->focus, 100, (s16)pitch, (s16)yaw);
            vec3f_copy(overriden_camera_pos, gCamera->pos);
            vec3f_copy(overriden_camera_focus, gCamera->focus);
            camVelSpeed = get_floating_point(data, &pointer);
            camVelRSpeed = get_floating_point(data, &pointer);
            camera_fov = get_floating_point(data, &pointer);
            camera_focus = get_floating_point(data, &pointer);
            current_eye_state = get_int8(data, &pointer);
            scrollHandState = get_int8(data, &pointer);
            scrollCapState = get_int8(data, &pointer);
            saturnModelState = get_int8(data, &pointer);
            gMarioState->faceAngle[1] = get_floating_point(data, &pointer);
            gMarioState->pos[0] = get_floating_point(data, &pointer);
            gMarioState->pos[1] = get_floating_point(data, &pointer);
            gMarioState->pos[2] = get_floating_point(data, &pointer);
            vec3f_copy(overriden_mario_pos, gMarioState->pos);
            overriden_mario_angle = gMarioState->faceAngle[1];
            world_light_dir4 = get_floating_point(data, &pointer);
            world_light_dir1 = get_floating_point(data, &pointer);
            world_light_dir2 = get_floating_point(data, &pointer);
            world_light_dir3 = get_floating_point(data, &pointer);
            marioScaleSizeX = get_floating_point(data, &pointer);
            marioScaleSizeY = get_floating_point(data, &pointer);
            marioScaleSizeZ = get_floating_point(data, &pointer);
            endFrame = get_int32(data, &pointer);
            endFrameText = endFrame;
            k_current_frame = -1;
        }
        if (std::strcmp(sectionIdentifier, SATURN_PROJECT_CHROMAKEY_IDENTIFIER) == 0) {
            uchar skybox = get_int8(data, &pointer);
            if (skybox == 0xFF || skybox == 0xFE) {
                use_color_background = true;
                renderFloor = skybox & 1;
            }
            else gChromaKeyBackground = skybox;
            uchar r = get_int8(data, &pointer);
            uchar g = get_int8(data, &pointer);
            uchar b = get_int8(data, &pointer);
            uiChromaColor.x = r / 255.0f;
            uiChromaColor.y = g / 255.0f;
            uiChromaColor.z = b / 255.0f;
            uiChromaColor.w = 1.0f;
            autoChroma = true;
        }
        if (std::strcmp(sectionIdentifier, SATURN_PROJECT_TIMELINE_IDENTIFIER) == 0) {
            KeyframeTimeline timeline = KeyframeTimeline();
            timeline.precision = (char)get_int8(data, &pointer);
            timeline.autoCreation = get_int8(data, &pointer);
            char id[257];
            get_string(data, &pointer, id, 256);
            std::cout << "\"" << id << "\"" << std::endl;
            auto timelineConfig = timelineDataTable[id];
            timeline.fdest = timelineConfig.first.first;
            timeline.bdest = timelineConfig.first.second;
            timeline.name = timelineConfig.second;
            k_frame_keys.insert({ std::string(id), { timeline, {} } });
        }
        if (std::strcmp(sectionIdentifier, SATURN_PROJECT_KEYFRAME_IDENTIFIER) == 0) {
            float value = get_floating_point(data, &pointer);
            uint position = get_int32(data, &pointer);
            InterpolationCurve curve = InterpolationCurve(get_int8(data, &pointer));
            Keyframe keyframe = Keyframe((int)position, curve);
            keyframe.value = value;
            char id[257];
            get_string(data, &pointer, id, 256);
            keyframe.timelineID = id;
            k_frame_keys[id].second.push_back(keyframe);
        }
        pointer = beginning;
        pointer += SATURN_PROJECT_BLOCK_SIZE * size;
    }
    file.close();
    free(headerData);
    free(data);
}
void saturn_save_project(char* filename) {
    int headerPointer = 0;
    int contentPointer = 0;
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return;
    char* header = (char*)malloc(SATURN_PROJECT_HEADER_SIZE);
    char* content = (char*)malloc(SATURN_PROJECT_MAX_CONTENT_SIZE);
    if (autoChroma) {
        put_identifier(content, &contentPointer, SATURN_PROJECT_CHROMAKEY_IDENTIFIER);
        put_int32(content, &contentPointer, 2);
        pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
        uchar skybox = 0;
        if (use_color_background) {
            if (renderFloor) skybox = 0xFF;
            else skybox = 0xFE;
        }
        else skybox = gChromaKeyBackground;
        put_int8(content, &contentPointer, skybox);
        put_int8(content, &contentPointer, chromaColor.red[0]);
        put_int8(content, &contentPointer, chromaColor.green[0]);
        put_int8(content, &contentPointer, chromaColor.blue[0]);
        pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
    }
    put_identifier(content, &contentPointer, SATURN_PROJECT_GAME_IDENTIFIER);
    put_int32(content, &contentPointer, 7);
    pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
    ushort flags = 0;
    uchar walkpoint = run_speed;
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
    put_int16(content, &contentPointer, flags);
    put_int8(content, &contentPointer, walkpoint);
    put_int8(content, &contentPointer, (get_saturn_level_id(gCurrLevelNum) << 2) | gCurrAreaIndex);
    put_floating_point(content, &contentPointer, spin_mult);
    put_floating_point(content, &contentPointer, gLakituState.pos[0]);
    put_floating_point(content, &contentPointer, gLakituState.pos[1]);
    put_floating_point(content, &contentPointer, gLakituState.pos[2]);
    s16 yaw;
    s16 pitch;
    float dist;
    vec3f_get_dist_and_angle(gLakituState.pos, gLakituState.focus, &dist, &pitch, &yaw);
    put_floating_point(content, &contentPointer, (float)yaw);
    put_floating_point(content, &contentPointer, (float)pitch);
    put_floating_point(content, &contentPointer, camVelSpeed);
    put_floating_point(content, &contentPointer, camVelRSpeed);
    put_floating_point(content, &contentPointer, camera_fov);
    put_floating_point(content, &contentPointer, camera_focus);
    put_int8(content, &contentPointer, current_eye_state);
    put_int8(content, &contentPointer, scrollHandState);
    put_int8(content, &contentPointer, scrollCapState);
    put_int8(content, &contentPointer, saturnModelState);
    put_floating_point(content, &contentPointer, gMarioState->faceAngle[1]);
    put_floating_point(content, &contentPointer, gMarioState->pos[0]);
    put_floating_point(content, &contentPointer, gMarioState->pos[1]);
    put_floating_point(content, &contentPointer, gMarioState->pos[2]);
    put_floating_point(content, &contentPointer, world_light_dir4);
    put_floating_point(content, &contentPointer, world_light_dir1);
    put_floating_point(content, &contentPointer, world_light_dir2);
    put_floating_point(content, &contentPointer, world_light_dir3);
    put_floating_point(content, &contentPointer, marioScaleSizeX);
    put_floating_point(content, &contentPointer, marioScaleSizeY);
    put_floating_point(content, &contentPointer, marioScaleSizeZ);
    put_int32(content, &contentPointer, endFrame);
    pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
    for (auto& entry : k_frame_keys) {
        int beginningPointer = contentPointer;
        put_identifier(content, &contentPointer, SATURN_PROJECT_TIMELINE_IDENTIFIER);
        pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
        put_int8(content, &contentPointer, entry.second.first.precision);
        put_bool(content, &contentPointer, entry.second.first.autoCreation);
        put_string(content, &contentPointer, (char*)entry.first.c_str());
        pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
        int blocks = (contentPointer - beginningPointer) / SATURN_PROJECT_BLOCK_SIZE;
        contentPointer = beginningPointer + 4;
        put_int32(content, &contentPointer, blocks);
        contentPointer += blocks * SATURN_PROJECT_BLOCK_SIZE - 8;
    }
    for (auto& entry : k_frame_keys) {
        char* name = (char*)entry.first.c_str();
        for (Keyframe keyframe : entry.second.second) {
            int beginningPointer = contentPointer;
            put_identifier(content, &contentPointer, SATURN_PROJECT_KEYFRAME_IDENTIFIER);
            pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
            put_floating_point(content, &contentPointer, keyframe.value);
            put_int32(content, &contentPointer, keyframe.position);
            put_int8(content, &contentPointer, keyframe.curve);
            put_string(content, &contentPointer, name);
            pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
            int blocks = (contentPointer - beginningPointer) / SATURN_PROJECT_BLOCK_SIZE;
            contentPointer = beginningPointer + 4;
            put_int32(content, &contentPointer, blocks);
            contentPointer += blocks * SATURN_PROJECT_BLOCK_SIZE - 8;
        }
    }
    put_identifier(content, &contentPointer, SATURN_PROJECT_DONE_IDENTIFIER);
    pad(content, &contentPointer, SATURN_PROJECT_BLOCK_SIZE);
    put_identifier(header, &headerPointer, SATURN_PROJECT_IDENTIFIER);
    put_int32(header, &headerPointer, contentPointer / SATURN_PROJECT_BLOCK_SIZE);
    put_int32(header, &headerPointer, hash(content, contentPointer));
    put_int32(header, &headerPointer, SATURN_PROJECT_VERSION);
    file.write(header, SATURN_PROJECT_HEADER_SIZE);
    file.write(content, contentPointer);
    file.close();
    std::cout << "Saved project " << filename << std::endl;
    free(header);
    free(content);
}