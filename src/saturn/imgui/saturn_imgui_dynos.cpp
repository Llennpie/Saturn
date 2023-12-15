#include "saturn_imgui_dynos.h"

#include "saturn/libs/portable-file-dialogs.h"

#include <algorithm>
#include <string>
#include <cstring>
#include <iostream>
#include "GL/glew.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <cstdlib>
#endif

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/libs/imgui/imgui-knobs.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/saturn_models.h"
#include "saturn/saturn_textures.h"
#include "saturn_imgui.h"
#include "saturn_imgui_cc_editor.h"
#include "saturn_imgui_expressions.h"
#include "pc/controller/controller_keyboard.h"
#include "data/dynos.cpp.h"
#include <SDL2/SDL.h>

#include "icons/IconsForkAwesome.h"

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "pc/platform.h"
#include "game/mario.h"
#include "game/level_update.h"
#include <mario_animation_ids.h>
#include "pc/cheats.h"
#include "include/sm64.h"
}

Array<PackData *> &sDynosPacks = DynOS_Gfx_GetPacks();

using namespace std;

int current_eye_state = 0;
int current_eye_id = 0;
string eye_name;
int current_mouth_id = 0;
string mouth_name;

int windowListSize = 325;

bool using_custom_eyes;

static char modelSearchTerm[128];

bool has_copy_mario;

bool last_model_had_model_eyes;

bool is_gameshark_open;

std::vector<std::string> choose_file_dialog(std::string windowTitle, std::vector<std::string> filetypes, bool multiselect) {
    return pfd::open_file(windowTitle, ".", filetypes, multiselect ? pfd::opt::multiselect : pfd::opt::none).result();
}

void open_directory(std::string path) {
#if defined(_WIN32) // Windows
    ShellExecuteA(NULL, "open", ("\"" + path + "\"").c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__) // macOS
    system(("open \"" + path + "\"").c_str());
#else // Linux
    system(("xdg-open \"" + path + "\"").c_str());
#endif
}

// UI

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

GLuint global_expression_preview = 0;
int global_expression_preview_width = 0;
int global_expression_preview_height = 0;
int global_sample_ratio;
string last_preview_name;

void expression_preview(const char* filename) {
    global_expression_preview = 0;
    bool ret = LoadTextureFromFile(filename, &global_expression_preview, &global_expression_preview_width, &global_expression_preview_height);
    IM_ASSERT(ret);
    global_sample_ratio = global_expression_preview_width / global_expression_preview_height;
    //if (global_expression_preview_height > 16 && global_expression_preview_width > 16) {
    //    global_sample_ratio = (256 / global_expression_preview_height);
    //}
}

void OpenModelSelector() {
    ImGui::Text("Model Packs");
    ImGui::SameLine(); imgui_bundled_help_marker(
        "DynOS v1.1 by PeachyPeach\n\nThese are DynOS model packs, used for live model loading.\nPlace packs in /dynos/packs.");

    // Model search when our list is 20 or more
    if (model_list.size() >= 20) {
        ImGui::InputTextWithHint("###model_search_text", ICON_FK_SEARCH " Search models...", modelSearchTerm, IM_ARRAYSIZE(modelSearchTerm), ImGuiInputTextFlags_AutoSelectAll);
    } else {
        // If our model list is reloaded, and we now have less than 20 packs, this can cause filter issues if not reset to nothing
        if (modelSearchTerm != "") strcpy(modelSearchTerm, "");
    }
    std::string modelSearchLower = modelSearchTerm;
    std::transform(modelSearchLower.begin(), modelSearchLower.end(), modelSearchLower.begin(),
        [](unsigned char c){ return std::tolower(c); });

    if (model_list.size() <= 0) {
        ImGui::TextDisabled("No model packs found.\nPlace model folders in\n/dynos/packs/.");
    } else {
        ImGui::BeginChild("###menu_model_selector", ImVec2(-FLT_MIN, 125), true);
        for (int i = 0; i < model_list.size(); i++) {
            Model model = model_list[i];
            if (model.Active) {
                bool is_selected = DynOS_Opt_GetValue(String("dynos_pack_%d", i));

                // If we're searching, only include CCs with the search keyword in the name
                // Also convert to lowercase
                std::string label_name_lower = model.SearchMeta();
                std::transform(label_name_lower.begin(), label_name_lower.end(), label_name_lower.begin(),
                        [](unsigned char c1){ return std::tolower(c1); });
                if (modelSearchLower != "") {
                    if (label_name_lower.find(modelSearchLower) == std::string::npos) {
                        continue;
                    }
                }

                std::string packLabelId = model.FolderName + "###s_model_pack_" + std::to_string(i);
                if (ImGui::Selectable(packLabelId.c_str(), &is_selected)) {
                    // Deselect other packs, but LSHIFT allows additive
                    for (int j = 0; j < sDynosPacks.Count(); j++) {
                        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT] == false)
                            DynOS_Opt_SetValue(String("dynos_pack_%d", j), false);
                    }

                    if (saturn_timeline_exists("k_mario_expr")) k_frame_keys.erase("k_mario_expr");
                    
                    // Select model
                    DynOS_Opt_SetValue(String("dynos_pack_%d", i), is_selected);
                    current_model = model;
                    current_model_id = i;

                    // Load expressions
                    current_model.Expressions.clear();
                    current_model.Expressions = LoadExpressions(current_model.FolderPath);

                    if (is_selected) {
                        std::cout << "Loaded " << model.Name << " by " << model.Author << std::endl;

                        // Load model CCs
                        RefreshColorCodeList();
                        if (is_selected && (current_color_code.IsDefaultColors() || last_model_cc_address == current_color_code.GameShark)) {
                            ResetColorCode(true);
                            last_model_cc_address = current_color_code.GameShark;
                        }
                    } else {
                        if (!AnyModelsEnabled()) {
                            current_model = Model();
                            current_model_id = -1;
                        }
                        
                        // Reset model CCs
                        model_color_code_list.clear();
                        RefreshColorCodeList();
                        if (last_model_cc_address == current_color_code.GameShark)
                            ResetColorCode(false);
                    }
                }
                std::string popupLabelId = "###menu_model_popup_" + std::to_string(i);
                if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                    ImGui::OpenPopup(popupLabelId.c_str());
                if (ImGui::BeginPopup(popupLabelId.c_str())) {
                    // Right-click menu
                    if (model.Author.empty()) ImGui::Text(ICON_FK_FOLDER_O " %s/", model.FolderName.c_str());
                    else ImGui::Text(ICON_FK_FOLDER " %s/", model.FolderName.c_str());

                    imgui_bundled_tooltip(("/%s", model.FolderPath).c_str());
                    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                        open_directory(std::string(sys_exe_path()) + "/" + model.FolderPath + "/");

                    ImGui::SameLine(); ImGui::TextDisabled(" Pack #%i", model.DynOSId + 1);

                    std::vector<std::string> cc_list = GetColorCodeList(model.FolderPath + "/colorcodes");
                    has_open_any_model_cc = true;

                    if (model.HasColorCodeFolder() && cc_list.size() > 0) {
                        // List of model color codes
                        ImGui::BeginChild("###menu_model_ccs", ImVec2(-FLT_MIN, 75), true);
                        OpenModelCCSelector(model, cc_list, "");
                        ImGui::EndChild();
                    }

                    ImGui::Separator();
                    ImGui::TextDisabled("%i model pack(s)", model_list.size());
                    if (ImGui::Button(ICON_FK_DOWNLOAD " Refresh Packs###refresh_dynos_packs")) {
                        sDynosPacks.Clear();
                        DynOS_Opt_Init();
                        model_list = GetModelList("dynos/packs");
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine(); imgui_bundled_help_marker("WARNING: Experimental - this will probably lag the game.");
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::EndChild();

        // Open DynOS Folder Button

        if (ImGui::Button(ICON_FK_FOLDER_OPEN_O " Open Packs Folder...###open_packs_folder"))
            open_directory(std::string(sys_exe_path()) + "/dynos/packs/");
    }
}



void sdynos_imgui_init() {
    model_list = GetModelList("dynos/packs");
    RefreshColorCodeList();

    LoadEyesFolder();

    //model_details = "" + std::to_string(sDynosPacks.Count()) + " model pack";
    //if (sDynosPacks.Count() != 1) model_details += "s";
}

void sdynos_imgui_menu() {
    if (ImGui::BeginMenu(ICON_FK_USER_CIRCLE " Edit Avatar###menu_edit_avatar")) {
        // Color Code Selection
        if (!support_color_codes || !current_model.ColorCodeSupport) ImGui::BeginDisabled();
            OpenCCSelector();
            if (ImGui::Button(ICON_FK_UNDO " Reset CC###reset_cc_button")) {
                ApplyColorCode(ColorCode());
            }
            // Open File Dialog
            if (ImGui::Button(ICON_FK_FILE_TEXT_O " Open CC Folder...###open_cc_folder"))
                open_directory(std::string(sys_exe_path()) + "/dynos/colorcodes/");
        if (!support_color_codes || !current_model.ColorCodeSupport) ImGui::EndDisabled();

        // Model Selection
        OpenModelSelector();

        ImGui::EndMenu();
    }

    // Color Code Editor
    if (ImGui::BeginMenu(ICON_FK_PAINT_BRUSH " Color Code Editor###menu_cc_editor")) {
        OpenCCEditor();
        ImGui::EndMenu();
    }

    /*if (ImGui::MenuItem(ICON_FK_PAINT_BRUSH " Color Code Editor###menu_cc_editor", NULL, windowCcEditor, support_color_codes & current_model.ColorCodeSupport)) {
        if (support_color_codes && current_model.ColorCodeSupport) {
            windowAnimPlayer = false;
            windowChromaKey = false;
            windowCcEditor = !windowCcEditor;
        }
    }*/

    // Animation Mixtape
    if (ImGui::MenuItem(ICON_FK_FILM " Animation Mixtape###menu_anim_player", NULL, windowAnimPlayer, mario_exists)) {
        windowCcEditor = false;
        windowChromaKey = false;
        windowAnimPlayer = !windowAnimPlayer;
    }

    ImGui::Separator();

    if (!current_model.ColorCodeSupport) ImGui::BeginDisabled();
        ImGui::Checkbox("Color Code Support", &support_color_codes);
        imgui_bundled_tooltip(
            "Toggles color code features.");

        if (!support_color_codes) ImGui::BeginDisabled();
            if (current_model.SparkSupport) {
                ImGui::Checkbox("CometSPARK Support", &support_spark);
                imgui_bundled_tooltip(
                    "Toggles SPARK features, which provides supported models with extra color values.");
            }
        if (!support_color_codes) ImGui::EndDisabled();
    if (!current_model.ColorCodeSupport) ImGui::EndDisabled();

    // Misc. Avatar Settings
    if (ImGui::BeginMenu("Customize...###menu_misc")) {

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("Misc.###misc_child", ImVec2(225, 175), true, ImGuiWindowFlags_None);
        if (ImGui::BeginTabBar("###misc_tabbar", ImGuiTabBarFlags_None)) {

            if (ImGui::BeginTabItem("Switches###switches_scale")) {
                const char* eyes[] = { "Blinking", "Open", "Half", "Closed", ICON_FK_EYE_SLASH " Custom A", ICON_FK_EYE_SLASH " Custom B", ICON_FK_EYE_SLASH " Custom C", ICON_FK_EYE_SLASH " Custom D", "Dead" };
                if (ImGui::Combo("Eyes###eye_state", &scrollEyeState, eyes, IM_ARRAYSIZE(eyes))) {
                    if (scrollEyeState < 4) custom_eyes_enabled = false;
                    else custom_eyes_enabled = true;
                }

                const char* hands[] = { "Fists", "Open", "Peace", "With Cap", "With Wing Cap", "Right Open" };
                ImGui::Combo("Hand###hand_state", &scrollHandState, hands, IM_ARRAYSIZE(hands));
                const char* caps[] = { "Cap On", "Cap Off", "Wing Cap" }; // unused "wing cap off" not included
                ImGui::Combo("Cap###cap_state", &scrollCapState, caps, IM_ARRAYSIZE(caps));
                const char* powerups[] = { "Default", "Metal", "Vanish", "Metal & Vanish" };
                ImGui::Combo("Powerup###powerup_state", &saturnModelState, powerups, IM_ARRAYSIZE(powerups));
                if (AnyModelsEnabled()) ImGui::BeginDisabled();
                ImGui::Checkbox("M Cap Emblem", &show_vmario_emblem);
                imgui_bundled_tooltip("Enables the signature \"M\" logo on Mario's cap.");
                saturn_keyframe_popout("k_v_cap_emblem");
                if (AnyModelsEnabled()) ImGui::EndDisabled();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shading###tab_shading")) {
                ImGui::SliderFloat("X###wdir_x", &world_light_dir1, -2.f, 2.f);
                saturn_keyframe_popout("k_shade_x");
                ImGui::SliderFloat("Y###wdir_y", &world_light_dir2, -2.f, 2.f);
                saturn_keyframe_popout("k_shade_y");
                ImGui::SliderFloat("Z###wdir_z", &world_light_dir3, -2.f, 2.f);
                saturn_keyframe_popout("k_shade_z");
                ImGui::SliderFloat("Tex###wdir_tex", &world_light_dir4, 1.f, 4.f);
                saturn_keyframe_popout("k_shade_t");

                ImGui::ColorEdit4("Col###wlight_col", gLightingColor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
                
                if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                    ImGui::OpenPopup("###texColColorPresets");

                if (ImGui::BeginPopup("###texColColorPresets")) {
                    if (ImGui::Selectable(ICON_FK_UNDO " Reset")) {
                        gLightingColor[0] = 1.f;
                        gLightingColor[1] = 1.f;
                        gLightingColor[2] = 1.f;
                    }
                    if (ImGui::Selectable("Randomize")) {
                        gLightingColor[0] = (rand() % 255) / 255.0f;
                        gLightingColor[1] = (rand() % 255) / 255.0f;
                        gLightingColor[2] = (rand() % 255) / 255.0f;
                    }
                    ImGui::EndPopup();
                }

                ImGui::SameLine(); ImGui::Text("Col");
                saturn_keyframe_popout("k_light_col");

                if (world_light_dir1 != 0.f || world_light_dir2 != 0.f || world_light_dir3 != 0.f || world_light_dir4 != 1.f) {
                    if (ImGui::Button("Reset###reset_wshading")) {
                        world_light_dir1 = 0.f;
                        world_light_dir2 = 0.f;
                        world_light_dir3 = 0.f;
                        world_light_dir4 = 1.f;
                        gLightingColor[0] = 1.f;
                        gLightingColor[1] = 1.f;
                        gLightingColor[2] = 1.f;
                    }
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Special###tab_special")) {
                if (linkMarioScale) {
                    ImGui::SliderFloat("Size###mscale_all", &marioScaleSizeX, 0.f, 2.f);
                    saturn_keyframe_popout("k_scale");
                } else {
                    ImGui::SliderFloat("X###mscale_x", &marioScaleSizeX, -2.f, 2.f);
                    saturn_keyframe_popout("k_scale_x");
                    ImGui::SliderFloat("Y###mscale_y", &marioScaleSizeY, -2.f, 2.f);
                    saturn_keyframe_popout("k_scale_y");
                    ImGui::SliderFloat("Z###mscale_z", &marioScaleSizeZ, -2.f, 2.f);
                    saturn_keyframe_popout("k_scale_z");
                }
                ImGui::Checkbox("Link###link_mario_scale", &linkMarioScale);
                if (marioScaleSizeX != 1.f || marioScaleSizeY != 1.f || marioScaleSizeZ != 1.f) {
                    ImGui::SameLine(); if (ImGui::Button("Reset###reset_mscale")) {
                        marioScaleSizeX = 1.f;
                        marioScaleSizeY = 1.f;
                        marioScaleSizeZ = 1.f;
                    }
                }

                if (mario_exists) {
                    ImGui::Dummy(ImVec2(0, 5));
                    ImGui::Text("Position");
                    ImGui::InputFloat3("###mario_set_pos", (float*)&gMarioState->pos);
                    if (ImGui::Button(ICON_FK_FILES_O " Copy###copy_mario")) {
                        vec3f_copy(stored_mario_pos, gMarioState->pos);
                        vec3s_copy(stored_mario_angle, gMarioState->faceAngle);
                        has_copy_mario = true;
                    } ImGui::SameLine();
                    if (!has_copy_mario) ImGui::BeginDisabled();
                    if (ImGui::Button(ICON_FK_CLIPBOARD " Paste###paste_mario")) {
                        if (has_copy_mario) {
                            vec3f_copy(gMarioState->pos, stored_mario_pos);
                            vec3f_copy(gMarioState->marioObj->header.gfx.pos, stored_mario_pos);
                            vec3s_copy(gMarioState->faceAngle, stored_mario_angle);
                            vec3s_set(gMarioState->marioObj->header.gfx.angle, 0, stored_mario_angle[1], 0);
                        }
                    }
                    if (!has_copy_mario) ImGui::EndDisabled();
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();

        if (ImGui::BeginTable("misc_table", 2)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Checkbox("Dust Particles", &enable_dust_particles);
            imgui_bundled_tooltip("Displays dust particles when Mario moves.");
            ImGui::Checkbox("Torso Rotations", &enable_torso_rotation);
            imgui_bundled_tooltip("Tilts Mario's torso when he moves; Disable for a \"beta running\" effect.");
            ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine(25); ImGui::Text(ICON_FK_SKATE " Walkpoint");
            ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine(25); ImGui::SliderFloat("###run_speed", &run_speed, 0.f, 127.f, "%.0f");
            imgui_bundled_tooltip("Controls the axis range for Mario's running input; Can be used to force walking (36) or tiptoe (25) animations.");

            ImGui::TableSetColumnIndex(1);
            if (gMarioState) {
                if (ImGuiKnobs::Knob("Angle", &this_face_angle, -180.f, 180.f, 0.f, "%.0f deg", ImGuiKnobVariant_Dot, 0.f, ImGuiKnobFlags_DragHorizontal)) {
                    gMarioState->faceAngle[1] = (s16)(this_face_angle * 182.04f);
                } else if (!k_popout_focused || keyframe_playing) {
                    this_face_angle = (float)gMarioState->faceAngle[1] / 182.04;
                }
                saturn_keyframe_popout("k_angle");
            }

            ImGui::Checkbox("Spin###spin_angle", &is_spinning);
            if (is_spinning) {
                ImGui::SliderFloat("Speed###spin,speed", &spin_mult, -2.f, 2.f, "%.1f");
            }

            if (this_face_angle > 180) this_face_angle = -180;
            if (this_face_angle < -180) this_face_angle = 180;

            ImGui::EndTable();
        }
        if (mario_exists) if (ImGui::BeginMenu("Head Rotations")) {
            ImGui::Checkbox("Enable", &enable_head_rotations);
            imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
            saturn_keyframe_popout("k_head_rot");
            ImGui::Separator();
            ImGui::Text("C-Up Settings");
            if (gMarioState->action != ACT_FIRST_PERSON) ImGui::BeginDisabled();
            ImGui::PushItemWidth(75);
            ImGui::SliderFloat("Speed", &mario_headrot_speed, 0, 50, "%.1f");
            ImGui::PopItemWidth();
            if (ImGui::BeginTable("headrot_table", 2)) {
                float fake_yaw = mario_headrot_yaw * 360 / 65536;
                float fake_pitch = mario_headrot_pitch * 360 / 65536;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGuiKnobs::Knob("Yaw", &fake_yaw, configCUpLimit ? -120.0f : -180.0f, configCUpLimit ? 120.0f : 180.0f, 0.0f, "%.0f deg", ImGuiKnobVariant_Dot, 0.f, ImGuiKnobFlags_DragHorizontal)) {
                    mario_headrot_yaw = fake_yaw * 65536 / 360;
                }
                ImGui::TableSetColumnIndex(1);
                if (ImGuiKnobs::Knob("Pitch", &fake_pitch, configCUpLimit ? -45.0f : -180.0f, configCUpLimit ? 80.0f : 180.0f, 0.0f, "%.0f deg", ImGuiKnobVariant_Dot, 0.f, ImGuiKnobFlags_DragHorizontal)) {
                    mario_headrot_pitch = fake_pitch * 65536 / 360;
                }
                ImGui::EndTable();
            }
            saturn_keyframe_popout("k_mario_headrot");
            if (gMarioState->action != ACT_FIRST_PERSON) ImGui::EndDisabled();
            ImGui::EndMenu();
        }

        ImGui::PopStyleVar();
        ImGui::EndMenu();
    }

    ImGui::Separator();

    // Model Metadata
    if (!current_model.Author.empty()) {
        string metaLabelText = (ICON_FK_USER " " + current_model.Name);
        string metaDataText = "v" + current_model.Version;
        if (current_model.Description != "")
            metaDataText = "v" + current_model.Version + "\n" + current_model.Description;

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("###model_metadata", ImVec2(0, 45), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::Text(metaLabelText.c_str()); imgui_bundled_tooltip(metaDataText.c_str());
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            open_directory(std::string(sys_exe_path()) + "/" + current_model.FolderPath + "/");
        ImGui::TextDisabled(("@ " + current_model.Author).c_str());
        ImGui::EndChild();
        ImGui::PopStyleVar();

        ImGui::Separator();
    }

    if (current_model.CustomEyeSupport) {
        // Custom Eyes Checkbox
        if (ImGui::Checkbox("Custom Eyes", &custom_eyes_enabled)) {
            if (scrollEyeState < 4 && custom_eyes_enabled) scrollEyeState = 4;
            else scrollEyeState = 0;
        }
        if (!AnyModelsEnabled())
            imgui_bundled_tooltip("Place custom eye PNG textures in /dynos/eyes/.");

        if ((custom_eyes_enabled && current_model.Expressions.size() > 0) || current_model.Expressions.size() > 1)
            ImGui::Separator();

        // Expressions Selector
        OpenExpressionSelector();

        // Keyframing
        if (custom_eyes_enabled ||
            current_model.Expressions.size() > 0)
                saturn_keyframe_popout_next_line("k_mario_expr");
    }
}