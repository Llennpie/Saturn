#ifndef SaturnImGuiDynOS
#define SaturnImGuiDynOS

#include "SDL2/SDL.h"
#include "saturn/libs/imgui/imgui.h"

#ifdef __cplusplus
#include <string>

extern void set_editor_from_global_cc(std::string);
extern int numColorCodes;
extern ImVec4 uiHatColor;
extern ImVec4 uiHatShadeColor;
extern ImVec4 uiOverallsColor;
extern ImVec4 uiOverallsShadeColor;
extern ImVec4 uiGlovesColor;
extern ImVec4 uiGlovesShadeColor;
extern ImVec4 uiShoesColor;
extern ImVec4 uiShoesShadeColor;
extern ImVec4 uiSkinColor;
extern ImVec4 uiSkinShadeColor;
extern ImVec4 uiHairColor;
extern ImVec4 uiHairShadeColor;
extern ImVec4 uiShirtColor;
extern ImVec4 uiShirtShadeColor;
extern ImVec4 uiShouldersColor;
extern ImVec4 uiShouldersShadeColor;
extern ImVec4 uiArmsColor;
extern ImVec4 uiArmsShadeColor;
extern ImVec4 uiOverallsBottomColor;
extern ImVec4 uiOverallsBottomShadeColor;
extern ImVec4 uiLegTopColor;
extern ImVec4 uiLegTopShadeColor;
extern ImVec4 uiLegBottomColor;
extern ImVec4 uiLegBottomShadeColor;

extern bool is_gameshark_open;

extern "C" {
#endif
    void apply_cc_from_editor(void);
    void sdynos_imgui_init(void);
    void sdynos_imgui_menu(void);
    void imgui_dynos_cc_editor(void);
#ifdef __cplusplus
}
#endif

#endif