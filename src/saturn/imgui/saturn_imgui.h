#ifndef SaturnImGui
#define SaturnImGui

#include <SDL2/SDL.h>

#ifdef __cplusplus

#include <string>

extern void imgui_update_theme();
extern void imgui_bundled_tooltip(const char*);
extern void imgui_bundled_help_marker(const char*);
extern void imgui_bundled_space(float, const char*, const char*);
extern void saturn_keyframe_popout(float*, std::string, std::string);

extern bool showMenu;
extern int currentMenu;

extern bool windowCcEditor;
extern bool windowAnimPlayer;
extern bool windowChromaKey;

extern bool chromaRequireReload;

extern SDL_Window* window;

extern "C" {
#endif
    void saturn_imgui_init(SDL_Window *, SDL_GLContext);
    void saturn_imgui_handle_events(SDL_Event *);
    void saturn_imgui_update(void);

    extern SDL_Scancode bind_to_sdl_scancode[512];
#ifdef __cplusplus
}
#endif

#endif