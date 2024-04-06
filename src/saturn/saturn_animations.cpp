#include "saturn_animations.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "saturn/saturn.h"
#include "saturn/imgui/saturn_imgui.h"

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"

#include "saturn/saturn_animation_ids.h"

#include "pc/configfile.h"

extern "C" {
#include "game/camera.h"
#include "game/level_update.h"
#include "sm64.h"
}

using namespace std;
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

#include "saturn/saturn_json.h"

std::vector<string> canim_array;
std::string current_anim_dir_path;
std::vector<std::string> previous_anim_paths;

std::string chainer_name;

extern "C" {
#include "game/mario.h"
}

const char* saturn_animations_list[] = {
    "SLOW_LEDGE_GRAB",
    "FALL_OVER_BACKWARDS",
    "BACKWARD_AIR_KB",
    "DYING_ON_BACK",
    "BACKFLIP",
    "CLIMB_UP_POLE",
    "GRAB_POLE_SHORT",
    "GRAB_POLE_SWING_PART1",
    "GRAB_POLE_SWING_PART2",
    "HANDSTAND_IDLE",
    "HANDSTAND_JUMP",
    "START_HANDSTAND",
    "RETURN_FROM_HANDSTAND",
    "IDLE_ON_POLE",
    "A_POSE",
    "SKID_ON_GROUND",
    "STOP_SKID",
    "CROUCH_FROM_FAST_LONGJUMP",
    "CROUCH_FROM_SLOW_LONGJUMP",
    "FAST_LONGJUMP",
    "SLOW_LONGJUMP",
    "AIRBORNE_ON_STOMACH",
    "WALK_WITH_LIGHT_OBJ",
    "RUN_WITH_LIGHT_OBJ",
    "SLOW_WALK_WITH_LIGHT_OBJ",
    "SHIVERING_WARMING_HAND",
    "SHIVERING_RETURN_TO_IDLE",
    "SHIVERING",
    "CLIMB_DOWN_LEDGE",
    "CREDITS_WAVING",
    "CREDITS_LOOK_UP",
    "CREDITS_RETURN_FROM_LOOK_UP",
    "CREDITS_RAISE_HAND",
    "CREDITS_LOWER_HAND",
    "CREDITS_TAKE_OFF_CAP",
    "CREDITS_START_WALK_LOOK_UP",
    "CREDITS_LOOK_BACK_THEN_RUN",
    "FINAL_BOWSER_RAISE_HAND_SPIN",
    "FINAL_BOWSER_WING_CAP_TAKE_OFF",
    "CREDITS_PEACE_SIGN",
    "STAND_UP_FROM_LAVA_BOOST",
    "FIRE_LAVA_BURN",
    "WING_CAP_FLY",
    "HANG_ON_OWL",
    "LAND_ON_STOMACH",
    "AIR_FORWARD_KB",
    "DYING_ON_STOMACH",
    "SUFFOCATING",
    "COUGHING",
    "THROW_CATCH_KEY",
    "DYING_FALL_OVER",
    "IDLE_ON_LEDGE",
    "FAST_LEDGE_GRAB",
    "HANG_ON_CEILING",
    "PUT_CAP_ON",
    "TAKE_CAP_OFF_THEN_ON",
    "QUICKLY_PUT_CAP_ON", // unused
    "HEAD_STUCK_IN_GROUND",
    "GROUND_POUND_LANDING",
    "TRIPLE_JUMP_GROUND_POUND",
    "START_GROUND_POUND",
    "GROUND_POUND",
    "BOTTOM_STUCK_IN_GROUND",
    "IDLE_WITH_LIGHT_OBJ",
    "JUMP_LAND_WITH_LIGHT_OBJ",
    "JUMP_WITH_LIGHT_OBJ",
    "FALL_LAND_WITH_LIGHT_OBJ",
    "FALL_WITH_LIGHT_OBJ",
    "FALL_FROM_SLIDING_WITH_LIGHT_OBJ",
    "SLIDING_ON_BOTTOM_WITH_LIGHT_OBJ",
    "STAND_UP_FROM_SLIDING_WITH_LIGHT_OBJ",
    "RIDING_SHELL",
    "WALKING",
    "FORWARD_FLIP", // unused
    "JUMP_RIDING_SHELL",
    "LAND_FROM_DOUBLE_JUMP",
    "DOUBLE_JUMP_FALL",
    "SINGLE_JUMP",
    "LAND_FROM_SINGLE_JUMP",
    "AIR_KICK",
    "DOUBLE_JUMP_RISE",
    "START_FORWARD_SPINNING", // unused
    "THROW_LIGHT_OBJECT",
    "FALL_FROM_SLIDE_KICK",
    "BEND_KNESS_RIDING_SHELL", // unused
    "LEGS_STUCK_IN_GROUND",
    "GENERAL_FALL",
    "GENERAL_LAND",
    "BEING_GRABBED",
    "GRAB_HEAVY_OBJECT",
    "SLOW_LAND_FROM_DIVE",
    "FLY_FROM_CANNON",
    "MOVE_ON_WIRE_NET_RIGHT",
    "MOVE_ON_WIRE_NET_LEFT",
    "MISSING_CAP",
    "PULL_DOOR_WALK_IN",
    "PUSH_DOOR_WALK_IN",
    "UNLOCK_DOOR",
    "START_REACH_POCKET", // unused", reaching keys maybe?
    "REACH_POCKET", // unused
    "STOP_REACH_POCKET", // unused
    "GROUND_THROW",
    "GROUND_KICK",
    "FIRST_PUNCH",
    "SECOND_PUNCH",
    "FIRST_PUNCH_FAST",
    "SECOND_PUNCH_FAST",
    "PICK_UP_LIGHT_OBJ",
    "PUSHING",
    "START_RIDING_SHELL",
    "PLACE_LIGHT_OBJ",
    "FORWARD_SPINNING",
    "BACKWARD_SPINNING",
    "BREAKDANCE",
    "RUNNING",
    "RUNNING_UNUSED", // unused duplicate", originally part 2?
    "SOFT_BACK_KB",
    "SOFT_FRONT_KB",
    "DYING_IN_QUICKSAND",
    "IDLE_IN_QUICKSAND",
    "MOVE_IN_QUICKSAND",
    "ELECTROCUTION",
    "SHOCKED",
    "BACKWARD_KB",
    "FORWARD_KB",
    "IDLE_HEAVY_OBJ",
    "STAND_AGAINST_WALL",
    "SIDESTEP_LEFT",
    "SIDESTEP_RIGHT",
    "START_SLEEP_IDLE",
    "START_SLEEP_SCRATCH",
    "START_SLEEP_YAWN",
    "START_SLEEP_SITTING",
    "SLEEP_IDLE",
    "SLEEP_START_LYING",
    "SLEEP_LYING",
    "DIVE",
    "SLIDE_DIVE",
    "GROUND_BONK",
    "STOP_SLIDE_LIGHT_OBJ",
    "SLIDE_KICK",
    "CROUCH_FROM_SLIDE_KICK",
    "SLIDE_MOTIONLESS", // unused
    "STOP_SLIDE",
    "FALL_FROM_SLIDE",
    "SLIDE",
    "TIPTOE",
    "TWIRL_LAND",
    "TWIRL",
    "START_TWIRL",
    "STOP_CROUCHING",
    "START_CROUCHING",
    "CROUCHING",
    "CRAWLING",
    "STOP_CRAWLING",
    "START_CRAWLING",
    "SUMMON_STAR",
    "RETURN_STAR_APPROACH_DOOR",
    "BACKWARDS_WATER_KB",
    "SWIM_WITH_OBJ_PART1",
    "SWIM_WITH_OBJ_PART2",
    "FLUTTERKICK_WITH_OBJ",
    "WATER_ACTION_END_WITH_OBJ", // either swimming or flutterkicking
    "STOP_GRAB_OBJ_WATER",
    "WATER_IDLE_WITH_OBJ",
    "DROWNING_PART1",
    "DROWNING_PART2",
    "WATER_DYING",
    "WATER_FORWARD_KB",
    "FALL_FROM_WATER",
    "SWIM_PART1",
    "SWIM_PART2",
    "FLUTTERKICK",
    "WATER_ACTION_END", // either swimming or flutterkicking
    "WATER_PICK_UP_OBJ",
    "WATER_GRAB_OBJ_PART2",
    "WATER_GRAB_OBJ_PART1",
    "WATER_THROW_OBJ",
    "WATER_IDLE",
    "WATER_STAR_DANCE",
    "RETURN_FROM_WATER_STAR_DANCE",
    "GRAB_BOWSER",
    "SWINGING_BOWSER",
    "RELEASE_BOWSER",
    "HOLDING_BOWSER",
    "HEAVY_THROW",
    "WALK_PANTING",
    "WALK_WITH_HEAVY_OBJ",
    "TURNING_PART1",
    "TURNING_PART2",
    "SLIDEFLIP_LAND",
    "SLIDEFLIP",
    "TRIPLE_JUMP_LAND",
    "TRIPLE_JUMP",
    "FIRST_PERSON",
    "IDLE_HEAD_LEFT",
    "IDLE_HEAD_RIGHT",
    "IDLE_HEAD_CENTER",
    "HANDSTAND_LEFT",
    "HANDSTAND_RIGHT",
    "WAKE_FROM_SLEEP",
    "WAKE_FROM_LYING",
    "START_TIPTOE",
    "SLIDEJUMP", // pole jump and wall kick
    "START_WALLKICK",
    "STAR_DANCE",
    "RETURN_FROM_STAR_DANCE",
    "FORWARD_SPINNING_FLIP",
    "TRIPLE_JUMP_FLY"
};

void saturn_load_anim_folder(string path, int* index) {
    canim_array.clear();

    // If anim folder is misplaced
    if (!fs::exists("dynos/anims/"))
        return;

    // Go back a subfolder
    if (path == "../") {
        // Only go back if the previous directory actually exists
        if (previous_anim_paths.size() < 1 || !fs::exists(previous_anim_paths[previous_anim_paths.size() - 2])) {
            path = "";
            current_anim_dir_path = "dynos/anims/";
            previous_anim_paths.clear();
        } else {
            current_anim_dir_path = previous_anim_paths[previous_anim_paths.size() - 2];
            previous_anim_paths.pop_back();
        }
    }
    if (path == "") current_anim_dir_path = "dynos/anims/";

    // only update current path if folder exists
    if (fs::is_directory(current_anim_dir_path + path) && path != "../") {
        previous_anim_paths.push_back(current_anim_dir_path + path);
        current_anim_dir_path = current_anim_dir_path + path;
    }

    if (current_anim_dir_path != "dynos/anims/") {
        canim_array.push_back("../");
    }

    for (const auto & entry : fs::directory_iterator(current_anim_dir_path)) {
        fs::path path = entry.path();

        if (path.extension().string() == ".json") {
            string filename = path.filename().string().substr(0, path.filename().string().size() - 5);
            if (::isdigit(filename.back()) && filename.find("_") != string::npos) {
                // Ignore
            } else {
                canim_array.push_back(path.filename().string());
            }
        }
        if (fs::is_directory(entry.path())) {
            canim_array.push_back(entry.path().stem().string() + "/");
        }
    }

    for (int j = 0; j < canim_array.size(); j++) {
        if (canim_array[j].find("/") == string::npos) {
            *index = j;
            break;
        }
    }
}

string current_canim_name;
string current_canim_author;
bool current_canim_looping;
int current_canim_length;
int current_canim_nodes;
std::vector<s16> current_canim_values;
std::vector<u16> current_canim_indices;
bool current_canim_has_extra;

void run_hex_array(Json::Value array, std::vector<s16>* dest) {
    string even_one, odd_one;
    for (int i = 0; i < array.size(); i++) {
        if (i % 2 == 0) {
            // Run on even
            even_one = array[i].asString();
        } else {
            // Run on odd
            odd_one = array[i].asString();

            int out = std::stoi(even_one, 0, 16) * 256 + std::stoi(odd_one, 0, 16);
            dest->push_back(out);
        }
    }
}

void saturn_read_mcomp_animation(string json_path) {
    // Load the json file
    std::ifstream file(current_anim_dir_path + json_path + ".json");
    if (!file.good()) { return; }

    // Check if we should enable chainer
    // This is only the case if we have a followup animation
    // i.e. specialist.json, specialist_1.json
    if (!using_chainer) {
        std::ifstream file_c(current_anim_dir_path + json_path + "_1.json");
        if (file_c.good() && chainer_index == 0) {
            using_chainer = true;
            chainer_name = json_path;
            // Chainer only works with looping off
            //is_anim_looped = false;
        }
    } else {
        // Check if we're at the end of our chain
        std::ifstream file_c(current_anim_dir_path + chainer_name + "_" + std::to_string(chainer_index) + ".json");
        if (!file_c.good()) {
            //if (is_anim_looped) {
                // Looping restarts from the beginning
            //    chainer_index = 0;
            //    saturn_read_mcomp_animation(chainer_name);
            //    saturn_play_animation(MARIO_ANIM_A_POSE);
            //    saturn_play_custom_animation();
            //    return;
            //}
            using_chainer = false;
            chainer_index = 0;
            is_anim_playing = false;
            is_anim_paused = false;
            return;
        }
    }

    // Begin reading
    Json::Value root;
    root << file;

    current_canim_name = root["name"].asString();
    current_canim_author = root["author"].asString();
    if (root.isMember("extra_bone")) {
        if (root["extra_bone"].asString() == "true") current_canim_has_extra = true;
        if (root["extra_bone"].asString() == "false") current_canim_has_extra = false;
    } else { current_canim_has_extra = false; }
    // A mess
    if (root["looping"].asString() == "true") current_canim_looping = true;
    if (root["looping"].asString() == "false") current_canim_looping = false;
    current_canim_length = root["length"].asInt();
    current_canim_nodes = root["nodes"].asInt();
    current_canim_indices.clear();
    current_canim_values.clear();
    run_hex_array(root["values"], (std::vector<s16>*)&current_canim_values);
    run_hex_array(root["indices"], (std::vector<s16>*)&current_canim_indices);

    return;
}

void saturn_play_custom_animation() {
    gMarioState->animation->targetAnim->flags = 0;
    gMarioState->animation->targetAnim->unk02 = 0;
    gMarioState->animation->targetAnim->unk04 = 0;
    gMarioState->animation->targetAnim->unk06 = 0;
    gMarioState->animation->targetAnim->unk08 = (s16)current_canim_length;
    gMarioState->animation->targetAnim->unk0A = current_canim_indices.size() / 6 - 1;
    gMarioState->animation->targetAnim->values = current_canim_values.data();
    gMarioState->animation->targetAnim->index = current_canim_indices.data();
    gMarioState->animation->targetAnim->length = 0;
    gMarioState->marioObj->header.gfx.unk38.curAnim = gMarioState->animation->targetAnim;
}

void saturn_run_chainer() {
    if (is_anim_playing && current_animation.custom) {
        if (is_anim_past_frame(gMarioState, (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08) || is_anim_at_end(gMarioState)) {
            // Check if our next animation exists
            std::ifstream file_c1(current_anim_dir_path + chainer_name + "_" + std::to_string(chainer_index) + ".json");
            string test = current_anim_dir_path + chainer_name + "_" + std::to_string(chainer_index) + ".json";
            std::cout << test << std::endl;
            if (file_c1.good()) {
                saturn_read_mcomp_animation(chainer_name + "_" + std::to_string(chainer_index));
                saturn_play_animation(MARIO_ANIM_A_POSE);
                saturn_play_custom_animation();
            } else {
                if (current_animation.loop) {
                    // Looping restarts from the beginning
                    is_anim_playing = false;
                    using_chainer = false;
                    chainer_index = 0;
                    saturn_read_mcomp_animation(chainer_name);
                    saturn_play_animation(MARIO_ANIM_A_POSE);
                    saturn_play_custom_animation();
                } else {
                    using_chainer = false;
                    chainer_index = 0;
                    is_anim_playing = false;
                    is_anim_paused = false;
                }
            }
        }
    }
}

int saturn_anim_by_name(std::string name) {
    for (auto map : sanim_maps) {
        for (auto& entry : map) {
            if (entry.first.second == name) return entry.second;
        }
    }
    return -1;
}