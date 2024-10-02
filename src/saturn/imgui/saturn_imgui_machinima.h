#ifndef SaturnImGuiMachinima
#define SaturnImGuiMachinima

#include "SDL2/SDL.h"
#include "include/types.h"

extern float gravity;
extern bool enable_time_freeze;
extern int current_sanim_id;

extern bool override_level;
extern struct GraphNode* override_level_geolayout;
extern Collision* override_level_collision;

extern void anim_play_button();
extern void saturn_create_object(int, const BehaviorScript*, float, float, float, s16, s16, s16, int);

#ifdef __cplusplus
extern "C" {
#endif

    void warp_to_level(int, int, int);
    int get_saturn_level_id(int);
    void smachinima_imgui_init(void);
    void smachinima_imgui_controls(SDL_Event * event);
    Gfx* geo_switch_override_model(s32 callContext, struct GraphNode *node, UNUSED Mat4 *mtx);

    void imgui_machinima_animation_player(void);
    void imgui_machinima_quick_options(void);
#ifdef __cplusplus
}
#endif

#endif