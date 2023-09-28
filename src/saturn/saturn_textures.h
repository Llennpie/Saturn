#ifndef SaturnTextures
#define SaturnTextures

#include <stdio.h>
#include <stdbool.h>



#ifdef __cplusplus
#include <string>
#include <vector>
#include <filesystem>

extern std::vector<std::string> eye_array;
extern int current_eye_index;
extern bool model_eyes_enabled;
extern std::string current_eye_dir_path;

extern int current_mouth_index;
extern std::vector<std::string> mouth_array;
extern bool model_mouth_enabled;

void saturn_load_eye_folder(std::string);
void saturn_eye_selectable(std::string, int);

// New System

struct Expression {
    std::string name;
    std::string path;
    std::vector<std::string> textures;
};

struct ModelData {
    std::string name;
    std::string author;
    std::string version;
    std::string description;
    std::string type;
    std::vector<Expression> expressions;
    bool cc_support;
    bool spark_support;
    bool eye_support;

    std::string hat_label = "Hat";
    std::string overalls_label = "Overalls";
    std::string gloves_label = "Gloves";
    std::string shoes_label = "Shoes";
    std::string skin_label = "Skin";
    std::string hair_label = "Hair";
    std::string shirt_label = "Shirt";
    std::string shoulders_label = "Shoulders";
    std::string arms_label = "Arms";
    std::string pelvis_label = "Pelvis";
    std::string thighs_label = "Thighs";
    std::string calves_label = "Calves";

    bool torso_rotations;
};

extern struct ModelData current_model_data;
extern bool using_model_eyes;
extern std::string current_folder_name;
extern std::string saturn_load_search(std::string folder_name);
void saturn_load_model_data(std::string folder_name, bool refresh_textures);
void saturn_set_model_texture(int expIndex, std::string path);
void saturn_set_model_blink_eye(int, int, std::string);
void saturn_load_expression_number(char number);
extern int current_exp_index[8];
extern int blink_eye_1_index;
extern int blink_eye_2_index;
extern int blink_eye_3_index;
extern std::string blink_eye_1;
extern std::string blink_eye_2;
extern std::string blink_eye_3;
void saturn_copy_file(std::string from, std::string to);
void saturn_delete_file(std::string file);
std::size_t number_of_files_in_directory(std::filesystem::path path);

extern bool show_vmario_emblem;

extern "C" {
#endif
    extern bool is_replacing_exp;
    extern bool is_replacing_eyes;

    const void* saturn_bind_texture(const void*);
    void saturn_set_eye_texture(int);
    extern bool force_blink;
    extern bool enable_blink_cycle;
#ifdef __cplusplus
}
#endif

#endif