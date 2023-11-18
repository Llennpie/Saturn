#include "saturn/saturn_models.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"

#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

#include <json/json.h>

#include "data/dynos.cpp.h"

int current_model_id = -1;
Model current_model;
Model previous_model;

std::string model_path = "dynos/packs";
std::vector<Model> model_list;

Array<PackData *> &mDynosPacks = DynOS_Gfx_GetPacks();

/*
Returns true if any models are currently selected.
*/
bool AnyModelsEnabled() {
    // Cycle through each DynOS pack,
    // Return true soon as one enabled
    for (int i = 0; i < mDynosPacks.Count(); i++) {
        if (DynOS_Opt_GetValue(String("dynos_pack_%d", i)))
            return true;
    }
    return false;
}

int current_dynos_id;
std::vector<Model> GetModelList(std::string folderPath) {
    std::vector<Model> m_list;

    if (fs::is_directory(folderPath)) {
        for (int i = 0; i < mDynosPacks.Count(); i++) {

            std::string packName = fs::path(mDynosPacks[i]->mPath).filename().u8string();
            Model model = LoadModelData(folderPath + "/" + packName);
            model.DynOSId = i;

            m_list.push_back(model);
        }
    }

    return m_list;
}

/*
Loads metadata from a model folder's "model.json".
*/
Model LoadModelData(std::string folderPath) {
    Model model;

    if (fs::is_directory(folderPath)) {
        // Model folder exists
        model.FolderName = fs::path(folderPath).filename().u8string();
        model.FolderPath = fs::path(folderPath).u8string();

        // Only load models intended for Mario
        if (!fs::is_directory(folderPath + "/mario") && !fs::exists(folderPath + "/mario_geo.bin"))
            return model;
    
        // Attempt to read "model.json"
        std::ifstream file(folderPath + "/model.json", std::ios::in | std::ios::binary);
        if (file.good()) {
            Json::Value root;
            file >> root;

            model.Name = root["name"].asString();
            model.Author = root["author"].asString();
            model.Version = root["version"].asString();
            // Description (optional)
            if (root.isMember("description")) {
                model.Description = root["description"].asString();
                // 500 character limit
                int characterLimit = 500;
                if (model.Description.length() > characterLimit) {
                    model.Description = model.Description.substr(0, characterLimit - 4);
                    model.Description += " ...";
                }
            }

            // Model Color Codes
            if (root.isMember("cc_support"))
                model.ColorCodeSupport = root["cc_support"].asBool();
            if (root.isMember("spark_support")) {
                model.SparkSupport = root["spark_support"].asBool();
                // If SPARK is enabled, enable CC support too (it needs it to work)
                if (model.SparkSupport)
                    model.ColorCodeSupport = true;
            }
            // Color Code Labels
            // Renames or hides the labels displayed in the Color Code Editor
            // Regardless of choices, the GS format remains the same
            if (root.isMember("colors")) {
                if (root["colors"].isMember("hat"))         model.Colors.Hat =          root["colors"]["hat"].asString();
                if (root["colors"].isMember("overalls"))    model.Colors.Overalls =     root["colors"]["overalls"].asString();
                if (root["colors"].isMember("gloves"))      model.Colors.Gloves =       root["colors"]["gloves"].asString();
                if (root["colors"].isMember("shoes"))       model.Colors.Shoes =        root["colors"]["shoes"].asString();
                if (root["colors"].isMember("skin"))        model.Colors.Skin =         root["colors"]["skin"].asString();
                if (root["colors"].isMember("hair"))        model.Colors.Hair =         root["colors"]["hair"].asString();
                if (root["colors"].isMember("shirt"))       model.Colors.Shirt =        root["colors"]["shirt"].asString();
                if (root["colors"].isMember("shoulders"))   model.Colors.Shoulders =    root["colors"]["shoulders"].asString();
                if (root["colors"].isMember("arms"))        model.Colors.Arms =         root["colors"]["arms"].asString();
                if (root["colors"].isMember("pelvis"))      model.Colors.Pelvis =       root["colors"]["pelvis"].asString();
                if (root["colors"].isMember("thighs"))      model.Colors.Thighs =       root["colors"]["thighs"].asString();
                if (root["colors"].isMember("calves"))      model.Colors.Calves =       root["colors"]["calves"].asString();
            }

            // Misc. Toggles

            if (root.isMember("torso_rotations")) {
                model.TorsoRotations = root["torso_rotations"].asBool();
            }
            enable_torso_rotation = model.TorsoRotations;

            // Custom Eyes
            // Enabled by default, but authors can optionally disable the feature
            // If disabled, the "Custom Eyes" checkbox will be hidden from the menu
            if (root.isMember("eye_support"))
                model.CustomEyeSupport = root["eye_support"].asBool();
            // EXPERIMENTAL: Custom Blink Cycle (optional)
            if (root.isMember("custom_blink_cycle"))
                model.CustomBlinkCycle = root["custom_blink_cycle"].asBool();

            model.Active = true;
        }
    }
    return model;
}