#ifndef SaturnImGui
#define SaturnImGui

#include "SDL2/SDL.h"

#ifdef __cplusplus

extern void imgui_update_theme();
extern void imgui_bundled_tooltip(const char*);
extern void imgui_bundled_help_marker(const char*);
extern void imgui_bundled_space(float, const char*, const char*);

extern bool showMenu;
extern int currentMenu;

extern SDL_Window* window;

extern "C" {
#endif
    void saturn_imgui_init(SDL_Window *, SDL_GLContext);
    void saturn_imgui_handle_events(SDL_Event *);
    void saturn_imgui_update(void);
#ifdef __cplusplus
}
#endif

#endif