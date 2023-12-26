#ifndef SaturnImGuiMachinima
#define SaturnImGuiMachinima

#include "SDL2/SDL.h"
#include "include/types.h"

extern float gravity;
extern int time_freeze_state;
extern int current_sanim_id;

extern void anim_play_button();
extern void saturn_create_object(int, const BehaviorScript*, float, float, float, s16, s16, s16, int);

#ifdef __cplusplus
extern "C" {
#endif
    void warp_to_level(int, int, int);
    int get_saturn_level_id(int);
    void smachinima_imgui_init(void);
    void smachinima_imgui_controls(SDL_Event * event);

    void imgui_machinima_animation_player(void);
    void imgui_machinima_quick_options(void);
#ifdef __cplusplus
}
#endif

#endif