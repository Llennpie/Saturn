#include "saturn_cmd.h"

#include <iostream>
#include <map>
#include <utility>
#include <array>

#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/imgui/saturn_imgui.h"
#include "saturn/imgui/saturn_imgui_machinima.h"
#include "saturn/filesystem/saturn_locationfile.h"
#include "saturn/saturn_animations.h"
#include "saturn/saturn_obj_def.h"

extern "C" {
#include "engine/math_util.h"
#include "game/level_update.h"
#include "game/camera.h"
#include "game/mario.h"
#include "pc/cheats.h"
#include "pc/configfile.h"
#include "game/envfx_snow.h"
}

#include "saturn/saturn_timelines.h"

#define arg_int(name) ((int)(uintptr_t)context[name])
#define arg_flt(name) (*(float*)&context[name])
#define arg_bin(name) ((int)(uintptr_t)context[name] != 0)
#define arg_str(name) (*(std::string*)context[name])
#define arg_reg(name) registers[(arg_int(name) % SATURN_CMD_REG_COUNT + SATURN_CMD_REG_COUNT) % SATURN_CMD_REG_COUNT]
#define arg_val(name, type) (arg_int(type) == 1 ? arg_reg(name) : arg_int(name))

#define no_cli if (arg_bin("cli")) return;

int defined_animation = -1;

int get_obj_model_by_name(std::string name) {
    for (auto entry : obj_models) {
        if (entry.first == name) return entry.second;
    }
    return MODEL_NONE;
}

void place_keyframe(std::string timelineID, int frame, int valueIndex, float value) {
    KeyframeTimeline timeline = k_frame_keys[timelineID].first;
    int index = -1;
    int prevKeyframeIndex = 0;
    for (int i = 0; i < k_frame_keys[timelineID].second.size(); i++) {
        if (k_frame_keys[timelineID].second[i].position < frame) prevKeyframeIndex = i;
        if (k_frame_keys[timelineID].second[i].position == frame) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        Keyframe keyframe = Keyframe();
        keyframe.curve = timeline.forceWait ? InterpolationCurve::WAIT : (prevKeyframeIndex == -1 ? InterpolationCurve::LINEAR : k_frame_keys[timelineID].second[prevKeyframeIndex].curve); 
        keyframe.position = frame;
        for (int i = 0; i < timeline.numValues; i++) {
            keyframe.value.push_back(0);
        }
        keyframe.value[valueIndex] = value;
        keyframe.timelineID = timelineID;
        k_frame_keys[timelineID].second.push_back(keyframe);
        saturn_keyframe_sort(&k_frame_keys[timelineID].second);
    }
    else k_frame_keys[timelineID].second[index].value[index] = value;
}

const BehaviorScript* get_obj_bhv_by_name(std::string name) {
    for (auto entry : obj_behaviors) {
        if (entry.first == name) return entry.second;
    }
    return MODEL_NONE;
}

void cmd_mario_angle(CommandContext context) {
    if (arg_int("operation") == 0) gMarioState->faceAngle[1] += (s16)arg_int("angle");
    else gMarioState->faceAngle[1] = (s16)arg_int("angle");
}
void cmd_mario_pos(CommandContext context) {
    if (arg_int("operation") == 0) {
        gMarioState->pos[0] += arg_flt("x");
        gMarioState->pos[1] += arg_flt("y");
        gMarioState->pos[2] += arg_flt("z");
    }
    else {
        gMarioState->pos[0] = arg_flt("x");
        gMarioState->pos[1] = arg_flt("y");
        gMarioState->pos[2] = arg_flt("z");
    }
}
void cmd_mario_size_linked(CommandContext context) {
    if (arg_int("operation") == 0) marioScaleSizeX += arg_flt("x");
    else marioScaleSizeX = arg_flt("x");
    marioScaleSizeY = marioScaleSizeZ = marioScaleSizeX;
}
void cmd_mario_size(CommandContext context) {
    if (arg_int("operation") == 0) {
        marioScaleSizeX += arg_flt("x");
        marioScaleSizeY += arg_flt("y");
        marioScaleSizeZ += arg_flt("z");
    }
    else {
        marioScaleSizeX = arg_flt("x");
        marioScaleSizeY = arg_flt("y");
        marioScaleSizeZ = arg_flt("z");
    }
}
void cmd_mario_walkpoint(CommandContext context) {
    run_speed = arg_flt("walkpoint");
}
void cmd_mario_state_powerup(CommandContext context) {
    int powerup = arg_int("powerup");
    bool enabled = arg_bin("enable");
    if (powerup == 0 && enabled) saturnModelState |= 1;
    if (powerup == 1 && enabled) saturnModelState |= 2;
    if (powerup == 0 && !enabled) saturnModelState &= ~1;
    if (powerup == 1 && !enabled) saturnModelState &= ~2;
}
void cmd_mario_state_eye(CommandContext context) {
    current_eye_state = arg_int("eye");
}
void cmd_mario_state_hands(CommandContext context) {
    scrollHandState = arg_int("type");
}
void cmd_mario_state_cap(CommandContext context) {
    scrollCapState = arg_int("type");
}
void cmd_camera_pos(CommandContext context) {
    if (arg_int("operation") == 0) {
        freezecamPos[0] += arg_flt("x");
        freezecamPos[1] += arg_flt("y");
        freezecamPos[2] += arg_flt("z");
    }
    else {
        freezecamPos[0] = arg_flt("x");
        freezecamPos[1] = arg_flt("y");
        freezecamPos[2] = arg_flt("z");
    }
}
void cmd_camera_angle(CommandContext context) {
    if (arg_int("operation") == 0) {
        freezecamYaw += arg_flt("yaw");
        freezecamPitch += arg_flt("pitch");
        freezecamRoll += arg_flt("roll");
    }
    else {
        freezecamYaw = arg_flt("x");
        freezecamPitch = arg_flt("y");
        freezecamRoll = arg_flt("z");
    }
}
void cmd_camera_fov(CommandContext context) {
    if (arg_int("operation")) camera_fov += arg_flt("fov");
    else camera_fov = arg_flt("fov");
}
void cmd_camera_follow(CommandContext context) {
    if (arg_int("operation")) camera_focus += arg_flt("follow");
    else camera_focus = arg_flt("follow");
}
void cmd_timeline_delete(CommandContext context) {
    std::string id = arg_str("timeline");
    int frame = arg_int("frame");
    if (!saturn_timeline_exists(id.c_str())) return;
    if (frame == 0) {
        k_frame_keys.erase(id);
        return;
    }
    int keyframe = -1;
    for (int i = 0; i < k_frame_keys[id].second.size(); i++) {
        if (k_frame_keys[id].second[i].position == frame) {
            keyframe = i;
            break;
        }
    }
    if (keyframe == -1) return;
    k_frame_keys[id].second.erase(k_frame_keys[id].second.begin() + keyframe);
    k_previous_frame = -1;
}
void cmd_timeline_curve(CommandContext context) {
    std::string id = arg_str("timeline");
    int frame = arg_int("frame");
    if (!saturn_timeline_exists(id.c_str())) return;
    int keyframe = -1;
    for (int i = 0; i < k_frame_keys[id].second.size(); i++) {
        if (k_frame_keys[id].second[i].position == frame) {
            keyframe = i;
            break;
        }
    }
    if (keyframe == -1) return;
    k_frame_keys[id].second[keyframe].curve = InterpolationCurve(arg_int("curve"));
    k_previous_frame = -1;
}
void cmd_timeline_value_float(CommandContext context) {
    place_keyframe(arg_str("timeline"), arg_int("frame"), arg_int("index"), arg_flt("value"));
}
void cmd_timeline_value_bool(CommandContext context) {
    place_keyframe(arg_str("timeline"), arg_int("frame"), arg_int("index"), *(float*)new int(arg_bin("value")));
}
void cmd_colorcode(CommandContext context) {
    ColorCode cc = LoadGSFile(arg_str("path"), "dynos/colocodes");
    ApplyColorCode(cc);

    // thank you rise for the refactor 🙏
    /*std::string orig_path = current_cc_path;
    current_cc_path = "";
    saturn_load_cc_directory();
    load_cc_file((char*)arg_str("path").c_str());
    current_cc_path = orig_path;
    saturn_load_cc_directory();*/
}
void cmd_warp_register(CommandContext context) {
    warp_to_level(arg_reg("lvlid"), 1, -1);
}
void cmd_warp_to(CommandContext context) {
    warp_to_level(arg_int("level"), 1, -1);
}
void cmd_object_mario(CommandContext context) {
    saturn_create_object(
        get_obj_model_by_name(arg_str("model")), get_obj_bhv_by_name(arg_str("bhv")),
        gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2],
        gMarioState->faceAngle[0], gMarioState->faceAngle[1], gMarioState->faceAngle[2],
        0
    );
}
void cmd_object_mario_behparam(CommandContext context) {
    saturn_create_object(
        get_obj_model_by_name(arg_str("model")), get_obj_bhv_by_name(arg_str("bhv")),
        gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2],
        gMarioState->faceAngle[0], gMarioState->faceAngle[1], gMarioState->faceAngle[2],
        ((arg_int("behparam1") & 0xFF) << 24) | ((arg_int("behparam2") & 0xFF) << 16) | ((arg_int("behparam3") & 0xFF) << 8) | (arg_int("behparam4") & 0xFF)
    );
}
void cmd_object_register_pos(CommandContext context) {
    saturn_create_object(
        get_obj_model_by_name(arg_str("model")), get_obj_bhv_by_name(arg_str("bhv")),
        arg_reg("x"), arg_reg("y"), arg_reg("z"),
        0, 0, 0,
        0
    );
}
void cmd_object_register_angle(CommandContext context) {
    saturn_create_object(
        get_obj_model_by_name(arg_str("model")), get_obj_bhv_by_name(arg_str("bhv")),
        arg_reg("x"), arg_reg("y"), arg_reg("z"),
        arg_reg("pitch"), arg_reg("yaw"), arg_reg("roll"),
        0
    );
}
void cmd_object_register_behparam(CommandContext context) {
    saturn_create_object(
        get_obj_model_by_name(arg_str("model")), get_obj_bhv_by_name(arg_str("bhv")),
        arg_reg("x"), arg_reg("y"), arg_reg("z"),
        arg_reg("pitch"), arg_reg("yaw"), arg_reg("roll"),
        ((arg_reg("behparam1") & 0xFF) << 24) | ((arg_reg("behparam2") & 0xFF) << 16) | ((arg_reg("behparam3") & 0xFF) << 8) | (arg_reg("behparam4") & 0xFF)
    );
}
void cmd_object_constant_pos(CommandContext context) {
    saturn_create_object(
        get_obj_model_by_name(arg_str("model")), get_obj_bhv_by_name(arg_str("bhv")),
        arg_flt("x"), arg_flt("y"), arg_flt("z"),
        0, 0, 0,
        0
    );
}
void cmd_object_constant_angle(CommandContext context) {
    saturn_create_object(
        get_obj_model_by_name(arg_str("model")), get_obj_bhv_by_name(arg_str("bhv")),
        arg_flt("x"), arg_flt("y"), arg_flt("z"),
        arg_int("pitch"), arg_int("yaw"), arg_int("roll"),
        0
    );
}
void cmd_object_constant_behparam(CommandContext context) {
    saturn_create_object(
        get_obj_model_by_name(arg_str("model")), get_obj_bhv_by_name(arg_str("bhv")),
        arg_flt("x"), arg_flt("y"), arg_flt("z"),
        arg_int("pitch"), arg_int("yaw"), arg_int("roll"),
        ((arg_int("behparam1") & 0xFF) << 24) | ((arg_int("behparam2") & 0xFF) << 16) | ((arg_int("behparam3") & 0xFF) << 8) | (arg_int("behparam4") & 0xFF)
    );
}
void cmd_animation(CommandContext context) {
    current_animation.custom = arg_int("custom") != 0;
    current_animation.loop = false;
    current_animation.hang = false;
    current_animation.speed = 1.0f;
    current_animation.id = saturn_anim_by_name(arg_str("anim"));
    anim_play_button();
}
void cmd_animation_speed(CommandContext context) {
    current_animation.custom = arg_int("custom") != 0;
    current_animation.loop = false;
    current_animation.hang = false;
    current_animation.speed = arg_flt("speed");
    current_animation.id = saturn_anim_by_name(arg_str("anim"));
    anim_play_button();
}
void cmd_animation_looping(CommandContext context) {
    current_animation.custom = arg_int("custom") != 0;
    current_animation.loop = true;
    current_animation.hang = false;
    current_animation.speed = arg_flt("speed");
    current_animation.id = saturn_anim_by_name(arg_str("anim"));
    anim_play_button();
}
void cmd_animation_hanging(CommandContext context) {
    current_animation.custom = arg_int("custom") != 0;
    current_animation.loop = false;
    current_animation.hang = true;
    current_animation.speed = arg_flt("speed");
    current_animation.id = saturn_anim_by_name(arg_str("anim"));
    anim_play_button();
}
void cmd_animation_stop(CommandContext context) {
    is_anim_playing = false;
    is_anim_paused = false;
    using_chainer = false;
    chainer_index = 0;
}
void cmd_location_warp(CommandContext context) {
    std::string locationName = arg_str("name");
    auto locations = saturn_get_locations();
    if (locations->find(locationName) == locations->end()) return;
    vec3f_set(gMarioState->pos, (*locations)[locationName].second[0], (*locations)[locationName].second[1], (*locations)[locationName].second[2]);
    gMarioState->faceAngle[1] = (*locations)[locationName].first;
}
void cmd_location_delete(CommandContext context) {
    std::string locationName = arg_str("name");
    auto locations = saturn_get_locations();
    if (locations->find(locationName) == locations->end()) return;
    locations->erase(locationName);
    saturn_save_locations();
}
void cmd_location_save_mario(CommandContext context) {
    saturn_add_location((char*)arg_str("name").c_str());
}
void cmd_location_save_constant(CommandContext context) {
    saturn_add_defined_location(arg_int("level"), arg_flt("x"), arg_flt("y"), arg_flt("z"), arg_int("angle"), (char*)arg_str("name").c_str());
}
void cmd_location_save_register(CommandContext context) {
    saturn_add_defined_location(arg_reg("level"), arg_reg("x"), arg_reg("y"), arg_reg("z"), arg_reg("angle"), (char*)arg_str("name").c_str());
}
void cmd_environment(CommandContext context) {
    gLevelEnv = arg_int("envtype");
}
void cmd_call(CommandContext context) {
    saturn_cmd_eval_file(arg_str("path"));
}
void cmd_if(CommandContext context) { no_cli;
    int value1 = arg_reg("register");
    int value2 = arg_val("value", "valuetype");
    int op = arg_int("operator");
    bool cond = false;
    if (op == 0) cond = value1 == value2;
    if (op == 1) cond = value1 != value2;
    if (op == 2) cond = value1 > value2;
    if (op == 3) cond = value1 < value2;
    if (op == 4) cond = value1 >= value2;
    if (op == 5) cond = value1 <= value2;
    if (!cond) line += arg_int("line");
}
// todo: define values that can be read/written to by registers
void cmd_register_set_from(CommandContext context) {}
void cmd_register_set_constant(CommandContext context) {
    arg_reg("register") = arg_int("value");
}
void cmd_register_assign(CommandContext context) {}
void cmd_register_copy(CommandContext context) {
    arg_reg("register") = arg_reg("source");
}
void cmd_register_show(CommandContext context) {
    std::cout << arg_reg("register") << std::endl;
}
void cmd_operation(CommandContext context) {
    int val1 = arg_reg("register");
    int op = arg_int("operation");
    int val2 = arg_val("value", "valuetype");
    if (op == 0) arg_reg("register") = val1 + val2;
    if (op == 1) arg_reg("register") = val1 - val2;
    if (op == 2) arg_reg("register") = val1 * val2;
    if (op == 3) arg_reg("register") = val1 / val2;
    if (op == 4) arg_reg("register") = val1 % val2;
    if (op == 5) arg_reg("register") = pow(val1, val2);
    if (op == 6) arg_reg("register") = val1 & val2;
    if (op == 7) arg_reg("register") = val1 | val2;
    if (op == 8) arg_reg("register") = val1 ^ val2;
    if (op == 9) arg_reg("register") = val1 << val2;
    if (op == 10) arg_reg("register") = val1 >> val2;
}
void cmd_goto(CommandContext context) { no_cli;
    int op = arg_int("operation");
    int value = arg_val("value", "valuetype");
    if (op == 0) line += value;
    else line = value - 1;
}
void cmd_wait(CommandContext context) { no_cli;
    saturn_cmd_pause();
}
void cmd_alias_set_value(CommandContext context) {
    aliases.insert({ arg_str("name"), arg_int("value") });
}
void cmd_alias_set_from(CommandContext context) {
    aliases.insert({ arg_str("name"), arg_reg("value") });
}
void cmd_alias_delete(CommandContext context) {
    if (aliases.find(arg_str("name")) == aliases.end()) return;
    aliases.erase(arg_str("name"));
}
void cmd_kill(CommandContext context) {
    godmode_temp_off = true;
    gMarioState->health = 0xFF;
}

#undef arg_int
#undef arg_flt
#undef arg_bin
#undef arg_str
#undef arg_reg
#undef arg_val
#undef no_cli