#include "saturn_imgui_chroma.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn_imgui.h"
#include "pc/controller/controller_keyboard.h"
#include "data/dynos.cpp.h"
#include <SDL2/SDL.h>

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "engine/level_script.h"
}

using namespace std;

// Variables

static ImVec4 uiChromaColor =              ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);

void set_chroma_color() {
    int r5 = ((int)(uiChromaColor.x * 255) * 31 / 255);
    int g5 = ((int)(uiChromaColor.y * 255) * 31 / 255);
    int b5 = ((int)(uiChromaColor.z * 255) * 31 / 255);
    int rShift = (int) r5 << 11;
    int bShift = (int) g5 << 6;
    int gShift = (int) b5 << 1;
    gChromaKeyColor = (int) (bShift | gShift | rShift | 1);
}

void schroma_imgui_init() {
    set_chroma_color();
}

void schroma_imgui_update() {
    ImGui::Text("Background Color");
    ImGui::ColorEdit4("Chroma Key Color", (float*)&uiChromaColor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoLabel);
    if (ImGui::IsItemActivated()) accept_text_input = false;
    if (ImGui::IsItemDeactivated()) accept_text_input = true;
    ImGui::SameLine(); ImGui::Text("Background");

    if (ImGui::Button("Apply###apply_chroma_color")) {
        set_chroma_color();
        mario_loaded = false;
        bool result = DynOS_Warp_RestartLevel();
    } imgui_bundled_tooltip("WARNING: This will reload the level!");

    ImGui::Checkbox("Shadows###chroma_shadows", &enable_shadows);
    ImGui::Checkbox("Dust Particles###chroma_dust", &enable_dust_particles);
    imgui_bundled_tooltip("Displays dust particles when Mario moves - better to leave off when chroma keying.");
}