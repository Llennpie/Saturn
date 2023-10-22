#include "saturn/saturn_textures.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/imgui/saturn_imgui.h"
#include "saturn/imgui/saturn_imgui_chroma.h"

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"

#include "pc/configfile.h"

extern "C" {
#include "game/mario.h"
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

#include <json/json.h>

bool is_replacing_exp;
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

int current_keybind_exp;
int current_exp_index[8];

bool show_vmario_emblem;

int blink_eye_1_index = -1;
int blink_eye_2_index = -1;
int blink_eye_3_index = -1;
string blink_eye_1;
string blink_eye_2;
string blink_eye_3;

bool force_blink;
bool enable_blink_cycle = false;

std::vector<std::string> previous_eye_paths;

std::vector<string> eye1_array;

// Eye Folders, Non-Model

void saturn_load_eye_folder(std::string path) {
    eye_array.clear();
    eye1_array.clear();
    fs::create_directory("res/gfx");

    // If eye folder is misplaced
    if (!fs::exists("dynos/eyes/"))
        return;

    // Go back a subfolder
    if (path.find("../") != string::npos) {
        try {
            // Only go back if the previous directory actually exists
            if (previous_eye_paths.size() < 1 || !fs::exists(previous_eye_paths[previous_eye_paths.size() - 2])) {
                path = "";
                current_eye_dir_path = "dynos/eyes/";
                previous_eye_paths.clear();
            } else {
                current_eye_dir_path = previous_eye_paths[previous_eye_paths.size() - 2];
                previous_eye_paths.pop_back();
            }
        }
        catch (std::exception& e) {
            std::cerr << "Exception caught : " << e.what() << std::endl;
        }
    }
    if (path == "") {
        current_eye_dir_path = "dynos/eyes/";
        previous_eye_paths.clear();
    }

    // only update current path if folder exists
    if (fs::is_directory(current_eye_dir_path + path) && path != "../") {
        previous_eye_paths.push_back(current_eye_dir_path + path);
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
                eye1_array.push_back(entryPath);
        }
    }

    eye_array.insert(eye_array.end(), eye1_array.begin(), eye1_array.end());
    eye1_array.clear();
    
    if (eye_array.size() > 0)
        saturn_set_eye_texture(0);
}

void saturn_eye_selectable(std::string name, int index) {
    if (name.find(".png") != string::npos) {
        // This is an eye
        saturn_set_eye_texture(index);
    } else {
        // This is a folder
        saturn_load_eye_folder(name);
    }
}

std::string current_mouth_folder;
std::string last_folder_name;

/*
    Sets an eye texture with an eye_array index.
*/
void saturn_set_eye_texture(int index) {
    if (eye_array[index].find(".png") == string::npos) {
        current_eye_index = -1;
        // Reset blink cycle (if it exists)
        blink_eye_2_index = -1; blink_eye_2 = "";
        blink_eye_3_index = -1; blink_eye_3 = "";
        force_blink = false;
        //current_eye = "actors/mario/mario_eyes_left_unused.rgba16";
        
        // Attempt to select the first actual PNG
        for (int i = 0; i < eye_array.size(); i++) {
            if (eye_array[i].find(".png") != string::npos) {
                saturn_set_eye_texture(i);
                break;
            }
        }

        return;
    } else {
        current_eye_index = index;
        current_eye = current_eye_pre_path + eye_array[index];
        current_eye = current_eye.substr(0, current_eye.size() - 4);
        saturn_set_model_blink_eye(index, 1, current_eye.substr(6, current_eye.size()));
    }
}

// NEW SYSTEM, Model

string current_model_exp_tex[8];
bool using_model_eyes;

/*
    Handles texture replacement. Called from gfx_pc.c
*/
const void* saturn_bind_texture(const void* input) {
    const char* inputTexture = static_cast<const char*>(input);
    const char* outputTexture;

    if (input == (const void*)0x7365727574786574) return input;
    
    string texName = inputTexture;

    // Custom model expressions and eye textures
    if (current_model_data.name != "") {
        for (int i = 0; i < current_model_data.expressions.size(); i++) {

            // Could be either "saturn_eye" or "saturn_eyes", check for both
            string pos_name1 = "saturn_" + current_model_data.expressions[i].name;
            string pos_name2 = pos_name1.substr(0, pos_name1.size() - 1);

            // Model custom blink cycle
            if (force_blink && eye_array.size() > 0 && is_replacing_eyes) {
                if (texName.find("saturn_1eye") != string::npos) {
                    outputTexture = blink_eye_1.c_str();
                    const void* output = static_cast<const void*>(outputTexture);
                    return output;
                } else if (texName.find("saturn_2eye") != string::npos) {
                    outputTexture = blink_eye_2.c_str();
                    const void* output = static_cast<const void*>(outputTexture);
                    return output;
                } else if (texName.find("saturn_3eye") != string::npos) {
                    outputTexture = blink_eye_3.c_str();
                    const void* output = static_cast<const void*>(outputTexture);
                    return output;
                }
            }

            // Replace expression textures
            if (texName.find(pos_name1) != string::npos || texName.find(pos_name2) != string::npos) {
                outputTexture = current_model_exp_tex[i].c_str();
                //std::cout << current_model_exp_tex[i] << std::endl;
                const void* output = static_cast<const void*>(outputTexture);
                return output;
            }

        }
    }

    // Overwrite the unused textures shown in eye switch options 3, 4, 5 and 6 with our custom ones
    if (eye_array.size() > 0 && is_replacing_eyes) {
        if (texName.find("saturn_eye") != string::npos ||
            texName == "actors/mario/mario_eyes_left_unused.rgba16" ||
            texName == "actors/mario/mario_eyes_right_unused.rgba16" ||
            texName == "actors/mario/mario_eyes_up_unused.rgba16" ||
            texName == "actors/mario/mario_eyes_down_unused.rgba16") {
                outputTexture = current_eye.c_str();
                const void* output = static_cast<const void*>(outputTexture);
                return output;
        }
    }

    // Non-model custom blink cycle
    if (force_blink && eye_array.size() > 0 && is_replacing_eyes) {
        if (texName == "actors/mario/mario_eyes_center.rgba16" && blink_eye_1 != "") {
            outputTexture = blink_eye_1.c_str();
            const void* output = static_cast<const void*>(outputTexture);
            return output;
        } else if (texName == "actors/mario/mario_eyes_half_closed.rgba16" && blink_eye_2 != "") {
            outputTexture = blink_eye_2.c_str();
            const void* output = static_cast<const void*>(outputTexture);
            return output;
        } else if (texName == "actors/mario/mario_eyes_closed.rgba16" && blink_eye_3 != "") {
            outputTexture = blink_eye_3.c_str();
            const void* output = static_cast<const void*>(outputTexture);
            return output;
        }
    }

    // AUTO-CHROMA

    // Overwrite skybox
    // This runs for both Auto-chroma and the Chroma Key Stage
    if (autoChroma || gCurrLevelNum == LEVEL_SA) {
        if (use_color_background) {
            // Use white, recolorable textures for our color background
            if (texName.find("textures/skybox_tiles/") != string::npos)
                return "textures/saturn/white.rgba16";
        } else {
            // Swapping skyboxes IDs
            if (texName.find("textures/skybox_tiles/water") != string::npos) {
                switch(gChromaKeyBackground) {
                    case 0: return static_cast<const void*>(texName.replace(22, 5, "water").c_str());
                    case 1: return static_cast<const void*>(texName.replace(22, 5, "bitfs").c_str());
                    case 2: return static_cast<const void*>(texName.replace(22, 5, "wdw").c_str());
                    case 3: return static_cast<const void*>(texName.replace(22, 5, "cloud_floor").c_str());
                    case 4: return static_cast<const void*>(texName.replace(22, 5, "ccm").c_str());
                    case 5: return static_cast<const void*>(texName.replace(22, 5, "ssl").c_str());
                    case 6: return static_cast<const void*>(texName.replace(22, 5, "bbh").c_str());
                    case 7: return static_cast<const void*>(texName.replace(22, 5, "bidw").c_str());
                    case 8:
                        // "Above Clouds" recycles textures for its bottom layer
                        // See /us_pc/bin/clouds_skybox.c @ line 138
                        if (texName == "textures/skybox_tiles/water.44.rgba16" ||
                            texName == "textures/skybox_tiles/water.45.rgba16") {
                                return "textures/skybox_tiles/clouds.40.rgba16";
                            }
                        else {
                            return static_cast<const void*>(texName.replace(22, 5, "clouds").c_str());
                        }
                    case 9: return static_cast<const void*>(texName.replace(22, 5, "bits").c_str());
                }
            }
        }

        if (autoChroma) {
            // Toggle object visibility
            if (!autoChromaObjects &&
                texName.find("castle_grounds_textures.0BC00.ia16") != string::npos ||
                texName.find("butterfly_wing.rgba16") != string::npos) {
                    return "textures/saturn/mario_logo.rgba16";
            }
            // Toggle level visibility
            if (!autoChromaLevel) {
                if (texName.find("saturn") == string::npos &&
                    texName.find("dynos") == string::npos &&
                    texName.find("mario_") == string::npos &&
                    texName.find("mario/") == string::npos &&
                    texName.find("skybox") == string::npos &&
                    texName.find("shadow_quarter_circle.ia8") == string::npos &&
                    texName.find("shadow_quarter_square.ia8") == string::npos) {

                        if (texName.find("segment2.11C58.rgba16") != string::npos ||
                            texName.find("segment2.12C58.rgba16") != string::npos ||
                            texName.find("segment2.13C58.rgba16") != string::npos) {
                                return "textures/saturn/mario_logo.rgba16";
                        }

                }
            }
            // To-do: Hide paintings as well (low priority)
        }
    }

    return input;
}

struct ModelData current_model_data;

void saturn_set_model_texture(int expIndex, string path) {
    current_model_exp_tex[expIndex] = "../../" + path;
    current_model_exp_tex[expIndex] = current_model_exp_tex[expIndex].substr(0, current_model_exp_tex[expIndex].size() - 4);
}

void saturn_set_model_blink_eye(int index, int blink_index, string path) {
    if (blink_index == 1) { blink_eye_1 = "../../" + path;
                            if (blink_eye_1.find(".png") != string::npos) blink_eye_1 = blink_eye_1.substr(0, blink_eye_1.size() - 4);
                            blink_eye_1_index = index;
                            // Reset blink cycle (if it exists)
                            blink_eye_2_index = -1;
                            blink_eye_3_index = -1; }
    if (blink_index == 2) { blink_eye_2 = "../../" + path;
                            if (blink_eye_2.find(".png") != string::npos) blink_eye_2 = blink_eye_2.substr(0, blink_eye_2.size() - 4);
                            blink_eye_2_index = index; }
    if (blink_index == 3) { blink_eye_3 = "../../" + path;
                            if (blink_eye_3.find(".png") != string::npos) blink_eye_3 = blink_eye_3.substr(0, blink_eye_3.size() - 4);
                            blink_eye_3_index = index; }

    force_blink = (blink_eye_2_index != -1 && blink_eye_3_index != -1) ? 1 : 0;
}

void saturn_load_model_expression_entry(string folder_name, string expression_name) {
    Expression ex_entry;

    // Folder path, could be either something like "eye" OR "eyes"
    string path = "";
    string pos_path1 = "dynos/packs/" + folder_name + "/expressions/" + expression_name + "/";
    string pos_path2 = "dynos/packs/" + folder_name + "/expressions/" + expression_name + "s/";

    // Prefer "eye" over "eyes"
    if (fs::is_directory(pos_path2)) { path = pos_path2; ex_entry.name = (expression_name + "s"); }
    if (fs::is_directory(pos_path1)) { path = pos_path1; ex_entry.name = (expression_name + ""); }
    // If both don't exist, cancel
    if (path == "") { return; }
    if (fs::is_empty(path)) { return; }

    ex_entry.path = path;

    // Load each .png in the path
    for (const auto & entry : fs::directory_iterator(path)) {
        if (fs::is_directory(entry.path())) {
            // Ignore, this is a folder
        } else {
            string entryName = entry.path().filename().u8string();
            if (entryName.find(".png") != string::npos) // Only allow .png files
                ex_entry.textures.push_back(entryName);
        }
    }

    if (ex_entry.textures.size() > 0)
        current_model_data.expressions.push_back(ex_entry);
}

/*
    Loads an expression with a given number - helpful for keybinds.
*/
void saturn_load_expression_number(char number) {
    // For models without expression support
    for (int n = 0; n < eye_array.size(); n++) {
        if (eye_array[n].front() == number) {
            saturn_set_eye_texture(n);
            break;
        }
    }
    
    for (int i = 0; i < current_model_data.expressions.size(); i++) {
        // For every expression
        Expression expression = current_model_data.expressions[i];
        for (int j = 0; j < expression.textures.size(); j++) {
            // For every texture in that expression
            if (expression.textures[j].front() == number) {
                // We found a matching expression
                std::cout << (expression.path + expression.textures[j]) << std::endl;
                saturn_set_model_texture(i, expression.path + expression.textures[j]);  
                current_exp_index[i] = j;
                break;
            }
        }
    }
}

string current_folder_name;

string saturn_load_search(std::string folder_name) {
    // Load the json file
    std::ifstream file("dynos/packs/" + folder_name + "/model.json");
    if (file.good()) {
        // Begin reading
        Json::Value root;
        file >> root;

        return folder_name + " " + root["name"].asString() + " " + root["author"].asString();
    }
    return folder_name;
}

std::string previous_model_name;

void saturn_load_model_data(std::string folder_name, bool refresh_textures) {
    // Reset current model data
    ModelData blank;
    current_model_data = blank;
    using_model_eyes = false;

    // Load the json file
    std::ifstream file("dynos/packs/" + folder_name + "/model.json");
    if (file.good()) {
        // Begin reading
        Json::Value root;
        file >> root;

        current_model_data.name = root["name"].asString();
        current_model_data.author = root["author"].asString();
        current_model_data.version = root["version"].asString();

        // Description (optional)
        if (root.isMember("description")) {
            current_model_data.description = root["description"].asString();
            // 500 character limit
            int characterLimit = 500;
            if (current_model_data.description.length() > characterLimit) {
                current_model_data.description = current_model_data.description.substr(0, characterLimit - 4);
                current_model_data.description += " ...";
            }
        }

        // Type (optional)
        if (root.isMember("type")) {
            current_model_data.type = root["type"].asString();
        }

        // CC support is enabled by default, SPARK is disabled
        // This is just in case it wasn't defined in the model.json
        current_model_data.cc_support = true;
        current_model_data.spark_support = false;

        if (root.isMember("cc_support")) {
            current_model_data.cc_support = root["cc_support"].asBool();
            cc_model_support = current_model_data.cc_support;
        }
        
        if (root.isMember("spark_support")) {
            current_model_data.spark_support = root["spark_support"].asBool();
            cc_spark_support = current_model_data.spark_support;

            // If SPARK is enabled, enable CC support too (it needs it to work)
            if (current_model_data.spark_support == true) {
                current_model_data.cc_support = true;
                cc_model_support = true;
            }
        }

        // CC Editor Labels (optional)
        if (root.isMember("colors")) {
            if (root["colors"].isMember("hat")) current_model_data.hat_label = root["colors"]["hat"].asString();
            if (root["colors"].isMember("overalls")) current_model_data.overalls_label = root["colors"]["overalls"].asString();
            if (root["colors"].isMember("gloves")) current_model_data.gloves_label = root["colors"]["gloves"].asString();
            if (root["colors"].isMember("shoes")) current_model_data.shoes_label = root["colors"]["shoes"].asString();
            if (root["colors"].isMember("skin")) current_model_data.skin_label = root["colors"]["skin"].asString();
            if (root["colors"].isMember("hair")) current_model_data.hair_label = root["colors"]["hair"].asString();
            if (root["colors"].isMember("shirt")) current_model_data.shirt_label = root["colors"]["shirt"].asString();
            if (root["colors"].isMember("shoulders")) current_model_data.shoulders_label = root["colors"]["shoulders"].asString();
            if (root["colors"].isMember("arms")) current_model_data.arms_label = root["colors"]["arms"].asString();
            if (root["colors"].isMember("pelvis")) current_model_data.pelvis_label = root["colors"]["pelvis"].asString();
            if (root["colors"].isMember("thighs")) current_model_data.thighs_label = root["colors"]["thighs"].asString();
            if (root["colors"].isMember("calves")) current_model_data.calves_label = root["colors"]["calves"].asString();
        }

        // Torso Rotations (optional)
        enable_torso_rotation = true;
        if (root.isMember("torso_rotations")) {
            current_model_data.torso_rotations = root["torso_rotations"].asBool();
            enable_torso_rotation = current_model_data.torso_rotations;
        }

        // Custom eyes - enabled by default, but authors can optionally disable the feature
        // If disabled, the "Custom Eyes" checkbox will be hidden from the menu
        current_model_data.eye_support = true;
        if (root.isMember("eye_support")) {
            current_model_data.eye_support = root["eye_support"].asBool();
        }

        // EXPERIMENTAL: Custom blink cycle (optional)
        // Disabled by default, authors need to opt in for now until it becomes standard
        enable_blink_cycle = false;
        if (root.isMember("custom_blink_cycle")) {
            enable_blink_cycle = root["custom_blink_cycle"].asBool();
        }
    }

    // Set the current folder name
    current_folder_name = folder_name;

    string path = "dynos/packs/" + folder_name + "/expressions/";
    if (!fs::is_directory(path)) return;

    previous_eye_paths.clear();

    int i = 0;
    for (const auto & entry : fs::directory_iterator(path)) {
        if (fs::is_directory(entry.path())) {
            string expression_name = entry.path().filename().u8string();
            saturn_load_model_expression_entry(folder_name, expression_name);

            // Choose first texture as default
            if (!refresh_textures) {
                current_model_exp_tex[i] = "../../" + current_model_data.expressions[i].path + current_model_data.expressions[i].textures[0];
                current_model_exp_tex[i] = current_model_exp_tex[i].substr(0, current_model_exp_tex[i].size() - 4);
            }

            // Toggle model eyes
            if (expression_name.find("eye") != string::npos) {
                using_model_eyes = true;
            } else {
                saturn_load_eye_folder("");
                previous_eye_paths.clear();
            }

            i++;
        } else {
            // Ignore, these are files
        }
    }

    previous_model_name = current_model_data.name;
}

void saturn_copy_file(string from, string to) {
    fs::path from_path(from);
    string final = "" + fs::current_path().generic_string() + "/" + to + from_path.filename().generic_string();

    fs::path final_path(final);
    // Convert TXT files to GS
    if (final_path.extension() == ".txt") {
        final = "" + fs::current_path().generic_string() + "/" + to + from_path.stem().generic_string() + ".gs";
    }

    std::cout << from << " - " << final << std::endl;

    // Skip existing files
    if (fs::exists(final))
        return;

    fs::copy(from, final, fs::copy_options::skip_existing);
}

void saturn_delete_file(string file) {
    remove(file.c_str());
}

std::size_t number_of_files_in_directory(std::filesystem::path path) {
    return (std::size_t)std::distance(std::filesystem::directory_iterator{path}, std::filesystem::directory_iterator{});
}