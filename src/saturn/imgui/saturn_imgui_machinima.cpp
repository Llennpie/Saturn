#include "saturn_imgui_machinima.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <map>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/libs/imgui/imgui_neo_sequencer.h"
#include "saturn/saturn.h"
#include "saturn/saturn_textures.h"
#include "saturn/saturn_animation_ids.h"
#include "saturn/saturn_animations.h"
#include "saturn_imgui.h"
#include "saturn/imgui/saturn_imgui_chroma.h"
#include "pc/controller/controller_keyboard.h"
#include <SDL2/SDL.h>

#include "data/dynos.cpp.h"

extern "C" {
#include "sm64.h"
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "pc/cheats.h"
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "engine/level_script.h"
#include "game/game_init.h"
#include "src/game/envfx_snow.h"
}

using namespace std;

int custom_anim_index;
int current_sanim_index = 7;
std::string current_sanim_name = "BREAKDANCE";
int current_sanim_id = MARIO_ANIM_BREAKDANCE;
std::map<std::pair<int, std::string>, int> current_anim_map = sanim_actions;
std::string anim_preview_name = "BREAKDANCE";
int current_sanim_group_index = 1;
int current_slevel_index;

int current_level_sel = 0;
void warp_to(s16 destLevel, s16 destArea = 0x01, s16 destWarpNode = 0x0A) {
    if (gCurrLevelNum == destLevel || !mario_exists)
        return;

    initiate_warp(destLevel, destArea, destWarpNode, 0);
    fade_into_special_warp(0,0);
}

void anim_play_button() {
    if (is_custom_anim) {
        saturn_read_mcomp_animation(anim_preview_name);
        saturn_play_animation(selected_animation);
        saturn_play_custom_animation();
    } else {
        saturn_play_animation(selected_animation);
    }
}

void smachinima_imgui_controls(SDL_Event * event) {
    switch (event->type){
        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_m && accept_text_input) {
                if (camera_fov <= 98.0f) camera_fov += 2.f;
            } else if (event->key.keysym.sym == SDLK_n && accept_text_input) {
                if (camera_fov >= 2.0f) camera_fov -= 2.f;
            }

            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT]) {
                if (event->key.keysym.sym == SDLK_0 && accept_text_input) saturn_load_expression_number('0');
                if (event->key.keysym.sym == SDLK_1 && accept_text_input) saturn_load_expression_number('1');
                if (event->key.keysym.sym == SDLK_2 && accept_text_input) saturn_load_expression_number('2');
                if (event->key.keysym.sym == SDLK_3 && accept_text_input) saturn_load_expression_number('3');
                if (event->key.keysym.sym == SDLK_4 && accept_text_input) saturn_load_expression_number('4');
                if (event->key.keysym.sym == SDLK_5 && accept_text_input) saturn_load_expression_number('5');
                if (event->key.keysym.sym == SDLK_6 && accept_text_input) saturn_load_expression_number('6');
                if (event->key.keysym.sym == SDLK_7 && accept_text_input) saturn_load_expression_number('7');
                if (event->key.keysym.sym == SDLK_8 && accept_text_input) saturn_load_expression_number('8');
                if (event->key.keysym.sym == SDLK_9 && accept_text_input) saturn_load_expression_number('9');
            }

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
    saturn_fetch_animations();
}

uint32_t currentFrame = 0;
uint32_t startFrame = 0;
uint32_t endFrame = 60;
std::vector<uint32_t> mcamera_keys = {0};

std::vector<float> mcamera_k_pos_x = {0.f};

int to_approach;

float increaseVal = 0.f;
float x_target = 0.f;
float newVal;

void smachinima_imgui_popout() {
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::Begin("Timeline###mcamera_timeline", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos(ImVec2(300, 30));
    ImGui::SetWindowSize(ImVec2(490, 175));

    if (!mcamera_playing) {
        if (ImGui::Button("Play###mcam_t_play")) {
            mcam_timer = 0;
            mcamera_playing = true;
        }
    } else {
        if (ImGui::Button("Stop###mcam_t_stop")) {
            mcamera_playing = false;
        }
    }

    ImGui::Separator();
            
    // Popout
    if(ImGui::BeginNeoSequencer("Sequencer###mcamera_sequencer", &currentFrame, &startFrame, &endFrame)) {
        if(ImGui::BeginNeoTimeline("Position###mcam_t_pos", mcamera_keys)) {
            ImGui::EndNeoTimeLine();
        }
        ImGui::EndNeoSequencer();
    }

    // Playing
    if (mcamera_playing) {
        currentFrame = (uint32_t)(mcam_timer / 10);

        auto it1 = std::find(mcamera_keys.begin(), mcamera_keys.end(), currentFrame);
        if (it1 != mcamera_keys.end()) {
            int thisCycleFrame = (it1 - mcamera_keys.begin());
            if (mcamera_k_pos_x.size() - 1 == thisCycleFrame) {
                mcamera_playing = false;
            } else {
                int this_length = mcamera_keys.at(thisCycleFrame + 1) - currentFrame;
                float size_up = mcamera_k_pos_x.at(thisCycleFrame + 1) - mcamera_k_pos_x.at(thisCycleFrame);
                increaseVal = size_up / this_length;
                x_target = mcamera_k_pos_x.at(thisCycleFrame + 1);
                newVal = mcamera_k_pos_x.at(thisCycleFrame);

                //std::cout << this_length << std::endl;
                //marioScaleSizeX = approach_f32_asymptotic(marioScaleSizeX, mcamera_k_pos_x.at(thisCycleFrame + 1), increaseVal / 10);

                //ImGui::Text("%f", increaseVal);
            }
        }
        newVal += increaseVal;
        marioScaleSizeX = approach_f32_asymptotic(marioScaleSizeX, x_target, newVal / 10);
        ImGui::Text("%f", newVal);
    }

    // UI Controls
    if (currentFrame != 0 && !mcamera_playing) {
        if (std::find(mcamera_keys.begin(), mcamera_keys.end(), currentFrame) != mcamera_keys.end()) {
            // We are hovering over a keyframe
            auto it = std::find(mcamera_keys.begin(), mcamera_keys.end(), currentFrame);
            if (it != mcamera_keys.end()) {
                int key_index = it - mcamera_keys.begin();

                if (ImGui::Button("Delete Keyframe###mcamera_d_frame")) {
                    key_index = 0;
                    currentFrame = 0;
                    mcamera_keys.clear();
                    mcamera_keys.push_back(0);
                    mcamera_k_pos_x.clear();
                    //mcamera_k_pos_y.clear();
                    //mcamera_k_pos_z.clear();
                    mcamera_k_pos_x.push_back(0.f);
                    //mcamera_k_pos_y.push_back(gLakituState.curPos[1]);
                    //mcamera_k_pos_z.push_back(gLakituState.curPos[2]);
                } ImGui::SameLine(); ImGui::Text("at %i", (int)key_index);

                // Show keyframe data
                //if (mcamera_k_pos_x.size() > 0 && key_index != 0)
                    //ImGui::Text("Position: (%f, %f, %f)", mcamera_k_pos_x.at(key_index), mcamera_k_pos_y.at(key_index), mcamera_k_pos_z.at(key_index));
            }
        } else {
            // No keyframe here
            if (ImGui::Button("Create Keyframe###mcamera_c_frame")) {
                mcamera_keys.push_back(currentFrame);
                mcamera_k_pos_x.push_back(marioScaleSizeX);

            } ImGui::SameLine(); ImGui::Text("at %i", (int)currentFrame);
        }
    }

    ImGui::End();
    ImGui::PopStyleColor();
}

void smachinima_imgui_update() {
    ImGui::Checkbox("Machinima Camera", &camera_frozen);
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

    /*ImGui::SameLine(); if (ImGui::Button("K###mcamera_keybtn")) {
        mcamera_is_keyframe = !mcamera_is_keyframe;
    }
    if (mcamera_is_keyframe) smachinima_imgui_popout();
    */

    ImGui::SliderFloat("FOV", &camera_fov, 0.0f, 100.0f);
    imgui_bundled_tooltip("Controls the FOV of the in-game camera. Default is 50.\nKeybind -> N/M");
    ImGui::Checkbox("Smooth###smooth_fov", &camera_fov_smooth);

    selected_animation = (MarioAnimID)current_sanim_id;
    if (mario_exists) {
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Text("Animations");
        ImGui::Dummy(ImVec2(0, 5));

        if (!is_anim_playing) {
            const char* anim_groups[] = { "Movement (50)", "Actions (25)", "Automatic (27)", "Damage/Deaths (22)",
                "Cutscenes (23)", "Water (16)", "Climbing (20)", "Object (24)", "CUSTOM..." };

            int animArraySize = (canim_array.size() > 0) ? IM_ARRAYSIZE(anim_groups) : IM_ARRAYSIZE(anim_groups) - 1;
            if (ImGui::BeginCombo("###anim_groups", anim_groups[current_sanim_group_index], ImGuiComboFlags_None)) {
                for (int n = 0; n < animArraySize; n++) {
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
                            //case 8: current_anim_map = sanim_misc; break;
                        }
                        if (current_sanim_group_index != 8) {
                            is_custom_anim = false;
                            current_sanim_index = current_anim_map.begin()->first.first;
                            current_sanim_name = current_anim_map.begin()->first.second;
                            current_sanim_id = current_anim_map.begin()->second;
                            anim_preview_name = current_sanim_name;
                        } else {
                            is_custom_anim = true;
                            current_sanim_index = 0;
                            current_sanim_name = canim_array[0];
                            current_sanim_id = MARIO_ANIM_A_POSE;
                            saturn_read_mcomp_animation(canim_array[0].substr(0, canim_array[0].size() - 5));
                            anim_preview_name = current_sanim_name;
                            anim_preview_name = anim_preview_name.substr(0, anim_preview_name.size() - 5);
                        }
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                if (canim_array.size() > 0)
                    ImGui::SameLine(); imgui_bundled_help_marker("These are custom METAL Composer+ JSON animations.\nPlace in dynos/anims.");
                    
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo("###anim_box", anim_preview_name.c_str(), ImGuiComboFlags_None)) {
                if (current_sanim_group_index != 8) {
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
                } else {
                    for (int i = 0; i < canim_array.size(); i++) {
                        current_sanim_name = canim_array[i];

                        const bool is_selected = (custom_anim_index == i);
                        if (ImGui::Selectable(current_sanim_name.c_str(), is_selected)) {
                            custom_anim_index = i;
                            current_sanim_name = canim_array[i];
                            anim_preview_name = current_sanim_name;
                            is_anim_looped = current_canim_looping;
                            // Remove .json extension
                            anim_preview_name = anim_preview_name.substr(0, anim_preview_name.size() - 5);
                            saturn_read_mcomp_animation(anim_preview_name);
                            // Stop anim
                            is_anim_playing = false;
                            is_anim_paused = false;
                            using_chainer = false;
                            chainer_index = 0;
                        }

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            if (is_custom_anim) {
                // Refresh
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::Button("Refresh###refresh_canim")) {
                        saturn_fetch_animations();
                        current_sanim_index = 0;
                        current_sanim_name = canim_array[0];
                        current_sanim_id = MARIO_ANIM_A_POSE;
                        saturn_read_mcomp_animation(canim_array[0].substr(0, canim_array[0].size() - 5));
                        anim_preview_name = current_sanim_name;
                        anim_preview_name = anim_preview_name.substr(0, anim_preview_name.size() - 5);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                // Animation Credit
                string anim_credit = (current_canim_name + "\nBy " + current_canim_author);
                ImGui::SameLine(); imgui_bundled_help_marker(anim_credit.c_str());
            }
        } else {
            if (is_custom_anim) {
                ImGui::Text("Now Playing: %s", current_canim_name.c_str());
                string anim_credit1 = ("By " + current_canim_author);
                ImGui::SameLine(); imgui_bundled_help_marker(anim_credit1.c_str());
                if (using_chainer) ImGui::Text("Chainer: Enabled");
            } else {
                ImGui::Text("Now Playing: %s", anim_preview_name.c_str());
            }

            if (ImGui::Button("Stop")) {
                is_anim_playing = false;
                is_anim_paused = false;
                using_chainer = false;
                chainer_index = 0;
            } ImGui::SameLine(); ImGui::Checkbox("Loop", &is_anim_looped);
            
            ImGui::SliderInt("Frame###animation_frames", &current_anim_frame, 0, current_anim_length);
            ImGui::Checkbox("Paused###animation_paused", &is_anim_paused);
        }
        if (!is_anim_playing) {
            if (ImGui::Button("Play")) {
                anim_play_button();
            } ImGui::SameLine(); ImGui::Checkbox("Loop", &is_anim_looped);

            ImGui::SliderFloat("Speed###anim_speed", &anim_speed, 0.1f, 2.0f);
            if (anim_speed != 1.0f) {
                if (ImGui::Button("Reset###reset_anim_speed"))
                    anim_speed = 1.0f;
            }
        }

        // Warp To Level

        ImGui::Dummy(ImVec2(0, 5));

        s16 levelList[] = { 
            LEVEL_CASTLE_GROUNDS, LEVEL_CASTLE, LEVEL_SA, LEVEL_BOB, 
            LEVEL_WF, LEVEL_PSS, LEVEL_TOTWC, LEVEL_JRB, LEVEL_CCM,
            LEVEL_BITDW, LEVEL_BBH, LEVEL_HMC, LEVEL_COTMC, LEVEL_LLL,
            LEVEL_SSL, LEVEL_VCUTM, LEVEL_DDD, LEVEL_BITFS, 
            LEVEL_SL, LEVEL_WDW, LEVEL_TTM, LEVEL_THI,
            LEVEL_TTC, LEVEL_WMOTR, LEVEL_RR, LEVEL_BITS
        };

        ImGui::Text("Warp to Level");
        if (ImGui::BeginCombo("###warp_to_level", saturn_get_stage_name(levelList[current_slevel_index]), ImGuiComboFlags_None)) {
            for (int n = 0; n < IM_ARRAYSIZE(levelList); n++) {
                const bool is_selected = (current_slevel_index == n);

                if (ImGui::Selectable(saturn_get_stage_name(levelList[n]), is_selected))
                    current_slevel_index = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Warp")) {
            is_anim_playing = false;
            is_anim_paused = false;

            switch (current_slevel_index) {
                case 0:
                    warp_to(LEVEL_CASTLE_GROUNDS, 0x01, 0x04);
                    break;
                case 1:
                    warp_to(LEVEL_CASTLE, 0x01, 0x01);
                    break;
                case 2:
                    DynOS_Warp_ToLevel(LEVEL_SA, (s32)currentChromaArea, gCurrActNum);
                    break;
                default:
                    warp_to(levelList[current_slevel_index]);
                    break;
            }
        }
    }

    ImGui::Dummy(ImVec2(0, 5));

    //if (ImGui::Button("plane mode")) {
    //    set_mario_action(gMarioState, ACT_DEBUG_FREE_MOVE, 0);
    //}

    if (ImGui::BeginTable("table_quick_toggles", 2)) {
        ImGui::TableNextColumn();
        ImGui::Checkbox("HUD", &configHUD);
        imgui_bundled_tooltip("Controls the in-game HUD visibility.");
        ImGui::TableNextColumn();
        ImGui::Checkbox("Shadows", &enable_shadows);
        imgui_bundled_tooltip("Displays the shadows of various objects.");
        ImGui::TableNextColumn();
        ImGui::Checkbox("Head Rotations", &enable_head_rotations);
        imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
        ImGui::TableNextColumn();
        ImGui::Checkbox("Dust Particles", &enable_dust_particles);
        imgui_bundled_tooltip("Displays dust particles when Mario moves.");
        ImGui::TableNextColumn();
        ImGui::Checkbox("Torso Rotations", &enable_torso_rotation);
        imgui_bundled_tooltip("Tilts Mario's torso when he moves; Disable for a \"beta running\" effect.");
        if (mario_exists && gMarioState->action != ACT_FIRST_PERSON) {
            ImGui::TableNextColumn();
            if (ImGui::Button("Sleep")) {
                set_mario_action(gMarioState, ACT_START_SLEEPING, 0);
            }
        }
        ImGui::EndTable();
    }
    if (mario_exists) {
        const char* mEnvSettings[] = { "Default", "None", "Snow", "Blizzard" };
        ImGui::Combo("Environment###env_dropdown", (int*)&gLevelEnv, mEnvSettings, IM_ARRAYSIZE(mEnvSettings));
    }
}