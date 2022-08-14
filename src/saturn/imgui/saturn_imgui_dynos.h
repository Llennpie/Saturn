#ifndef SaturnImGuiDynOS
#define SaturnImGuiDynOS

#include "SDL2/SDL.h"

#ifdef __cplusplus
#include <string>

extern void set_editor_from_global_cc(std::string);
extern int numColorCodes;

extern "C" {
#endif
    void sdynos_imgui_init(void);
    void sdynos_imgui_menu(void);
    void sdynos_imgui_update(void);
#ifdef __cplusplus
}
#endif

#endif