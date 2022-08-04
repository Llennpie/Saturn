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
void saturn_load_eyes_from_model(std::string, std::string);

void saturn_load_mouths_from_model(std::string, std::string);
void saturn_mouth_selectable(std::string, int);

extern "C" {
#endif
    extern bool is_replacing_eyes;

    const void* saturn_bind_texture(const void*);
    void saturn_set_eye_texture(int);
    void saturn_set_mouth_texture(int);
#ifdef __cplusplus
}
#endif

#endif