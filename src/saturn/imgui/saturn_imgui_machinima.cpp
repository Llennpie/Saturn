#include "saturn_imgui_machinima.h"

#include <string>
#include <iostream>
#include <map>

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

int current_sanim_index = 7;
std::string current_sanim_name = "BREAKDANCE";
int current_sanim_id = MARIO_ANIM_BREAKDANCE;
std::map<std::pair<int, std::string>, int> current_anim_map = sanim_actions;
std::string anim_preview_name = "BREAKDANCE";
int current_sanim_group_index = 1;

void smachinima_imgui_controls(SDL_Event * event) {
    switch (event->type){
        case SDL_KEYDOWN:
            // Camera
            if(event->key.keysym.sym == SDLK_f && accept_text_input)
                camera_frozen = !camera_frozen;

            if (event->key.keysym.sym == SDLK_m && accept_text_input) {
                if (camera_fov <= 98.0f) camera_fov += 2.f;
            } else if (event->key.keysym.sym == SDLK_n && accept_text_input) {
                if (camera_fov >= 2.0f) camera_fov -= 2.f;
            }
            // Animations
            if(event->key.keysym.sym == SDLK_o && accept_text_input)
                if (!is_anim_playing) {
                    saturn_play_animation(selected_animation);
                } else {
                    is_anim_playing = false;
                    is_anim_paused = false;
                }
            if(event->key.keysym.sym == SDLK_p && accept_text_input)
                if (is_anim_playing)
                    is_anim_paused = !is_anim_paused;
            if(event->key.keysym.sym == SDLK_i && accept_text_input)
                is_anim_looped = !is_anim_looped;

        case SDL_CONTROLLERBUTTONDOWN:
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)
                camera_frozen = !camera_frozen;
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
                if (!is_anim_playing) {
                    saturn_play_animation(selected_animation);
                } else {
                    is_anim_playing = false;
                }
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
}

void smachinima_imgui_update() {
    ImGui::Checkbox("Machinima Camera", &camera_frozen);
    imgui_bundled_tooltip("Toggles the machinima camera.");
    if (configMCameraMode == 2) {
        ImGui::SameLine(); imgui_bundled_help_marker("Move Camera -> LShift + Mouse Buttons");
    } else if (configMCameraMode == 1) {
        ImGui::SameLine(); imgui_bundled_help_marker("Pan Camera -> R + C-Buttons\nRaise/Lower Camera -> L + C-Buttons\nRotate Camera -> L + Crouch + C-Buttons");
    } else if (configMCameraMode == 0) {
        ImGui::SameLine(); imgui_bundled_help_marker("Move Camera -> Y/G/H/J\nRaise/Lower Camera -> T/U\nRotate Camera -> R + Y/G/H/J");
    }
    if (configMCameraMode == 0 && camera_frozen || configMCameraMode == 1 && camera_frozen) {
        ImGui::SliderFloat("Speed", &camVelSpeed, 0.0f, 2.0f);
        imgui_bundled_tooltip("Controls the speed of the machinima camera while enabled. Default is 1.");
    }

    ImGui::SliderFloat("FOV", &camera_fov, 0.0f, 100.0f);
    imgui_bundled_tooltip("Controls the FOV of the in-game camera. Default is 50.\nKeybind -> N/M");
    ImGui::Checkbox("Smooth###smooth_fov", &camera_fov_smooth);

    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Text("Animations");
    ImGui::Dummy(ImVec2(0, 5));

    const char* anim_groups[] = { "Movement (50)", "Actions (24)", "Automatic (27)", "Damage/Deaths (22)",
        "Cutscenes (23)", "Water (16)", "Climbing (20)", "Object (24)", "Misc. (2)" };

    if (ImGui::BeginCombo("###anim_groups", anim_groups[current_sanim_group_index], ImGuiComboFlags_None)) {
        for (int n = 0; n < IM_ARRAYSIZE(anim_groups); n++) {
            const bool is_selected = (current_sanim_group_index == n);
            if (ImGui::Selectable(anim_groups[n], is_selected)) {
                current_sanim_group_index = n;
                switch(current_sanim_group_index) {
                    case 0: current_anim_map = sanim_movement; break;
                    case 1: current_anim_map = sanim_actions; break;
                    case 2: current_anim_map = sanim_automatic; break;
                    case 3: current_anim_map = sanim_damagedeaths; break;
                    case 4: current_anim_map = sanim_cutscenes; break;
                    case 5: current_anim_map = sanim_water; break;
                    case 6: current_anim_map = sanim_climbing; break;
                    case 7: current_anim_map = sanim_object; break;
                    case 8: current_anim_map = sanim_misc; break;
                }
                current_sanim_index = current_anim_map.begin()->first.first;
                current_sanim_name = current_anim_map.begin()->first.second;
                current_sanim_id = current_anim_map.begin()->second;
                anim_preview_name = current_sanim_name;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("###anim_box", anim_preview_name.c_str(), ImGuiComboFlags_None)) {
        for (auto &[a,b]:current_anim_map) {
            current_sanim_index = a.first;
            current_sanim_name = a.second;

            const bool is_selected = (current_sanim_id == b);
            if (ImGui::Selectable(current_sanim_name.c_str(), is_selected)) {
                current_sanim_index = a.first;
                current_sanim_name = a.second;
                current_sanim_id = b;
                anim_preview_name = current_sanim_name;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    selected_animation = (MarioAnimID)current_sanim_id;

    if (mario_exists) {
        if (is_anim_playing) {
            if (ImGui::Button("Stop")) {
                is_anim_playing = false;
                is_anim_paused = false;
            } ImGui::SameLine(); ImGui::Checkbox("Loop", &is_anim_looped);
            
            ImGui::Text("Now Playing: %s", anim_preview_name.c_str());
            ImGui::SliderInt("Frame###animation_frames", &current_anim_frame, 0, current_anim_length);
            ImGui::Checkbox("Paused###animation_paused", &is_anim_paused);
        } else {
            if (ImGui::Button("Play")) {
                saturn_play_animation(selected_animation);
            } ImGui::SameLine(); ImGui::Checkbox("Loop", &is_anim_looped);
        }
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
            if (gCurrLevelNum != LEVEL_SA) {
                ImGui::Combo("Model", &Cheats.PlayAs, playAsModels, IM_ARRAYSIZE(playAsModels));
                ImGui::SameLine(); imgui_bundled_tooltip(
                    "EXPERIMENTAL: \"Play as\" cheats, allowing you to use a vanilla model. Prone to crashing and scaling issues.");
            }
            ImGui::Dummy(ImVec2(0, 5));
        }
    }

    ImGui::Text("World Lighting");

    if (world_light_dir1 != 0.f || world_light_dir2 != 0.f || world_light_dir3 != 0.f || world_light_dir4 != 0.f) {
        if (ImGui::Button("Reset###reset_wshading")) {
            world_light_dir1 = 0.f;
            world_light_dir2 = 0.f;
            world_light_dir3 = 0.f;
            world_light_dir4 = 1.f;
        }
    }

    ImGui::SliderFloat("X###wdir_x", &world_light_dir1, -2.f, 2.f);
    ImGui::SliderFloat("Y###wdir_y", &world_light_dir2, -2.f, 2.f);
    ImGui::SliderFloat("Z###wdir_z", &world_light_dir3, -2.f, 2.f);
    ImGui::SliderFloat("Tex###wdir_tex", &world_light_dir4, 1.f, 4.f);
}