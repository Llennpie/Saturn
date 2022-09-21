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

        if (path.extension().u8string() == ".json")
            canim_array.push_back(path.filename().u8string());
    }
}

string current_canim_name;
string current_canim_author;
bool current_canim_looping;
int current_canim_length;
int current_canim_nodes;
s16 current_canim_values[99999] = {};
u16 current_canim_indices[99999] = {};

void run_hex_array(Json::Value root, string type) {
    int i, j;
    string even_one, odd_one;
    for (auto itr : root[type]) {
        if (i % 2 == 0 || i == 0) {
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
                current_canim_values[j] = (s16)output;
            else
                current_canim_indices[j] = (u16)output;
            j++;
        }
        i++;
    }
}

void saturn_read_mcomp_animation(string json_path) {
    // Load the json file
    std::ifstream file("dynos/anims/" + json_path + ".json");
    if (!file.good()) { return; }

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
    gMarioState->animation->targetAnim->unk0A = ANIMINDEX_NUMPARTS(current_canim_indices);
    gMarioState->animation->targetAnim->values = (const s16*)current_canim_values;
    gMarioState->animation->targetAnim->index = (const u16*)current_canim_indices;
    gMarioState->animation->targetAnim->length = 0;
    gMarioState->marioObj->header.gfx.unk38.curAnim = gMarioState->animation->targetAnim;
}