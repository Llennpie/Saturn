#include "saturn_imgui_chroma.h"

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
#include "game/camera.h"
#include "game/level_update.h"
#include "engine/level_script.h"
#include "engine/geo_layout.h"
#include "src/game/envfx_snow.h"
}

using namespace std;

// Variables

static ImVec4 uiChromaColor =              ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
bool renderFloor = false;
int currentChromaArea = 1;

void set_chroma_color() {
    int r5 = ((int)(uiChromaColor.x * 255) * 31 / 255);
    int g5 = ((int)(uiChromaColor.y * 255) * 31 / 255);
    int b5 = ((int)(uiChromaColor.z * 255) * 31 / 255);
    int rShift = (int) r5 << 11;
    int bShift = (int) g5 << 6;
    int gShift = (int) b5 << 1;
    gChromaKeyColor = (int) (bShift | gShift | rShift | 1);

    chromaColor[0] = (int)(uiChromaColor.x * 255);
    chromaColor[2] = (int)(uiChromaColor.y * 255);
    chromaColor[4] = (int)(uiChromaColor.z * 255);
    // No shading
    chromaColor[1] = chromaColor[0];
    chromaColor[3] = chromaColor[2];
    chromaColor[5] = chromaColor[4];
}

void schroma_imgui_init() {
    set_chroma_color();
}

void schroma_imgui_update() {
    if (ImGui::Checkbox("Color Skybox", &use_color_background))
        chromaRequireReload = true;

    if (use_color_background) {
        if (ImGui::ColorEdit4("Chroma Key Color", (float*)&uiChromaColor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoLabel))
            chromaRequireReload = true;

        ImGui::SameLine(); ImGui::Text("Color");
        if (gCurrLevelNum == LEVEL_SA) {
            if (ImGui::Checkbox("Render Floor", &renderFloor))
                chromaRequireReload = true;

            ImGui::SameLine(); imgui_bundled_help_marker("Renders a floor object; Useful for animations that clip through the ground.");
        }
    } else {
        const char* mSkyboxSettings[] = { "Ocean Sky", "Flaming Sky", "Underwater City", "Below Clouds", "Snow Mountains", "Desert", "Haunted", "Green Sky", "Above Clouds", "Purple Sky" };
        if (ImGui::Combo("###skybox_background", (int*)&gChromaKeyBackground, mSkyboxSettings, IM_ARRAYSIZE(mSkyboxSettings)))
            chromaRequireReload = true;
    }
    currentChromaArea = (renderFloor & use_color_background) ? 1 : 2;
    if (gCurrLevelNum != LEVEL_SA) currentChromaArea = gCurrAreaIndex;
        
    if (ImGui::IsItemActivated()) accept_text_input = false;
    if (ImGui::IsItemDeactivated()) accept_text_input = true;

    if (ImGui::Button("Reload###apply_chroma_color")) {
        set_chroma_color();
        mario_loaded = false;
        chromaRequireReload = false;
        bool result = DynOS_Warp_ToLevel(gCurrLevelNum, (s32)currentChromaArea, gCurrActNum);
    } imgui_bundled_tooltip("WARNING: This will restart the level!");

    ImGui::Dummy(ImVec2(0, 5));

    ImGui::Checkbox("Shadows###chroma_shadows", &enable_shadows);
    ImGui::Checkbox("Dust Particles###chroma_dust", &enable_dust_particles);
    imgui_bundled_tooltip("Displays dust particles when Mario moves; Better to leave off when chroma keying.");
    
    const char* mEnvSettings[] = { "Default", "None", "Snow", "Blizzard" };
    ImGui::Combo("Environment###env_dropdown", (int*)&gLevelEnv, mEnvSettings, IM_ARRAYSIZE(mEnvSettings));
}
