#ifndef SaturnAnimations
#define SaturnAnimations

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
#include <string>
#include <vector>
#include <filesystem>
#include <PR/ultratypes.h>

extern std::string current_canim_name;
extern std::string current_canim_author;
extern bool current_canim_looping;
extern int current_canim_length;
extern int current_canim_nodes;
extern std::vector<s16> current_canim_values;
extern std::vector<u16> current_canim_indices;

extern std::vector<std::string> canim_array;
extern std::string current_anim_dir_path;

extern void saturn_read_mcomp_animation(std::string);
extern void saturn_load_anim_folder(std::string, int*);
extern void saturn_play_custom_animation();
extern void saturn_run_chainer();
extern int saturn_anim_by_name(std::string);

extern const char* saturn_animations_list[];

#endif

extern bool current_canim_has_extra;

#endif