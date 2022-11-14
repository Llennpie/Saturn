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

#include <json/json.h>

std::vector<string> canim_array;
std::string canim_directory;
std::string chainer_name;

extern "C" {
#include "game/mario.h"
}

void saturn_fetch_animations() {
    canim_array.clear();
    
#ifdef __MINGW32__
    // windows moment
    canim_directory = "dynos\\anims\\";
#else
    canim_directory = "dynos/anims/";
#endif

    if (!fs::exists(canim_directory))
        return;

    for (const auto & entry : fs::directory_iterator(canim_directory)) {
        fs::path path = entry.path();

        if (path.extension().u8string() == ".json") {
            string filename = path.filename().u8string().substr(0, path.filename().u8string().size() - 5);
            if (::isdigit(filename.back()) && filename.find("_") != string::npos) {
                // Ignore
            } else {
                canim_array.push_back(path.filename().u8string());
            }
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

void run_hex_array(Json::Value root, string type) {
    int i;
    string even_one, odd_one;
    for (auto itr : root[type]) {
        if (i % 2 == 0) {
            // Run on even
            even_one = itr.asString();
            even_one.erase(0, 2);
        } else {
            // Run on odd
            std::stringstream ss;
            odd_one = itr.asString();
            odd_one.erase(0, 2);

            string newValue = "0x" + even_one + odd_one;
            int output;
            ss << std::hex << newValue;
            ss >> output;
            if (type == "values")
                current_canim_values.push_back(output);
            else
                current_canim_indices.push_back(output);
        }
        i++;
    }
}

void saturn_read_mcomp_animation(string json_path) {
    // Load the json file
    std::ifstream file("dynos/anims/" + json_path + ".json");
    if (!file.good()) { return; }

    // Check if we should enable chainer
    // This is only the case if we have a followup animation
    // i.e. specialist.json, specialist_1.json
    if (!using_chainer) {
        std::ifstream file_c("dynos/anims/" + json_path + "_1.json");
        if (file_c.good() && chainer_index == 0) {
            using_chainer = true;
            chainer_name = json_path;
            // Chainer only works with looping off
            //is_anim_looped = false;
        }
    } else {
        // Check if we're at the end of our chain
        std::ifstream file_c("dynos/anims/" + chainer_name + "_" + std::to_string(chainer_index) + ".json");
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
    file >> root;

    current_canim_name = root["name"].asString();
    current_canim_author = root["author"].asString();
    // A mess
    if (root["looping"].asString() == "true") current_canim_looping = true;
    if (root["looping"].asString() == "false") current_canim_looping = false;
    current_canim_length = std::stoi(root["length"].asString());
    current_canim_nodes = std::stoi(root["nodes"].asString());
    current_canim_indices.clear();
    current_canim_values.clear();
    run_hex_array(root, "values");
    run_hex_array(root, "indices");

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
    if (is_anim_playing && is_custom_anim) {
        if (is_anim_past_frame(gMarioState, (int)gMarioState->marioObj->header.gfx.unk38.curAnim->unk08) || is_anim_at_end(gMarioState)) {
            // Check if our next animation exists
            std::ifstream file_c1("dynos/anims/" + chainer_name + "_" + std::to_string(chainer_index) + ".json");
            string test = "dynos/anims/" + chainer_name + "_" + std::to_string(chainer_index) + ".json";
            std::cout << test << std::endl;
            if (file_c1.good()) {
                saturn_read_mcomp_animation(chainer_name + "_" + std::to_string(chainer_index));
                saturn_play_animation(MARIO_ANIM_A_POSE);
                saturn_play_custom_animation();
            } else {
                if (is_anim_looped) {
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