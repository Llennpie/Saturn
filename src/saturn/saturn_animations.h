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
extern s16 current_canim_values[99999];
extern u16 current_canim_indices[99999];

extern std::vector<std::string> canim_array;
extern std::string canim_directory;

extern void saturn_read_mcomp_animation(std::string);
extern void saturn_fetch_animations();
extern void saturn_play_custom_animation();
extern void saturn_run_chainer();

#endif

#endif