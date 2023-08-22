#include "saturn_imgui_machinima.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <map>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn/saturn_textures.h"
#include "saturn/saturn_animation_ids.h"
#include "saturn/saturn_animations.h"
#include "saturn/saturn_obj_def.h"
#include "saturn_imgui.h"
#include "saturn/imgui/saturn_imgui_chroma.h"
#include "pc/controller/controller_keyboard.h"
#include <SDL2/SDL.h>

#include "icons/IconsForkAwesome.h"

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
#include "src/game/interaction.h"
#include "include/behavior_data.h"
#include "game/object_helpers.h"
}

using namespace std;

int custom_anim_index = -1;
int current_sanim_index = 7;
std::string current_sanim_name = "RUNNING";
int current_sanim_id = MARIO_ANIM_RUNNING;
std::map<std::pair<int, std::string>, int> current_anim_map = sanim_movement;
std::string anim_preview_name = "RUNNING";
int current_sanim_group_index = 0;
int current_slevel_index = 1;
Vec3f obj_pos;
int obj_rot[3];
int obj_beh_params[4];
int obj_model;
int obj_beh;

s16 levelList[] = { 
    LEVEL_SA, LEVEL_CASTLE_GROUNDS, LEVEL_CASTLE, LEVEL_CASTLE_COURTYARD, LEVEL_BOB, 
    LEVEL_WF, LEVEL_PSS, LEVEL_TOTWC, LEVEL_JRB, LEVEL_CCM,
    LEVEL_BITDW, LEVEL_BBH, LEVEL_HMC, LEVEL_COTMC, LEVEL_LLL,
    LEVEL_SSL, LEVEL_VCUTM, LEVEL_DDD, LEVEL_BITFS, 
    LEVEL_SL, LEVEL_WDW, LEVEL_TTM, LEVEL_THI,
    LEVEL_TTC, LEVEL_WMOTR, LEVEL_RR, LEVEL_BITS
};

int current_level_sel = 0;
void warp_to(s16 destLevel, s16 destArea = 0x01, s16 destWarpNode = 0x0A) {
    if (!mario_exists)
        return;

    if (destLevel == gCurrLevelNum && destArea == gCurrAreaIndex) {
        if (current_slevel_index < 4)
            return;
            
        DynOS_Warp_ToLevel(gCurrLevelNum, gCurrAreaIndex, gCurrActNum);
    }

    initiate_warp(destLevel, destArea, destWarpNode, 0);
    fade_into_special_warp(0,0);
}

void anim_play_button() {
    current_anim_frame = 0;
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
                if (accept_text_input) {
                    if (event->key.keysym.sym >= SDLK_0 && event->key.keysym.sym <= SDLK_9) {
                        saturn_load_expression_number(event->key.keysym.sym);
                    }
                };
            }

        case SDL_MOUSEMOTION:
            SDL_Delay(2);
            camera_view_move_x = event->motion.xrel;
            camera_view_move_y = event->motion.yrel;
        
        break;
    }
}

void warp_to_level(int level, int area, int act = -1) {
    is_anim_playing = false;
    is_anim_paused = false;

    if (level != 0) enable_shadows = true;
    else enable_shadows = false;

    s32 levelID = levelList[level];
    s32 warpnode = 0x0A;

    switch (level) {
        case 1:
            warpnode = 0x04;
            break;
        case 2:
            warpnode = 0x01;
            break;
        case 3:
            warpnode = 0x0B;
            break;
    }

    if (act == -1) warp_to(levelID, area, warpnode);
    else DynOS_Warp_ToWarpNode(levelID, area, act, warpnode);
}

int get_saturn_level_id(int level) {
    for (int i = 0; i < IM_ARRAYSIZE(levelList); i++) {
        if (levelList[i] == level) return i;
    }
}

void smachinima_imgui_init() {
    Cheats.EnableCheats = true;
    Cheats.GodMode = true;
    Cheats.ExitAnywhere = true;
    saturn_load_anim_folder("", &custom_anim_index);
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

    ImGui::SliderFloat("FOV", &camera_fov, 0.0f, 100.0f);
    imgui_bundled_tooltip("Controls the FOV of the in-game camera. Default is 50.\nKeybind -> N/M");
    ImGui::Checkbox("Smooth###smooth_fov", &camera_fov_smooth);

    if (mario_exists) {
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Text("Animations");
        ImGui::Dummy(ImVec2(0, 5));

        // Warp To Level

        ImGui::Dummy(ImVec2(0, 5));
    }

    ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::BeginTable("table_quick_toggles", 2)) {
        ImGui::TableNextColumn();
        ImGui::Checkbox("HUD", &configHUD);
        imgui_bundled_tooltip("Controls the in-game HUD visibility.");
        ImGui::TableNextColumn();
        ImGui::Checkbox("Shadows", &enable_shadows);
        imgui_bundled_tooltip("Displays the shadows of various objects.");
        ImGui::TableNextColumn();
        ImGui::Checkbox("Invulnerability", (bool*)&enable_immunity);
        imgui_bundled_tooltip("If enabled, Mario will be invulnerable to most enemies and hazards.");
        ImGui::TableNextColumn();
        ImGui::Checkbox("NPC Dialogue", (bool*)&enable_dialogue);
        imgui_bundled_tooltip("Whether or not to trigger dialogue when interacting with an NPC or readable sign.");
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

void imgui_machinima_quick_options() {
    if (ImGui::MenuItem(ICON_FK_CLOCK_O " Limit FPS",      "F4", limit_fps)) {
        limit_fps = !limit_fps;
        configWindow.fps_changed = true;
    }

    if (mario_exists) {
        if (ImGui::MenuItem(ICON_FK_PAPER_PLANE_O " Fly Mode",      "F2", gMarioState->action == ACT_DEBUG_FREE_MOVE, gMarioState->action == ACT_IDLE | gMarioState->action == ACT_DEBUG_FREE_MOVE)) {
            if (gMarioState->action == ACT_IDLE) set_mario_action(gMarioState, ACT_DEBUG_FREE_MOVE, 0);
            else set_mario_action(gMarioState, ACT_IDLE, 0);
        }
        ImGui::Separator();

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

        if (ImGui::Button("Warp to Level")) {
            autoChroma = false;
            warp_to_level(current_slevel_index, (s32)currentChromaArea, -1);

            // Erase existing timelines
            k_frame_keys.clear();
        }
    }
    if (mario_exists) {
        ImGui::Separator();
        if (ImGui::BeginMenu("Spawn Object")) {
            ImGui::Text("Position");
            ImGui::SameLine();
            ImGui::InputFloat3("###obj_set_pos", (float*)&obj_pos);
            ImGui::Text("Rotation");
            ImGui::SameLine();
            ImGui::InputInt3("###obj_set_rot", (int*)&obj_rot);
            if (ImGui::Button("Copy Mario")) {
                vec3f_copy(obj_pos, gMarioState->pos);
                obj_rot[0] = gMarioState->faceAngle[0];
                obj_rot[1] = gMarioState->faceAngle[1];
                obj_rot[2] = gMarioState->faceAngle[2];
            }
            ImGui::Separator();
            ImGui::Text("Model");
            if (ImGui::BeginCombo("###obj_model", obj_models[obj_model].first.c_str())) {
                for (int i = 0; i < IM_ARRAYSIZE(obj_models); i++) {
                    bool selected = obj_model == i;
                    if (ImGui::Selectable(obj_models[i].first.c_str())) obj_model = i;
                    if (selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Behavior");
            if (ImGui::BeginCombo("###obj_beh", obj_behaviors[obj_beh].first.c_str())) {
                for (int i = 0; i < IM_ARRAYSIZE(obj_behaviors); i++) {
                    bool selected = obj_beh == i;
                    if (ImGui::Selectable(obj_behaviors[i].first.c_str())) obj_beh = i;
                    if (selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Behavior Parameters");
            ImGui::InputInt4("###obj_beh_params", obj_beh_params);
            ImGui::Separator();
            if (ImGui::Button("Spawn Object")) {
                Object* obj = spawn_object(gMarioState->marioObj, obj_models[obj_model].second, obj_behaviors[obj_beh].second);
                obj->oPosX = obj_pos[0];
                obj->oPosY = obj_pos[1];
                obj->oPosZ = obj_pos[2];
                obj->oHomeX = obj_pos[0];
                obj->oHomeY = obj_pos[1];
                obj->oHomeZ = obj_pos[2];
                obj->oFaceAnglePitch = obj_rot[0];
                obj->oFaceAngleYaw = obj_rot[1];
                obj->oFaceAngleRoll = obj_rot[2];
                obj->oBehParams = ((obj_beh_params[0] & 0xFF) << 24) | ((obj_beh_params[1] & 0xFF) << 16) | ((obj_beh_params[2] & 0xFF) << 8) | (obj_beh_params[3] & 0xFF);
            }
            ImGui::EndMenu();
        }
    }
    ImGui::Separator();
    ImGui::Checkbox("HUD", &configHUD);
    imgui_bundled_tooltip("Controls the in-game HUD visibility.");
    saturn_keyframe_bool_popout(&configHUD, "HUD", "k_hud");
    ImGui::Checkbox("Shadows", &enable_shadows);
    imgui_bundled_tooltip("Displays the shadows of various objects.");
    saturn_keyframe_bool_popout(&enable_shadows, "Shadows", "k_shadows");
    ImGui::Checkbox("Invulnerability", (bool*)&enable_immunity);
    imgui_bundled_tooltip("If enabled, Mario will be invulnerable to most enemies and hazards.");
    ImGui::Checkbox("NPC Dialogue", (bool*)&enable_dialogue);
    imgui_bundled_tooltip("Whether or not to trigger dialogue when interacting with an NPC or readable sign.");
    if (mario_exists) {
        if (gMarioState->action == ACT_IDLE) {
            if (ImGui::Button("Sleep")) {
                set_mario_action(gMarioState, ACT_START_SLEEPING, 0);
            }
        }
        ImGui::Separator();
        const char* mEnvSettings[] = { "Default", "None", "Snow", "Blizzard" };
        ImGui::PushItemWidth(100);
        ImGui::Combo("Environment###env_dropdown", (int*)&gLevelEnv, mEnvSettings, IM_ARRAYSIZE(mEnvSettings));
        ImGui::PopItemWidth();
    }
}

static char animSearchTerm[128];

void imgui_machinima_animation_player() {
    selected_animation = (MarioAnimID)current_sanim_id;
    
    if (is_anim_playing)
        ImGui::BeginDisabled();

    const char* anim_groups[] = { "Movement (50)", "Actions (25)", "Automatic (27)", "Damage/Deaths (22)",
        "Cutscenes (23)", "Water (16)", "Climbing (20)", "Object (24)", "CUSTOM..." };
    int animArraySize = (canim_array.size() > 0) ? IM_ARRAYSIZE(anim_groups) : IM_ARRAYSIZE(anim_groups) - 1;

    ImGui::PushItemWidth(290);
    if (ImGui::BeginCombo("###anim_group", anim_groups[current_sanim_group_index], ImGuiComboFlags_HeightLarge)) {
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
                    current_sanim_id = MARIO_ANIM_A_POSE;

                    current_sanim_name = canim_array[custom_anim_index];
                    anim_preview_name = current_sanim_name;
                    anim_preview_name = anim_preview_name.substr(0, anim_preview_name.size() - 5);
                    saturn_read_mcomp_animation(anim_preview_name);
                    is_anim_looped = current_canim_looping;
                }
            }

            if (n == 8) {
                ImGui::SameLine();
                imgui_bundled_help_marker("These are custom METAL Composer+ JSON animations.\nPlace in dynos/anims.");
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (current_sanim_group_index == 8 && canim_array.size() >= 20) {
        ImGui::InputTextWithHint("###anim_search_text", ICON_FK_SEARCH " Search animations...", animSearchTerm, IM_ARRAYSIZE(animSearchTerm), ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::IsItemActivated()) accept_text_input = false;
        if (ImGui::IsItemDeactivated()) accept_text_input = true;
    } else {
        // If our anim list is reloaded, and we now have less than 20 anims, this can cause filter issues if not reset to nothing
        if (animSearchTerm != "") strcpy(animSearchTerm, "");
    }
    string animSearchLower = animSearchTerm;
    std::transform(animSearchLower.begin(), animSearchLower.end(), animSearchLower.begin(),
        [](unsigned char c){ return std::tolower(c); });

    ImGui::PopItemWidth();

    ImGui::BeginChild("###anim_box_child", ImVec2(290, 100), true);
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
            if (canim_array[i].find("/") != string::npos)
                current_sanim_name = ICON_FK_FOLDER_O " " + canim_array[i];

            const bool is_selected = (custom_anim_index == i);

            // If we're searching, only include anims with the search keyword in the name
            // Also convert to lowercase
            if (animSearchLower != "") {
                string nameLower = current_sanim_name;
                std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                    [](unsigned char c1){ return std::tolower(c1); });

                if (nameLower.find(animSearchLower) == string::npos) {
                    continue;
                }
            }

            if (ImGui::Selectable(current_sanim_name.c_str(), is_selected)) {
                custom_anim_index = i;
                current_sanim_name = canim_array[i];
                anim_preview_name = current_sanim_name;
                is_anim_looped = current_canim_looping;
                // Remove .json extension
                if (canim_array[i].find(".json") != string::npos) {
                    anim_preview_name = anim_preview_name.substr(0, anim_preview_name.size() - 5);
                    saturn_read_mcomp_animation(anim_preview_name);
                    is_anim_looped = current_canim_looping;
                } else if (canim_array[i].find("/") != string::npos) {
                    saturn_load_anim_folder(anim_preview_name, &custom_anim_index);
                    current_sanim_name = canim_array[custom_anim_index];
                    anim_preview_name = current_sanim_name;
                    anim_preview_name = anim_preview_name.substr(0, anim_preview_name.size() - 5);
                    saturn_read_mcomp_animation(anim_preview_name);
                    is_anim_looped = current_canim_looping;
                }
                // Stop anim
                is_anim_playing = false;
                is_anim_paused = false;
                using_chainer = false;
                chainer_index = 0;
            }

            if (ImGui::BeginPopupContextItem()) {
                ImGui::Text(canim_array[i].c_str());
                imgui_bundled_tooltip((current_anim_dir_path + canim_array[i]).c_str());
                ImGui::Separator();
                ImGui::TextDisabled("%i MComp+ animation(s)", canim_array.size());
                if (ImGui::Button("Refresh###refresh_canim")) {
                    saturn_load_anim_folder(current_anim_dir_path, &custom_anim_index);
                    current_sanim_name = canim_array[custom_anim_index];
                    anim_preview_name = current_sanim_name;
                    anim_preview_name = anim_preview_name.substr(0, anim_preview_name.size() - 5);
                    saturn_read_mcomp_animation(anim_preview_name);
                    is_anim_looped = current_canim_looping;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
    }
    ImGui::EndChild();

    if (is_anim_playing)
        ImGui::EndDisabled();

    ImGui::PushItemWidth(290);
    ImGui::Separator();

    // Metadata
    if (is_custom_anim && custom_anim_index != -1) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("###anim_metadata", ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 3), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::Text(current_canim_name.c_str());
        ImGui::TextDisabled(("@ " + current_canim_author).c_str());
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    ImGui::PopItemWidth();

    // Player
    if (is_anim_playing) {
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
        
        ImGui::PushItemWidth(150);
        ImGui::SliderInt("Frame###animation_frames", &current_anim_frame, 0, current_anim_length);
        ImGui::PopItemWidth();
        ImGui::Checkbox("Paused###animation_paused", &is_anim_paused);
    } else {
        ImGui::Text("");
        if (ImGui::Button("Play")) {
            anim_play_button();
        } ImGui::SameLine(); ImGui::Checkbox("Loop", &is_anim_looped);

        ImGui::PushItemWidth(150);
        ImGui::SliderFloat("Speed###anim_speed", &anim_speed, 0.1f, 2.0f);
        ImGui::PopItemWidth();
        if (anim_speed != 1.0f) {
            if (ImGui::Button("Reset###reset_anim_speed"))
                anim_speed = 1.0f;
        }
    }
}
