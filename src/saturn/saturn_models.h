#ifndef SaturnModels
#define SaturnModels

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
#include <string>
#include <vector>
#include <filesystem>
#include "saturn/saturn_textures.h"

class Model {
    public:
        /* Returns true if the model.json was successfully loaded */
        bool Active;
        int DynOSId = -1;

        std::string FolderName;
        std::string FolderPath;

        // Metadata
        std::string Name;
        std::string Author;
        std::string Version = "1.0.0"; // String, so version format can be anything
        std::string Description;

        // Expressions
        std::vector<Texpression> Expressions;
        bool CustomEyeSupport = true;
        /* Returns true if the model uses the default /dynos/eyes/ folder for its eye expressions */
        bool UsingVanillaEyes() {
            // Always enabled for non-model Mario
            if (this->DynOSId == -1) return (true);
            else {
                if (!std::filesystem::is_directory(this->FolderPath + "/expressions/eyes"))
                    return (this->Active && this->CustomEyeSupport);
            }
            return false;
        }

        // Color Codes
        bool ColorCodeSupport = true;
        bool SparkSupport = false;
        /* Custom color labels as defined in the CC editor */
        struct ColorCodeLabels {
            std::string Hat = "Hat";
            std::string Overalls = "Overalls";
            std::string Gloves = "Gloves";
            std::string Shoes = "Shoes";
            std::string Skin = "Skin";
            std::string Hair = "Hair";
            std::string Shirt = "Shirt";
            std::string Shoulders = "Shoulders";
            std::string Arms = "Arms";
            std::string Pelvis = "Pelvis";
            std::string Thighs = "Thighs";
            std::string Calves = "Calves";
        };
        ColorCodeLabels Colors;
        /* Returns true if the model has its own color code folder */
        bool HasColorCodeFolder() {
            return (this->Active &&
                    this->ColorCodeSupport && (
                        std::filesystem::is_directory(this->FolderPath + "/colorcodes") ||
                        std::filesystem::exists(this->FolderPath + "/default.gs"))
                    );
        }

        bool TorsoRotations = true;

        bool CustomBlinkCycle;

        /* A loose cluster of the model's metadata strings, useful for search indexing */
        std::string SearchMeta() {
            if (!this->Name.empty())
                return (this->Name + "" + this->Author + this->FolderName); 
        }
};

extern int current_model_id;
extern Model current_model;
extern Model previous_model;
extern std::vector<Model> model_list;

extern bool AnyModelsEnabled();
extern std::vector<Model> GetModelList(std::string);
extern Model LoadModelData(std::string);

#endif
#endif