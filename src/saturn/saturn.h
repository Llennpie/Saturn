#ifndef Saturn
#define Saturn

#include <stdio.h>
#include <stdbool.h>
#include <mario_animation_ids.h>

extern bool mario_exists;

extern bool camera_frozen;
extern float camera_speed;

extern int current_eye_state;

extern bool enable_head_rotations;
extern bool enable_shadows;
extern bool enable_dust_particles;

extern bool is_anim_playing;
extern enum MarioAnimID selected_animation;
extern bool is_anim_looped;
extern int current_anim_frame;
extern int current_anim_id;
extern int current_anim_length;
extern bool is_anim_paused;

#ifdef __cplusplus
extern "C" {
#endif
    void saturn_update(void);
    void saturn_play_animation(MarioAnimID);
#ifdef __cplusplus
}
#endif

#endif