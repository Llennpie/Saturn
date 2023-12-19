#ifndef SaturnImGuiDynOS
#define SaturnImGuiDynOS

#include "SDL2/SDL.h"
#include "saturn/libs/imgui/imgui.h"

#ifdef __cplusplus
#include <string>
#include <vector>

extern bool is_gameshark_open;

extern std::vector<std::string> choose_file_dialog(std::string windowTitle, std::vector<std::string> filetypes, bool multiselect);

extern "C" {
#endif
    //void apply_cc_from_editor(void);
    void sdynos_imgui_init(void);
    void sdynos_imgui_menu(void);
#ifdef __cplusplus
}
#endif

#endif