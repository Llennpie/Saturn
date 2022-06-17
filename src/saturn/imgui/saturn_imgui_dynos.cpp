#include "saturn_imgui_dynos.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
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
}

const Array<PackData *> &sDynosPacks = DynOS_Gfx_GetPacks();

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

static char uiGsCcName[128] = "Sample";
static int current_cc_id = 0;
string cc_name;
static char cc_gameshark[1024 * 16] = "";

bool one_pack_selectable;
bool any_packs_selected;
int windowListSize = 325;
int cc_model_id;

void apply_cc_editor() {
    defaultColorHatRLight = (int)(uiHatColor.x * 255);
    defaultColorHatGLight = (int)(uiHatColor.y * 255);
    defaultColorHatBLight = (int)(uiHatColor.z * 255);
    defaultColorHatRDark = (int)(uiHatShadeColor.x * 255);
    defaultColorHatGDark = (int)(uiHatShadeColor.y * 255);
    defaultColorHatBDark = (int)(uiHatShadeColor.z * 255);
    defaultColorOverallsRLight = (int)(uiOverallsColor.x * 255);
    defaultColorOverallsGLight = (int)(uiOverallsColor.y * 255);
    defaultColorOverallsBLight = (int)(uiOverallsColor.z * 255);
    defaultColorOverallsRDark = (int)(uiOverallsShadeColor.x * 255);
    defaultColorOverallsGDark = (int)(uiOverallsShadeColor.y * 255);
    defaultColorOverallsBDark = (int)(uiOverallsShadeColor.z * 255);
    defaultColorGlovesRLight = (int)(uiGlovesColor.x * 255);
    defaultColorGlovesGLight = (int)(uiGlovesColor.y * 255);
    defaultColorGlovesBLight = (int)(uiGlovesColor.z * 255);
    defaultColorGlovesRDark = (int)(uiGlovesShadeColor.x * 255);
    defaultColorGlovesGDark = (int)(uiGlovesShadeColor.y * 255);
    defaultColorGlovesBDark = (int)(uiGlovesShadeColor.z * 255);
    defaultColorShoesRLight = (int)(uiShoesColor.x * 255);
    defaultColorShoesGLight = (int)(uiShoesColor.y * 255);
    defaultColorShoesBLight = (int)(uiShoesColor.z * 255);
    defaultColorShoesRDark = (int)(uiShoesShadeColor.x * 255);
    defaultColorShoesGDark = (int)(uiShoesShadeColor.y * 255);
    defaultColorShoesBDark = (int)(uiShoesShadeColor.z * 255);
    defaultColorSkinRLight = (int)(uiSkinColor.x * 255);
    defaultColorSkinGLight = (int)(uiSkinColor.y * 255);
    defaultColorSkinBLight = (int)(uiSkinColor.z * 255);
    defaultColorSkinRDark = (int)(uiSkinShadeColor.x * 255);
    defaultColorSkinGDark = (int)(uiSkinShadeColor.y * 255);
    defaultColorSkinBDark = (int)(uiSkinShadeColor.z * 255);
    defaultColorHairRLight = (int)(uiHairColor.x * 255);
    defaultColorHairGLight = (int)(uiHairColor.y * 255);
    defaultColorHairBLight = (int)(uiHairColor.z * 255);
    defaultColorHairRDark = (int)(uiHairShadeColor.x * 255);
    defaultColorHairGDark = (int)(uiHairShadeColor.y * 255);
    defaultColorHairBDark = (int)(uiHairShadeColor.z * 255);
    if (cc_spark_support) {
        sparkColorShirtRLight = (int)(uiShirtColor.x * 255);
        sparkColorShirtGLight = (int)(uiShirtColor.y * 255);
        sparkColorShirtBLight = (int)(uiShirtColor.z * 255);
        sparkColorShirtRDark = (int)(uiShirtShadeColor.x * 255);
        sparkColorShirtGDark = (int)(uiShirtShadeColor.y * 255);
        sparkColorShirtBDark = (int)(uiShirtShadeColor.z * 255);
        sparkColorShouldersRLight = (int)(uiShouldersColor.x * 255);
        sparkColorShouldersGLight = (int)(uiShouldersColor.y * 255);
        sparkColorShouldersBLight = (int)(uiShouldersColor.z * 255);
        sparkColorShouldersRDark = (int)(uiShouldersShadeColor.x * 255);
        sparkColorShouldersGDark = (int)(uiShouldersShadeColor.y * 255);
        sparkColorShouldersBDark = (int)(uiShouldersShadeColor.z * 255);
        sparkColorArmsRLight = (int)(uiArmsColor.x * 255);
        sparkColorArmsGLight = (int)(uiArmsColor.y * 255);
        sparkColorArmsBLight = (int)(uiArmsColor.z * 255);
        sparkColorArmsRDark = (int)(uiArmsShadeColor.x * 255);
        sparkColorArmsGDark = (int)(uiArmsShadeColor.y * 255);
        sparkColorArmsBDark = (int)(uiArmsShadeColor.z * 255);
        sparkColorOverallsBottomRLight = (int)(uiOverallsBottomColor.x * 255);
        sparkColorOverallsBottomGLight = (int)(uiOverallsBottomColor.y * 255);
        sparkColorOverallsBottomBLight = (int)(uiOverallsBottomColor.z * 255);
        sparkColorOverallsBottomRDark = (int)(uiOverallsBottomShadeColor.x * 255);
        sparkColorOverallsBottomGDark = (int)(uiOverallsBottomShadeColor.y * 255);
        sparkColorOverallsBottomBDark = (int)(uiOverallsBottomShadeColor.z * 255);
        sparkColorLegTopRLight = (int)(uiLegTopColor.x * 255);
        sparkColorLegTopGLight = (int)(uiLegTopColor.y * 255);
        sparkColorLegTopBLight = (int)(uiLegTopColor.z * 255);
        sparkColorLegTopRDark = (int)(uiLegTopShadeColor.x * 255);
        sparkColorLegTopGDark = (int)(uiLegTopShadeColor.y * 255);
        sparkColorLegTopBDark = (int)(uiLegTopShadeColor.z * 255);
        sparkColorLegBottomRLight = (int)(uiLegBottomColor.x * 255);
        sparkColorLegBottomGLight = (int)(uiLegBottomColor.y * 255);
        sparkColorLegBottomBLight = (int)(uiLegBottomColor.z * 255);
        sparkColorLegBottomRDark = (int)(uiLegBottomShadeColor.x * 255);
        sparkColorLegBottomGDark = (int)(uiLegBottomShadeColor.y * 255);
        sparkColorLegBottomBDark = (int)(uiLegBottomShadeColor.z * 255);
    }

    strcpy(cc_gameshark, global_gs_code().c_str());
}

void apply_editor_global_cc() {
    uiHatColor = ImVec4(float(defaultColorHatRLight) / 255.0f, float(defaultColorHatGLight) / 255.0f, float(defaultColorHatBLight) / 255.0f, 255.0f / 255.0f);
    uiHatShadeColor = ImVec4(float(defaultColorHatRDark) / 255.0f, float(defaultColorHatGDark) / 255.0f, float(defaultColorHatBDark) / 255.0f, 255.0f / 255.0f);
    uiOverallsColor = ImVec4(float(defaultColorOverallsRLight) / 255.0f, float(defaultColorOverallsGLight) / 255.0f, float(defaultColorOverallsBLight) / 255.0f, 255.0f / 255.0f);
    uiOverallsShadeColor = ImVec4(float(defaultColorOverallsRDark) / 255.0f, float(defaultColorOverallsGDark) / 255.0f, float(defaultColorOverallsBDark) / 255.0f, 255.0f / 255.0f);
    uiGlovesColor = ImVec4(float(defaultColorGlovesRLight) / 255.0f, float(defaultColorGlovesGLight) / 255.0f, float(defaultColorGlovesBLight) / 255.0f, 255.0f / 255.0f);
    uiGlovesShadeColor = ImVec4(float(defaultColorGlovesRDark) / 255.0f, float(defaultColorGlovesGDark) / 255.0f, float(defaultColorGlovesBDark) / 255.0f, 255.0f / 255.0f);
    uiShoesColor = ImVec4(float(defaultColorShoesRLight) / 255.0f, float(defaultColorShoesGLight) / 255.0f, float(defaultColorShoesBLight) / 255.0f, 255.0f / 255.0f);
    uiShoesShadeColor = ImVec4(float(defaultColorShoesRDark) / 255.0f, float(defaultColorShoesGDark) / 255.0f, float(defaultColorShoesBDark) / 255.0f, 255.0f / 255.0f);
    uiSkinColor = ImVec4(float(defaultColorSkinRLight) / 255.0f, float(defaultColorSkinGLight) / 255.0f, float(defaultColorSkinBLight) / 255.0f, 255.0f / 255.0f);
    uiSkinShadeColor = ImVec4(float(defaultColorSkinRDark) / 255.0f, float(defaultColorSkinGDark) / 255.0f, float(defaultColorSkinBDark) / 255.0f, 255.0f / 255.0f);
    uiHairColor = ImVec4(float(defaultColorHairRLight) / 255.0f, float(defaultColorHairGLight) / 255.0f, float(defaultColorHairBLight) / 255.0f, 255.0f / 255.0f);
    uiHairShadeColor = ImVec4(float(defaultColorHairRDark) / 255.0f, float(defaultColorHairGDark) / 255.0f, float(defaultColorHairBDark) / 255.0f, 255.0f / 255.0f);

    if (cc_spark_support) {
        uiShirtColor = ImVec4(float(sparkColorShirtRLight) / 255.0f, float(sparkColorShirtGLight) / 255.0f, float(sparkColorShirtBLight) / 255.0f, 255.0f / 255.0f);
        uiShirtShadeColor = ImVec4(float(sparkColorShirtRDark) / 255.0f, float(sparkColorShirtGDark) / 255.0f, float(sparkColorShirtBDark) / 255.0f, 255.0f / 255.0f);
        uiShouldersColor = ImVec4(float(sparkColorShouldersRLight) / 255.0f, float(sparkColorShouldersGLight) / 255.0f, float(sparkColorShouldersBLight) / 255.0f, 255.0f / 255.0f);
        uiShouldersShadeColor = ImVec4(float(sparkColorShouldersRDark) / 255.0f, float(sparkColorShouldersGDark) / 255.0f, float(sparkColorShouldersBDark) / 255.0f, 255.0f / 255.0f);
        uiArmsColor = ImVec4(float(sparkColorArmsRLight) / 255.0f, float(sparkColorArmsGLight) / 255.0f, float(sparkColorArmsBLight) / 255.0f, 255.0f / 255.0f);
        uiArmsShadeColor = ImVec4(float(sparkColorArmsRDark) / 255.0f, float(sparkColorArmsGDark) / 255.0f, float(sparkColorArmsBDark) / 255.0f, 255.0f / 255.0f);
        uiOverallsBottomColor = ImVec4(float(sparkColorOverallsBottomRLight) / 255.0f, float(sparkColorOverallsBottomGLight) / 255.0f, float(sparkColorOverallsBottomBLight) / 255.0f, 255.0f / 255.0f);
        uiOverallsBottomShadeColor = ImVec4(float(sparkColorOverallsBottomRDark) / 255.0f, float(sparkColorOverallsBottomGDark) / 255.0f, float(sparkColorOverallsBottomBDark) / 255.0f, 255.0f / 255.0f);
        uiLegTopColor = ImVec4(float(sparkColorLegTopRLight) / 255.0f, float(sparkColorLegTopGLight) / 255.0f, float(sparkColorLegTopBLight) / 255.0f, 255.0f / 255.0f);
        uiLegTopShadeColor = ImVec4(float(sparkColorLegTopRDark) / 255.0f, float(sparkColorLegTopGDark) / 255.0f, float(sparkColorLegTopBDark) / 255.0f, 255.0f / 255.0f);
        uiLegBottomColor = ImVec4(float(sparkColorLegBottomRLight) / 255.0f, float(sparkColorLegBottomGLight) / 255.0f, float(sparkColorLegBottomBLight) / 255.0f, 255.0f / 255.0f);
        uiLegBottomShadeColor = ImVec4(float(sparkColorLegBottomRDark) / 255.0f, float(sparkColorLegBottomGDark) / 255.0f, float(sparkColorLegBottomBDark) / 255.0f, 255.0f / 255.0f);
    }

    cc_name = cc_array[current_cc_id].substr(0, cc_array[current_cc_id].size() - 3);
    strcpy(cc_gameshark, global_gs_code().c_str());

    // We never want to use the name "Mario" when saving/loading a CC, as it will cause file issues.
    if (cc_name == "Mario") {
        strcpy(uiGsCcName, "Sample");
    } else {
        strcpy(uiGsCcName, cc_name.c_str());
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

void sdynos_imgui_init() {
    load_cc_directory();
    strcpy(cc_gameshark, global_gs_code().c_str());
}

void sdynos_imgui_update() {
    if (cc_array.size() > 0) {
        ImGui::Text("Select Color Code");
        cc_name = cc_array[current_cc_id].substr(0, cc_array[current_cc_id].size() - 3);
        if (ImGui::BeginCombo(".gs", cc_name.c_str()))
        {
            for (int n = 0; n < cc_array.size(); n++)
            {
                const bool is_selected = (current_cc_id == n);
                cc_name = cc_array[n].substr(0, cc_array[n].size() - 3);
                if (ImGui::Selectable(cc_name.c_str(), is_selected)) {
                    current_cc_id = n;
                }
                if (cc_name != "Mario") {
                    if (ImGui::BeginPopupContextItem())
                    {
                        ImGui::Text("%s.gs", cc_name.c_str());
                        if (ImGui::Button("Delete File")) {
                            delete_cc_file(cc_name);
                            ImGui::CloseCurrentPopup();
                        } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                        ImGui::EndPopup();
                    }
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine(); imgui_bundled_help_marker("These are GameShark color codes, which overwrite Mario's lights. Place in dynos/colorcodes.");
        if (ImGui::Button("Load CC")) {
            load_cc_file((char*)cc_array[current_cc_id].c_str());
            apply_editor_global_cc();
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh"))
            load_cc_directory();

    } else {
        ImGui::Text("Could not load color code directory...");
    }

    ImGui::Dummy(ImVec2(0, 5)); ImGui::Separator(); ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::BeginTabBar("###dynos_tabbar", ImGuiTabBarFlags_None)) {
        ImGui::SetWindowSize(ImVec2(275, 475));

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
                    apply_cc_editor();
                }
            } else {
                apply_cc_editor();
            }

            ImGui::Dummy(ImVec2(0, 5));

            ImGui::InputText(".gs", uiGsCcName, IM_ARRAYSIZE(uiGsCcName));
            if (ImGui::IsItemActivated()) accept_text_input = false;
            if (ImGui::IsItemDeactivated()) accept_text_input = true;

            if (ImGui::Button("Save to File")) {
                apply_cc_editor();
                std::string cc_name = uiGsCcName;
                // We don't want to save a CC named "Mario", as it may cause file issues.
                if (cc_name != "Mario") {
                    save_cc_file(cc_name);
                } else {
                    strcpy(uiGsCcName, "Sample");
                    save_cc_file("Sample");
                }
                load_cc_directory();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("GameShark")) {
            ImGui::SetWindowSize(ImVec2(275, 475));

            ImGui::InputTextMultiline("###gameshark_box", cc_gameshark, IM_ARRAYSIZE(cc_gameshark), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25), ImGuiInputTextFlags_CharsUppercase);
            if (ImGui::IsItemActivated()) accept_text_input = false;
            if (ImGui::IsItemDeactivated()) accept_text_input = true;

            if (ImGui::Button("Paste GS Code")) {
                string cc_gameshark_input = cc_gameshark;
                paste_gs_code(cc_gameshark_input);
                apply_editor_global_cc();
                strcpy(cc_gameshark, global_gs_code().c_str());
                strcpy(uiGsCcName, "Sample");
            } ImGui::SameLine(); imgui_bundled_help_marker(
                "Copy/paste a GameShark color code from here!");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Model Packs")) {
            ImGui::SameLine(); imgui_bundled_help_marker(
                "These are DynOS model packs, used for live model loading.\nPlace packs in dynos/packs.");

            ImGui::SetWindowSize(ImVec2(275, windowListSize));

            if (ImGui::BeginListBox("", ImVec2(-FLT_MIN, 200))) {
                for (int i = 0; i < sDynosPacks.Count(); i++) {
                    u64 _DirSep1 = sDynosPacks[i]->mPath.find_last_of('\\');
                    u64 _DirSep2 = sDynosPacks[i]->mPath.find_last_of('/');
                    if (_DirSep1++ == SysPath::npos) _DirSep1 = 0;
                    if (_DirSep2++ == SysPath::npos) _DirSep2 = 0;

                    std::string label = sDynosPacks[i]->mPath.substr(MAX(_DirSep1, _DirSep2));
                    bool selected = DynOS_Opt_GetValue(String("dynos_pack_%d", i));

                    if (ImGui::Selectable(label.c_str(), &selected)) {
                        // Deselect other packs, but LSHIFT allows additive
                        for (int j = 0; j < sDynosPacks.Count(); j++) {
                            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT] == false)
                                DynOS_Opt_SetValue(String("dynos_pack_%d", j), false);
                        }
                        
                        DynOS_Opt_SetValue(String("dynos_pack_%d", i), selected);

                        one_pack_selectable = false;
                        for (int k = 0; k < sDynosPacks.Count(); k++) {
                            if (DynOS_Opt_GetValue(String("dynos_pack_%d", k)))
                                one_pack_selectable = true;
                        }

                        any_packs_selected = one_pack_selectable;
                    }
                    if (ImGui::BeginPopupContextItem())
                    {
                        ImGui::Text("%s", label.c_str());
                        if (ImGui::Button("Set Model CC")) {
                            set_cc_from_model(sDynosPacks[i]->mPath);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndListBox();

                if (any_packs_selected) {
                    windowListSize = 375;
                    ImGui::Checkbox("CC Compatibility", &cc_model_support);
                    ImGui::SameLine(); imgui_bundled_help_marker(
                        "Toggles color code compatibility for model packs that support it.");

                    ImGui::Checkbox("CometSPARK Support", &cc_spark_support);
                    ImGui::SameLine(); imgui_bundled_help_marker(
                        "Grants a model extra color values. Automatically enabled for DynOS packs with the keyword \"CmtSPARK\".");
                } else {
                    windowListSize = 325;
                }
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}