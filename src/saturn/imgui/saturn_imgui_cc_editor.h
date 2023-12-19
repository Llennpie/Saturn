#ifndef SaturnImGuiCcEditor
#define SaturnImGuiCcEditor

#ifdef __cplusplus
    #include <string>
    #include "saturn/libs/imgui/imgui.h"
    #include "saturn/saturn_models.h"

    extern int uiCcListId;

    extern void UpdateEditorFromPalette();

    extern void ResetColorCode(bool);
    extern void RefreshColorCodeList();

    extern void OpenCCSelector();
    extern void OpenModelCCSelector(Model, std::vector<std::string>, std::string);
    extern void OpenCCEditor();

    extern bool has_open_any_model_cc;

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

#endif

#endif