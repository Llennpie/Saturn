#ifndef Saturn
#define Saturn

#include <stdio.h>
#include <stdbool.h>
#include <mario_animation_ids.h>

extern bool camera_frozen;
extern float camera_speed;

extern int current_eye_state;

extern bool enable_head_rotations;
extern bool enable_shadows;
extern bool enable_dust_particles;

extern bool is_anim_playing;
extern enum MarioAnimID selected_animation;

void saturn_update(void);
void saturn_play_animation(MarioAnimID);

#endif