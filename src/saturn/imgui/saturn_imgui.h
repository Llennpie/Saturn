#ifndef SaturnImGui
#define SaturnImGui

#include <SDL2/SDL.h>
#include <PR/ultratypes.h>

#ifdef __cplusplus

#include <string>
#include "saturn/saturn.h"

#define UNSTABLE if (configUnstableFeatures)

extern void imgui_update_theme();
extern void imgui_bundled_tooltip(const char*);
extern void imgui_bundled_help_marker(const char*);
extern void imgui_bundled_space(float, const char*, const char*);

enum {
    KEY_FLOAT,
    KEY_INT,
    KEY_BOOL,
    KEY_CAMERA,
    KEY_EXPRESSION,
    KEY_COLOR,
};

extern void saturn_create_keyframe(std::string id, InterpolationCurve curve);
extern void saturn_keyframe_popout(std::string id);
extern void saturn_keyframe_popout(std::vector<std::string> id);
extern void saturn_keyframe_popout_next_line(std::string id);
extern void saturn_keyframe_popout_next_line(std::vector<std::string> id);

template <typename T>
extern void saturn_keyframe_popout(const T &edit_value, s32 data_type, std::string, std::string);

extern void saturn_keyframe_context_popout(Keyframe keyframe);
extern void saturn_keyframe_show_kf_content(Keyframe keyframe);

extern void saturn_keyframe_sort(std::vector<Keyframe>* keyframes);

extern bool showMenu;
extern int currentMenu;

extern bool windowCcEditor;
extern bool windowAnimPlayer;
extern bool windowChromaKey;

extern bool chromaRequireReload;

extern SDL_Window* window;

extern uint32_t endFrame;
extern int endFrameText;

extern bool splash_finished;

extern std::string editor_theme;
extern std::vector<std::pair<std::string, std::string>> theme_list;

extern bool k_context_popout_open;

extern "C" {
#endif
    void saturn_imgui_init_backend(SDL_Window *, SDL_GLContext);
    void saturn_imgui_init();
    void saturn_imgui_handle_events(SDL_Event *);
    void saturn_imgui_update(void);
    bool saturn_disable_sm64_input();

    extern SDL_Scancode bind_to_sdl_scancode[512];
#ifdef __cplusplus
}
#endif

#endif