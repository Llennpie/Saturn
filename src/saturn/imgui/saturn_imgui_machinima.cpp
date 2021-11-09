#include "saturn_imgui_machinima.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn/saturn_animations.h"
#include "saturn_imgui.h"
#include <SDL2/SDL.h>

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
}

using namespace std;

int anim_index = 113;



void smachinima_imgui_init() {

}

void smachinima_imgui_update() {
    ImGui::Checkbox("Machinima Camera", &camera_frozen);
    imgui_bundled_tooltip("Toggles the machinima camera.");
            
    if (camera_frozen == true) {
        ImGui::SliderFloat("Speed", &camVelSpeed, 0.0f, 2.0f);
        imgui_bundled_tooltip("Controls the speed of the machinima camera. Default is 1.");
    }
    ImGui::Dummy(ImVec2(0, 5));

    const char* eyes[] = { "Blinking", "Open", "Half", "Closed", "Left", "Right", "Up", "Down", "Dead" };
    ImGui::Combo("Eyes", &scrollEyeState, eyes, IM_ARRAYSIZE(eyes));

    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Text("Animations");
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Combo("", &anim_index, saturn_animations, IM_ARRAYSIZE(saturn_animations));
    selected_animation = (MarioAnimID)anim_index;
    if (ImGui::Button("Play")) {
        saturn_play_animation(selected_animation);
    }

    imgui_bundled_space(20, "Quick Toggles", NULL);

    ImGui::Checkbox("HUD", &configHUD);
    imgui_bundled_tooltip("Controls the in-game HUD visibility.");
    if (ImGui::CollapsingHeader("Mario")) {
        ImGui::Checkbox("Head Rotations", &enable_head_rotations);
        imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
        const char* hands[] = { "Fists", "Open", "Peace", "With Cap", "With Wing Cap", "Right Open" };
        ImGui::Combo("Hand State", &scrollHandState, hands, IM_ARRAYSIZE(hands));
        const char* caps[] = { "Cap On", "Cap Off", "Wing Cap" }; // unused "wing cap off" not included
        ImGui::Combo("Cap State", &scrollCapState, caps, IM_ARRAYSIZE(caps));
    }
    ImGui::Checkbox("Shadows", &enable_shadows);
    imgui_bundled_tooltip("Displays Mario's shadow.");
    ImGui::Checkbox("Dust Particles", &enable_dust_particles);
    imgui_bundled_tooltip("Displays dust particles when Mario moves.");
}