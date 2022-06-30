#include "saturn/saturn_textures.h"

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
#include "pc/gfx/gfx_pc.h"
}

using namespace std;
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

bool is_replacing_eyes;

std::vector<string> eye_array;
string eye_dir;
string current_eye_pre_path;
string current_eye_path;
bool model_eyes_enabled;

std::vector<string> mouth_array;
string current_mouth_pre_path;
string current_mouth_path;
bool model_mouth_enabled;

/*
    Loads the dynos/eyes/ folder into the eye texture array.
*/
void saturn_load_eye_directory() {
    eye_array.clear();

#ifdef __MINGW32__
    // windows moment
    eye_dir = "dynos\\eyes\\";
#else
    eye_dir = "dynos/eyes/";
#endif
    current_eye_pre_path = "../../dynos/eyes/";

    for (const auto & entry : fs::directory_iterator(eye_dir)) {
        string entryPath = entry.path().filename().u8string();
        if (entryPath.find(".png") != string::npos) // only allow png files
            eye_array.push_back(entryPath.substr(0, entryPath.size() - 4)); // remove the ".png" file extension
    }

    if (eye_array.size() > 0)
        saturn_set_eye_texture(0);

    model_eyes_enabled = false;
}

/*
    Loads a model's eye directory, if it exists.
*/
void saturn_load_eyes_from_model(string folder_name) {
    eye_array.clear();

    string filePath;
#ifdef __MINGW32__
    // windows moment
    filePath = "dynos\\packs\\" + folder_name + "\\eyes\\";
#else
    filePath = "dynos/packs/" + folder_name + "/eyes/";
#endif
    current_eye_pre_path = "../../dynos/packs/" + folder_name + "/eyes/";

    if (!fs::is_directory(filePath)) {
        saturn_load_eye_directory();
        return;
    }

    eye_array.clear();

    for (const auto & entry : fs::directory_iterator(filePath)) {
        string entryPath = entry.path().filename().u8string();
        if (entryPath.find(".png") != string::npos) { // only allow png files
            eye_array.push_back(entryPath.substr(0, entryPath.size() - 4)); // remove the ".png" file extension
        }
    }

    model_eyes_enabled = true;
}

/*
    Loads a model's mouth directory, if it exists.
*/
void saturn_load_mouths_from_model(string folder_name) {
    string filePath;
#ifdef __MINGW32__
    // windows moment
    filePath = "dynos\\packs\\" + folder_name + "\\mouths\\";
#else
    filePath = "dynos/packs/" + folder_name + "/mouths/";
#endif
    current_mouth_pre_path = "../../dynos/packs/" + folder_name + "/mouths/";

    model_mouth_enabled = fs::is_directory(filePath);
    if (!model_mouth_enabled) {
        return;
    }

    mouth_array.clear();

    for (const auto & entry : fs::directory_iterator(filePath)) {
        string entryPath = entry.path().filename().u8string();
        if (entryPath.find(".png") != string::npos) { // only allow png files
            mouth_array.push_back(entryPath.substr(0, entryPath.size() - 4)); // remove the ".png" file extension
        }
    }
}

/*
    Handles texture replacement. Called from gfx_pc.c
*/
const void* saturn_bind_texture(const void* input) {
    const char* inputTexture = static_cast<const char*>(input);
    const char* outputTexture;

    string texName = string(inputTexture);

    if (texName == "actors/mario/mario_eyes_left_unused.rgba16" || texName.find("saturn_eye") != string::npos) {
        outputTexture = current_eye_path.c_str();
        const void* output = static_cast<const void*>(outputTexture);
        return output;
    }
    if (texName.find("saturn_mouth") != string::npos && model_mouth_enabled) {
        outputTexture = current_mouth_path.c_str();
        const void* output = static_cast<const void*>(outputTexture);
        return output;
    }

    return input;
}

/*
    Sets an eye texture with an eye_array index.
*/
void saturn_set_eye_texture(int index) {
    current_eye_path = current_eye_pre_path + eye_array[index];
}

/*
    Sets a mouth texture with a mouth_array index, if mouth support is enabled.
*/
void saturn_set_mouth_texture(int index) {
    if (!model_mouth_enabled) return;
    current_mouth_path = current_mouth_pre_path + mouth_array[index];
}