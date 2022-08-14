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
    std::vector<Expression> expressions;
    bool cc_support;
    bool spark_support;
};

extern struct ModelData current_model_data;
extern bool using_model_eyes;
void saturn_load_model_json(std::string folder_name);
void saturn_load_model_data(std::string folder_name);
void saturn_set_model_texture(int expIndex, std::string path);

extern "C" {
#endif
    extern bool is_replacing_eyes;

    const void* saturn_bind_texture(const void*);
    void saturn_set_eye_texture(int);
#ifdef __cplusplus
}
#endif

#endif