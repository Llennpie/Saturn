#ifndef SaturnTextures
#define SaturnTextures

#include <stdio.h>
#include <stdbool.h>



#ifdef __cplusplus
#include <string>
#include <vector>

extern std::vector<std::string> eye_array;
extern bool model_eyes_enabled;
extern std::vector<std::string> mouth_array;
extern bool model_mouth_enabled;

void saturn_load_eyes_from_model(std::string);
void saturn_load_mouths_from_model(std::string);

extern "C" {
#endif
    extern bool is_replacing_eyes;

    void saturn_load_eye_directory();
    const void* saturn_bind_texture(const void*);
    void saturn_set_eye_texture(int);
    void saturn_set_mouth_texture(int);
#ifdef __cplusplus
}
#endif

#endif