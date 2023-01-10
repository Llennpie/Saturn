#ifndef Saturn
#define Saturn

#include <stdio.h>
#include <PR/ultratypes.h>
#include <stdbool.h>
#include <mario_animation_ids.h>
#include <SDL2/SDL.h>

extern bool mario_exists;

extern bool camera_frozen;
extern float camera_speed;
extern float camera_fov;
extern bool camera_fov_smooth;

extern bool camera_view_enabled;
extern bool camera_view_moving;
extern bool camera_view_zooming;
extern bool camera_view_rotating;
extern int camera_view_move_x;
extern int camera_view_move_y;

extern int current_eye_state;

extern bool enable_head_rotations;
extern bool enable_shadows;
extern bool enable_dust_particles;
extern bool enable_torso_rotation;
extern bool can_fall_asleep;
extern int saturnModelState;
extern bool linkMarioScale;
extern bool is_spinning;
extern float spin_mult;

extern bool is_custom_anim;
extern bool using_chainer;
extern int chainer_index;
extern bool is_anim_playing;
extern enum MarioAnimID selected_animation;
extern bool is_anim_looped;
extern float anim_speed;
extern int current_anim_frame;
extern int current_anim_id;
extern int current_anim_length;
extern bool is_anim_paused;

extern bool limit_fps;
extern bool has_discord_init;

extern unsigned int chromaKeyColorR;
extern unsigned int chromaKeyColorG;
extern unsigned int chromaKeyColorB;

extern u16 gChromaKeyColor;
extern u16 gChromaKeyBackground;

extern bool mcamera_is_keyframe;
extern bool mcamera_playing;
extern int mcam_timer;

extern SDL_Scancode saturn_key_to_scancode(unsigned int key[]);

#ifdef __cplusplus
extern "C" {
#endif
    void saturn_update(void);
    void saturn_play_animation(MarioAnimID);
    void saturn_print(const char*);
    const char* saturn_get_stage_name(int);
#ifdef __cplusplus
}
#endif

#endif
