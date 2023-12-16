#include "saturn_imgui_cc_editor.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include "saturn/imgui/saturn_file_browser.h"
#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/libs/imgui/imgui-knobs.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/saturn_models.h"
#include "saturn/saturn_textures.h"
#include "saturn/imgui/saturn_imgui.h"
#include "saturn/imgui/saturn_imgui_dynos.h"
#include "icons/IconsForkAwesome.h"

// UI Variables; Used by specifically ImGui

int uiCcListId;
static char uiCcLabelName[128] = "Sample";
static char uiGameShark[1024 * 16] =    "8107EC40 FF00\n8107EC42 0000\n8107EC38 7F00\n8107EC3A 0000\n"
                                        "8107EC28 0000\n8107EC2A FF00\n8107EC20 0000\n8107EC22 7F00\n"
                                        "8107EC58 FFFF\n8107EC5A FF00\n8107EC50 7F7F\n8107EC52 7F00\n"
                                        "8107EC70 721C\n8107EC72 0E00\n8107EC68 390E\n8107EC6A 0700\n"
                                        "8107EC88 FEC1\n8107EC8A 7900\n8107EC80 7F60\n8107EC82 3C00\n"
                                        "8107ECA0 7306\n8107ECA2 0000\n8107EC98 3903\n8107EC9A 0000";

ImVec4 uiHatColor =              ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiHatShadeColor =         ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiOverallsColor =         ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiOverallsShadeColor =    ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiGlovesColor =           ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiGlovesShadeColor =      ImVec4(127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiShoesColor =            ImVec4(114.0f / 255.0f, 28.0f / 255.0f, 14.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiShoesShadeColor =       ImVec4(57.0f / 255.0f, 14.0f / 255.0f, 7.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiSkinColor =             ImVec4(254.0f / 255.0f, 193.0f / 255.0f, 121.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiSkinShadeColor =        ImVec4(127.0f / 255.0f, 96.0f / 255.0f, 60.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiHairColor =             ImVec4(115.0f / 255.0f, 6.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiHairShadeColor =        ImVec4(57.0f / 255.0f, 3.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
// SPARK
ImVec4 uiShirtColor =                    ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiShirtShadeColor =               ImVec4(127.0f / 255.0f, 127.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiShouldersColor =                ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiShouldersShadeColor =           ImVec4(0.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiArmsColor =                     ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiArmsShadeColor =                ImVec4(0.0f / 255.0f, 127.0f / 255.0f, 64.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiOverallsBottomColor =           ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiOverallsBottomShadeColor =      ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiLegTopColor =                   ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiLegTopShadeColor =              ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 64.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiLegBottomColor =                ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
ImVec4 uiLegBottomShadeColor =           ImVec4(64.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);

static char ccSearchTerm[128];
bool has_open_any_model_cc;

bool auto_onehalf = false;

/*
Update the "defaultColor" presets with our CC Editor colors.
These values are directly used in gfx_pc.c to overwrite incoming lights.
*/
void UpdatePaletteFromEditor() {
    defaultColorHat.red[0] = (int)(uiHatColor.x * 255);
    defaultColorHat.green[0] = (int)(uiHatColor.y * 255);
    defaultColorHat.blue[0] = (int)(uiHatColor.z * 255);
    defaultColorHat.red[1] = (int)(uiHatShadeColor.x * 255);
    defaultColorHat.green[1] = (int)(uiHatShadeColor.y * 255);
    defaultColorHat.blue[1] = (int)(uiHatShadeColor.z * 255);
    defaultColorOveralls.red[0] = (int)(uiOverallsColor.x * 255);
    defaultColorOveralls.green[0] = (int)(uiOverallsColor.y * 255);
    defaultColorOveralls.blue[0] = (int)(uiOverallsColor.z * 255);
    defaultColorOveralls.red[1] = (int)(uiOverallsShadeColor.x * 255);
    defaultColorOveralls.green[1] = (int)(uiOverallsShadeColor.y * 255);
    defaultColorOveralls.blue[1] = (int)(uiOverallsShadeColor.z * 255);
    defaultColorGloves.red[0] = (int)(uiGlovesColor.x * 255);
    defaultColorGloves.green[0] = (int)(uiGlovesColor.y * 255);
    defaultColorGloves.blue[0] = (int)(uiGlovesColor.z * 255);
    defaultColorGloves.red[1] = (int)(uiGlovesShadeColor.x * 255);
    defaultColorGloves.green[1] = (int)(uiGlovesShadeColor.y * 255);
    defaultColorGloves.blue[1] = (int)(uiGlovesShadeColor.z * 255);
    defaultColorShoes.red[0] = (int)(uiShoesColor.x * 255);
    defaultColorShoes.green[0] = (int)(uiShoesColor.y * 255);
    defaultColorShoes.blue[0] = (int)(uiShoesColor.z * 255);
    defaultColorShoes.red[1] = (int)(uiShoesShadeColor.x * 255);
    defaultColorShoes.green[1] = (int)(uiShoesShadeColor.y * 255);
    defaultColorShoes.blue[1] = (int)(uiShoesShadeColor.z * 255);
    defaultColorSkin.red[0] = (int)(uiSkinColor.x * 255);
    defaultColorSkin.green[0] = (int)(uiSkinColor.y * 255);
    defaultColorSkin.blue[0] = (int)(uiSkinColor.z * 255);
    defaultColorSkin.red[1] = (int)(uiSkinShadeColor.x * 255);
    defaultColorSkin.green[1] = (int)(uiSkinShadeColor.y * 255);
    defaultColorSkin.blue[1] = (int)(uiSkinShadeColor.z * 255);
    defaultColorHair.red[0] = (int)(uiHairColor.x * 255);
    defaultColorHair.green[0] = (int)(uiHairColor.y * 255);
    defaultColorHair.blue[0] = (int)(uiHairColor.z * 255);
    defaultColorHair.red[1] = (int)(uiHairShadeColor.x * 255);
    defaultColorHair.green[1] = (int)(uiHairShadeColor.y * 255);
    defaultColorHair.blue[1] = (int)(uiHairShadeColor.z * 255);

    sparkColorShirt.red[0] = (int)(uiShirtColor.x * 255);
    sparkColorShirt.green[0] = (int)(uiShirtColor.y * 255);
    sparkColorShirt.blue[0] = (int)(uiShirtColor.z * 255);
    sparkColorShirt.red[1] = (int)(uiShirtShadeColor.x * 255);
    sparkColorShirt.green[1] = (int)(uiShirtShadeColor.y * 255);
    sparkColorShirt.blue[1] = (int)(uiShirtShadeColor.z * 255);
    sparkColorShoulders.red[0] = (int)(uiShouldersColor.x * 255);
    sparkColorShoulders.green[0] = (int)(uiShouldersColor.y * 255);
    sparkColorShoulders.blue[0] = (int)(uiShouldersColor.z * 255);
    sparkColorShoulders.red[1] = (int)(uiShouldersShadeColor.x * 255);
    sparkColorShoulders.green[1] = (int)(uiShouldersShadeColor.y * 255);
    sparkColorShoulders.blue[1] = (int)(uiShouldersShadeColor.z * 255);
    sparkColorArms.red[0] = (int)(uiArmsColor.x * 255);
    sparkColorArms.green[0] = (int)(uiArmsColor.y * 255);
    sparkColorArms.blue[0] = (int)(uiArmsColor.z * 255);
    sparkColorArms.red[1] = (int)(uiArmsShadeColor.x * 255);
    sparkColorArms.green[1] = (int)(uiArmsShadeColor.y * 255);
    sparkColorArms.blue[1] = (int)(uiArmsShadeColor.z * 255);
    sparkColorOverallsBottom.red[0] = (int)(uiOverallsBottomColor.x * 255);
    sparkColorOverallsBottom.green[0] = (int)(uiOverallsBottomColor.y * 255);
    sparkColorOverallsBottom.blue[0] = (int)(uiOverallsBottomColor.z * 255);
    sparkColorOverallsBottom.red[1] = (int)(uiOverallsBottomShadeColor.x * 255);
    sparkColorOverallsBottom.green[1] = (int)(uiOverallsBottomShadeColor.y * 255);
    sparkColorOverallsBottom.blue[1] = (int)(uiOverallsBottomShadeColor.z * 255);
    sparkColorLegTop.red[0] = (int)(uiLegTopColor.x * 255);
    sparkColorLegTop.green[0] = (int)(uiLegTopColor.y * 255);
    sparkColorLegTop.blue[0] = (int)(uiLegTopColor.z * 255);
    sparkColorLegTop.red[1] = (int)(uiLegTopShadeColor.x * 255);
    sparkColorLegTop.green[1] = (int)(uiLegTopShadeColor.y * 255);
    sparkColorLegTop.blue[1] = (int)(uiLegTopShadeColor.z * 255);
    sparkColorLegBottom.red[0] = (int)(uiLegBottomColor.x * 255);
    sparkColorLegBottom.green[0] = (int)(uiLegBottomColor.y * 255);
    sparkColorLegBottom.blue[0] = (int)(uiLegBottomColor.z * 255);
    sparkColorLegBottom.red[1] = (int)(uiLegBottomShadeColor.x * 255);
    sparkColorLegBottom.green[1] = (int)(uiLegBottomShadeColor.y * 255);
    sparkColorLegBottom.blue[1] = (int)(uiLegBottomShadeColor.z * 255);

    current_color_code.ParseGameShark();
    strcpy(uiGameShark, current_color_code.GameShark.c_str());
    current_color_code.Name = uiCcLabelName;

    std::replace(current_color_code.Name.begin(), current_color_code.Name.end(), '.', '-');
    std::replace(current_color_code.Name.begin(), current_color_code.Name.end(), '/', '-');
    std::replace(current_color_code.Name.begin(), current_color_code.Name.end(), '\\', '-');
} 

/*
Update our CC Editor colors with our "defaultColor" values.
This should be called when loading a CC, to insert our new colors into the editor.
*/
void UpdateEditorFromPalette() {
    uiHatColor = ImVec4(float(defaultColorHat.red[0]) / 255.0f, float(defaultColorHat.green[0]) / 255.0f, float(defaultColorHat.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiHatShadeColor = ImVec4(float(defaultColorHat.red[1]) / 255.0f, float(defaultColorHat.green[1]) / 255.0f, float(defaultColorHat.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiOverallsColor = ImVec4(float(defaultColorOveralls.red[0]) / 255.0f, float(defaultColorOveralls.green[0]) / 255.0f, float(defaultColorOveralls.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiOverallsShadeColor = ImVec4(float(defaultColorOveralls.red[1]) / 255.0f, float(defaultColorOveralls.green[1]) / 255.0f, float(defaultColorOveralls.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiGlovesColor = ImVec4(float(defaultColorGloves.red[0]) / 255.0f, float(defaultColorGloves.green[0]) / 255.0f, float(defaultColorGloves.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiGlovesShadeColor = ImVec4(float(defaultColorGloves.red[1]) / 255.0f, float(defaultColorGloves.green[1]) / 255.0f, float(defaultColorGloves.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiShoesColor = ImVec4(float(defaultColorShoes.red[0]) / 255.0f, float(defaultColorShoes.green[0]) / 255.0f, float(defaultColorShoes.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiShoesShadeColor = ImVec4(float(defaultColorShoes.red[1]) / 255.0f, float(defaultColorShoes.green[1]) / 255.0f, float(defaultColorShoes.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiSkinColor = ImVec4(float(defaultColorSkin.red[0]) / 255.0f, float(defaultColorSkin.green[0]) / 255.0f, float(defaultColorSkin.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiSkinShadeColor = ImVec4(float(defaultColorSkin.red[1]) / 255.0f, float(defaultColorSkin.green[1]) / 255.0f, float(defaultColorSkin.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiHairColor = ImVec4(float(defaultColorHair.red[0]) / 255.0f, float(defaultColorHair.green[0]) / 255.0f, float(defaultColorHair.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiHairShadeColor = ImVec4(float(defaultColorHair.red[1]) / 255.0f, float(defaultColorHair.green[1]) / 255.0f, float(defaultColorHair.blue[1]) / 255.0f, 255.0f / 255.0f);

    uiShirtColor = ImVec4(float(sparkColorShirt.red[0]) / 255.0f, float(sparkColorShirt.green[0]) / 255.0f, float(sparkColorShirt.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiShirtShadeColor = ImVec4(float(sparkColorShirt.red[1]) / 255.0f, float(sparkColorShirt.green[1]) / 255.0f, float(sparkColorShirt.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiShouldersColor = ImVec4(float(sparkColorShoulders.red[0]) / 255.0f, float(sparkColorShoulders.green[0]) / 255.0f, float(sparkColorShoulders.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiShouldersShadeColor = ImVec4(float(sparkColorShoulders.red[1]) / 255.0f, float(sparkColorShoulders.green[1]) / 255.0f, float(sparkColorShoulders.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiArmsColor = ImVec4(float(sparkColorArms.red[0]) / 255.0f, float(sparkColorArms.green[0]) / 255.0f, float(sparkColorArms.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiArmsShadeColor = ImVec4(float(sparkColorArms.red[1]) / 255.0f, float(sparkColorArms.green[1]) / 255.0f, float(sparkColorArms.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiOverallsBottomColor = ImVec4(float(sparkColorOverallsBottom.red[0]) / 255.0f, float(sparkColorOverallsBottom.green[0]) / 255.0f, float(sparkColorOverallsBottom.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiOverallsBottomShadeColor = ImVec4(float(sparkColorOverallsBottom.red[1]) / 255.0f, float(sparkColorOverallsBottom.green[1]) / 255.0f, float(sparkColorOverallsBottom.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiLegTopColor = ImVec4(float(sparkColorLegTop.red[0]) / 255.0f, float(sparkColorLegTop.green[0]) / 255.0f, float(sparkColorLegTop.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiLegTopShadeColor = ImVec4(float(sparkColorLegTop.red[1]) / 255.0f, float(sparkColorLegTop.green[1]) / 255.0f, float(sparkColorLegTop.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiLegBottomColor = ImVec4(float(sparkColorLegBottom.red[0]) / 255.0f, float(sparkColorLegBottom.green[0]) / 255.0f, float(sparkColorLegBottom.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiLegBottomShadeColor = ImVec4(float(sparkColorLegBottom.red[1]) / 255.0f, float(sparkColorLegBottom.green[1]) / 255.0f, float(sparkColorLegBottom.blue[1]) / 255.0f, 255.0f / 255.0f);

    current_color_code.ParseGameShark();
    strcpy(uiGameShark, current_color_code.GameShark.c_str());
    strcpy(uiCcLabelName, current_color_code.Name.c_str());
}

void SparkilizeEditor() {
    uiShirtColor = uiHatColor;
    uiShirtShadeColor = uiHatShadeColor;
    uiShouldersColor= uiHatColor;
    uiShouldersShadeColor = uiHatShadeColor;
    uiArmsColor = uiHatColor;
    uiArmsShadeColor = uiHatShadeColor;
    uiOverallsBottomColor = uiOverallsColor;
    uiOverallsBottomShadeColor = uiOverallsShadeColor;
    uiLegTopColor = uiOverallsColor;
    uiLegTopShadeColor = uiOverallsShadeColor;
    uiLegBottomColor = uiOverallsColor;
    uiLegBottomShadeColor = uiOverallsShadeColor;
    UpdatePaletteFromEditor();
}

// Util Stuff

void ResetColorCode(bool usingModel) {
    uiCcListId = 1;
    // Use Mario's default palette (class init)
    current_color_code = ColorCode();
    ApplyColorCode(current_color_code);

    // Model CCs will attempt to select their first color code instead
    if (usingModel && model_color_code_list.size() > 0 && current_model.HasColorCodeFolder()) {
        uiCcListId = -1;
        current_color_code = LoadGSFile(model_color_code_list[0], current_model.FolderPath + "/colorcodes");
        ApplyColorCode(current_color_code);
        if (current_color_code.Name == "default")
            current_color_code.Name = "Sample";
        last_model_cc_address = current_color_code.GameShark;
    } else if (!usingModel)
        // Also reset the current CC labels
        current_model.Colors = Model().Colors;

    UpdateEditorFromPalette();
}

void RefreshColorCodeList() {
    if (current_model.HasColorCodeFolder()) {
        model_color_code_list.clear();
        model_color_code_list = GetColorCodeList(current_model.FolderPath + "/colorcodes");
    } else {
        color_code_list.clear();
        color_code_list = GetColorCodeList("dynos/colorcodes");
    }
}

// UI Segments

void OpenModelCCSelector(Model model, std::vector<std::string> list, std::string ccSearchLower) {
    for (int n = 0; n < list.size(); n++) {
        const bool is_selected = (uiCcListId == ((n + 1) * -1) && model.FolderPath == current_model.FolderPath);
        std::string label_name = list[n].substr(0, list[n].size() - 3);

        // If we're searching, only include CCs with the search keyword in the name
        // Also convert to lowercase
        std::string label_name_lower = label_name;
        std::transform(label_name_lower.begin(), label_name_lower.end(), label_name_lower.begin(),
                [](unsigned char c1){ return std::tolower(c1); });
        if (ccSearchLower != "") {
            if (label_name_lower.find(ccSearchLower) == std::string::npos) {
                continue;
            }
        }

        // Selectable
        if (ImGui::Selectable((ICON_FK_USER " " + label_name).c_str(), is_selected)) {
            if (model.FolderPath == current_model.FolderPath)
                uiCcListId = (n + 1) * -1;
            else uiCcListId = 0;

            // Overwrite current color code
            current_color_code = LoadGSFile(list[n], model.FolderPath + "/colorcodes");
            ApplyColorCode(current_color_code);
            if (label_name_lower == "default" || label_name_lower != "../default") {
                label_name = "Sample";
                //ResetColorCode(current_model.HasColorCodeFolder());
            }
            UpdateEditorFromPalette();
            last_model_cc_address = uiGameShark;
        }

        // Right-Click Menu
        if (ImGui::BeginPopupContextItem()) {
            if (label_name_lower != "default" || label_name_lower != "../default") {
                ImGui::Text("%s.gs", label_name.c_str());
                imgui_bundled_tooltip((model.FolderPath + "/colorcodes/" + label_name + ".gs").c_str());
                ImGui::Separator();

                // Delete GS File
                if (ImGui::Button(ICON_FK_TRASH " Delete File"))
                    ImGui::OpenPopup("###delete_m_gs_file");
                if (ImGui::BeginPopup("###delete_m_gs_file")) {

                    ImGui::Text("Are you sure you want to delete %s?", label_name.c_str());
                    if (ImGui::Button("Yes")) {
                        DeleteGSFile(model.FolderPath + "/colorcodes/" + label_name + ".gs");
                        RefreshColorCodeList();
                        ImGui::CloseCurrentPopup();
                    } ImGui::SameLine();
                    if (ImGui::Button("No")) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::TextDisabled("%i color code(s)", list.size());
            if (ImGui::Button(ICON_FK_UNDO " Refresh")) {
                RefreshColorCodeList();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void OpenCCSelector() {
    ImGui::Text("Color Codes");
    ImGui::SameLine(); imgui_bundled_help_marker(
        "These are GameShark color codes, which overwrite Mario's lights. Place GS files in dynos/colorcodes.");

    /* Search feature temporarily removed
       TODO: add it back

    // If we have 18 or more color codes, a search bar is added
    if (color_code_list.size() >= 18) {
        ImGui::InputTextWithHint("###cc_search_text", ICON_FK_SEARCH " Search color codes...", ccSearchTerm, IM_ARRAYSIZE(ccSearchTerm), ImGuiInputTextFlags_AutoSelectAll);
    } else {
        // If our CC list is reloaded, and we now have less than 18 files, this can cause filter issues if not reset to nothing
        if (ccSearchTerm != "") strcpy(ccSearchTerm, "");
    }
    std::string ccSearchLower = ccSearchTerm;
    std::transform(ccSearchLower.begin(), ccSearchLower.end(), ccSearchLower.begin(),
        [](unsigned char c){ return std::tolower(c); });
    */

    std::string inPath = "dynos/colorcodes";
    saturn_file_browser_filter_extension("gs");
    if (model_color_code_list.size() > 0 && current_model.HasColorCodeFolder()) inPath = current_model.FolderPath + "/colorcodes";
    else saturn_file_browser_item("Mario.gs");
    saturn_file_browser_scan_directory(inPath);
    if (saturn_file_browser_show("colorcodes")) {
        ApplyColorCode(LoadGSFile(saturn_file_browser_get_selected().string(), inPath));
    }

    // UI list
    /*ImGui::BeginChild("###menu_cc_selector", ImVec2(-FLT_MIN, 100), true);

    //if (!ImGui::IsPopupOpen(0u, ImGuiPopupFlags_AnyPopupId)) {
        if (model_color_code_list.size() > 0 && current_model.HasColorCodeFolder()) {
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode((current_model.FolderName + "###model_cc_header").c_str())) {
                OpenModelCCSelector(current_model, model_color_code_list, ccSearchLower);
                ImGui::TreePop();
            }
        }
    //}

    for (int n = 0; n < color_code_list.size(); n++) {

        const bool is_selected = (uiCcListId == n + 1);
        std::string label_name = color_code_list[n].substr(0, color_code_list[n].size() - 3);

        // If we're searching, only include CCs with the search keyword in the name
        // Also convert to lowercase
        std::string label_name_lower = label_name;
        std::transform(label_name_lower.begin(), label_name_lower.end(), label_name_lower.begin(),
                [](unsigned char c1){ return std::tolower(c1); });
        if (ccSearchLower != "") {
            if (label_name_lower.find(ccSearchLower) == std::string::npos) {
                continue;
            }
        }

        // Selectable
        if (ImGui::Selectable(label_name.c_str(), is_selected)) {
            uiCcListId = n + 1;

            // Overwrite current color code
            current_color_code = LoadGSFile(color_code_list[n], "dynos/colorcodes");
            ApplyColorCode(current_color_code);
            if (label_name_lower == "mario") {
                label_name = "Sample";
                current_color_code = ColorCode();
            }

            UpdateEditorFromPalette();

        }
        // Right-Click Menu
        if (ImGui::BeginPopupContextItem()) {
            if (label_name_lower != "mario") {
                ImGui::Text("%s.gs", label_name.c_str());
                imgui_bundled_tooltip(("/dynos/colorcodes/" + label_name + ".gs").c_str());
                ImGui::Separator();

                // Delete GS File
                if (ImGui::Button(ICON_FK_TRASH " Delete File"))
                    ImGui::OpenPopup("###delete_v_gs_file");
                if (ImGui::BeginPopup("###delete_v_gs_file")) {

                    ImGui::Text("Are you sure you want to delete %s?", label_name.c_str());
                    if (ImGui::Button("Yes")) {
                        DeleteGSFile("dynos/colorcodes/" + label_name + ".gs");
                        RefreshColorCodeList();
                        ImGui::CloseCurrentPopup();
                    } ImGui::SameLine();
                    if (ImGui::Button("No")) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                ImGui::Separator();
            }

            ImGui::TextDisabled("%i color code(s)", color_code_list.size());
            if (ImGui::Button(ICON_FK_UNDO " Refresh")) {
                RefreshColorCodeList();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();*/
}

void ColorPartBox(std::string name, const char* mainName, const char* shadeName, ImVec4* colorValue, ImVec4* shadeColorValue, std::string id) {
    if (name == "") return;

    // MAIN Color Picker
    ImGui::ColorEdit4(mainName, (float*)colorValue, ImGuiColorEditFlags_NoAlpha |
                                                    ImGuiColorEditFlags_InputRGB |
                                                    ImGuiColorEditFlags_Uint8 |
                                                    ImGuiColorEditFlags_NoLabel |
                                                    ImGuiColorEditFlags_NoOptions |
                                                    ImGuiColorEditFlags_NoInputs);
    if (ImGui::IsItemHovered()) {
        UpdatePaletteFromEditor();
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) ImGui::OpenPopup(id.c_str());
    }
    if (ImGui::BeginPopup(id.c_str())) {
        // Misc. Options

        // Multiply shade by 2
        if (ImGui::Selectable("×2")) {
            colorValue->x = shadeColorValue->x * 1.99f;
            colorValue->y = shadeColorValue->y * 1.99f;
            colorValue->z = shadeColorValue->z * 1.99f;
            UpdatePaletteFromEditor();
        }
        // Reset
        if (ImGui::Selectable(ICON_FK_UNDO " Reset")) {
            if (mainName == "Hat, Main")                PasteGameShark("8107EC40 FF00\n8107EC42 0000");
            if (mainName == "Overalls, Main")           PasteGameShark("8107EC28 0000\n8107EC2A FF00");
            if (mainName == "Gloves, Main")             PasteGameShark("8107EC58 FFFF\n8107EC5A FF00");
            if (mainName == "Shoes, Main")              PasteGameShark("8107EC70 721C\n8107EC72 0E00");
            if (mainName == "Skin, Main")               PasteGameShark("8107EC88 FEC1\n8107EC8A 7900");
            if (mainName == "Hair, Main")               PasteGameShark("8107ECA0 7306\n8107ECA2 0000");
            if (mainName == "Shirt, Main")              PasteGameShark("8107ECB8 FFFF\n8107ECBA 0000");
            if (mainName == "Shoulders, Main")          PasteGameShark("8107ECD0 00FF\n8107ECD2 FF00");
            if (mainName == "Arms, Main")               PasteGameShark("8107ECE8 00FF\n8107ECEA 7F00");
            if (mainName == "Overalls (Bottom), Main")  PasteGameShark("8107ED00 FF00\n8107ED02 FF00");
            if (mainName == "Leg (Top), Main")          PasteGameShark("8107ED18 FF00\n8107ED1A 7F00");
            if (mainName == "Leg (Bottom), Main")       PasteGameShark("8107ED30 7F00\n8107ED32 FF00");
            UpdateEditorFromPalette();
        }
        // Randomize
        if (ImGui::Selectable("Randomize")) {
            colorValue->x = (rand() % 255) / 255.0f;
            colorValue->y = (rand() % 255) / 255.0f;
            colorValue->z = (rand() % 255) / 255.0f;
            UpdatePaletteFromEditor();
        }

        // Keyframes
        ImGui::Dummy(ImVec2(0, 15));
        std::string id2 = "k_" + id + "_1";
        saturn_keyframe_popout(id2);

        ImGui::EndPopup();
    } ImGui::SameLine();

    // SHADE Color Picker
    ImGui::ColorEdit4(shadeName, (float*)shadeColorValue,   ImGuiColorEditFlags_NoAlpha |
                                                            ImGuiColorEditFlags_InputRGB |
                                                            ImGuiColorEditFlags_Uint8 |
                                                            ImGuiColorEditFlags_NoLabel |
                                                            ImGuiColorEditFlags_NoOptions |
                                                            ImGuiColorEditFlags_NoInputs);

    if (ImGui::IsItemHovered()) {
        UpdatePaletteFromEditor();
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) ImGui::OpenPopup((id + "1").c_str());
    }
    if (ImGui::BeginPopup((id + "1").c_str())) {
        // Misc. Options

        // Divide main by 2
        if (ImGui::Selectable("1/2")) {
            shadeColorValue->x = floor(colorValue->x / 2.0f * 255.0f) / 255.0f;
            shadeColorValue->y = floor(colorValue->y / 2.0f * 255.0f) / 255.0f;
            shadeColorValue->z = floor(colorValue->z / 2.0f * 255.0f) / 255.0f;
            UpdatePaletteFromEditor();
        }
        // Reset
        if (ImGui::Selectable(ICON_FK_UNDO " Reset")) {
            if (shadeName == "Hat, Shade")                PasteGameShark("8107EC38 7F00\n8107EC3A 0000");
            if (shadeName == "Overalls, Shade")           PasteGameShark("8107EC20 0000\n8107EC22 7F00");
            if (shadeName == "Gloves, Shade")             PasteGameShark("8107EC50 7F7F\n8107EC52 7F00");
            if (shadeName == "Shoes, Shade")              PasteGameShark("8107EC68 390E\n8107EC6A 0700");
            if (shadeName == "Skin, Shade")               PasteGameShark("8107EC80 7F60\n8107EC82 3C00");
            if (shadeName == "Hair, Shade")               PasteGameShark("8107EC98 3903\n8107EC9A 0000");
            if (shadeName == "Shirt, Shade")              PasteGameShark("8107ECB0 7F7F\n8107ECB2 0000");
            if (shadeName == "Shoulders, Shade")          PasteGameShark("8107ECC8 007F\n8107ECCA 7F00");
            if (shadeName == "Arms, Shade")               PasteGameShark("8107ECE0 007F\n8107ECE2 4000");
            if (shadeName == "Overalls (Bottom), Shade")  PasteGameShark("8107ECF8 7F00\n8107ECFA 7F00");
            if (shadeName == "Leg (Top), Shade")          PasteGameShark("8107ED10 7F00\n8107ED12 4000");
            if (shadeName == "Leg (Bottom), Shade")       PasteGameShark("8107ED28 4000\n8107ED2A 7F00");
            UpdateEditorFromPalette();
        }
        // Randomize
        if (ImGui::Selectable("Randomize")) {
            shadeColorValue->x = (rand() % 127) / 255.0f;
            shadeColorValue->y = (rand() % 127) / 255.0f;
            shadeColorValue->z = (rand() % 127) / 255.0f;
            UpdatePaletteFromEditor();
        }

        // Keyframes
        ImGui::Dummy(ImVec2(0, 15));
        std::string id3 = "k_" + id + "_2";
        saturn_keyframe_popout(id3);

        ImGui::EndPopup();
    } ImGui::SameLine();

    ImGui::Text(name.c_str());

    if (ImGui::IsPopupOpen(ImGui::GetID((std::string(mainName) + "picker").c_str()), ImGuiPopupFlags_None) && auto_onehalf) {
        shadeColorValue->x = floor(colorValue->x / 2.0f * 255.0f) / 255.0f;
        shadeColorValue->y = floor(colorValue->y / 2.0f * 255.0f) / 255.0f;
        shadeColorValue->z = floor(colorValue->z / 2.0f * 255.0f) / 255.0f;
    }
    if (ImGui::IsPopupOpen(ImGui::GetID((std::string(shadeName) + "picker").c_str()), ImGuiPopupFlags_None) && auto_onehalf) {
        colorValue->x = floor(shadeColorValue->x * 2.0f * 255.0f) / 255.0f;
        colorValue->y = floor(shadeColorValue->y * 2.0f * 255.0f) / 255.0f;
        colorValue->z = floor(shadeColorValue->z * 2.0f * 255.0f) / 255.0f;
    }

    if (ImGui::IsPopupOpen(ImGui::GetID((std::string(mainName) + "picker").c_str()), ImGuiPopupFlags_None) ||
        ImGui::IsPopupOpen(ImGui::GetID((std::string(shadeName) + "picker").c_str()), ImGuiPopupFlags_None)) {

            UpdatePaletteFromEditor();
    }
}

void OpenCCEditor() {
    ImGui::PushItemWidth(100);
    ImGui::InputText(".gs", uiCcLabelName, IM_ARRAYSIZE(uiCcLabelName));
    ImGui::PopItemWidth();

    ImGui::SameLine(150);

    if (ImGui::Button(ICON_FK_FILE_TEXT " File###save_cc_to_file")) {
        UpdatePaletteFromEditor();
        SaveGSFile(current_color_code, "dynos/colorcodes");
        RefreshColorCodeList();
    }
    ImGui::Dummy(ImVec2(0, 0));
    ImGui::SameLine(150);
    if (current_model.ColorCodeSupport && AnyModelsEnabled()) {
        if (ImGui::Button(ICON_FK_FOLDER_OPEN_O " Model###save_cc_to_model")) {
            UpdatePaletteFromEditor();
            SaveGSFile(current_color_code, current_model.FolderPath + "/colorcodes");
            RefreshColorCodeList();
        }
    }

    ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::Button(ICON_FK_UNDO " Reset Colors"))
        ResetColorCode(current_model.HasColorCodeFolder());

    ImGui::SameLine(); if (ImGui::SmallButton(ICON_FK_RANDOM "###randomize_all")) {
        uiHatColor.x = (rand() % 255) / 255.0f; uiHatColor.y = (rand() % 255) / 255.0f; uiHatColor.z = (rand() % 255) / 255.0f;
        uiHatShadeColor.x = (rand() % 127) / 255.0f; uiHatShadeColor.y = (rand() % 127) / 255.0f; uiHatShadeColor.z = (rand() % 127) / 255.0f;
        uiOverallsColor.x = (rand() % 255) / 255.0f; uiOverallsColor.y = (rand() % 255) / 255.0f; uiOverallsColor.z = (rand() % 255) / 255.0f;
        uiOverallsShadeColor.x = (rand() % 127) / 255.0f; uiOverallsShadeColor.y = (rand() % 127) / 255.0f; uiOverallsShadeColor.z = (rand() % 127) / 255.0f;
        uiGlovesColor.x = (rand() % 255) / 255.0f; uiGlovesColor.y = (rand() % 255) / 255.0f; uiGlovesColor.z = (rand() % 255) / 255.0f;
        uiGlovesShadeColor.x = (rand() % 127) / 255.0f; uiGlovesShadeColor.y = (rand() % 127) / 255.0f; uiGlovesShadeColor.z = (rand() % 127) / 255.0f;
        uiShoesColor.x = (rand() % 255) / 255.0f; uiShoesColor.y = (rand() % 255) / 255.0f; uiShoesColor.z = (rand() % 255) / 255.0f;
        uiShoesShadeColor.x = (rand() % 127) / 255.0f; uiShoesShadeColor.y = (rand() % 127) / 255.0f; uiShoesShadeColor.z = (rand() % 127) / 255.0f;
        uiSkinColor.x = (rand() % 255) / 255.0f; uiSkinColor.y = (rand() % 255) / 255.0f; uiSkinColor.z = (rand() % 255) / 255.0f;
        uiSkinShadeColor.x = (rand() % 127) / 255.0f; uiSkinShadeColor.y = (rand() % 127) / 255.0f; uiSkinShadeColor.z = (rand() % 127) / 255.0f;
        uiHairColor.x = (rand() % 255) / 255.0f; uiHairColor.y = (rand() % 255) / 255.0f; uiHairColor.z = (rand() % 255) / 255.0f;
        uiHairShadeColor.x = (rand() % 127) / 255.0f; uiHairShadeColor.y = (rand() % 127) / 255.0f; uiHairShadeColor.z = (rand() % 127) / 255.0f;
        uiShirtColor.x = (rand() % 255) / 255.0f; uiShirtColor.y = (rand() % 255) / 255.0f; uiShirtColor.z = (rand() % 255) / 255.0f;
        uiShirtShadeColor.x = (rand() % 127) / 255.0f; uiShirtShadeColor.y = (rand() % 127) / 255.0f; uiShirtShadeColor.z = (rand() % 127) / 255.0f;
        uiShouldersColor.x = (rand() % 255) / 255.0f; uiShouldersColor.y = (rand() % 255) / 255.0f; uiShouldersColor.z = (rand() % 255) / 255.0f;
        uiShouldersShadeColor.x = (rand() % 127) / 255.0f; uiShouldersShadeColor.y = (rand() % 127) / 255.0f; uiShouldersShadeColor.z = (rand() % 127) / 255.0f;
        uiArmsColor.x = (rand() % 255) / 255.0f; uiArmsColor.y = (rand() % 255) / 255.0f; uiArmsColor.z = (rand() % 255) / 255.0f;
        uiArmsShadeColor.x = (rand() % 127) / 255.0f; uiArmsShadeColor.y = (rand() % 127) / 255.0f; uiArmsShadeColor.z = (rand() % 127) / 255.0f;
        uiOverallsBottomColor.x = (rand() % 255) / 255.0f; uiOverallsBottomColor.y = (rand() % 255) / 255.0f; uiOverallsBottomColor.z = (rand() % 255) / 255.0f;
        uiOverallsBottomShadeColor.x = (rand() % 127) / 255.0f; uiOverallsBottomShadeColor.y = (rand() % 127) / 255.0f; uiOverallsBottomShadeColor.z = (rand() % 127) / 255.0f;
        uiLegTopColor.x = (rand() % 255) / 255.0f; uiLegTopColor.y = (rand() % 255) / 255.0f; uiLegTopColor.z = (rand() % 255) / 255.0f;
        uiLegTopShadeColor.x = (rand() % 127) / 255.0f; uiLegTopShadeColor.y = (rand() % 127) / 255.0f; uiLegTopShadeColor.z = (rand() % 127) / 255.0f;
        uiLegBottomColor.x = (rand() % 255) / 255.0f; uiLegBottomColor.y = (rand() % 255) / 255.0f; uiLegBottomColor.z = (rand() % 255) / 255.0f;
        uiLegBottomShadeColor.x = (rand() % 127) / 255.0f; uiLegBottomShadeColor.y = (rand() % 127) / 255.0f; uiLegBottomShadeColor.z = (rand() % 127) / 255.0f;
        UpdatePaletteFromEditor();
    } imgui_bundled_tooltip("Randomizes all color values; WARNING: This will overwrite your active color code.");

    // Editor Tabs

    // Visual CC Editor
    ImGui::Checkbox("Auto 1/2", &auto_onehalf);
    imgui_bundled_tooltip("Automatically updates the shade color to be a darker version of the main color, and vice versa.");
    if (ImGui::BeginTabBar("###dynos_tabbar", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("Editor")) {

            ColorPartBox(current_model.Colors.Hat.c_str(), "Hat, Main", "Hat, Shade", &uiHatColor, &uiHatShadeColor, "1/2###hat_half");
            ColorPartBox(current_model.Colors.Overalls.c_str(), "Overalls, Main", "Overalls, Shade", &uiOverallsColor, &uiOverallsShadeColor, "1/2###overalls_half");
            ColorPartBox(current_model.Colors.Gloves.c_str(), "Gloves, Main", "Gloves, Shade", &uiGlovesColor, &uiGlovesShadeColor, "1/2###gloves_half");
            ColorPartBox(current_model.Colors.Shoes.c_str(), "Shoes, Main", "Shoes, Shade", &uiShoesColor, &uiShoesShadeColor, "1/2###shoes_half");
            ColorPartBox(current_model.Colors.Skin.c_str(), "Skin, Main", "Skin, Shade", &uiSkinColor, &uiSkinShadeColor, "1/2###skin_half");
            ColorPartBox(current_model.Colors.Hair.c_str(), "Hair, Main", "Hair, Shade", &uiHairColor, &uiHairShadeColor, "1/2###hair_half");

            if (current_model.SparkSupport) {
                if (!support_spark) ImGui::BeginDisabled();
                    // SPARKILIZE
                    if (ImGui::SmallButton("v SPARKILIZE v###cc_editor_sparkilize")) {
                        SparkilizeEditor();
                    } ImGui::SameLine(); imgui_bundled_help_marker("Automatically converts a regular CC to a SPARK CC; WARNING: This will overwrite your active color code.");

                    ColorPartBox(current_model.Colors.Shirt.c_str(), "Shirt, Main", "Shirt, Shade", &uiShirtColor, &uiShirtShadeColor, "1/2###shirt_half");
                    ColorPartBox(current_model.Colors.Shoulders.c_str(), "Shoulders, Main", "Shoulders, Shade", &uiShouldersColor, &uiShouldersShadeColor, "1/2###shoulders_half");
                    ColorPartBox(current_model.Colors.Arms.c_str(), "Arms, Main", "Arms, Shade", &uiArmsColor, &uiArmsShadeColor, "1/2###arms_half");
                    ColorPartBox(current_model.Colors.Pelvis.c_str(), "Overalls (Bottom), Main", "Overalls (Bottom), Shade", &uiOverallsBottomColor, &uiOverallsBottomShadeColor, "1/2###overalls_bottom_half");
                    ColorPartBox(current_model.Colors.Thighs.c_str(), "Leg (Top), Main", "Leg (Top), Shade", &uiLegTopColor, &uiLegTopShadeColor, "1/2###leg_top_half");
                    ColorPartBox(current_model.Colors.Calves.c_str(), "Leg (Bottom), Main", "Leg (Bottom), Shade", &uiLegBottomColor, &uiLegBottomShadeColor, "1/2###leg_bottom_half");
                if (!support_spark) ImGui::EndDisabled();
            }
            
            ImGui::EndTabItem();
        }
        // GameShark Editor
        if (ImGui::BeginTabItem("GameShark")) {

            ImGui::InputTextMultiline("###gameshark_box", uiGameShark, IM_ARRAYSIZE(uiGameShark), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25),    ImGuiInputTextFlags_CharsUppercase |
                                                                                                                                                        ImGuiInputTextFlags_AutoSelectAll);

            if (ImGui::Button(ICON_FK_CLIPBOARD " Apply GS Code")) {
                PasteGameShark(std::string(uiGameShark));
                current_color_code.ParseGameShark();
                UpdateEditorFromPalette();
            } ImGui::SameLine(); imgui_bundled_help_marker(
                "Copy/paste a GameShark color code from here!");                                                                                                                 

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}