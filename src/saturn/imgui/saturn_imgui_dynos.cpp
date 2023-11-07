#include "saturn_imgui_dynos.h"

#include "saturn/libs/portable-file-dialogs.h"

#include <algorithm>
#include <string>
#include <cstring>
#include <iostream>
#include "GL/glew.h"

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/libs/imgui/imgui-knobs.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/saturn_textures.h"
#include "saturn_imgui.h"
#include "saturn_imgui_cc_editor.h"
#include "pc/controller/controller_keyboard.h"
#include "data/dynos.cpp.h"
#include <SDL2/SDL.h>

#include "icons/IconsForkAwesome.h"

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/level_update.h"
#include <mario_animation_ids.h>
#include "pc/cheats.h"
}

Array<PackData *> &sDynosPacks = DynOS_Gfx_GetPacks();

using namespace std;

int current_eye_state = 0;
int current_eye_id = 0;
string eye_name;
int current_mouth_id = 0;
string mouth_name;

string ui_mfolder_name;
string ui_mfolder_path;
bool one_pack_selectable;
bool any_packs_selected;
int windowListSize = 325;

bool using_custom_eyes;

static char modelSearchTerm[128];

bool has_copy_mario;

bool last_model_had_model_eyes;

bool is_gameshark_open;

std::vector<std::string> choose_file_dialog(std::string windowTitle, std::vector<std::string> filetypes, bool multiselect) {
    return pfd::open_file(windowTitle, ".", filetypes, multiselect ? pfd::opt::multiselect : pfd::opt::none).result();
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

void blink_cycle_preview(std::string label, int index, std::string eye_path, int eye_index) {
    string preview_path;
    if (index == 1) preview_path = blink_eye_1;
    else if (index == 2) preview_path = blink_eye_2;
    else if (index == 3) preview_path = blink_eye_3;
    if (preview_path != "") preview_path = preview_path.substr(6, preview_path.size()) + ".png";
    
    if (ImGui::SmallButton(label.c_str())) {
        saturn_set_model_blink_eye(eye_index, index, eye_path);
    }

    if (ImGui::IsItemHovered() && preview_path != "") {
        ImGui::BeginTooltip();
        // ImageTest
        if (last_preview_name != preview_path) {
            expression_preview(preview_path.c_str());
            last_preview_name = preview_path;
        } else {
            ImGui::Image((void*)(intptr_t)global_expression_preview, ImVec2(global_sample_ratio * 37, 37));
            ImGui::SameLine();
        }

        ImGui::EndTooltip();
    }
}

void sdynos_imgui_init() {
    RefreshColorCodeList();

    saturn_load_eye_folder("");

    model_details = "" + std::to_string(sDynosPacks.Count()) + " model pack";
    if (sDynosPacks.Count() != 1) model_details += "s";
}

void sdynos_imgui_menu() {
    if (ImGui::BeginMenu(ICON_FK_USER_CIRCLE " Edit Avatar###menu_edit_avatar")) {

        OpenCCSelector();

        if (ImGui::Button(ICON_FK_FILE_TEXT_O " Add CC File...###add_v_cc")) {
            auto selection3 = choose_file_dialog("Select a file", { "Color Code Files", "*.gs *.txt", "All Files", "*" }, true);

            // Do something with selection
            for (auto const &filename3 : selection3) {
                saturn_copy_file(filename3, "dynos/colorcodes/");
                RefreshColorCodeList();
            }
        }

        ImGui::Text("Model Packs");
        ImGui::SameLine(); imgui_bundled_help_marker(
            "DynOS v1.1 by PeachyPeach\n\nThese are DynOS model packs, used for live model loading.\nPlace packs in /dynos/packs.");

        if (sDynosPacks.Count() >= 20) {
            ImGui::InputTextWithHint("###model_search_text", ICON_FK_SEARCH " Search models...", modelSearchTerm, IM_ARRAYSIZE(modelSearchTerm), ImGuiInputTextFlags_AutoSelectAll);
        } else {
            // If our model list is reloaded, and we now have less than 20 packs, this can cause filter issues if not reset to nothing
            if (modelSearchTerm != "") strcpy(modelSearchTerm, "");
        }
        string modelSearchLower = modelSearchTerm;
        std::transform(modelSearchLower.begin(), modelSearchLower.end(), modelSearchLower.begin(),
            [](unsigned char c){ return std::tolower(c); });

        if (sDynosPacks.Count() <= 0) {
            ImGui::TextDisabled("No model packs found.\nPlace model folders in\n/dynos/packs/.");
        } else {
            ImGui::BeginChild("###menu_model_selector", ImVec2(-FLT_MIN, 125), true);
            for (int i = 0; i < sDynosPacks.Count(); i++) {
                u64 _DirSep1 = sDynosPacks[i]->mPath.find_last_of('\\');
                u64 _DirSep2 = sDynosPacks[i]->mPath.find_last_of('/');
                if (_DirSep1++ == SysPath::npos) _DirSep1 = 0;
                if (_DirSep2++ == SysPath::npos) _DirSep2 = 0;

                std::string label = sDynosPacks[i]->mPath.substr(MAX(_DirSep1, _DirSep2));
                bool is_selected = DynOS_Opt_GetValue(String("dynos_pack_%d", i));

                // If we're searching, only include models with the search keyword in the name
                // Also convert to lowercase
                if (modelSearchLower != "") {
                    string labelLower = saturn_load_search(label);
                    std::transform(labelLower.begin(), labelLower.end(), labelLower.begin(),
                        [](unsigned char c1){ return std::tolower(c1); });

                    if (labelLower.find(modelSearchLower) == string::npos) {
                        continue;
                    }
                }

                if (ImGui::Selectable(label.c_str(), &is_selected)) {
                    // Deselect other packs, but LSHIFT allows additive
                    for (int j = 0; j < sDynosPacks.Count(); j++) {
                        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT] == false)
                            DynOS_Opt_SetValue(String("dynos_pack_%d", j), false);
                    }
                            
                    DynOS_Opt_SetValue(String("dynos_pack_%d", i), is_selected);
                    ui_mfolder_name = label;
                    ui_mfolder_path = sDynosPacks[i]->mPath;

                    gfx_precache_textures();

                    // Fetch model data
                    saturn_load_model_data(label, false);
                    for (int i = 0; i < 8; i++) {
                        current_exp_index[i] = 0;
                    }

                    if (is_selected && current_model_data.name != "")
                        std::cout << "Loaded " << current_model_data.name << " by " << current_model_data.author << std::endl;

                    // Load model CCs

                    RefreshModelColorCodeList();
                    if (current_color_code.IsDefaultColors() || current_color_code.GameShark == last_model_cc_address) {
                        ResetColorCode();
                        last_model_cc_address = current_color_code.GameShark;
                    }

                    // Reset blink cycle (if it exists)
                    if (using_model_eyes || last_model_had_model_eyes) {
                        if (last_model_had_model_eyes) last_model_had_model_eyes = false;
                        else if (!last_model_had_model_eyes && using_model_eyes) last_model_had_model_eyes = true;
                        //blink_eye_1_index = -1; blink_eye_1 = "";
                        blink_eye_2_index = -1; blink_eye_2 = "";
                        blink_eye_3_index = -1; blink_eye_3 = "";
                        force_blink = false;
                    }

                    one_pack_selectable = false;
                    for (int k = 0; k < sDynosPacks.Count(); k++) {
                        if (DynOS_Opt_GetValue(String("dynos_pack_%d", k)))
                            one_pack_selectable = true;
                    }

                    any_packs_selected = one_pack_selectable;
                    if (!any_packs_selected) {
                        model_color_code_list.clear();
                        RefreshColorCodeList();

                        if (last_model_cc_address == current_color_code.GameShark)
                            ResetColorCode();

                        // Reset model data

                        ModelData blank;
                        current_model_data = blank;
                        using_model_eyes = false;
                        cc_model_support = true;
                        cc_spark_support = false;
                        enable_torso_rotation = true;
                    }
                }
                if (ImGui::BeginPopupContextItem()) {
                    model_color_code_list.clear();
                    model_color_code_list = GetColorCodeList(sDynosPacks[i]->mPath + "\\colorcodes");
                    has_open_any_model_cc = true;
                    ImGui::Text("%s/", label.c_str());
                    imgui_bundled_tooltip(("/dynos/packs/" + label).c_str());
                    ImGui::BeginChild("###menu_model_ccs", ImVec2(125, 75), true);
                    for (int n = 0; n < model_color_code_list.size(); n++) {
                        const bool is_selected = (uiCcListId == ((n + 1) * -1) && sDynosPacks[i]->mPath == ui_mfolder_path);
                        std::string label_name = model_color_code_list[n].substr(0, model_color_code_list[n].size() - 3);

                        if (ImGui::Selectable(label_name.c_str(), is_selected)) {
                            if (sDynosPacks[i]->mPath == ui_mfolder_path)
                                uiCcListId = (n + 1) * -1;

                            // Overwrite current color code
                            current_color_code = LoadGSFile(model_color_code_list[n], sDynosPacks[i]->mPath + "\\colorcodes");
                            ApplyColorCode(current_color_code);
                            UpdateEditorFromPalette();
                            last_model_cc_address = current_color_code.GameShark;
                        }

                        if (ImGui::BeginPopupContextItem()) {
                            ImGui::Text("%s.gs", label_name.c_str());
                            imgui_bundled_tooltip(("/dynos/packs/" + label + "/colorcodes/" + label_name + ".gs").c_str());
                            ImGui::Separator();
                            ImGui::TextDisabled("%i color code(s)", model_color_code_list.size());
                            if (ImGui::Button(ICON_FK_UNDO " Refresh")) {
                                model_color_code_list.clear();
                                model_color_code_list = GetColorCodeList(sDynosPacks[i]->mPath + "\\colorcodes");
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        }
                    }
                    ImGui::EndChild();
                    ImGui::Separator();
                    ImGui::TextDisabled("%i model pack(s)", sDynosPacks.Count());
                    if (ImGui::Button(ICON_FK_DOWNLOAD " Refresh Packs###refresh_dynos_packs")) {
                        sDynosPacks.Clear();
                        DynOS_Opt_Init();
                        model_details = "" + std::to_string(DynOS_Gfx_GetPacks().Count()) + " model pack";
                        if (DynOS_Gfx_GetPacks().Count() != 1) model_details += "s";
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine(); imgui_bundled_help_marker("WARNING: Experimental - this will probably lag the game.");
                    ImGui::EndPopup();
                } else {
                    if (has_open_any_model_cc) {
                        has_open_any_model_cc = false;
                        model_color_code_list.clear();
                        model_color_code_list = GetColorCodeList(sDynosPacks[i]->mPath + "\\colorcodes");
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem(ICON_FK_PAINT_BRUSH " Color Code Editor###menu_cc_editor", NULL, windowCcEditor, cc_model_support)) {
        if (cc_model_support) {
            windowAnimPlayer = false;
            windowChromaKey = false;
            windowCcEditor = !windowCcEditor;
        }
    }
    if (ImGui::MenuItem(ICON_FK_FILM " Animation Mixtape###menu_anim_player", NULL, windowAnimPlayer, mario_exists)) {
        windowCcEditor = false;
        windowChromaKey = false;
        windowAnimPlayer = !windowAnimPlayer;
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("Color Settings###menu_color", ((current_model_data.name != "" && current_model_data.cc_support == true) || current_model_data.name == "") || ((current_model_data.name != "" && current_model_data.spark_support == true) || current_model_data.name == ""))) {
        // CC Compatibility - Allows colors to be edited for models that support it
        if ((current_model_data.name != "" && current_model_data.cc_support == true) || current_model_data.name == "") {
            ImGui::Checkbox("CC Compatibility", &cc_model_support);
            ImGui::SameLine(); imgui_bundled_help_marker(
                "Toggles color code compatibility for model packs that support it.");
        }
        // CometSPARK Support - When a model is present, allows SPARK colors to be edited for models that support it
        if (any_packs_selected) {
            if ((current_model_data.name != "" && current_model_data.spark_support == true) || current_model_data.name == "") {
                ImGui::Checkbox("CometSPARK Support", &cc_spark_support);
                ImGui::SameLine(); imgui_bundled_help_marker(
                    "Grants a model extra color values. See the GitHub wiki for setup instructions.");
            }
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Misc.###menu_misc")) {

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("Misc.###misc_child", ImVec2(225, 175), true, ImGuiWindowFlags_None);
        if (ImGui::BeginTabBar("###misc_tabbar", ImGuiTabBarFlags_None)) {

            if (ImGui::BeginTabItem("Switches###switches_scale")) {
                const char* eyes[] = { "Blinking", "Open", "Half", "Closed", ICON_FK_EYE_SLASH " Custom A", ICON_FK_EYE_SLASH " Custom B", ICON_FK_EYE_SLASH " Custom C", ICON_FK_EYE_SLASH " Custom D", "Dead" };
                if (ImGui::Combo("Eyes###eye_state", &current_eye_state, eyes, IM_ARRAYSIZE(eyes))) {
                    if (current_eye_state < 4) using_custom_eyes = false;
                    else using_custom_eyes = true;
                }

                const char* hands[] = { "Fists", "Open", "Peace", "With Cap", "With Wing Cap", "Right Open" };
                ImGui::Combo("Hand###hand_state", &scrollHandState, hands, IM_ARRAYSIZE(hands));
                const char* caps[] = { "Cap On", "Cap Off", "Wing Cap" }; // unused "wing cap off" not included
                ImGui::Combo("Cap###cap_state", &scrollCapState, caps, IM_ARRAYSIZE(caps));
                const char* powerups[] = { "Default", "Metal", "Vanish", "Metal & Vanish" };
                ImGui::Combo("Powerup###powerup_state", &saturnModelState, powerups, IM_ARRAYSIZE(powerups));
                if (any_packs_selected) ImGui::BeginDisabled();
                ImGui::Checkbox("M Cap Emblem", &show_vmario_emblem);
                imgui_bundled_tooltip("Enables the signature \"M\" logo on Mario's cap.");
                saturn_keyframe_bool_popout(&show_vmario_emblem, "M Cap Emblem", "k_v_cap_emblem");
                if (any_packs_selected) ImGui::EndDisabled();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shading###tab_shading")) {
                ImGui::SliderFloat("X###wdir_x", &world_light_dir1, -2.f, 2.f);
                saturn_keyframe_float_popout(&world_light_dir1, "Mario Shade X", "k_shade_x");
                ImGui::SliderFloat("Y###wdir_y", &world_light_dir2, -2.f, 2.f);
                saturn_keyframe_float_popout(&world_light_dir2, "Mario Shade Y", "k_shade_y");
                ImGui::SliderFloat("Z###wdir_z", &world_light_dir3, -2.f, 2.f);
                saturn_keyframe_float_popout(&world_light_dir3, "Mario Shade Z", "k_shade_z");
                ImGui::SliderFloat("Tex###wdir_tex", &world_light_dir4, 1.f, 4.f);
                saturn_keyframe_float_popout(&world_light_dir4, "Mario Shade Tex", "k_shade_t");

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
                saturn_keyframe_color_popout("Light Color", "k_light_col", &gLightingColor[0], &gLightingColor[1], &gLightingColor[2]);

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
                    saturn_keyframe_float_popout(&marioScaleSizeX, "Mario Scale", "k_scale");
                } else {
                    ImGui::SliderFloat("X###mscale_x", &marioScaleSizeX, -2.f, 2.f);
                    saturn_keyframe_float_popout(&marioScaleSizeX, "Mario Scale X", "k_scale_x");
                    ImGui::SliderFloat("Y###mscale_y", &marioScaleSizeY, -2.f, 2.f);
                    saturn_keyframe_float_popout(&marioScaleSizeY, "Mario Scale Y", "k_scale_y");
                    ImGui::SliderFloat("Z###mscale_z", &marioScaleSizeZ, -2.f, 2.f);
                    saturn_keyframe_float_popout(&marioScaleSizeZ, "Mario Scale Z", "k_scale_z");
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
            ImGui::Checkbox("Head Rotations", &enable_head_rotations);
            imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
            saturn_keyframe_bool_popout(&enable_head_rotations, "Head Rotations", "k_head_rot");
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
                } else if (!k_popout_open || keyframe_playing) {
                    this_face_angle = (float)gMarioState->faceAngle[1] / 182.04;
                }
                saturn_keyframe_float_popout(&this_face_angle, "Mario Angle", "k_angle");
            }

            ImGui::Checkbox("Spin###spin_angle", &is_spinning);
            if (is_spinning) {
                ImGui::SliderFloat("Speed###spin,speed", &spin_mult, -2.f, 2.f, "%.1f");
            }

            if (this_face_angle > 180) this_face_angle = -180;
            if (this_face_angle < -180) this_face_angle = 180;

            ImGui::EndTable();
        }

        ImGui::PopStyleVar();
        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (current_model_data.name != "") {

        // Metadata
        string metaLabelText = (ICON_FK_USER " " + current_model_data.name);
        string metaDataText = "v" + current_model_data.version;
        if (current_model_data.description != "")
            metaDataText = "v" + current_model_data.version + "\n" + current_model_data.description;

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("###model_metadata", ImVec2(0, 45), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::Text(metaLabelText.c_str()); imgui_bundled_tooltip(metaDataText.c_str());
        ImGui::TextDisabled(("@ " + current_model_data.author).c_str());
        ImGui::EndChild();
        ImGui::PopStyleVar();

        ImGui::Separator();
    }

    // Custom Eyes - Overwrites the left eye switch with an external PNG image choice
    if ((current_model_data.name != "" && current_model_data.eye_support == true) || current_model_data.name == "") {
        if (ImGui::Checkbox("Custom Eyes", &using_custom_eyes)) {
            if (current_eye_state < 4) current_eye_state = 4;
            else current_eye_state = 0;
        } 
        if (!any_packs_selected) {
            ImGui::SameLine(); imgui_bundled_help_marker(
                "Place custom eye PNG textures in dynos/eyes.");
        }
    } else {
        using_custom_eyes = false;
        force_blink = false;
    }

    if (using_custom_eyes) {
        //scrollEyeState = 4;

        // Eyes (No Model)

        if (!using_model_eyes) {
            if (eye_array.size() > 0 && using_custom_eyes) {
                ImGui::BeginChild("###menu_custom_eye_selector", ImVec2(-FLT_MIN, 150), true);
                for (int n = 0; n < eye_array.size(); n++) {
                    const bool is_eye_selected = (current_eye_index == n || blink_eye_2_index == n || blink_eye_3_index == n);
                    string entry_name = eye_array[n];
                    string preview_name = entry_name;
                    
                    if (eye_array[n].find("/") != string::npos)
                        preview_name = ICON_FK_FOLDER " " + eye_array[n].substr(0, eye_array[n].size() - 1);

                    if (eye_array[n] == "../")
                        preview_name = ICON_FK_FOLDER " ../";

                    if (ImGui::Selectable(preview_name.c_str(), is_eye_selected)) {
                        gfx_precache_textures();
                        current_eye_index = n;
                        saturn_eye_selectable(entry_name, n);
                    }
                    if (entry_name.find(".png") != string::npos && configEditorExpressionPreviews) {
                        if (ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                            // ImageTest
                            if (last_preview_name != current_eye_dir_path + entry_name) {
                                expression_preview((current_eye_dir_path + entry_name).c_str());
                                last_preview_name = current_eye_dir_path + entry_name;
                            } else {
                                ImGui::Image((void*)(intptr_t)global_expression_preview, ImVec2(global_sample_ratio * 37, 37));
                                ImGui::SameLine();
                            }

                            ImGui::EndTooltip();
                        }
                    }

                    // Refresh
                    if (ImGui::BeginPopupContextItem()) {
                        if (entry_name.find(".png") != string::npos) {
                            ImGui::Text("%s", entry_name.c_str());
                            imgui_bundled_tooltip((current_eye_dir_path + entry_name).c_str());
                            if (eye_array.size() > 1) {
                                if (ImGui::SmallButton(ICON_FK_TRASH_O " Delete File")) {
                                    saturn_delete_file(current_eye_dir_path + entry_name);
                                    saturn_load_eye_folder(current_eye_dir_path);
                                    ImGui::CloseCurrentPopup();
                                } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                            }
                            ImGui::Separator();
                        }

                        if (enable_blink_cycle || current_model_data.name == "") {
                            ImGui::Separator();
                            ImGui::Text(ICON_FK_EYE_SLASH " Add to Blink Cycle");
                            blink_cycle_preview("1###blink_1", 1, current_eye_dir_path + entry_name, n); ImGui::SameLine();
                            blink_cycle_preview("2###blink_2", 2, current_eye_dir_path + entry_name, n); ImGui::SameLine();
                            blink_cycle_preview("3###blink_3", 3, current_eye_dir_path + entry_name, n);

                            if (force_blink) {
                                if (ImGui::Button(ICON_FK_ERASER " Reset###reset_blink")) {
                                    // Reset blink cycle (if it exists)
                                    blink_eye_2_index = -1; blink_eye_2 = "";
                                    blink_eye_3_index = -1; blink_eye_3 = "";
                                    force_blink = false;
                                }
                            }

                            /*if (cycle_blink_disabled && force_blink) {
                                ImGui::BeginDisabled();
                                ImGui::TextWrapped("ERROR: It appears this model does not support custom blink cycles.\nIf you are a model author, please refer to the GitHub wiki for setup instructions.");
                                ImGui::EndDisabled();
                            }*/
                        }

                        ImGui::Separator();
                        ImGui::TextDisabled("%i eye texture(s)", eye_array.size());
                        if (ImGui::Button(ICON_FK_UNDO " Refresh###refresh_v_eyes")) {
                            saturn_load_eye_folder(current_eye_dir_path);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndChild();
                if (ImGui::Button(ICON_FK_FILE_IMAGE_O " Add Eye...###add_v_eye")) {
                    auto selection = choose_file_dialog("Select a file", { "PNG Textures", "*.png", "All Files", "*" }, true);

                    // Do something with selection
                    for (auto const &filename : selection) {
                        saturn_copy_file(filename, current_eye_dir_path);
                        saturn_load_eye_folder(current_eye_dir_path);
                    }
                }
            } else {
                ImGui::Text("No eye textures found.\nPlace custom eye PNG textures\nin dynos/eyes/.");
            }
            if (current_model_data.expressions.size() >= 1) ImGui::Separator();
        }
    } else {
        force_blink = false;
        if (current_model_data.expressions.size() >= 1) ImGui::Separator();
    }

    scrollEyeState = current_eye_state;
    // Expressions (including vanilla eyes)
    is_replacing_exp = (using_custom_eyes && scrollEyeState > 3 || current_model_data.expressions.size() > 0);
    // Vanilla and model eyes
    is_replacing_eyes = (using_custom_eyes && scrollEyeState > 3 || current_model_data.expressions.size() > 0 && using_custom_eyes);

    if (current_model_data.expressions.size() > 0) {

        // Model Expressions

        for (int i; i < current_model_data.expressions.size(); i++) {
            Expression expression = current_model_data.expressions[i];

            if (expression.name == "eye" || expression.name == "eyes") {
                if (scrollEyeState < 4 || !using_custom_eyes) {
                    continue;
                }

                // Eyes
                ImGui::BeginChild(("###menu_custom_%s", expression.name.c_str()), ImVec2(-FLT_MIN, 75), true);
                for (int n = 0; n < expression.textures.size(); n++) {
                    string entry_name = expression.textures[n];
                    bool is_selected = (current_exp_index[i] == n || blink_eye_2_index == n || blink_eye_3_index == n);

                    if (ImGui::Selectable(entry_name.c_str(), is_selected)) {
                        gfx_precache_textures();
                        current_exp_index[i] = n;
                        saturn_set_model_texture(i, expression.path + expression.textures[n]);
                        saturn_set_model_blink_eye(i, 1, expression.path + expression.textures[n]);
                    }
                    if (entry_name.find(".png") != string::npos && configEditorExpressionPreviews) {
                        if (ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                            // ImageTest
                            if (last_preview_name != expression.path + entry_name) {
                                expression_preview((expression.path + expression.textures[n]).c_str());
                                last_preview_name = expression.path + entry_name;
                            } else {
                                ImGui::Image((void*)(intptr_t)global_expression_preview, ImVec2(global_sample_ratio * 37, 37));
                                ImGui::SameLine();
                            }

                            ImGui::EndTooltip();
                        }
                    }

                    // Refresh
                    if (ImGui::BeginPopupContextItem()) {
                        ImGui::Text("%s", entry_name.c_str());
                        imgui_bundled_tooltip(("dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/" + entry_name).c_str());
                        if (expression.textures.size() > 1) {
                            if (ImGui::SmallButton(ICON_FK_TRASH_O " Delete File")) {
                                saturn_delete_file("dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/" + entry_name);
                                saturn_load_model_data(current_folder_name, true);
                                for (int i = 0; i < 8; i++) {
                                    if (current_exp_index[i] > expression.textures.size())
                                        current_exp_index[i] = 0;
                                }
                                // Reset blink cycle (if it exists)
                                blink_eye_2_index = -1; blink_eye_2 = "";
                                blink_eye_3_index = -1; blink_eye_3 = "";
                                force_blink = false;
                                ImGui::CloseCurrentPopup();
                            } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                        }

                        ImGui::Separator();
                        ImGui::Text(ICON_FK_EYE_SLASH " Add to Blink Cycle");
                        blink_cycle_preview("1###blink_1", 1, expression.path + expression.textures[n], n); ImGui::SameLine();
                        blink_cycle_preview("2###blink_2", 2, expression.path + expression.textures[n], n); ImGui::SameLine();
                        blink_cycle_preview("3###blink_3", 3, expression.path + expression.textures[n], n);

                        if (force_blink) {
                            if (ImGui::Button(ICON_FK_ERASER " Reset###reset_blink")) {
                                // Reset blink cycle (if it exists)
                                blink_eye_2_index = -1; blink_eye_2 = "";
                                blink_eye_3_index = -1; blink_eye_3 = "";
                                force_blink = false;
                            }
                        }

                        ImGui::Separator();
                        ImGui::TextDisabled("%i eye texture(s)", expression.textures.size());
                        if (ImGui::Button(ICON_FK_UNDO " Refresh###refresh_m_eyes")) {
                            saturn_load_model_data(current_folder_name, true);
                            for (int i = 0; i < 8; i++) {
                                if (current_exp_index[i] > expression.textures.size())
                                    current_exp_index[i] = 0;
                            }
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndChild();
                if (ImGui::Button(ICON_FK_FILE_IMAGE_O " Add Eye...###add_m_eye")) {
                    auto selection1 = choose_file_dialog("Select a file", { "PNG Textures", "*.png", "All Files", "*" }, true);

                    // Do something with selection
                    for (auto const &filename1 : selection1) {
                        saturn_copy_file(filename1, "dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/");
                        saturn_load_model_data(current_folder_name, true);
                    }
                }
                if (current_model_data.expressions.size() >= 1) ImGui::Separator();
            }
        }
        for (int i; i < current_model_data.expressions.size(); i++) {
            Expression expression = current_model_data.expressions[i];

            if (expression.name != "eye" && expression.name != "eyes") {

                // Everything but eyes
                if (expression.textures.size() > 0) {
                    string label_name = "###menu_" + expression.name;

                    // If we just have "none" and "default" for an expression...
                    // Use a checkbox instead for nicer UI
                    if (expression.textures.size() == 2 && expression.textures[0].find("default") != string::npos && expression.textures[1].find("none") != string::npos ||
                        expression.textures.size() == 2 && expression.textures[1].find("default") != string::npos && expression.textures[0].find("none") != string::npos) {

                        int cselect_index = (expression.textures[0].find("none") != string::npos) ? 1 : 0;
                        int cdeselect_index = (expression.textures[0].find("none") != string::npos) ? 0 : 1;
                        bool is_selected = (current_exp_index[i] == cselect_index);

                        ImGui::Text(expression.name.c_str());
                        ImGui::SameLine(75);
                        if (ImGui::Checkbox(label_name.c_str(), &is_selected)) {
                            gfx_precache_textures();
                            if (is_selected) {
                                current_exp_index[i] = cselect_index;
                                saturn_set_model_texture(i, expression.path + expression.textures[cselect_index]);
                            } else {
                                current_exp_index[i] = cdeselect_index;
                                saturn_set_model_texture(i, expression.path + expression.textures[cdeselect_index]);
                            }
                        }
                    } else {
                        string label_name = "###menu_" + expression.name;
                        ImGui::Text(expression.name.c_str());
                        ImGui::SameLine(75);
                        ImGui::PushItemWidth(150);
                        if (ImGui::BeginCombo(label_name.c_str(), expression.textures[current_exp_index[i]].c_str(), ImGuiComboFlags_None)) {
                            for (int n = 0; n < expression.textures.size(); n++) {
                                bool is_selected = (current_exp_index[i] == n);
                                string entry_name = expression.textures[n];

                                if (ImGui::Selectable(entry_name.c_str(), is_selected)) {
                                    gfx_precache_textures();
                                    current_exp_index[i] = n;
                                    saturn_set_model_texture(i, expression.path + expression.textures[n]);
                                }
                                if (entry_name.find(".png") != string::npos && configEditorExpressionPreviews) {
                                    if (ImGui::IsItemHovered()) {
                                        ImGui::BeginTooltip();
                                        // ImageTest
                                        if (last_preview_name != expression.path + entry_name) {
                                            expression_preview((expression.path + expression.textures[n]).c_str());
                                            last_preview_name = expression.path + entry_name;
                                        } else {
                                            ImGui::Image((void*)(intptr_t)global_expression_preview, ImVec2(global_sample_ratio * 37, 37));
                                            ImGui::SameLine();
                                        }

                                        ImGui::EndTooltip();
                                    }
                                }

                                // Add / Delete
                                if (ImGui::BeginPopupContextItem()) {
                                    ImGui::Text("%s", entry_name.c_str());
                                    imgui_bundled_tooltip(("dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/" + entry_name).c_str());
                                    if (expression.textures.size() > 1) {
                                        if (ImGui::SmallButton(ICON_FK_TRASH_O " Delete File")) {
                                            saturn_delete_file("dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/" + entry_name);
                                            saturn_load_model_data(current_folder_name, true);
                                            for (int i = 0; i < 8; i++) {
                                                if (current_exp_index[i] > expression.textures.size())
                                                    current_exp_index[i] = 0;
                                            }
                                            ImGui::CloseCurrentPopup();
                                        } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                                    }
                                    ImGui::EndPopup();
                                }
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::PopItemWidth();
                    }

                    // Refresh
                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::Button(ICON_FK_FILE_IMAGE_O " Add Expression...###add_m_exp")) {
                            auto selection1 = choose_file_dialog("Select a file", { "PNG Textures", "*.png", "All Files", "*" }, true);

                            // Do something with selection
                            for (auto const &filename1 : selection1) {
                                saturn_copy_file(filename1, "dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/");
                                saturn_load_model_data(current_folder_name, true);
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::Separator();
                        ImGui::TextDisabled("%i %s expression(s)", expression.textures.size(), expression.name.c_str());
                        if (ImGui::Button(ICON_FK_UNDO " Refresh###refresh_m_exp")) {
                            saturn_load_model_data(current_folder_name, true);
                            for (int i = 0; i < 8; i++) {
                                if (current_exp_index[i] > expression.textures.size())
                                    current_exp_index[i] = 0;
                            }
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
            }
        }
    }
}