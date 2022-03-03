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
#include "pc/cheats.h"
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
}

using namespace std;

int anim_index = 113;

void smachinima_imgui_controls(SDL_Event * event) {
    switch (event->type){
        case SDL_KEYDOWN:
            if(event->key.keysym.sym == SDLK_f)
                camera_frozen = !camera_frozen;
            if(event->key.keysym.sym == SDLK_g)
                saturn_play_animation(selected_animation);
            if(event->key.keysym.sym == SDLK_h)
                configHUD = !configHUD;
            if(event->key.keysym.sym == SDLK_p)
                is_anim_paused = !is_anim_paused;

        case SDL_CONTROLLERBUTTONDOWN:
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)
                camera_frozen = !camera_frozen;
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                saturn_play_animation(selected_animation);
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
                configHUD = !configHUD;
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
                is_anim_paused = !is_anim_paused;
        
        break;
    }
}

void smachinima_imgui_init() {
    Cheats.EnableCheats = true;
    Cheats.GodMode = true;
    Cheats.ExitAnywhere = true;
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
    } ImGui::SameLine(); ImGui::Checkbox("Loop", &is_anim_looped);

    if (mario_exists) {
        ImGui::Text("Now Playing: %s", saturn_animations[current_anim_id]);
        if (is_anim_playing) {
            ImGui::SliderInt("Frame###animation_frames", &current_anim_frame, 0, current_anim_length);
            ImGui::Checkbox("Paused###animation_paused", &is_anim_paused);
        }
    }

    imgui_bundled_space(20, "Quick Toggles", NULL);

    ImGui::Checkbox("HUD", &configHUD);
    imgui_bundled_tooltip("Controls the in-game HUD visibility.");
    ImGui::Checkbox("Shadows", &enable_shadows);
    imgui_bundled_tooltip("Displays the shadows of various objects.");
    if (ImGui::CollapsingHeader("Mario")) {
        ImGui::Checkbox("Head Rotations", &enable_head_rotations);
        imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
        const char* hands[] = { "Fists", "Open", "Peace", "With Cap", "With Wing Cap", "Right Open" };
        ImGui::Combo("Hand State", &scrollHandState, hands, IM_ARRAYSIZE(hands));
        const char* caps[] = { "Cap On", "Cap Off", "Wing Cap" }; // unused "wing cap off" not included
        ImGui::Combo("Cap State", &scrollCapState, caps, IM_ARRAYSIZE(caps));
        ImGui::Checkbox("Custom Mario Scale", &Cheats.CustomMarioScale);
        if (Cheats.CustomMarioScale)
            ImGui::SliderFloat("Scale ###mario_scale", &marioScaleSize, 0.2f, 5.0f);
        ImGui::Checkbox("Dust Particles", &enable_dust_particles);
        imgui_bundled_tooltip("Displays dust particles when Mario moves.");
        ImGui::Dummy(ImVec2(0, 5));
    }
    ImGui::Checkbox("Infinite Health", &Cheats.GodMode);
    ImGui::Checkbox("Moon Jump", &Cheats.MoonJump);
    imgui_bundled_tooltip("Just like '07! Hold L to in the air to moon jump.");
    ImGui::Checkbox("Exit Anywhere", &Cheats.ExitAnywhere);
    imgui_bundled_tooltip("Allows the level to be exited from any state.");
}