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

static ImVec4 uiHatColor =              ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiHatShadeColor =         ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiOverallsColor =         ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiOverallsShadeColor =    ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiGlovesColor =           ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiGlovesShadeColor =      ImVec4(127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShoesColor =            ImVec4(114.0f / 255.0f, 28.0f / 255.0f, 14.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShoesShadeColor =       ImVec4(57.0f / 255.0f, 14.0f / 255.0f, 7.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiSkinColor =             ImVec4(254.0f / 255.0f, 193.0f / 255.0f, 121.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiSkinShadeColor =        ImVec4(127.0f / 255.0f, 96.0f / 255.0f, 60.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiHairColor =             ImVec4(115.0f / 255.0f, 6.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiHairShadeColor =        ImVec4(57.0f / 255.0f, 3.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
// SPARK
static ImVec4 uiShirtColor =                    ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShirtShadeColor =               ImVec4(127.0f / 255.0f, 127.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShouldersColor =                ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiShouldersShadeColor =           ImVec4(0.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiArmsColor =                     ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiArmsShadeColor =                ImVec4(0.0f / 255.0f, 127.0f / 255.0f, 64.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiOverallsBottomColor =           ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiOverallsBottomShadeColor =      ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiLegTopColor =                   ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiLegTopShadeColor =              ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 64.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiLegBottomColor =                ImVec4(127.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
static ImVec4 uiLegBottomShadeColor =           ImVec4(64.0f / 255.0f, 0.0f / 255.0f, 127.0f / 255.0f, 255.0f / 255.0f);

static char ui_cc_name[128] = "Sample";
static int current_cc_id = 0;
string cc_name;
static char ui_gameshark[1024 * 16] = "";

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
int current_model_id;
int cc_model_id;
int current_cc_to_model_index;
string ui_model_gameshark;
bool has_open_any_model_cc;

bool using_custom_eyes;

static char modelSearchTerm[128];
static char ccSearchTerm[128];
static int current_mcc_id = 0;

float this_face_angle;

bool has_copy_mario;

bool last_model_had_model_eyes;

/*
Sets Mario's global colors from the CC editor color values.
*/
void apply_cc_from_editor() {
    defaultColorHat.red[0] = (int)(uiHatColor.x * 255);
    defaultColorHat.green[0] = (int)(uiHatColor.y * 255);
    defaultColorHat.blue[0] = (int)(uiHatColor.z * 255);
    defaultColorHat.red[1] = (int)(uiHatShadeColor.x * 255);
    defaultColorHat.green[1] = (int)(uiHatShadeColor.y * 255);
    defaultColorHat.blue[1] = (int)(uiHatShadeColor.z * 255);
    defaultColorOveralls.red[0] = (int)(uiOverallsColor.x * 255);
    defaultColorOveralls.green[0] = (int)(uiOverallsColor.y * 255);
    defaultColorOveralls.blue[0] = (int)(uiOverallsColor.z * 255);
    defaultColorOveralls.red[1] = (int)(uiOverallsShadeColor.x * 255);
    defaultColorOveralls.green[1] = (int)(uiOverallsShadeColor.y * 255);
    defaultColorOveralls.blue[1] = (int)(uiOverallsShadeColor.z * 255);
    defaultColorGloves.red[0] = (int)(uiGlovesColor.x * 255);
    defaultColorGloves.green[0] = (int)(uiGlovesColor.y * 255);
    defaultColorGloves.blue[0] = (int)(uiGlovesColor.z * 255);
    defaultColorGloves.red[1] = (int)(uiGlovesShadeColor.x * 255);
    defaultColorGloves.green[1] = (int)(uiGlovesShadeColor.y * 255);
    defaultColorGloves.blue[1] = (int)(uiGlovesShadeColor.z * 255);
    defaultColorShoes.red[0] = (int)(uiShoesColor.x * 255);
    defaultColorShoes.green[0] = (int)(uiShoesColor.y * 255);
    defaultColorShoes.blue[0] = (int)(uiShoesColor.z * 255);
    defaultColorShoes.red[1] = (int)(uiShoesShadeColor.x * 255);
    defaultColorShoes.green[1] = (int)(uiShoesShadeColor.y * 255);
    defaultColorShoes.blue[1] = (int)(uiShoesShadeColor.z * 255);
    defaultColorSkin.red[0] = (int)(uiSkinColor.x * 255);
    defaultColorSkin.green[0] = (int)(uiSkinColor.y * 255);
    defaultColorSkin.blue[0] = (int)(uiSkinColor.z * 255);
    defaultColorSkin.red[1] = (int)(uiSkinShadeColor.x * 255);
    defaultColorSkin.green[1] = (int)(uiSkinShadeColor.y * 255);
    defaultColorSkin.blue[1] = (int)(uiSkinShadeColor.z * 255);
    defaultColorHair.red[0] = (int)(uiHairColor.x * 255);
    defaultColorHair.green[0] = (int)(uiHairColor.y * 255);
    defaultColorHair.blue[0] = (int)(uiHairColor.z * 255);
    defaultColorHair.red[1] = (int)(uiHairShadeColor.x * 255);
    defaultColorHair.green[1] = (int)(uiHairShadeColor.y * 255);
    defaultColorHair.blue[1] = (int)(uiHairShadeColor.z * 255);
    if (cc_spark_support) {
        sparkColorShirt.red[0] = (int)(uiShirtColor.x * 255);
        sparkColorShirt.green[0] = (int)(uiShirtColor.y * 255);
        sparkColorShirt.blue[0] = (int)(uiShirtColor.z * 255);
        sparkColorShirt.red[1] = (int)(uiShirtShadeColor.x * 255);
        sparkColorShirt.green[1] = (int)(uiShirtShadeColor.y * 255);
        sparkColorShirt.blue[1] = (int)(uiShirtShadeColor.z * 255);
        sparkColorShoulders.red[0] = (int)(uiShouldersColor.x * 255);
        sparkColorShoulders.green[0] = (int)(uiShouldersColor.y * 255);
        sparkColorShoulders.blue[0] = (int)(uiShouldersColor.z * 255);
        sparkColorShoulders.red[1] = (int)(uiShouldersShadeColor.x * 255);
        sparkColorShoulders.green[1] = (int)(uiShouldersShadeColor.y * 255);
        sparkColorShoulders.blue[1] = (int)(uiShouldersShadeColor.z * 255);
        sparkColorArms.red[0] = (int)(uiArmsColor.x * 255);
        sparkColorArms.green[0] = (int)(uiArmsColor.y * 255);
        sparkColorArms.blue[0] = (int)(uiArmsColor.z * 255);
        sparkColorArms.red[1] = (int)(uiArmsShadeColor.x * 255);
        sparkColorArms.green[1] = (int)(uiArmsShadeColor.y * 255);
        sparkColorArms.blue[1] = (int)(uiArmsShadeColor.z * 255);
        sparkColorOverallsBottom.red[0] = (int)(uiOverallsBottomColor.x * 255);
        sparkColorOverallsBottom.green[0] = (int)(uiOverallsBottomColor.y * 255);
        sparkColorOverallsBottom.blue[0] = (int)(uiOverallsBottomColor.z * 255);
        sparkColorOverallsBottom.red[1] = (int)(uiOverallsBottomShadeColor.x * 255);
        sparkColorOverallsBottom.green[1] = (int)(uiOverallsBottomShadeColor.y * 255);
        sparkColorOverallsBottom.blue[1] = (int)(uiOverallsBottomShadeColor.z * 255);
        sparkColorLegTop.red[0] = (int)(uiLegTopColor.x * 255);
        sparkColorLegTop.green[0] = (int)(uiLegTopColor.y * 255);
        sparkColorLegTop.blue[0] = (int)(uiLegTopColor.z * 255);
        sparkColorLegTop.red[1] = (int)(uiLegTopShadeColor.x * 255);
        sparkColorLegTop.green[1] = (int)(uiLegTopShadeColor.y * 255);
        sparkColorLegTop.blue[1] = (int)(uiLegTopShadeColor.z * 255);
        sparkColorLegBottom.red[0] = (int)(uiLegBottomColor.x * 255);
        sparkColorLegBottom.green[0] = (int)(uiLegBottomColor.y * 255);
        sparkColorLegBottom.blue[0] = (int)(uiLegBottomColor.z * 255);
        sparkColorLegBottom.red[1] = (int)(uiLegBottomShadeColor.x * 255);
        sparkColorLegBottom.green[1] = (int)(uiLegBottomShadeColor.y * 255);
        sparkColorLegBottom.blue[1] = (int)(uiLegBottomShadeColor.z * 255);
    }

    // Also set the editor GameShark code
    strcpy(ui_gameshark, global_gs_code().c_str());
} 

/*
Sets the CC editor color values from Mario's global colors.
*/
void set_editor_from_global_cc(std::string cc_name) {
    uiHatColor = ImVec4(float(defaultColorHat.red[0]) / 255.0f, float(defaultColorHat.green[0]) / 255.0f, float(defaultColorHat.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiHatShadeColor = ImVec4(float(defaultColorHat.red[1]) / 255.0f, float(defaultColorHat.green[1]) / 255.0f, float(defaultColorHat.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiOverallsColor = ImVec4(float(defaultColorOveralls.red[0]) / 255.0f, float(defaultColorOveralls.green[0]) / 255.0f, float(defaultColorOveralls.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiOverallsShadeColor = ImVec4(float(defaultColorOveralls.red[1]) / 255.0f, float(defaultColorOveralls.green[1]) / 255.0f, float(defaultColorOveralls.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiGlovesColor = ImVec4(float(defaultColorGloves.red[0]) / 255.0f, float(defaultColorGloves.green[0]) / 255.0f, float(defaultColorGloves.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiGlovesShadeColor = ImVec4(float(defaultColorGloves.red[1]) / 255.0f, float(defaultColorGloves.green[1]) / 255.0f, float(defaultColorGloves.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiShoesColor = ImVec4(float(defaultColorShoes.red[0]) / 255.0f, float(defaultColorShoes.green[0]) / 255.0f, float(defaultColorShoes.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiShoesShadeColor = ImVec4(float(defaultColorShoes.red[1]) / 255.0f, float(defaultColorShoes.green[1]) / 255.0f, float(defaultColorShoes.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiSkinColor = ImVec4(float(defaultColorSkin.red[0]) / 255.0f, float(defaultColorSkin.green[0]) / 255.0f, float(defaultColorSkin.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiSkinShadeColor = ImVec4(float(defaultColorSkin.red[1]) / 255.0f, float(defaultColorSkin.green[1]) / 255.0f, float(defaultColorSkin.blue[1]) / 255.0f, 255.0f / 255.0f);
    uiHairColor = ImVec4(float(defaultColorHair.red[0]) / 255.0f, float(defaultColorHair.green[0]) / 255.0f, float(defaultColorHair.blue[0]) / 255.0f, 255.0f / 255.0f);
    uiHairShadeColor = ImVec4(float(defaultColorHair.red[1]) / 255.0f, float(defaultColorHair.green[1]) / 255.0f, float(defaultColorHair.blue[1]) / 255.0f, 255.0f / 255.0f);

    if (cc_spark_support) {
        uiShirtColor = ImVec4(float(sparkColorShirt.red[0]) / 255.0f, float(sparkColorShirt.green[0]) / 255.0f, float(sparkColorShirt.blue[0]) / 255.0f, 255.0f / 255.0f);
        uiShirtShadeColor = ImVec4(float(sparkColorShirt.red[1]) / 255.0f, float(sparkColorShirt.green[1]) / 255.0f, float(sparkColorShirt.blue[1]) / 255.0f, 255.0f / 255.0f);
        uiShouldersColor = ImVec4(float(sparkColorShoulders.red[0]) / 255.0f, float(sparkColorShoulders.green[0]) / 255.0f, float(sparkColorShoulders.blue[0]) / 255.0f, 255.0f / 255.0f);
        uiShouldersShadeColor = ImVec4(float(sparkColorShoulders.red[1]) / 255.0f, float(sparkColorShoulders.green[1]) / 255.0f, float(sparkColorShoulders.blue[1]) / 255.0f, 255.0f / 255.0f);
        uiArmsColor = ImVec4(float(sparkColorArms.red[0]) / 255.0f, float(sparkColorArms.green[0]) / 255.0f, float(sparkColorArms.blue[0]) / 255.0f, 255.0f / 255.0f);
        uiArmsShadeColor = ImVec4(float(sparkColorArms.red[1]) / 255.0f, float(sparkColorArms.green[1]) / 255.0f, float(sparkColorArms.blue[1]) / 255.0f, 255.0f / 255.0f);
        uiOverallsBottomColor = ImVec4(float(sparkColorOverallsBottom.red[0]) / 255.0f, float(sparkColorOverallsBottom.green[0]) / 255.0f, float(sparkColorOverallsBottom.blue[0]) / 255.0f, 255.0f / 255.0f);
        uiOverallsBottomShadeColor = ImVec4(float(sparkColorOverallsBottom.red[1]) / 255.0f, float(sparkColorOverallsBottom.green[1]) / 255.0f, float(sparkColorOverallsBottom.blue[1]) / 255.0f, 255.0f / 255.0f);
        uiLegTopColor = ImVec4(float(sparkColorLegTop.red[0]) / 255.0f, float(sparkColorLegTop.green[0]) / 255.0f, float(sparkColorLegTop.blue[0]) / 255.0f, 255.0f / 255.0f);
        uiLegTopShadeColor = ImVec4(float(sparkColorLegTop.red[1]) / 255.0f, float(sparkColorLegTop.green[1]) / 255.0f, float(sparkColorLegTop.blue[1]) / 255.0f, 255.0f / 255.0f);
        uiLegBottomColor = ImVec4(float(sparkColorLegBottom.red[0]) / 255.0f, float(sparkColorLegBottom.green[0]) / 255.0f, float(sparkColorLegBottom.blue[0]) / 255.0f, 255.0f / 255.0f);
        uiLegBottomShadeColor = ImVec4(float(sparkColorLegBottom.red[1]) / 255.0f, float(sparkColorLegBottom.green[1]) / 255.0f, float(sparkColorLegBottom.blue[1]) / 255.0f, 255.0f / 255.0f);
    }

    // Also set the editor GameShark code
    strcpy(ui_gameshark, global_gs_code().c_str());

    // We never want to use the name "Mario" when saving/loading a CC, as it will cause file issues
    // Instead, we'll change the UI name to "Sample"
    if (cc_name == "Mario") {
        strcpy(ui_cc_name, "Sample");
    } else if (cc_name != "") {
        strcpy(ui_cc_name, cc_name.c_str());
    }
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

void handle_cc_box(const char* name, const char* mainName, const char* shadeName, ImVec4* colorValue, ImVec4* shadeColorValue, string id) {
    string nameStr = name;
    if (nameStr != "") {
        ImGui::ColorEdit4(mainName, (float*)colorValue, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);
        if (ImGui::IsItemActivated()) accept_text_input = false;
        if (ImGui::IsItemDeactivated()) accept_text_input = true;

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
            ImGui::OpenPopup(id.c_str());
        if (ImGui::BeginPopup(id.c_str())) {
            if (ImGui::Selectable("×2")) {
                colorValue->x = shadeColorValue->x * 1.99f;
                colorValue->y = shadeColorValue->y * 1.99f;
                colorValue->z = shadeColorValue->z * 1.99f;
            }
            if (ImGui::Selectable(ICON_FK_UNDO " Reset")) {
                if (mainName == "Hat, Main")                paste_gs_code("8107EC40 FF00\n8107EC42 0000");
                if (mainName == "Overalls, Main")           paste_gs_code("8107EC28 0000\n8107EC2A FF00");
                if (mainName == "Gloves, Main")             paste_gs_code("8107EC58 FFFF\n8107EC5A FF00");
                if (mainName == "Shoes, Main")              paste_gs_code("8107EC70 721C\n8107EC72 0E00");
                if (mainName == "Skin, Main")               paste_gs_code("8107EC88 FEC1\n8107EC8A 7900");
                if (mainName == "Hair, Main")               paste_gs_code("8107ECA0 7306\n8107ECA2 0000");
                if (mainName == "Shirt, Main")              paste_gs_code("8107ECB8 FFFF\n8107ECBA 0000");
                if (mainName == "Shoulders, Main")          paste_gs_code("8107ECD0 00FF\n8107ECD2 FF00");
                if (mainName == "Arms, Main")               paste_gs_code("8107ECE8 00FF\n8107ECEA 7F00");
                if (mainName == "Overalls (Bottom), Main")  paste_gs_code("8107ED00 FF00\n8107ED02 FF00");
                if (mainName == "Leg (Top), Main")          paste_gs_code("8107ED18 FF00\n8107ED1A 7F00");
                if (mainName == "Leg (Bottom), Main")       paste_gs_code("8107ED30 7F00\n8107ED32 FF00");
                set_editor_from_global_cc("");
            }
            if (ImGui::Selectable("Randomize")) {
                colorValue->x = (rand() % 255) / 255.0f;
                colorValue->y = (rand() % 255) / 255.0f;
                colorValue->z = (rand() % 255) / 255.0f;
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::ColorEdit4(shadeName, (float*)shadeColorValue, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);
        if (ImGui::IsItemActivated()) accept_text_input = false;
        if (ImGui::IsItemDeactivated()) accept_text_input = true;

        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
            ImGui::OpenPopup((id + "1").c_str());
        if (ImGui::BeginPopup((id + "1").c_str())) {
            if (ImGui::Selectable("1/2")) {
                shadeColorValue->x = floor(colorValue->x / 2.0f * 255.0f) / 255.0f;
                shadeColorValue->y = floor(colorValue->y / 2.0f * 255.0f) / 255.0f;
                shadeColorValue->z = floor(colorValue->z / 2.0f * 255.0f) / 255.0f;
            }
            if (ImGui::Selectable(ICON_FK_UNDO " Reset")) {
                if (shadeName == "Hat, Shade")                paste_gs_code("8107EC38 7F00\n8107EC3A 0000");
                if (shadeName == "Overalls, Shade")           paste_gs_code("8107EC20 0000\n8107EC22 7F00");
                if (shadeName == "Gloves, Shade")             paste_gs_code("8107EC50 7F7F\n8107EC52 7F00");
                if (shadeName == "Shoes, Shade")              paste_gs_code("8107EC68 390E\n8107EC6A 0700");
                if (shadeName == "Skin, Shade")               paste_gs_code("8107EC80 7F60\n8107EC82 3C00");
                if (shadeName == "Hair, Shade")               paste_gs_code("8107EC98 3903\n8107EC9A 0000");
                if (shadeName == "Shirt, Shade")              paste_gs_code("8107ECB0 7F7F\n8107ECB2 0000");
                if (shadeName == "Shoulders, Shade")          paste_gs_code("8107ECC8 007F\n8107ECCA 7F00");
                if (shadeName == "Arms, Shade")               paste_gs_code("8107ECE0 007F\n8107ECE2 4000");
                if (shadeName == "Overalls (Bottom), Shade")  paste_gs_code("8107ECF8 7F00\n8107ECFA 7F00");
                if (shadeName == "Leg (Top), Shade")          paste_gs_code("8107ED10 7F00\n8107ED12 4000");
                if (shadeName == "Leg (Bottom), Shade")       paste_gs_code("8107ED28 4000\n8107ED2A 7F00");
                set_editor_from_global_cc("");
            }
            if (ImGui::Selectable("Randomize")) {
                shadeColorValue->x = (rand() % 127) / 255.0f;
                shadeColorValue->y = (rand() % 127) / 255.0f;
                shadeColorValue->z = (rand() % 127) / 255.0f;
            }
            ImGui::EndPopup();
        }

        //paste_gs_code(ui_gameshark_input);
        //set_editor_from_global_cc("Sample");

        ImGui::SameLine();
        ImGui::Text(name);
    }
}

int numColorCodes;

void sdynos_imgui_init() {
    saturn_load_cc_directory();
    //saturn_load_eye_directory();
    saturn_load_eye_folder("");
    strcpy(ui_gameshark, global_gs_code().c_str());

    model_details = "" + std::to_string(sDynosPacks.Count()) + " model pack";
    if (sDynosPacks.Count() != 1) model_details += "s";
}

void sdynos_imgui_menu() {
    if (ImGui::BeginMenu(ICON_FK_USER_CIRCLE " Edit Avatar###menu_edit_avatar")) {

        ImGui::Text("Color Codes");
        ImGui::SameLine(); imgui_bundled_help_marker(
            "These are GameShark color codes, which overwrite Mario's lights. Place GS files in dynos/colorcodes.");

        if (cc_array.size() >= 18) {
            ImGui::InputTextWithHint("###cc_search_text", ICON_FK_SEARCH " Search color codes...", ccSearchTerm, IM_ARRAYSIZE(ccSearchTerm), ImGuiInputTextFlags_AutoSelectAll);
            if (ImGui::IsItemActivated()) accept_text_input = false;
            if (ImGui::IsItemDeactivated()) accept_text_input = true;
        } else {
            // If our CC list is reloaded, and we now have less than 18 files, this can cause filter issues if not reset to nothing
            if (ccSearchTerm != "") strcpy(ccSearchTerm, "");
        }
        string ccSearchLower = ccSearchTerm;
        std::transform(ccSearchLower.begin(), ccSearchLower.end(), ccSearchLower.begin(),
            [](unsigned char c){ return std::tolower(c); });

        ImGui::BeginChild("###menu_cc_selector", ImVec2(-FLT_MIN, 100), true);
        if (any_packs_selected) {
            if (model_cc_array.size() > 0 && current_model_data.cc_support && !has_open_any_model_cc) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::TreeNode((ui_mfolder_name + "###model_cc_header").c_str())) {
                    for (int n = 0; n < model_cc_array.size(); n++) {
                        const bool is_selected = (current_mcc_id == n);
                        cc_name = model_cc_array[n].substr(0, model_cc_array[n].size() - 3);

                        // If we're searching, only include CCs with the search keyword in the name
                        // Also convert to lowercase
                        if (ccSearchLower != "") {
                            string nameLower = cc_name;
                            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                                [](unsigned char c1){ return std::tolower(c1); });

                            if (nameLower.find(ccSearchLower) == string::npos) {
                                continue;
                            }
                        }

                        string label = cc_name;
                        if (model_cc_array[n] == "../default.gs")
                            label = ui_mfolder_name;

                        if (ImGui::Selectable((ICON_FK_USER " " + cc_name).c_str(), is_selected)) {
                            current_mcc_id = n;
                            current_cc_id = -1;
                            set_cc_from_model(ui_mfolder_path + "/colorcodes/" + cc_name + ".gs");
                            set_editor_from_global_cc(label);
                            ui_model_gameshark = ui_gameshark;
                            last_model_cc_address = ui_model_gameshark;
                        }

                        if (ImGui::BeginPopupContextItem()) {
                            ImGui::Text("%s.gs", cc_name.c_str());
                            imgui_bundled_tooltip(("/dynos/packs/" + label + "/colorcodes/" + cc_name + ".gs").c_str());
                            if (cc_name != "../default") {
                                if (ImGui::SmallButton(ICON_FK_TRASH_O " Delete File")) {
                                    delete_model_cc_file(cc_name, label);
                                    ImGui::CloseCurrentPopup();
                                } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                            }
                            ImGui::Separator();
                            if (cc_spark_support) {
                                if (ImGui::Button("SPARKILIZE###cc_context_sparkilize")) {
                                    current_mcc_id = n;
                                    current_cc_id = -1;
                                    set_cc_from_model(ui_mfolder_path + "/colorcodes/" + cc_name + ".gs");
                                    set_editor_from_global_cc(label);
                                    ui_model_gameshark = ui_gameshark;
                                    last_model_cc_address = ui_model_gameshark;

                                    uiShirtColor = uiHatColor;
                                    uiShirtShadeColor = uiHatShadeColor;
                                    uiShouldersColor= uiHatColor;
                                    uiShouldersShadeColor = uiShirtShadeColor;
                                    uiArmsColor = uiHatColor;
                                    uiArmsShadeColor = uiShirtShadeColor;
                                    uiOverallsBottomColor = uiOverallsColor;
                                    uiOverallsBottomShadeColor = uiOverallsShadeColor;
                                    uiLegTopColor = uiOverallsColor;
                                    uiLegTopShadeColor = uiOverallsShadeColor;
                                    uiLegBottomColor = uiOverallsColor;
                                    uiLegBottomShadeColor = uiOverallsShadeColor;
                                    apply_cc_from_editor();
                                } ImGui::SameLine(); imgui_bundled_help_marker("Automatically converts a regular CC to a SPARK CC; WARNING: This will overwrite your active color code.");

                                ImGui::Separator();
                            }
                            ImGui::TextDisabled("%i model color code(s)", model_cc_array.size());
                            if (ImGui::Button(ICON_FK_UNDO " Refresh")) {
                                get_ccs_from_model(ui_mfolder_path);
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        }
                    }
                    ImGui::TreePop();
                }
            }
        }
        for (int n = 0; n < cc_array.size(); n++) {
            const bool is_selected = (current_cc_id == n);
            cc_name = cc_array[n].substr(0, cc_array[n].size() - 3);

            // If we're searching, only include CCs with the search keyword in the name
            // Also convert to lowercase
            if (ccSearchLower != "") {
                string nameLower = cc_name;
                std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                    [](unsigned char c1){ return std::tolower(c1); });

                if (nameLower.find(ccSearchLower) == string::npos) {
                    continue;
                }
            }

            if (ImGui::Selectable(cc_name.c_str(), is_selected)) {
                current_cc_id = n;
                current_mcc_id = -1;
                load_cc_file((char*)cc_array[current_cc_id].c_str());
                set_editor_from_global_cc(cc_array[current_cc_id].substr(0, cc_array[current_cc_id].size() - 3));

                cc_details = "" + std::to_string(cc_array.size()) + " color code";
                if (cc_array.size() != 1) cc_details += "s";
            }

            if (ImGui::BeginPopupContextItem()) {
                if (cc_name != "Mario") {
                    ImGui::Text("%s.gs", cc_name.c_str());
                    imgui_bundled_tooltip(("/dynos/colorcodes/" + cc_name + ".gs").c_str());
                    if (ImGui::SmallButton(ICON_FK_TRASH_O " Delete File")) {
                        delete_cc_file(cc_name);
                        current_cc_id = -1;
                        ImGui::CloseCurrentPopup();
                    } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                    ImGui::Separator();
                }
                if (cc_spark_support) {
                    if (ImGui::Button("SPARKILIZE###cc_context_sparkilize")) {
                        current_cc_id = n;
                        load_cc_file((char*)cc_array[current_cc_id].c_str());
                        set_editor_from_global_cc(cc_array[current_cc_id].substr(0, cc_array[current_cc_id].size() - 3));

                        uiShirtColor = uiHatColor;
                        uiShirtShadeColor = uiHatShadeColor;
                        uiShouldersColor= uiHatColor;
                        uiShouldersShadeColor = uiShirtShadeColor;
                        uiArmsColor = uiHatColor;
                        uiArmsShadeColor = uiShirtShadeColor;
                        uiOverallsBottomColor = uiOverallsColor;
                        uiOverallsBottomShadeColor = uiOverallsShadeColor;
                        uiLegTopColor = uiOverallsColor;
                        uiLegTopShadeColor = uiOverallsShadeColor;
                        uiLegBottomColor = uiOverallsColor;
                        uiLegBottomShadeColor = uiOverallsShadeColor;
                        apply_cc_from_editor();
                    } ImGui::SameLine(); imgui_bundled_help_marker("Automatically converts a regular CC to a SPARK CC; WARNING: This will overwrite your active color code.");

                    ImGui::Separator();
                }
                ImGui::TextDisabled("%i color code(s)", cc_array.size());
                if (ImGui::Button(ICON_FK_UNDO " Refresh")) {
                    saturn_load_cc_directory();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::EndChild();
        if (ImGui::Button(ICON_FK_FILE_TEXT_O " Add CC File...###add_v_cc")) {
            auto selection3 = pfd::open_file("Select a file", ".",
                        { "Color Code Files", "*.gs *.txt",
                            "All Files", "*" },
                        pfd::opt::multiselect).result();

            // Do something with selection
            for (auto const &filename3 : selection3) {
                saturn_copy_file(filename3, "dynos/colorcodes/");
                saturn_load_cc_directory();
            }
        }

        ImGui::Text("Model Packs");
        ImGui::SameLine(); imgui_bundled_help_marker(
            "DynOS v1.1 by PeachyPeach\n\nThese are DynOS model packs, used for live model loading.\nPlace packs in /dynos/packs.");

        if (sDynosPacks.Count() >= 20) {
            ImGui::InputTextWithHint("###model_search_text", ICON_FK_SEARCH " Search models...", modelSearchTerm, IM_ARRAYSIZE(modelSearchTerm), ImGuiInputTextFlags_AutoSelectAll);
            if (ImGui::IsItemActivated()) accept_text_input = false;
            if (ImGui::IsItemDeactivated()) accept_text_input = true;
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
                    saturn_load_model_data(label);
                    for (int i = 0; i < 8; i++) {
                        current_exp_index[i] = 0;
                    }

                    if (is_selected && current_model_data.name != "")
                        std::cout << "Loaded " << current_model_data.name << " by " << current_model_data.author << std::endl;

                    // Load model CCs

                    get_ccs_from_model(sDynosPacks[i]->mPath);
                    current_mcc_id = -1;
                    current_model_id = i;

                    model_details = "" + std::to_string(sDynosPacks.Count()) + " model pack";
                    if (sDynosPacks.Count() != 1) model_details += "s";

                    if (is_default_cc(ui_gameshark)) {
                        if (model_cc_array.size() > 0) {
                            current_mcc_id = 0;
                            current_cc_id = -1;
                            set_cc_from_model(sDynosPacks[i]->mPath + "/colorcodes/" + model_cc_array[0].substr(0, model_cc_array[0].size()));
                            set_editor_from_global_cc("Sample");
                            ui_model_gameshark = ui_gameshark;
                            last_model_cc_address = ui_model_gameshark;
                        }
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
                        if (ui_model_gameshark == ui_gameshark) {
                            ui_model_gameshark = "";
                            current_cc_id = 0;
                            load_cc_file((char*)cc_array[current_cc_id].c_str());
                            set_editor_from_global_cc("Sample");
                        }

                        // Reset model data

                        ModelData blank;
                        current_model_data = blank;
                        using_model_eyes = false;
                        cc_model_support = true;
                        cc_spark_support = false;
                        set_editor_from_global_cc("Sample");
                        strcpy(ui_gameshark, global_gs_code().c_str());
                        enable_torso_rotation = true;
                    }
                }
                if (ImGui::BeginPopupContextItem()) {
                    get_ccs_from_model(sDynosPacks[i]->mPath);
                    has_open_any_model_cc = true;
                    ImGui::Text("%s/", label.c_str());
                    imgui_bundled_tooltip(("/dynos/packs/" + label).c_str());
                    ImGui::BeginChild("###menu_model_ccs", ImVec2(125, 75), true);
                    for (int n = 0; n < model_cc_array.size(); n++) {
                        const bool is_selected = (current_mcc_id == n && sDynosPacks[i]->mPath == ui_mfolder_path);
                        cc_name = model_cc_array[n].substr(0, model_cc_array[n].size() - 3);

                        if (ImGui::Selectable(cc_name.c_str(), is_selected)) {
                            if (sDynosPacks[i]->mPath == ui_mfolder_path) {
                                current_mcc_id = n;
                                current_cc_id = -1;
                            }
                            set_cc_from_model(sDynosPacks[i]->mPath + "/colorcodes/" + cc_name + ".gs");
                            set_editor_from_global_cc("Sample");
                            ui_model_gameshark = ui_gameshark;
                            last_model_cc_address = ui_model_gameshark;
                        }

                        if (ImGui::BeginPopupContextItem()) {
                            ImGui::Text("%s.gs", cc_name.c_str());
                            imgui_bundled_tooltip(("/dynos/packs/" + label + "/colorcodes/" + cc_name + ".gs").c_str());
                            if (cc_name != "../default") {
                                if (ImGui::SmallButton(ICON_FK_TRASH_O " Delete File")) {
                                    delete_model_cc_file(cc_name, label);
                                    ImGui::CloseCurrentPopup();
                                } ImGui::SameLine(); imgui_bundled_help_marker("WARNING: This action is irreversible!");
                            }
                            ImGui::Separator();
                            if (cc_spark_support) {
                                if (ImGui::Button("SPARKILIZE###cc_context_sparkilize")) {
                                    if (sDynosPacks[i]->mPath == ui_mfolder_path) {
                                        current_mcc_id = n;
                                        current_cc_id = -1;
                                    }
                                    set_cc_from_model(sDynosPacks[i]->mPath + "/colorcodes/" + cc_name + ".gs");
                                    set_editor_from_global_cc("Sample");
                                    ui_model_gameshark = ui_gameshark;
                                    last_model_cc_address = ui_model_gameshark;

                                    uiShirtColor = uiHatColor;
                                    uiShirtShadeColor = uiHatShadeColor;
                                    uiShouldersColor= uiHatColor;
                                    uiShouldersShadeColor = uiShirtShadeColor;
                                    uiArmsColor = uiHatColor;
                                    uiArmsShadeColor = uiShirtShadeColor;
                                    uiOverallsBottomColor = uiOverallsColor;
                                    uiOverallsBottomShadeColor = uiOverallsShadeColor;
                                    uiLegTopColor = uiOverallsColor;
                                    uiLegTopShadeColor = uiOverallsShadeColor;
                                    uiLegBottomColor = uiOverallsColor;
                                    uiLegBottomShadeColor = uiOverallsShadeColor;
                                    apply_cc_from_editor();
                                } ImGui::SameLine(); imgui_bundled_help_marker("Automatically converts a regular CC to a SPARK CC; WARNING: This will overwrite your active color code.");

                                ImGui::Separator();
                            }
                            ImGui::TextDisabled("%i color code(s)", model_cc_array.size());
                            if (ImGui::Button(ICON_FK_UNDO " Refresh")) {
                                get_ccs_from_model(sDynosPacks[i]->mPath);
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
                        DynOS_Gfx_Init();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine(); imgui_bundled_help_marker("WARNING: Experimental - this will probably lag the game.");
                    ImGui::EndPopup();
                } else {
                    if (has_open_any_model_cc) {
                        has_open_any_model_cc = false;
                        get_ccs_from_model(ui_mfolder_path);
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

        if (mario_exists) {
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

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("Misc.###misc_child", ImVec2(225, 150), true, ImGuiWindowFlags_None);
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
                ImGui::ColorEdit4("Col###wlight_col", gLightingColor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoOptions);
                saturn_keyframe_color_popout("Light Color", "k_light_col");

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
            if (ImGui::BeginTabItem("Scale###tab_scale")) {
                if (linkMarioScale) {
                    ImGui::Dummy(ImVec2(8, 0)); ImGui::SameLine(); ImGui::SliderFloat("Size###mscale_all", &marioScaleSizeX, 0.f, 2.f);
                    saturn_keyframe_float_popout(&marioScaleSizeX, "Mario Scale", "k_scale");
                } else {
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("X###mscale_x", &marioScaleSizeX, -2.f, 2.f);
                    saturn_keyframe_float_popout(&marioScaleSizeX, "Mario Scale X", "k_scale_x");
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("Y###mscale_y", &marioScaleSizeY, -2.f, 2.f);
                    saturn_keyframe_float_popout(&marioScaleSizeY, "Mario Scale Y", "k_scale_y");
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::SliderFloat("Z###mscale_z", &marioScaleSizeZ, -2.f, 2.f);
                    saturn_keyframe_float_popout(&marioScaleSizeZ, "Mario Scale Z", "k_scale_z");
                }
                ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine(); ImGui::Checkbox("Link###link_mario_scale", &linkMarioScale);
                if (marioScaleSizeX != 1.f || marioScaleSizeY != 1.f || marioScaleSizeZ != 1.f) {
                    ImGui::Dummy(ImVec2(15, 0)); ImGui::SameLine();
                    if (ImGui::Button("Reset###reset_mscale")) {
                        marioScaleSizeX = 1.f;
                        marioScaleSizeY = 1.f;
                        marioScaleSizeZ = 1.f;
                    }
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

            ImGui::Dummy(ImVec2(0, 5));

            if (!any_packs_selected) {
                ImGui::Checkbox("M Cap Emblem", &show_vmario_emblem);
                imgui_bundled_tooltip("Enables the signature \"M\" logo on Mario's cap.");
                saturn_keyframe_bool_popout(&show_vmario_emblem, "M Cap Emblem", "k_v_cap_emblem");
            }

            ImGui::TableSetColumnIndex(1);
            if (gMarioState) {
                if (ImGuiKnobs::Knob("Angle", &this_face_angle, -180.f, 180.f, 0.f, "%.0f deg", ImGuiKnobVariant_Dot, 0.f, ImGuiKnobFlags_DragHorizontal)) {
                    gMarioState->faceAngle[1] = (s16)(this_face_angle * 182.04f);
                } else {
                    this_face_angle = (float)gMarioState->faceAngle[1] / 182.04;
                }
                saturn_keyframe_float_popout((float*)&gMarioState->faceAngle[1], "Mario Angle", "k_angle");
            }

            ImGui::Checkbox("Spin###spin_angle", &is_spinning);
            if (is_spinning) {
                ImGui::SliderFloat("Speed###spin,speed", &spin_mult, -2.f, 2.f, "%.1f");
            }

            if (this_face_angle > 180) this_face_angle = -180;
            if (this_face_angle < -180) this_face_angle = 180;

            ImGui::EndTable();
        }

        /*ImGui::Checkbox("Head Rotations", &enable_head_rotations);
        imgui_bundled_tooltip("Whether or not Mario's head rotates in his idle animation.");
        ImGui::Checkbox("Dust Particles", &enable_dust_particles);
        imgui_bundled_tooltip("Displays dust particles when Mario moves.");
        ImGui::Checkbox("Torso Rotations", &enable_torso_rotation);
        imgui_bundled_tooltip("Tilts Mario's torso when he moves; Disable for a \"beta running\" effect.");
        */

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
                        preview_name = ICON_FK_FOLDER_O " " + eye_array[n];

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
                    auto selection = pfd::open_file("Select a file", ".",
                                { "PNG Textures", "*.png ",
                                  "All Files", "*" },
                                pfd::opt::multiselect).result();

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
                                saturn_load_model_data(current_folder_name);
                                for (int i = 0; i < 8; i++) {
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
                            saturn_load_model_data(current_folder_name);
                            for (int i = 0; i < 8; i++) {
                                current_exp_index[i] = 0;
                            }
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndChild();
                if (ImGui::Button(ICON_FK_FILE_IMAGE_O " Add Eye...###add_m_eye")) {
                    auto selection1 = pfd::open_file("Select a file", ".",
                                { "PNG Textures", "*.png ",
                                  "All Files", "*" },
                                pfd::opt::multiselect).result();

                    // Do something with selection
                    for (auto const &filename1 : selection1) {
                        saturn_copy_file(filename1, "dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/");
                        saturn_load_model_data(current_folder_name);
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
                                            saturn_load_model_data(current_folder_name);
                                            for (int i = 0; i < 8; i++) {
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
                            auto selection1 = pfd::open_file("Select a file", ".",
                                        { "PNG Textures", "*.png ",
                                        "All Files", "*" },
                                        pfd::opt::multiselect).result();

                            // Do something with selection
                            for (auto const &filename1 : selection1) {
                                saturn_copy_file(filename1, "dynos/packs/" + current_folder_name + "/expressions/" + expression.name + "/");
                                saturn_load_model_data(current_folder_name);
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::Separator();
                        ImGui::TextDisabled("%i %s expression(s)", expression.textures.size(), expression.name.c_str());
                        if (ImGui::Button(ICON_FK_UNDO " Refresh###refresh_m_exp")) {
                            saturn_load_model_data(current_folder_name);
                            for (int i = 0; i < 8; i++) {
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

void imgui_dynos_cc_editor() {
    if (!cc_model_support) {
        ImGui::BeginDisabled();
        if (current_model_data.name != "") {
            ImGui::TextWrapped("%s does not include color code features", current_model_data.name.c_str());
        } else {
            ImGui::TextWrapped("Color code support is disabled");
        }
        ImGui::EndDisabled();
        return;
    }

    ImGui::PushItemWidth(100);
    ImGui::InputText(".gs", ui_cc_name, IM_ARRAYSIZE(ui_cc_name));
    if (ImGui::IsItemActivated()) accept_text_input = false;
    if (ImGui::IsItemDeactivated()) accept_text_input = true;
    ImGui::PopItemWidth();

    ImGui::SameLine(150);

    //ImGui::Text("Save to:"); ImGui::SameLine(); ImGui::Dummy(ImVec2(5, 0));
    //ImGui::SameLine();
    if (ImGui::Button(ICON_FK_FILE_TEXT " File###save_cc_to_file")) {
        apply_cc_from_editor();
        std::string cc_name = ui_cc_name;
        // Filter out potential paths
        std::replace(cc_name.begin(), cc_name.end(), '.', '-');
        std::replace(cc_name.begin(), cc_name.end(), '/', '-');
        std::replace(cc_name.begin(), cc_name.end(), '\\', '-');
        // We don't want to save a CC named "Mario", as it may cause file issues.
        if (cc_name != "Mario") {
            save_cc_file(cc_name, global_gs_code());
        } else {
            strcpy(ui_cc_name, "Sample");
            save_cc_file("Sample", global_gs_code());
        }
        saturn_load_cc_directory();
    }
    ImGui::Text("");
    ImGui::SameLine(150);
    if (sDynosPacks.Count() > 0 && one_pack_selectable && cc_model_support) {
        string buttonLabel = ICON_FK_FOLDER_OPEN_O " Model###save_cc_to_model";
        if (ImGui::Button(buttonLabel.c_str())) {
            apply_cc_from_editor();
            std::string cc_name = ui_cc_name;
            // Filter out potential paths
            std::replace(cc_name.begin(), cc_name.end(), '.', '-');
            std::replace(cc_name.begin(), cc_name.end(), '/', '-');
            std::replace(cc_name.begin(), cc_name.end(), '\\', '-');
            // We don't want to save a CC named "Mario", as it may cause file issues.
            if (cc_name != "Mario") {
                save_cc_model_file(cc_name, global_gs_code(), ui_mfolder_name);
            } else {
                strcpy(ui_cc_name, "Sample");
                save_cc_model_file("Sample", global_gs_code(), ui_mfolder_name);
            }
            saturn_load_cc_directory();
        }
        string tooltipLabel = "dynos/packs/" + ui_mfolder_name + "/colorcodes/" + ui_cc_name;
        imgui_bundled_tooltip(tooltipLabel.c_str());
    }

    ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::Button(ICON_FK_UNDO " Reset Colors")) {
        current_cc_id = 0;

        if (any_packs_selected && model_cc_array.size() > 0) {
            get_ccs_from_model(sDynosPacks[current_model_id]->mPath);
            if (cc_model_support) {
                set_cc_from_model(sDynosPacks[current_model_id]->mPath + "/colorcodes/" + model_cc_array[0].substr(0, model_cc_array[0].size()));
                set_editor_from_global_cc("Sample");
                ui_model_gameshark = ui_gameshark;
                last_model_cc_address = ui_model_gameshark;
            }
        } else {
            load_cc_file((char*)cc_array[current_cc_id].c_str());
            set_editor_from_global_cc("Sample");
        }
    }

    ImGui::SameLine(); if (ImGui::SmallButton(ICON_FK_RANDOM "###randomize_all")) {
        current_cc_id = -1;

        uiHatColor.x = (rand() % 255) / 255.0f; uiHatColor.y = (rand() % 255) / 255.0f; uiHatColor.z = (rand() % 255) / 255.0f;
        uiHatShadeColor.x = (rand() % 127) / 255.0f; uiHatShadeColor.y = (rand() % 127) / 255.0f; uiHatShadeColor.z = (rand() % 127) / 255.0f;
        uiOverallsColor.x = (rand() % 255) / 255.0f; uiOverallsColor.y = (rand() % 255) / 255.0f; uiOverallsColor.z = (rand() % 255) / 255.0f;
        uiOverallsShadeColor.x = (rand() % 127) / 255.0f; uiOverallsShadeColor.y = (rand() % 127) / 255.0f; uiOverallsShadeColor.z = (rand() % 127) / 255.0f;
        uiGlovesColor.x = (rand() % 255) / 255.0f; uiGlovesColor.y = (rand() % 255) / 255.0f; uiGlovesColor.z = (rand() % 255) / 255.0f;
        uiGlovesShadeColor.x = (rand() % 127) / 255.0f; uiGlovesShadeColor.y = (rand() % 127) / 255.0f; uiGlovesShadeColor.z = (rand() % 127) / 255.0f;
        uiShoesColor.x = (rand() % 255) / 255.0f; uiShoesColor.y = (rand() % 255) / 255.0f; uiShoesColor.z = (rand() % 255) / 255.0f;
        uiShoesShadeColor.x = (rand() % 127) / 255.0f; uiShoesShadeColor.y = (rand() % 127) / 255.0f; uiShoesShadeColor.z = (rand() % 127) / 255.0f;
        uiSkinColor.x = (rand() % 255) / 255.0f; uiSkinColor.y = (rand() % 255) / 255.0f; uiSkinColor.z = (rand() % 255) / 255.0f;
        uiSkinShadeColor.x = (rand() % 127) / 255.0f; uiSkinShadeColor.y = (rand() % 127) / 255.0f; uiSkinShadeColor.z = (rand() % 127) / 255.0f;
        uiHairColor.x = (rand() % 255) / 255.0f; uiHairColor.y = (rand() % 255) / 255.0f; uiHairColor.z = (rand() % 255) / 255.0f;
        uiHairShadeColor.x = (rand() % 127) / 255.0f; uiHairShadeColor.y = (rand() % 127) / 255.0f; uiHairShadeColor.z = (rand() % 127) / 255.0f;

        if (cc_spark_support) {
            uiShirtColor.x = (rand() % 255) / 255.0f; uiShirtColor.y = (rand() % 255) / 255.0f; uiShirtColor.z = (rand() % 255) / 255.0f;
            uiShirtShadeColor.x = (rand() % 127) / 255.0f; uiShirtShadeColor.y = (rand() % 127) / 255.0f; uiShirtShadeColor.z = (rand() % 127) / 255.0f;
            uiShouldersColor.x = (rand() % 255) / 255.0f; uiShouldersColor.y = (rand() % 255) / 255.0f; uiShouldersColor.z = (rand() % 255) / 255.0f;
            uiShouldersShadeColor.x = (rand() % 127) / 255.0f; uiShouldersShadeColor.y = (rand() % 127) / 255.0f; uiShouldersShadeColor.z = (rand() % 127) / 255.0f;
            uiArmsColor.x = (rand() % 255) / 255.0f; uiArmsColor.y = (rand() % 255) / 255.0f; uiArmsColor.z = (rand() % 255) / 255.0f;
            uiArmsShadeColor.x = (rand() % 127) / 255.0f; uiArmsShadeColor.y = (rand() % 127) / 255.0f; uiArmsShadeColor.z = (rand() % 127) / 255.0f;
            uiOverallsBottomColor.x = (rand() % 255) / 255.0f; uiOverallsBottomColor.y = (rand() % 255) / 255.0f; uiOverallsBottomColor.z = (rand() % 255) / 255.0f;
            uiOverallsBottomShadeColor.x = (rand() % 127) / 255.0f; uiOverallsBottomShadeColor.y = (rand() % 127) / 255.0f; uiOverallsBottomShadeColor.z = (rand() % 127) / 255.0f;
            uiLegTopColor.x = (rand() % 255) / 255.0f; uiLegTopColor.y = (rand() % 255) / 255.0f; uiLegTopColor.z = (rand() % 255) / 255.0f;
            uiLegTopShadeColor.x = (rand() % 127) / 255.0f; uiLegTopShadeColor.y = (rand() % 127) / 255.0f; uiLegTopShadeColor.z = (rand() % 127) / 255.0f;
            uiLegBottomColor.x = (rand() % 255) / 255.0f; uiLegBottomColor.y = (rand() % 255) / 255.0f; uiLegBottomColor.z = (rand() % 255) / 255.0f;
            uiLegBottomShadeColor.x = (rand() % 127) / 255.0f; uiLegBottomShadeColor.y = (rand() % 127) / 255.0f; uiLegBottomShadeColor.z = (rand() % 127) / 255.0f;
        }

        apply_cc_from_editor();
    } imgui_bundled_tooltip("Randomizes all color values; WARNING: This will overwrite your active color code.");

    if (ImGui::BeginTabBar("###dynos_tabbar", ImGuiTabBarFlags_None)) {

        if (ImGui::BeginTabItem("Editor")) {
            handle_cc_box(current_model_data.hat_label.c_str(), "Hat, Main", "Hat, Shade", &uiHatColor, &uiHatShadeColor, "1/2###hat_half");
            handle_cc_box(current_model_data.overalls_label.c_str(), "Overalls, Main", "Overalls, Shade", &uiOverallsColor, &uiOverallsShadeColor, "1/2###overalls_half");
            handle_cc_box(current_model_data.gloves_label.c_str(), "Gloves, Main", "Gloves, Shade", &uiGlovesColor, &uiGlovesShadeColor, "1/2###gloves_half");
            handle_cc_box(current_model_data.shoes_label.c_str(), "Shoes, Main", "Shoes, Shade", &uiShoesColor, &uiShoesShadeColor, "1/2###shoes_half");
            handle_cc_box(current_model_data.skin_label.c_str(), "Skin, Main", "Skin, Shade", &uiSkinColor, &uiSkinShadeColor, "1/2###skin_half");
            handle_cc_box(current_model_data.hair_label.c_str(), "Hair, Main", "Hair, Shade", &uiHairColor, &uiHairShadeColor, "1/2###hair_half");

            if (cc_spark_support) {
                if (ImGui::SmallButton("v SPARKILIZE v###cc_editor_sparkilize")) {
                    uiShirtColor = uiHatColor;
                    uiShirtShadeColor = uiHatShadeColor;
                    uiShouldersColor= uiHatColor;
                    uiShouldersShadeColor = uiShirtShadeColor;
                    uiArmsColor = uiHatColor;
                    uiArmsShadeColor = uiShirtShadeColor;
                    uiOverallsBottomColor = uiOverallsColor;
                    uiOverallsBottomShadeColor = uiOverallsShadeColor;
                    uiLegTopColor = uiOverallsColor;
                    uiLegTopShadeColor = uiOverallsShadeColor;
                    uiLegBottomColor = uiOverallsColor;
                    uiLegBottomShadeColor = uiOverallsShadeColor;
                    apply_cc_from_editor();
                } ImGui::SameLine(); imgui_bundled_help_marker("Automatically converts a regular CC to a SPARK CC; WARNING: This will overwrite your active color code.");

                handle_cc_box(current_model_data.shirt_label.c_str(), "Shirt, Main", "Shirt, Shade", &uiShirtColor, &uiShirtShadeColor, "1/2###shirt_half");
                handle_cc_box(current_model_data.shoulders_label.c_str(), "Shoulders, Main", "Shoulders, Shade", &uiShouldersColor, &uiShouldersShadeColor, "1/2###shoulders_half");
                handle_cc_box(current_model_data.arms_label.c_str(), "Arms, Main", "Arms, Shade", &uiArmsColor, &uiArmsShadeColor, "1/2###arms_half");
                handle_cc_box(current_model_data.pelvis_label.c_str(), "Overalls (Bottom), Main", "Overalls (Bottom), Shade", &uiOverallsBottomColor, &uiOverallsBottomShadeColor, "1/2###overalls_bottom_half");
                handle_cc_box(current_model_data.thighs_label.c_str(), "Leg (Top), Main", "Leg (Top), Shade", &uiLegTopColor, &uiLegTopShadeColor, "1/2###leg_top_half");
                handle_cc_box(current_model_data.calves_label.c_str(), "Leg (Bottom), Main", "Leg (Bottom), Shade", &uiLegBottomColor, &uiLegBottomShadeColor, "1/2###leg_bottom_half");
            }

            ImGui::Dummy(ImVec2(0, 5));

            if (!configEditorFastApply) {
                if (ImGui::Button(ICON_FK_CLIPBOARD " Apply CC###apply_cc_from_editor")) {
                    apply_cc_from_editor();
                }
                ImGui::Dummy(ImVec2(0, 5));
            } else {
                apply_cc_from_editor();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("GameShark")) {
            ImGui::InputTextMultiline("###gameshark_box", ui_gameshark, IM_ARRAYSIZE(ui_gameshark), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25), ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
            if (ImGui::IsItemActivated()) accept_text_input = false;
            if (ImGui::IsItemDeactivated()) accept_text_input = true;

            if (ImGui::Button(ICON_FK_CLIPBOARD " Apply GS Code")) {
                string ui_gameshark_input = ui_gameshark;
                paste_gs_code(ui_gameshark_input);
                set_editor_from_global_cc("Sample");
                strcpy(ui_gameshark, global_gs_code().c_str());
            } ImGui::SameLine(); imgui_bundled_help_marker(
                "Copy/paste a GameShark color code from here!");

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}