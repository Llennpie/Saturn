#include "saturn_imgui_dynos.h"

#include "saturn/libs/portable-file-dialogs.h"

#include <algorithm>
#include <string>
#include <cstring>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/libs/imgui/imgui-knobs.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/saturn_textures.h"
#include "saturn_imgui.h"
#include "pc/controller/controller_keyboard.h"
#include "data/dynos.cpp.h"
#include <SDL2/SDL.h>

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/level_update.h"
#include <mario_animation_ids.h>
#include "pc/cheats.h"
}

Array<PackData *> &sDynosPacks = DynOS_Gfx_GetPacks();

using namespace std;

static ImVec4 uiHatColor =              ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiHatShadeColor =         ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiOverallsColor =         ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiOverallsShadeColor =    ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiGlovesColor =           ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiGlovesShadeColor =      ImVec4(127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShoesColor =            ImVec4(114.0f / 255.0f, 28.0f / 255.0f, 14.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShoesShadeColor =       ImVec4(57.0f / 255.0f, 14.0f / 255.0f, 7.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiSkinColor =             ImVec4(254.0f / 255.0f, 193.0f / 255.0f, 121.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiSkinShadeColor =        ImVec4(127.0f / 255.0f, 96.0f / 255.0f, 60.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiHairColor =             ImVec4(115.0f / 255.0f, 6.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiHairShadeColor =        ImVec4(57.0f / 255.0f, 3.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
// SPARK
static ImVec4 uiShirtColor =                    ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShirtShadeColor =               ImVec4(127.0f / 255.0f, 127.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShouldersColor =                ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShouldersShadeColor =           ImVec4(0.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiArmsColor =                     ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiArmsShadeColor =                ImVec4(0.0f / 255.0f, 127.0f / 255.0f, 64.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiOverallsBottomColor =           ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiOverallsBottomShadeColor =      ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiLegTopColor =                   ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiLegTopShadeColor =              ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 64.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiLegBottomColor =                ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiLegBottomShadeColor =           ImVec4(64.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);

static char ui_cc_name[128] = "Sample";
static int current_cc_id = 0;
string cc_name;
static char ui_gameshark[1024 * 16] = "";

int current_eye_state = 0;
int current_eye_id = 0;
string eye_name;
int current_mouth_id = 0;
string mouth_name;

string ui_mfolder_name;
bool one_pack_selectable;
bool any_packs_selected;
int windowListSize = 325;
int cc_model_id;
int current_cc_to_model_index;
string ui_model_gameshark;

bool using_custom_eyes;

static char searchTerm[128];
static int current_mcc_id = 0;

float this_face_angle;

/*
Sets Mario's global colors from the CC editor color values.
*/
void apply_cc_from_editor() {
    defaultColorHat[0] = (int)(uiHatColor.x * 255);
    defaultColorHat[2] = (int)(uiHatColor.y * 255);
    defaultColorHat[4] = (int)(uiHatColor.z * 255);
    defaultColorHat[1] = (int)(uiHatShadeColor.x * 255);
    defaultColorHat[3] = (int)(uiHatShadeColor.y * 255);
    defaultColorHat[5] = (int)(uiHatShadeColor.z * 255);
    defaultColorOveralls[0] = (int)(uiOverallsColor.x * 255);
    defaultColorOveralls[2] = (int)(uiOverallsColor.y * 255);
    defaultColorOveralls[4] = (int)(uiOverallsColor.z * 255);
    defaultColorOveralls[1] = (int)(uiOverallsShadeColor.x * 255);
    defaultColorOveralls[3] = (int)(uiOverallsShadeColor.y * 255);
    defaultColorOveralls[5] = (int)(uiOverallsShadeColor.z * 255);
    defaultColorGloves[0] = (int)(uiGlovesColor.x * 255);
    defaultColorGloves[2] = (int)(uiGlovesColor.y * 255);
    defaultColorGloves[4] = (int)(uiGlovesColor.z * 255);
    defaultColorGloves[1] = (int)(uiGlovesShadeColor.x * 255);
    defaultColorGloves[3] = (int)(uiGlovesShadeColor.y * 255);
    defaultColorGloves[5] = (int)(uiGlovesShadeColor.z * 255);
    defaultColorShoes[0] = (int)(uiShoesColor.x * 255);
    defaultColorShoes[2] = (int)(uiShoesColor.y * 255);
    defaultColorShoes[4] = (int)(uiShoesColor.z * 255);
    defaultColorShoes[1] = (int)(uiShoesShadeColor.x * 255);
    defaultColorShoes[3] = (int)(uiShoesShadeColor.y * 255);
    defaultColorShoes[5] = (int)(uiShoesShadeColor.z * 255);
    defaultColorSkin[0] = (int)(uiSkinColor.x * 255);
    defaultColorSkin[2] = (int)(uiSkinColor.y * 255);
    defaultColorSkin[4] = (int)(uiSkinColor.z * 255);
    defaultColorSkin[1] = (int)(uiSkinShadeColor.x * 255);
    defaultColorSkin[3] = (int)(uiSkinShadeColor.y * 255);
    defaultColorSkin[5] = (int)(uiSkinShadeColor.z * 255);
    defaultColorHair[0] = (int)(uiHairColor.x * 255);
    defaultColorHair[2] = (int)(uiHairColor.y * 255);
    defaultColorHair[4] = (int)(uiHairColor.z * 255);
    defaultColorHair[1] = (int)(uiHairShadeColor.x * 255);
    defaultColorHair[3] = (int)(uiHairShadeColor.y * 255);
    defaultColorHair[5] = (int)(uiHairShadeColor.z * 255);
    if (cc_spark_support) {
        sparkColorShirt[0] = (int)(uiShirtColor.x * 255);
        sparkColorShirt[2] = (int)(uiShirtColor.y * 255);
        sparkColorShirt[4] = (int)(uiShirtColor.z * 255);
        sparkColorShirt[1] = (int)(uiShirtShadeColor.x * 255);
        sparkColorShirt[3] = (int)(uiShirtShadeColor.y * 255);
        sparkColorShirt[5] = (int)(uiShirtShadeColor.z * 255);
        sparkColorShoulders[0] = (int)(uiShouldersColor.x * 255);
        sparkColorShoulders[2] = (int)(uiShouldersColor.y * 255);
        sparkColorShoulders[4] = (int)(uiShouldersColor.z * 255);
        sparkColorShoulders[1] = (int)(uiShouldersShadeColor.x * 255);
        sparkColorShoulders[3] = (int)(uiShouldersShadeColor.y * 255);
        sparkColorShoulders[5] = (int)(uiShouldersShadeColor.z * 255);
        sparkColorArms[0] = (int)(uiArmsColor.x * 255);
        sparkColorArms[2] = (int)(uiArmsColor.y * 255);
        sparkColorArms[4] = (int)(uiArmsColor.z * 255);
        sparkColorArms[1] = (int)(uiArmsShadeColor.x * 255);
        sparkColorArms[3] = (int)(uiArmsShadeColor.y * 255);
        sparkColorArms[5] = (int)(uiArmsShadeColor.z * 255);
        sparkColorOverallsBottom[0] = (int)(uiOverallsBottomColor.x * 255);
        sparkColorOverallsBottom[2] = (int)(uiOverallsBottomColor.y * 255);
        sparkColorOverallsBottom[4] = (int)(uiOverallsBottomColor.z * 255);
        sparkColorOverallsBottom[1] = (int)(uiOverallsBottomShadeColor.x * 255);
        sparkColorOverallsBottom[3] = (int)(uiOverallsBottomShadeColor.y * 255);
        sparkColorOverallsBottom[5] = (int)(uiOverallsBottomShadeColor.z * 255);
        sparkColorLegTop[0] = (int)(uiLegTopColor.x * 255);
        sparkColorLegTop[2] = (int)(uiLegTopColor.y * 255);
        sparkColorLegTop[4] = (int)(uiLegTopColor.z * 255);
        sparkColorLegTop[1] = (int)(uiLegTopShadeColor.x * 255);
        sparkColorLegTop[3] = (int)(uiLegTopShadeColor.y * 255);
        sparkColorLegTop[5] = (int)(uiLegTopShadeColor.z * 255);
        sparkColorLegBottom[0] = (int)(uiLegBottomColor.x * 255);
        sparkColorLegBottom[2] = (int)(uiLegBottomColor.y * 255);
        sparkColorLegBottom[4] = (int)(uiLegBottomColor.z * 255);
        sparkColorLegBottom[1] = (int)(uiLegBottomShadeColor.x * 255);
        sparkColorLegBottom[3] = (int)(uiLegBottomShadeColor.y * 255);
        sparkColorLegBottom[5] = (int)(uiLegBottomShadeColor.z * 255);
    }

    // Also set the editor GameShark code
    strcpy(ui_gameshark, global_gs_code().c_str());
} 

/*
Sets the CC editor color values from Mario's global colors.
*/
void set_editor_from_global_cc(std::string cc_name) {
    uiHatColor = ImVec4(float(defaultColorHat[0]) / 255.0f, float(defaultColorHat[2]) / 255.0f, float(defaultColorHat[4]) / 255.0f, 255.0f / 255.0f);
    uiHatShadeColor = ImVec4(float(defaultColorHat[1]) / 255.0f, float(defaultColorHat[3]) / 255.0f, float(defaultColorHat[5]) / 255.0f, 255.0f / 255.0f);
    uiOverallsColor = ImVec4(float(defaultColorOveralls[0]) / 255.0f, float(defaultColorOveralls[2]) / 255.0f, float(defaultColorOveralls[4]) / 255.0f, 255.0f / 255.0f);
    uiOverallsShadeColor = ImVec4(float(defaultColorOveralls[1]) / 255.0f, float(defaultColorOveralls[3]) / 255.0f, float(defaultColorOveralls[5]) / 255.0f, 255.0f / 255.0f);
    uiGlovesColor = ImVec4(float(defaultColorGloves[0]) / 255.0f, float(defaultColorGloves[2]) / 255.0f, float(defaultColorGloves[4]) / 255.0f, 255.0f / 255.0f);
    uiGlovesShadeColor = ImVec4(float(defaultColorGloves[1]) / 255.0f, float(defaultColorGloves[3]) / 255.0f, float(defaultColorGloves[5]) / 255.0f, 255.0f / 255.0f);
    uiShoesColor = ImVec4(float(defaultColorShoes[0]) / 255.0f, float(defaultColorShoes[2]) / 255.0f, float(defaultColorShoes[4]) / 255.0f, 255.0f / 255.0f);
    uiShoesShadeColor = ImVec4(float(defaultColorShoes[1]) / 255.0f, float(defaultColorShoes[3]) / 255.0f, float(defaultColorShoes[5]) / 255.0f, 255.0f / 255.0f);
    uiSkinColor = ImVec4(float(defaultColorSkin[0]) / 255.0f, float(defaultColorSkin[2]) / 255.0f, float(defaultColorSkin[4]) / 255.0f, 255.0f / 255.0f);
    uiSkinShadeColor = ImVec4(float(defaultColorSkin[1]) / 255.0f, float(defaultColorSkin[3]) / 255.0f, float(defaultColorSkin[5]) / 255.0f, 255.0f / 255.0f);
    uiHairColor = ImVec4(float(defaultColorHair[0]) / 255.0f, float(defaultColorHair[2]) / 255.0f, float(defaultColorHair[4]) / 255.0f, 255.0f / 255.0f);
    uiHairShadeColor = ImVec4(float(defaultColorHair[1]) / 255.0f, float(defaultColorHair[3]) / 255.0f, float(defaultColorHair[5]) / 255.0f, 255.0f / 255.0f);

    if (cc_spark_support) {
        uiShirtColor = ImVec4(float(sparkColorShirt[0]) / 255.0f, float(sparkColorShirt[2]) / 255.0f, float(sparkColorShirt[4]) / 255.0f, 255.0f / 255.0f);
        uiShirtShadeColor = ImVec4(float(sparkColorShirt[1]) / 255.0f, float(sparkColorShirt[3]) / 255.0f, float(sparkColorShirt[5]) / 255.0f, 255.0f / 255.0f);
        uiShouldersColor = ImVec4(float(sparkColorShoulders[0]) / 255.0f, float(sparkColorShoulders[2]) / 255.0f, float(sparkColorShoulders[4]) / 255.0f, 255.0f / 255.0f);
        uiShouldersShadeColor = ImVec4(float(sparkColorShoulders[1]) / 255.0f, float(sparkColorShoulders[3]) / 255.0f, float(sparkColorShoulders[5]) / 255.0f, 255.0f / 255.0f);
        uiArmsColor = ImVec4(float(sparkColorArms[0]) / 255.0f, float(sparkColorArms[2]) / 255.0f, float(sparkColorArms[4]) / 255.0f, 255.0f / 255.0f);
        uiArmsShadeColor = ImVec4(float(sparkColorArms[1]) / 255.0f, float(sparkColorArms[3]) / 255.0f, float(sparkColorArms[5]) / 255.0f, 255.0f / 255.0f);
        uiOverallsBottomColor = ImVec4(float(sparkColorOverallsBottom[0]) / 255.0f, float(sparkColorOverallsBottom[2]) / 255.0f, float(sparkColorOverallsBottom[4]) / 255.0f, 255.0f / 255.0f);
        uiOverallsBottomShadeColor = ImVec4(float(sparkColorOverallsBottom[1]) / 255.0f, float(sparkColorOverallsBottom[3]) / 255.0f, float(sparkColorOverallsBottom[5]) / 255.0f, 255.0f / 255.0f);
        uiLegTopColor = ImVec4(float(sparkColorLegTop[0]) / 255.0f, float(sparkColorLegTop[2]) / 255.0f, float(sparkColorLegTop[4]) / 255.0f, 255.0f / 255.0f);
        uiLegTopShadeColor = ImVec4(float(sparkColorLegTop[1]) / 255.0f, float(sparkColorLegTop[3]) / 255.0f, float(sparkColorLegTop[5]) / 255.0f, 255.0f / 255.0f);
        uiLegBottomColor = ImVec4(float(sparkColorLegBottom[0]) / 255.0f, float(sparkColorLegBottom[2]) / 255.0f, float(sparkColorLegBottom[4]) / 255.0f, 255.0f / 255.0f);
        uiLegBottomShadeColor = ImVec4(float(sparkColorLegBottom[1]) / 255.0f, float(sparkColorLegBottom[3]) / 255.0f, float(sparkColorLegBottom[5]) / 255.0f, 255.0f / 255.0f);
    }

    // Also set the editor GameShark code
    strcpy(ui_gameshark, global_gs_code().c_str());

    // We never want to use the name "Mario" when saving/loading a CC, as it will cause file issues
    // Instead, we'll change the UI name to "Sample"
    if (cc_name == "Mario") {
        strcpy(ui_cc_name, "Sample");
    } else {
        strcpy(ui_cc_name, cc_name.c_str());
    }
}

// UI

void handle_cc_box(const char* name, const char* mainName, const char* shadeName, ImVec4* colorValue, ImVec4* shadeColorValue, string id) {
    ImGui::ColorEdit4(mainName, (float*)colorValue, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoLabel);
    if (ImGui::IsItemActivated()) accept_text_input = false;
    if (ImGui::IsItemDeactivated()) accept_text_input = true;
    ImGui::SameLine(); ImGui::Text(name);

    ImGui::ColorEdit4(shadeName, (float*)shadeColorValue, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoLabel);
    if (ImGui::IsItemActivated()) accept_text_input = false;
    if (ImGui::IsItemDeactivated()) accept_text_input = true;

    ImGui::SameLine();
    if (ImGui::Button(id.c_str())) {
        shadeColorValue->x = colorValue->x / 2.0f;
        shadeColorValue->y = colorValue->y / 2.0f;
        shadeColorValue->z = colorValue->z / 2.0f;
    }
}

int numColorCodes;

void sdynos_imgui_init() {
    saturn_load_cc_directory();
    //saturn_load_eye_directory();
    saturn_load_eye_folder("");
    strcpy(ui_gameshark, global_gs_code().c_str());
}

void sdynos_imgui_menu() {
    if (ImGui::BeginMenu("Edit Avatar###menu_edit_avatar")) {

        ImGui::Text("Color Codes");
        ImGui::SameLine(); imgui_bundled_help_marker(
            "These are GameShark color codes, which overwrite Mario's lights. Place GS files in dynos/colorcodes.");

        ImGui::BeginChild("###menu_cc_selector", ImVec2(208, 100), true);
        for (int n = 0; n < cc_array.size(); n++) {
            const bool is_selected = (current_cc_id == n);
            cc_name = cc_array[n].substr(0, cc_array[n].size() - 3);

            if (ImGui::Selectable(cc_name.c_str(), is_selected)) {
                current_cc_id = n;
                load_cc_file((char*)cc_array[current_cc_id].c_str());
                set_editor_from_global_cc(cc_array[current_cc_id].substr(0, cc_array[current_cc_id].size() - 3));
            }

            if (ImGui::BeginPopupContextItem()) {
                if (cc_name != "Mario") {
                    ImGui::Text("%s.gs", cc_name.c_str());
                    imgui_bundled_tooltip(("/dynos/colorcodes/" + cc_name + ".gs").c_str());
                    if (ImGui::SmallButton("Delete File")) {
                        delete_cc_file(cc_name);
                        current_cc_id = -1;
                        ImGui::CloseCurrentPopup();
                    } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                    ImGui::Separator();
                }
                ImGui::TextDisabled("%i color code(s)", cc_array.size());
                if (ImGui::Button("Refresh")) {
                    saturn_load_cc_directory();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::EndChild();
        if (ImGui::Button("Add CC File...###add_v_cc")) {
            auto selection3 = pfd::open_file("Select a file", ".",
                        { "Color Code Files", "*.gs *.txt",
                            "All Files", "*" },
                        pfd::opt::multiselect).result();

            // Do something with selection
            for (auto const &filename3 : selection3) {
                saturn_copy_file(filename3, "dynos/colorcodes/");
                saturn_load_cc_directory();
            }
        }

        ImGui::Text("Model Packs");
        ImGui::SameLine(); imgui_bundled_help_marker(
            "DynOS v1.1 by PeachyPeach\n\nThese are DynOS model packs, used for live model loading.\nPlace packs in /dynos/packs.");

        if (sDynosPacks.Count() >= 20) {
            ImGui::InputTextWithHint("###model_search_text", "Search models...", searchTerm, IM_ARRAYSIZE(searchTerm), ImGuiInputTextFlags_AutoSelectAll);
            if (ImGui::IsItemActivated()) accept_text_input = false;
            if (ImGui::IsItemDeactivated()) accept_text_input = true;
        } else {
            // If our model list is reloaded, and we now have less than 20 packs, this can cause filter issues if not reset to nothing
            if (searchTerm != "") strcpy(searchTerm, "");
        }
        string searchLower = searchTerm;
        std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(),
            [](unsigned char c){ return std::tolower(c); });

        if (sDynosPacks.Count() <= 0) {
            ImGui::TextDisabled("No model packs found.\nPlace model folders in\n/dynos/packs/.");
        } else {
            ImGui::BeginChild("###menu_model_selector", ImVec2(208, 125), true);
            for (int i = 0; i < sDynosPacks.Count(); i++) {
                u64 _DirSep1 = sDynosPacks[i]->mPath.find_last_of('\\');
                u64 _DirSep2 = sDynosPacks[i]->mPath.find_last_of('/');
                if (_DirSep1++ == SysPath::npos) _DirSep1 = 0;
                if (_DirSep2++ == SysPath::npos) _DirSep2 = 0;

                std::string label = sDynosPacks[i]->mPath.substr(MAX(_DirSep1, _DirSep2));
                bool is_selected = DynOS_Opt_GetValue(String("dynos_pack_%d", i));

                // If we're searching, only include models with the search keyword in the name
                // Also convert to lowercase
                if (searchLower != "") {
                    string labelLower = label;
                    std::transform(labelLower.begin(), labelLower.end(), labelLower.begin(),
                        [](unsigned char c1){ return std::tolower(c1); });

                    if (labelLower.find(searchLower) == string::npos) {
                        continue;
                    }
                }

                if (ImGui::Selectable(label.c_str(), &is_selected)) {
                    // Deselect other packs, but LSHIFT allows additive
                    for (int j = 0; j < sDynosPacks.Count(); j++) {
                        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT] == false)
                            DynOS_Opt_SetValue(String("dynos_pack_%d", j), false);
                    }
                            
                    DynOS_Opt_SetValue(String("dynos_pack_%d", i), is_selected);
                    ui_mfolder_name = label;

                    gfx_precache_textures();

                    // Fetch model data
                    saturn_load_model_data(label);
                    for (int i = 0; i < 8; i++) {
                        current_exp_index[i] = 0;
                    }
                    
                    if (is_selected)
                        std::cout << "Loaded " << current_model_data.name << " by " << current_model_data.author << std::endl;

                    current_mcc_id = -1;

                    if (is_default_cc(ui_gameshark)) {
                        get_ccs_from_model(sDynosPacks[i]->mPath);
                        if (model_cc_array.size() > 0) {
                            set_cc_from_model(sDynosPacks[i]->mPath + "/colorcodes/" + model_cc_array[0].substr(0, model_cc_array[0].size()));
                            set_editor_from_global_cc("Sample");
                            ui_model_gameshark = ui_gameshark;
                        }
                    }

                    one_pack_selectable = false;
                    for (int k = 0; k < sDynosPacks.Count(); k++) {
                        if (DynOS_Opt_GetValue(String("dynos_pack_%d", k)))
                            one_pack_selectable = true;
                    }

                    any_packs_selected = one_pack_selectable;
                    if (!any_packs_selected) {

                        if (ui_model_gameshark == ui_gameshark) {
                            ui_model_gameshark = "";
                            current_cc_id = 0;
                            load_cc_file((char*)cc_array[current_cc_id].c_str());
                            set_editor_from_global_cc("Sample");
                        }

                        ModelData blank;
                        current_model_data = blank;
                        using_model_eyes = false;
                        cc_model_support = true;
                        cc_spark_support = false;
                    }
                }
                if (ImGui::BeginPopupContextItem()) {
                    ImGui::Text("%s/", label.c_str());
                    imgui_bundled_tooltip(("/dynos/packs/" + label).c_str());

                    // Model CCs
                    get_ccs_from_model(sDynosPacks[i]->mPath);
                    if (model_cc_array.size() > 0) {
                        ImGui::BeginChild("###menu_model_ccs", ImVec2(125, 75), true);
                        for (int n = 0; n < model_cc_array.size(); n++) {
                            const bool is_selected = (current_mcc_id == n);
                            cc_name = model_cc_array[n].substr(0, model_cc_array[n].size() - 3);

                            if (ImGui::Selectable(cc_name.c_str(), is_selected)) {
                                current_mcc_id = n;
                                current_cc_id = -1;
                                set_cc_from_model(sDynosPacks[i]->mPath + "/colorcodes/" + cc_name + ".gs");
                                set_editor_from_global_cc("Sample");
                            }

                            if (ImGui::BeginPopupContextItem()) {
                                ImGui::Text("%s.gs", cc_name.c_str());
                                imgui_bundled_tooltip(("/dynos/packs/" + label + "/colorcodes/" + cc_name + ".gs").c_str());
                                if (cc_name != "../default") {
                                    if (ImGui::SmallButton("Delete File")) {
                                        delete_model_cc_file(cc_name, label);
                                        ImGui::CloseCurrentPopup();
                                    } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                                }
                                ImGui::Separator();
                                ImGui::TextDisabled("%i color code(s)", model_cc_array.size());
                                if (ImGui::Button("Refresh")) {
                                    get_ccs_from_model(sDynosPacks[i]->mPath);
                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::EndPopup();
                            }
                        }
                        ImGui::EndChild();
                        if (ImGui::Button("Add CC File...###add_m_cc")) {
                            auto selection4 = pfd::open_file("Select a file", ".",
                                        { "Color Code Files", "*.gs *.txt",
                                            "All Files", "*" },
                                        pfd::opt::multiselect).result();

                            // Do something with selection
                            for (auto const &filename4 : selection4) {
                                saturn_copy_file(filename4, "dynos/packs/" + label + "/colorcodes/");
                                get_ccs_from_model(sDynosPacks[i]->mPath);
                            }
                        }
                    }
                    ImGui::Separator();
                    ImGui::TextDisabled("%i model pack(s)", sDynosPacks.Count());
                    if (ImGui::Button("Refresh Packs###refresh_dynos_packs")) {
                        sDynosPacks.Clear();
                        DynOS_Gfx_Init();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine(); imgui_bundled_help_marker("WARNING: Experimental - this will probably lag the game.");
                    ImGui::EndPopup();
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Color Code Editor###menu_cc_editor"))
        currentMenu = 2;

    if (ImGui::BeginMenu("Misc.###menu_misc")) {

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("Misc.###misc_child", ImVec2(225, 150), true, ImGuiWindowFlags_None);
        if (ImGui::BeginTabBar("###misc_tabbar", ImGuiTabBarFlags_None)) {

            if (ImGui::BeginTabItem("Switches###switches_scale")) {
                const char* eyes[] = { "Blinking", "Open", "Half", "Closed", "Left", "Right", "Up", "Down", "Dead" };
                if (ImGui::Combo("Eyes###eye_state", &current_eye_state, eyes, IM_ARRAYSIZE(eyes))) {
                    if (current_eye_state < 4) using_custom_eyes = false;
                    else using_custom_eyes = true;
                }

                const char* hands[] = { "Fists", "Open", "Peace", "With Cap", "With Wing Cap", "Right Open" };
                ImGui::Combo("Hand###hand_state", &scrollHandState, hands, IM_ARRAYSIZE(hands));
                const char* caps[] = { "Cap On", "Cap Off", "Wing Cap" }; // unused "wing cap off" not included
                ImGui::Combo("Cap###cap_state", &scrollCapState, caps, IM_ARRAYSIZE(caps));
                const char* powerups[] = { "Default", "Metal", "Vanish", "Metal & Vanish" };
                ImGui::Combo("Powerup###powerup_state", &saturnModelState, powerups, IM_ARRAYSIZE(powerups));

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shading###tab_shading")) {
                ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("X###wdir_x", &world_light_dir1, -2.f, 2.f);
                ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("Y###wdir_y", &world_light_dir2, -2.f, 2.f);
                ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("Z###wdir_z", &world_light_dir3, -2.f, 2.f);
                ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("Tex###wdir_tex", &world_light_dir4, 1.f, 4.f);

                if (world_light_dir1 != 0.f || world_light_dir2 != 0.f || world_light_dir3 != 0.f || world_light_dir4 != 1.f) {
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine();
                    if (ImGui::Button("Reset###reset_wshading")) {
                        world_light_dir1 = 0.f;
                        world_light_dir2 = 0.f;
                        world_light_dir3 = 0.f;
                        world_light_dir4 = 1.f;
                    }
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Scale###tab_scale")) {
                if (linkMarioScale) {
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("Size###mscale_all", &marioScaleSizeX, 0.f, 2.f);
                } else {
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("X###mscale_x", &marioScaleSizeX, -2.f, 2.f);
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("Y###mscale_y", &marioScaleSizeY, -2.f, 2.f);
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("Z###mscale_z", &marioScaleSizeZ, -2.f, 2.f);
                }
                ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::Checkbox("Link###link_mario_scale", &linkMarioScale);
                if (marioScaleSizeX != 1.f || marioScaleSizeY != 1.f || marioScaleSizeZ != 1.f) {
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine();
                    if (ImGui::Button("Reset###reset_mscale")) {
                        marioScaleSizeX = 1.f;
                        marioScaleSizeY = 1.f;
                        marioScaleSizeZ = 1.f;
                    }
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();

        if (ImGui::BeginTable("misc_table", 2)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Checkbox("Head Rotations", &enable_head_rotations);
            imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
            ImGui::Checkbox("Dust Particles", &enable_dust_particles);
            imgui_bundled_tooltip("Displays dust particles when Mario moves.");
            ImGui::Checkbox("Torso Rotations", &enable_torso_rotation);
            imgui_bundled_tooltip("Tilts Mario's torso when he moves; Disable for a \"beta running\" effect.");

            ImGui::Dummy(ImVec2(0, 5));

            if (!any_packs_selected) {
                ImGui::Checkbox("M Cap Emblem", &show_vmario_emblem);
                imgui_bundled_tooltip("Enables the signature \"M\" logo on Mario's cap.");
            }

            ImGui::TableSetColumnIndex(1);
            if (gMarioState) {
                if (ImGuiKnobs::Knob("Angle", &this_face_angle, -180.f, 180.f, 0.f, "%.0f deg", ImGuiKnobVariant_Dot, 0.f, ImGuiKnobFlags_DragHorizontal)) {
                    gMarioState->faceAngle[1] = (s16)(this_face_angle * 182.04f);
                } else {
                    this_face_angle = (float)gMarioState->faceAngle[1] / 182.04;
                }
            }

            ImGui::Checkbox("Spin###spin_angle", &is_spinning);
            if (is_spinning) {
                ImGui::SliderFloat("Speed###spin,speed", &spin_mult, -2.f, 2.f, "%.1f");
            }

            if (this_face_angle > 180) this_face_angle = -180;
            if (this_face_angle < -180) this_face_angle = 180;

            ImGui::EndTable();
        }

        /*ImGui::Checkbox("Head Rotations", &enable_head_rotations);
        imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
        ImGui::Checkbox("Dust Particles", &enable_dust_particles);
        imgui_bundled_tooltip("Displays dust particles when Mario moves.");
        ImGui::Checkbox("Torso Rotations", &enable_torso_rotation);
        imgui_bundled_tooltip("Tilts Mario's torso when he moves; Disable for a \"beta running\" effect.");
        */

        ImGui::PopStyleVar();
        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (current_model_data.name != "") {

        // Metadata

        string metaDataText = "v" + current_model_data.version;
        if (current_model_data.description != "")
            metaDataText = "v" + current_model_data.version + "\n" + current_model_data.description;

        ImGui::Text(current_model_data.name.c_str());
        ImGui::SameLine(); imgui_bundled_help_marker(
            metaDataText.c_str());
        ImGui::Text(("By " + current_model_data.author).c_str());

        ImGui::Separator();
    }

    // Custom Eyes - Overwrites the left eye switch with an external PNG image choice
    if ((current_model_data.name != "" && current_model_data.eye_support == true) || current_model_data.name == "") {
        if (ImGui::Checkbox("Custom Eyes", &using_custom_eyes)) {
            if (current_eye_state < 4) current_eye_state = 4;
            else current_eye_state = 0;
        } 
        if (!any_packs_selected) {
            ImGui::SameLine(); imgui_bundled_help_marker(
                "Place custom eye PNG textures in dynos/eyes.");
        }
    } else {
        using_custom_eyes = false;
    }

    if (using_custom_eyes) {
        //scrollEyeState = 4;

        // Eyes (No Model)

        if (!using_model_eyes) {
            if (eye_array.size() > 0 && using_custom_eyes) {
                ImGui::BeginChild("###menu_custom_eye_selector", ImVec2(-FLT_MIN, 150), true);
                for (int n = 0; n < eye_array.size(); n++) {
                    const bool is_eye_selected = (current_eye_index == n);
                    string entry_name = eye_array[n];

                    if (ImGui::Selectable(entry_name.c_str(), is_eye_selected)) {
                        gfx_precache_textures();
                        current_eye_index = n;
                        saturn_eye_selectable(entry_name, n);
                    }

                    // Refresh
                    if (ImGui::BeginPopupContextItem()) {
                        if (entry_name.find(".png") != string::npos) {
                            ImGui::Text("%s", entry_name.c_str());
                            imgui_bundled_tooltip((current_eye_dir_path + entry_name).c_str());
                            if (eye_array.size() > 1) {
                                if (ImGui::SmallButton("Delete File")) {
                                    saturn_delete_file(current_eye_dir_path + entry_name);
                                    saturn_load_eye_folder(current_eye_dir_path);
                                    ImGui::CloseCurrentPopup();
                                } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                            }
                            ImGui::Separator();
                        }
                        ImGui::TextDisabled("%i eye texture(s)", eye_array.size());
                        if (ImGui::Button("Refresh###refresh_v_eyes")) {
                            saturn_load_eye_folder(current_eye_dir_path);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndChild();
                if (ImGui::Button("Add Eye...###add_v_eye")) {
                    auto selection = pfd::open_file("Select a file", ".",
                                { "PNG Textures", "*.png ",
                                  "All Files", "*" },
                                pfd::opt::multiselect).result();

                    // Do something with selection
                    for (auto const &filename : selection) {
                        saturn_copy_file(filename, current_eye_dir_path);
                        saturn_load_eye_folder(current_eye_dir_path);
                    }
                }
            } else {
                ImGui::Text("No eye textures found.\nPlace custom eye PNG textures\nin dynos/eyes/.");
            }
            ImGui::Separator();
        }
    } else {
        if (current_model_data.eye_support && current_model_data.name != "") {
            ImGui::Separator();
        }
    }

    scrollEyeState = current_eye_state;
    // Expressions (including vanilla eyes)
    is_replacing_exp = (using_custom_eyes && scrollEyeState > 3 || current_model_data.expressions.size() > 0);
    // Vanilla and model eyes
    is_replacing_eyes = (using_custom_eyes && scrollEyeState > 3 || current_model_data.expressions.size() > 0 && using_custom_eyes);

    if (current_model_data.expressions.size() > 0) {

        // Model Expressions

        for (int i; i < current_model_data.expressions.size(); i++) {
            Expression expression = current_model_data.expressions[i];

            if (expression.name == "eye" || expression.name == "eyes") {
                if (scrollEyeState < 4 || !using_custom_eyes) {
                    continue;
                }

                // Eyes
                ImGui::BeginChild(("###menu_custom_%s", expression.name.c_str()), ImVec2(-FLT_MIN, 75), true);
                for (int n = 0; n < expression.textures.size(); n++) {
                    string entry_name = expression.textures[n];
                    bool is_selected = (current_exp_index[i] == n);

                    if (ImGui::Selectable(entry_name.c_str(), is_selected)) {
                        gfx_precache_textures();
                        current_exp_index[i] = n;
                        saturn_set_model_texture(i, expression.path + expression.textures[n]);
                    }

                    // Refresh
                    if (ImGui::BeginPopupContextItem()) {
                        ImGui::Text("%s", entry_name.c_str());
                        imgui_bundled_tooltip(("dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/" + entry_name).c_str());
                        if (expression.textures.size() > 1) {
                            if (ImGui::SmallButton("Delete File")) {
                                saturn_delete_file("dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/" + entry_name);
                                saturn_load_model_data(current_folder_name);
                                for (int i = 0; i < 8; i++) {
                                    current_exp_index[i] = 0;
                                }
                                ImGui::CloseCurrentPopup();
                            } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                        }
                        ImGui::Separator();

                        ImGui::TextDisabled("%i eye texture(s)", expression.textures.size());
                        if (ImGui::Button("Refresh###refresh_m_eyes")) {
                            saturn_load_model_data(current_folder_name);
                            for (int i = 0; i < 8; i++) {
                                current_exp_index[i] = 0;
                            }
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndChild();
                if (ImGui::Button("Add Eye...###add_m_eye")) {
                    auto selection1 = pfd::open_file("Select a file", ".",
                                { "PNG Textures", "*.png ",
                                  "All Files", "*" },
                                pfd::opt::multiselect).result();

                    // Do something with selection
                    for (auto const &filename1 : selection1) {
                        saturn_copy_file(filename1, "dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/");
                        saturn_load_model_data(current_folder_name);
                    }
                }

                if (current_model_data.eye_support)
                    ImGui::Separator();
            }
        }
        for (int i; i < current_model_data.expressions.size(); i++) {
            Expression expression = current_model_data.expressions[i];

            if (expression.name != "eye" && expression.name != "eyes") {

                // Everything but eyes
                if (expression.textures.size() > 0) {
                    string label_name = expression.name + "###menu_" + expression.name;

                    // If we just have "none" and "default" for an expression...
                    // Use a checkbox instead for nicer UI
                    if (expression.textures.size() == 2 && expression.textures[0].find("default") != string::npos && expression.textures[1].find("none") != string::npos ||
                        expression.textures.size() == 2 && expression.textures[1].find("default") != string::npos && expression.textures[0].find("none") != string::npos) {

                        int cselect_index = (expression.textures[0].find("none") != string::npos) ? 1 : 0;
                        int cdeselect_index = (expression.textures[0].find("none") != string::npos) ? 0 : 1;
                        bool is_selected = (current_exp_index[i] == cselect_index);

                        if (ImGui::Checkbox(label_name.c_str(), &is_selected)) {
                            gfx_precache_textures();
                            if (is_selected) {
                                current_exp_index[i] = cselect_index;
                                saturn_set_model_texture(i, expression.path + expression.textures[cselect_index]);
                            } else {
                                current_exp_index[i] = cdeselect_index;
                                saturn_set_model_texture(i, expression.path + expression.textures[cdeselect_index]);
                            }
                        }
                    } else {
                        string label_name = "###menu_" + expression.name;
                        const char* preview_label_name = (expression.name + " >> " + expression.textures[current_exp_index[i]]).c_str();
                        if (ImGui::BeginCombo(label_name.c_str(), preview_label_name, ImGuiComboFlags_None)) {
                            for (int n = 0; n < expression.textures.size(); n++) {
                                bool is_selected = (current_exp_index[i] == n);
                                string entry_name = expression.textures[n];

                                if (ImGui::Selectable(entry_name.c_str(), is_selected)) {
                                    gfx_precache_textures();
                                    current_exp_index[i] = n;
                                    saturn_set_model_texture(i, expression.path + expression.textures[n]);
                                }

                                // Add / Delete
                                if (ImGui::BeginPopupContextItem()) {
                                    ImGui::Text("%s", entry_name.c_str());
                                    imgui_bundled_tooltip(("dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/" + entry_name).c_str());
                                    if (expression.textures.size() > 1) {
                                        if (ImGui::SmallButton("Delete File")) {
                                            saturn_delete_file("dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/" + entry_name);
                                            saturn_load_model_data(current_folder_name);
                                            for (int i = 0; i < 8; i++) {
                                                current_exp_index[i] = 0;
                                            }
                                            ImGui::CloseCurrentPopup();
                                        } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                                    }
                                    ImGui::EndPopup();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }

                    // Refresh
                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::Button("Add Expression...###add_m_exp")) {
                            auto selection1 = pfd::open_file("Select a file", ".",
                                        { "PNG Textures", "*.png ",
                                        "All Files", "*" },
                                        pfd::opt::multiselect).result();

                            // Do something with selection
                            for (auto const &filename1 : selection1) {
                                saturn_copy_file(filename1, "dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/");
                                saturn_load_model_data(current_folder_name);
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::Separator();
                        ImGui::TextDisabled("%i %s expression(s)", expression.textures.size(), expression.name.c_str());
                        if (ImGui::Button("Refresh###refresh_m_exp")) {
                            saturn_load_model_data(current_folder_name);
                            for (int i = 0; i < 8; i++) {
                                current_exp_index[i] = 0;
                            }
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
            }
        }
    }

    // CC Compatibility - Allows colors to be edited for models that support it
    if ((current_model_data.name != "" && current_model_data.cc_support == true) || current_model_data.name == "") {
        if (current_model_data.expressions.size() > 1 && current_model_data.eye_support) ImGui::Separator();
        ImGui::Checkbox("CC Compatibility", &cc_model_support);
        ImGui::SameLine(); imgui_bundled_help_marker(
            "Toggles color code compatibility for model packs that support it.");
    }
    // CometSPARK Support - When a model is present, allows SPARK colors to be edited for models that support it
    if (any_packs_selected) {
        if ((current_model_data.name != "" && current_model_data.spark_support == true) || current_model_data.name == "") {
            ImGui::Checkbox("CometSPARK Support", &cc_spark_support);
            ImGui::SameLine(); imgui_bundled_help_marker(
                "Grants a model extra color values. See the GitHub wiki for setup instructions.");
        }
    }

    ImGui::EndMenu();
}

void sdynos_imgui_update() {
    if (currentMenu != 2) return;

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::Begin("Color Code Editor", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos(ImVec2(10, 30));
    ImGui::SetWindowSize(ImVec2(275, 435));

    if (ImGui::Button("Reset Colors")) {
        current_cc_id = 0;
        load_cc_file((char*)cc_array[current_cc_id].c_str());
        set_editor_from_global_cc("Sample");
    }

    ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::BeginTabBar("###dynos_tabbar", ImGuiTabBarFlags_None)) {

        if (ImGui::BeginTabItem("CC Editor")) {
            handle_cc_box("Hat", "Hat, Main", "Hat, Shade", &uiHatColor, &uiHatShadeColor, "1/2###hat_half");
            handle_cc_box("Overalls", "Overalls, Main", "Overalls, Shade", &uiOverallsColor, &uiOverallsShadeColor, "1/2###overalls_half");
            handle_cc_box("Gloves", "Gloves, Main", "Gloves, Shade", &uiGlovesColor, &uiGlovesShadeColor, "1/2###gloves_half");
            handle_cc_box("Shoes", "Shoes, Main", "Shoes, Shade", &uiShoesColor, &uiShoesShadeColor, "1/2###shoes_half");
            handle_cc_box("Skin", "Skin, Main", "Skin, Shade", &uiSkinColor, &uiSkinShadeColor, "1/2###skin_half");
            handle_cc_box("Hair", "Hair, Main", "Hair, Shade", &uiHairColor, &uiHairShadeColor, "1/2###hair_half");
            if (cc_spark_support) {
                if (ImGui::CollapsingHeader("SPARK###spark_color_edit")) {
                    handle_cc_box("Shirt", "Shirt, Main", "Shirt, Shade", &uiShirtColor, &uiShirtShadeColor, "1/2###shirt_half");
                    handle_cc_box("Shoulders", "Shoulders, Main", "Shoulders, Shade", &uiShouldersColor, &uiShouldersShadeColor, "1/2###shoulders_half");
                    handle_cc_box("Arms", "Arms, Main", "Arms, Shade", &uiArmsColor, &uiArmsShadeColor, "1/2###arms_half");
                    handle_cc_box("Overalls (Bottom)", "Overalls (Bottom), Main", "Overalls (Bottom), Shade", &uiOverallsBottomColor, &uiOverallsBottomShadeColor, "1/2###overalls_bottom_half");
                    handle_cc_box("Leg (Top)", "Leg (Top), Main", "Leg (Top), Shade", &uiLegTopColor, &uiLegTopShadeColor, "1/2###leg_top_half");
                    handle_cc_box("Leg (Bottom)", "Leg (Bottom), Main", "Leg (Bottom), Shade", &uiLegBottomColor, &uiLegBottomShadeColor, "1/2###leg_bottom_half");
                }
            }

            if (!configEditorFastApply) {
                if (ImGui::Button("Apply CC###apply_cc_from_editor")) {
                    apply_cc_from_editor();
                }
            } else {
                apply_cc_from_editor();
            }

            ImGui::Dummy(ImVec2(0, 5));

            ImGui::InputText(".gs", ui_cc_name, IM_ARRAYSIZE(ui_cc_name));
            if (ImGui::IsItemActivated()) accept_text_input = false;
            if (ImGui::IsItemDeactivated()) accept_text_input = true;

            ImGui::Text("Save to:"); ImGui::SameLine(); ImGui::Dummy(ImVec2(5, 0));
            ImGui::SameLine(); if (ImGui::Button("File###save_cc_to_file")) {
                apply_cc_from_editor();
                std::string cc_name = ui_cc_name;
                // We don't want to save a CC named "Mario", as it may cause file issues.
                if (cc_name != "Mario") {
                    save_cc_file(cc_name, global_gs_code());
                } else {
                    strcpy(ui_cc_name, "Sample");
                    save_cc_file("Sample", global_gs_code());
                }
                saturn_load_cc_directory();
            }
            if (sDynosPacks.Count() > 0 && one_pack_selectable && cc_model_support) {
                string buttonLabel = ui_mfolder_name + "###save_cc_to_model";
                ImGui::SameLine(); if (ImGui::Button(buttonLabel.c_str())) {
                    apply_cc_from_editor();
                    std::string cc_name = ui_cc_name;
                    // We don't want to save a CC named "Mario", as it may cause file issues.
                    if (cc_name != "Mario") {
                        save_cc_model_file(cc_name, global_gs_code(), ui_mfolder_name);
                    } else {
                        strcpy(ui_cc_name, "Sample");
                        save_cc_model_file("Sample", global_gs_code(), ui_mfolder_name);
                    }
                    saturn_load_cc_directory();
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("GameShark")) {
            ImGui::InputTextMultiline("###gameshark_box", ui_gameshark, IM_ARRAYSIZE(ui_gameshark), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25), ImGuiInputTextFlags_CharsUppercase);
            if (ImGui::IsItemActivated()) accept_text_input = false;
            if (ImGui::IsItemDeactivated()) accept_text_input = true;

            if (ImGui::Button("Paste GS Code")) {
                string ui_gameshark_input = ui_gameshark;
                paste_gs_code(ui_gameshark_input);
                set_editor_from_global_cc("Sample");
                strcpy(ui_gameshark, global_gs_code().c_str());
            } ImGui::SameLine(); imgui_bundled_help_marker(
                "Copy/paste a GameShark color code from here!");

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
    ImGui::PopStyleColor();
}
