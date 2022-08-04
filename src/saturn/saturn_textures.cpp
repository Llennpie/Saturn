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
#include <algorithm>
#include <assert.h>
#include <stdlib.h>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

bool is_replacing_eyes;

std::vector<string> eye_array;
string current_eye_pre_path;
string current_eye;
string current_eye_dir_path = "dynos/eyes/";
int current_eye_index;
bool model_eyes_enabled;

std::vector<string> mouth_array;
string current_mouth_pre_path;
string current_mouth;
string current_mouth_dir_path;
int current_mouth_index;
bool model_mouth_enabled;

// Eye Folders

void saturn_load_eye_folder(std::string path) {
    eye_array.clear();
    fs::create_directory("res/gfx");

    // reset dir if we last used models or returned to root
    if (path == "../") path = "";
    if (model_eyes_enabled || path == "") {
        model_eyes_enabled = false;
        current_eye_dir_path = "dynos/eyes/";
    }

    // only update current path if folder exists
    if (fs::is_directory(current_eye_dir_path + path)) {
        current_eye_dir_path = current_eye_dir_path + path;
    }

    current_eye_pre_path = "../../" + current_eye_dir_path;

    if (current_eye_dir_path != "dynos/eyes/") {
        eye_array.push_back("../");
    }

    for (const auto & entry : fs::directory_iterator(current_eye_dir_path)) {
        if (fs::is_directory(entry.path())) {
            eye_array.push_back(entry.path().stem().u8string() + "/");
        } else {
            string entryPath = entry.path().filename().u8string();
            if (entryPath.find(".png") != string::npos) // only allow png files
                eye_array.push_back(entryPath);
        }
    }
    
    if (eye_array.size() > 0)
        saturn_set_eye_texture(0);
}

/*
    Loads a model's eye directory, if it exists.
*/
void saturn_load_eyes_from_model(std::string path, std::string folder_name) {
    eye_array.clear();
    fs::create_directory("res/gfx");

    // our model may not have a eye folder
    if (!fs::is_directory("dynos/packs/" + folder_name + "/eyes/")) {
        model_eyes_enabled = false;
        saturn_load_eye_folder("../");
        return;
    }

    // return to root if we haven't loaded our model eyes yet
    if (path == "../") path = "";
    if (!model_eyes_enabled || path == "") {
        current_eye_dir_path = "dynos/packs/" + folder_name + "/eyes/";
        model_eyes_enabled = true;
    }

    // only update current path if folder exists
    if (fs::is_directory(current_eye_dir_path + path)) {
        current_eye_dir_path = current_eye_dir_path + path;
    }

    current_eye_pre_path = "../../" + current_eye_dir_path;

    if (current_eye_dir_path != "dynos/packs/" + folder_name + "/eyes/") {
        eye_array.push_back("../");
    }

    for (const auto & entry : fs::directory_iterator(current_eye_dir_path)) {
        if (fs::is_directory(entry.path())) {
            //eye_array.push_back(entry.path().stem().u8string() + "/");
        } else {
            string entryPath = entry.path().filename().u8string();
            if (entryPath.find(".png") != string::npos) // only allow png files
                eye_array.push_back(entryPath);
        }
    }
    
    if (eye_array.size() > 0)
        saturn_set_eye_texture(0);
}

void saturn_eye_selectable(std::string name, int index) {
    if (name.find(".png") != string::npos) {
        // this is an eye
        saturn_set_eye_texture(index);
        std::cout << current_eye << std::endl;
    } else {
        saturn_load_eye_folder(name);
    }
}

std::string current_mouth_folder;
std::string last_folder_name;

/*
    Loads a model's mouth directory, if it exists.
*/
void saturn_load_mouths_from_model(std::string path, std::string folder_name) {
    mouth_array.clear();
    fs::create_directory("res/gfx");

    // allows folder_name to be undefined
    if (folder_name == "") { folder_name = last_folder_name; }
    else { last_folder_name = folder_name; }

    // our model may not have a mouth folder
    model_mouth_enabled = fs::is_directory("dynos/packs/" + folder_name + "/mouths/");
    if (!model_mouth_enabled) {
        return;
    }

    // return to root
    if (path == "../") path = "";
    if (path == "dynos/packs/" || path == "") {
        current_mouth_dir_path = "dynos/packs/" + folder_name + "/mouths/";
    }

    // only update current path if folder exists
    if (fs::is_directory(current_mouth_dir_path + path)) {
        current_mouth_dir_path = current_mouth_dir_path + path;
    }

    current_mouth_pre_path = "../../dynos/packs/" + folder_name + "/mouths/";

    if (current_mouth_dir_path != "dynos/packs/" + folder_name + "/mouths/") {
        mouth_array.push_back("../");
    }

    for (const auto & entry : fs::directory_iterator(current_mouth_dir_path)) {
        if (fs::is_directory(entry.path())) {
            //mouth_array.push_back(entry.path().stem().u8string() + "/");
        } else {
            string entryPath = entry.path().filename().u8string();
            if (entryPath.find(".png") != string::npos) // only allow png files
                mouth_array.push_back(entryPath);
        }
    }
}

void saturn_mouth_selectable(std::string name, int index) {
    if (name.find(".png") != string::npos) {
        // this is a mouth
        saturn_set_mouth_texture(index);
        std::cout << current_mouth << std::endl;
    } else {
        saturn_load_mouths_from_model(name, ""); // not defined, we should have already picked a folder_name
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
        outputTexture = current_eye.c_str();
        const void* output = static_cast<const void*>(outputTexture);
        return output;
    }
    if (texName.find("saturn_mouth") != string::npos && model_mouth_enabled) {
        outputTexture = current_mouth.c_str();
        const void* output = static_cast<const void*>(outputTexture);
        return output;
    }

    return input;
}

/*
    Sets an eye texture with an eye_array index.
*/
void saturn_set_eye_texture(int index) {
    if (eye_array[index].find(".png") == string::npos) {
        // keep trying till we get a non-folder
        saturn_set_eye_texture(index + 1);
        return;
    }
    current_eye_index = index;
    current_eye = current_eye_pre_path + eye_array[index];
    current_eye = current_eye.substr(0, current_eye.size() - 4);
}

/*
    Sets a mouth texture with a mouth_array index, if mouth support is enabled.
*/
void saturn_set_mouth_texture(int index) {
    if (!model_mouth_enabled) return;

    if (mouth_array[index].find(".png") == string::npos) {
        // keep trying till we get a non-folder
        saturn_set_mouth_texture(index + 1);
        return;
    }
    current_mouth_index = index;
    current_mouth = current_mouth_pre_path + mouth_array[index];
    current_mouth = current_mouth.substr(0, current_mouth.size() - 4);
}