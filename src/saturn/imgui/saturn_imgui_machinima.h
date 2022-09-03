#ifndef SaturnImGuiMachinima
#define SaturnImGuiMachinima

#include "SDL2/SDL.h"

extern void anim_play_button();

#ifdef __cplusplus
extern "C" {
#endif
    void smachinima_imgui_init(void);
    void smachinima_imgui_update(void);
    void smachinima_imgui_controls(SDL_Event * event);
#ifdef __cplusplus
}
#endif

#endif