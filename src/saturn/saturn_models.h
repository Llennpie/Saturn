#ifndef SaturnModels
#define SaturnModels

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
#include <string>
#include <vector>
#include <filesystem>

class Model {
    public:
        /* Returns true if the model.json was successfully loaded */
        bool Active;
        int DynOSId;

        std::string Name;
        std::string Author;
        std::string Version = "1.0.0"; // String, so version format can be anything
        std::string Description;

        std::string FolderName;
        std::string FolderPath;

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

        bool CustomEyeSupport = true;
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