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
#include "pc/controller/controller_keyboard.h"
#include <SDL2/SDL.h>

extern "C" {
#include "sm64.h"
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "pc/cheats.h"
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "engine/level_script.h"
}

using namespace std;

int anim_index = 113;

void smachinima_imgui_controls(SDL_Event * event) {
    switch (event->type){
        case SDL_KEYDOWN:
            if(event->key.keysym.sym == SDLK_f && accept_text_input)
                camera_frozen = !camera_frozen;
            if(event->key.keysym.sym == SDLK_g && accept_text_input)
                saturn_play_animation(selected_animation);
            if(event->key.keysym.sym == SDLK_h && accept_text_input)
                configHUD = !configHUD;
            if(event->key.keysym.sym == SDLK_p && accept_text_input)
                if (is_anim_playing)
                    is_anim_paused = !is_anim_paused;

        case SDL_CONTROLLERBUTTONDOWN:
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)
                camera_frozen = !camera_frozen;
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                saturn_play_animation(selected_animation);
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
                configHUD = !configHUD;
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
                if (is_anim_playing)
                    is_anim_paused = !is_anim_paused;

        case SDL_MOUSEMOTION:
            SDL_Delay(2);
            camera_view_move_x = event->motion.xrel;
            camera_view_move_y = event->motion.yrel;
        
        break;
    }
}

void smachinima_imgui_init() {
    Cheats.EnableCheats = true;
    Cheats.GodMode = true;
    Cheats.ExitAnywhere = true;

    //SDL_SetRelativeMouseMode(SDL_TRUE);
}

void smachinima_imgui_update() {
    ImGui::Checkbox("Machinima Camera", &camera_frozen);
    imgui_bundled_tooltip("Toggles the machinima camera.");
    if (configMCameraMode == 0) {
        ImGui::SameLine(); imgui_bundled_help_marker("LShift + Mouse Buttons = Move Camera");
    } else if (configMCameraMode == 1) {
        ImGui::SameLine(); imgui_bundled_help_marker("R + C-Buttons = Pan Camera, L + C-Buttons = Raise/Lower Camera");
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
    if (selected_animation == MARIO_ANIM_TILT_SPAZ)
        imgui_bundled_tooltip("by Weegeepie");
    if (ImGui::Button("Play")) {
        saturn_play_animation(selected_animation);
    } ImGui::SameLine(); ImGui::Checkbox("Loop", &is_anim_looped);

    if (mario_exists) {
        if (is_anim_playing) {
            ImGui::Text("Now Playing: %s", saturn_animations[current_anim_id]);
            ImGui::SliderInt("Frame###animation_frames", &current_anim_frame, 0, current_anim_length);
            ImGui::Checkbox("Paused###animation_paused", &is_anim_paused);
        }

        /*
        ImGui::Dummy(ImVec2(0, 5));

        if (gCurrLevelNum != LEVEL_SA) {
            if (ImGui::Button("Warp to Chroma Key Stage")) {
                initiate_warp(LEVEL_SA, 0x01, 0xF8, 0);
                fade_into_special_warp(0,0);
            }
        } else {
            if (ImGui::Button("Warp Home")) {
                initiate_warp(LEVEL_CASTLE_GROUNDS, 0x01, 0x04, 0);
                fade_into_special_warp(0,0);
            }
        }
        */
    }

    imgui_bundled_space(20, "Quick Toggles", NULL);

    ImGui::Checkbox("HUD", &configHUD);
    imgui_bundled_tooltip("Controls the in-game HUD visibility.");
    ImGui::Checkbox("Shadows", &enable_shadows);
    imgui_bundled_tooltip("Displays the shadows of various objects.");
    if (mario_exists) {
        if (ImGui::CollapsingHeader("Mario")) {
            ImGui::Checkbox("Head Rotations", &enable_head_rotations);
            imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
            ImGui::Checkbox("Dust Particles", &enable_dust_particles);
            imgui_bundled_tooltip("Displays dust particles when Mario moves.");
            if (gMarioState->action != ACT_FIRST_PERSON) {
                if (ImGui::Button("Sleep")) {
                    set_mario_action(gMarioState, ACT_START_SLEEPING, 0);
                }
                ImGui::SameLine();
            }
            ImGui::Checkbox("Can Fall Asleep", &can_fall_asleep);
            ImGui::Dummy(ImVec2(0, 5));
            ImGui::Text("States");
            const char* hands[] = { "Fists", "Open", "Peace", "With Cap", "With Wing Cap", "Right Open" };
            ImGui::Combo("Hand###hand_state", &scrollHandState, hands, IM_ARRAYSIZE(hands));
            const char* caps[] = { "Cap On", "Cap Off", "Wing Cap" }; // unused "wing cap off" not included
            ImGui::Combo("Cap###cap_state", &scrollCapState, caps, IM_ARRAYSIZE(caps));
            const char* powerups[] = { "Default", "Metal", "Vanish", "Metal & Vanish" };
            ImGui::Combo("Powerup###powerup_state", &saturnModelState, powerups, IM_ARRAYSIZE(powerups));
            ImGui::Dummy(ImVec2(0, 5));
            ImGui::Checkbox("Custom Mario Scale", &Cheats.CustomMarioScale);
            if (Cheats.CustomMarioScale)
                ImGui::SliderFloat("Scale ###mario_scale", &marioScaleSize, 0.2f, 5.0f);
            const char* playAsModels[] = { "Mario", "Bob-omb", "Bob-omb Buddy", "Goomba", "Koopa Shell", "Chuckya", "Fly Guy" };
            ImGui::Combo("Model", &Cheats.PlayAs, playAsModels, IM_ARRAYSIZE(playAsModels));
            ImGui::Dummy(ImVec2(0, 5));
        }
    }
    ImGui::Checkbox("Infinite Health", &Cheats.GodMode);
    ImGui::Checkbox("Moon Jump", &Cheats.MoonJump);
    imgui_bundled_tooltip("Just like '07! Hold L to in the air to moon jump.");
}