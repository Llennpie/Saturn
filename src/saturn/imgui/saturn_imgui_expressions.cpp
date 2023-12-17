#include "saturn_imgui_dynos.h"

#include "saturn/libs/portable-file-dialogs.h"

#include <algorithm>
#include <string>

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
#include "data/dynos.cpp.h"
#include <SDL2/SDL.h>

#include "icons/IconsForkAwesome.h"

extern "C" {
#include "pc/gfx/gfx_pc.h"
}

extern void open_directory(std::string);

/* Creates a right-click menu for an Expression */
void ShowExpressionContextMenu(Expression* expression, int id) {
    std::string popupLabelId = "###menu_expression_popup_" + expression->Name;
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        ImGui::OpenPopup(popupLabelId.c_str());
    if (ImGui::BeginPopup(popupLabelId.c_str())) {
        // Label
        ImGui::Text(ICON_FK_EYE " %s/", expression->Name.c_str());
        imgui_bundled_tooltip(("/%s", expression->FolderPath).c_str());
        // Open Directory
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            open_directory(std::string(sys_exe_path()) + "/" + expression->FolderPath + "/");
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine(); ImGui::TextDisabled(" Exp #%i", id + 1);

        // Refresh Textures
        if (ImGui::Button(ICON_FK_REFRESH " Refresh Textures###refresh_textures")) {
            expression->Textures = LoadExpressionTextures(*expression);
            expression->Folders = LoadExpressionFolders(*expression);
            // We attempt to keep each currently-selected texture
            // If our index is out of bounds (e.g. PNG was deleted) reset it
            for (int i = 0; i < expression->Textures.size(); i++) {
                if (expression->CurrentIndex > i || expression->CurrentIndex < i)
                    expression->CurrentIndex = 0;
            }
            gfx_precache_textures();
            ImGui::CloseCurrentPopup();
        }

        // Total Expression Count (models only)
        if (current_model.Active) {
            ImGui::Separator();
            ImGui::TextDisabled("%i expression(s)", current_model.Expressions.size());
        }

        ImGui::EndPopup();
    }
}

void RecursiveSelector(Expression* expression, int recurse_index, std::string parent_path) {
    // Folders
    for (int n = recurse_index; n < expression->Folders.size(); n++) {
        TexturePath folder = expression->Folders[n];
        if (folder.ParentPath() != parent_path &&
            folder.ParentPath() != parent_path + "/") continue;

        if (ImGui::TreeNode(folder.FileName.c_str())) {
            RecursiveSelector(expression, recurse_index + 1, folder.FilePath);
            ImGui::TreePop();
        }
    }
    // Textures
    for (int i = 0; i < expression->Textures.size(); i++) {
        TexturePath texture = expression->Textures[i];
        if (texture.ParentPath() != parent_path &&
            texture.ParentPath() != parent_path + "/") continue;

        if (ImGui::Selectable(texture.FileName.c_str(), expression->CurrentIndex == i)) {
            expression->CurrentIndex = i;
        }
    }
}

/* A selection menu for a single Expression; A larger, one-click child window */
void OpenEyeSelector(Expression* expression) {
    if (expression->Textures.size() <= 0) {
        ImGui::TextWrapped("No eye textures found. Place PNG files in /%s.", expression->FolderPath.c_str());
        return;
    };

    ImGui::BeginChild(("###menu_eye_%s", expression->Name.c_str()), ImVec2(200, 125), true);
    RecursiveSelector(expression, 0, expression->FolderPath);
    ImGui::EndChild();
    ShowExpressionContextMenu(expression, 0);
}

/* Creates a nested expression selection menu for a model; Contains dropdown OR checkbox widgets */
void OpenExpressionSelector() {
    if (custom_eyes_enabled) {
        if (current_model.Expressions.size() > 0 && current_model.CustomEyeSupport)
        // Model Eye Selector
        OpenEyeSelector(&current_model.Expressions[0]);
    }

    if (!current_model.Active) return;
    if (current_model.Expressions.size() > 1) {
        // Other Expressions
        if (current_model.Expressions.size() > 8) ImGui::BeginChild(("###menu_exp_model"), ImVec2(200, 190), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
        for (int i = 0; i < current_model.Expressions.size(); i++) {
            if (current_model.Expressions[i].Name == "eyes") continue;

            std::string label_name = "###exp_label_" + current_model.Expressions[i].Name;
            ImGui::Text(current_model.Expressions[i].Name.c_str());
            ImGui::SameLine(75);
            ImGui::PushItemWidth(120);

            // Use checkbox
            if (current_model.Expressions[i].Textures.size() == 2 && (
                current_model.Expressions[i].Textures[0].FileName.find("default") != std::string::npos ||
                current_model.Expressions[i].Textures[1].FileName.find("default") != std::string::npos )) {
                    // Check which index "default.png" is (0 or 1) to determine default value
                    int select_index = (current_model.Expressions[i].Textures[0].FileName.find("default") != std::string::npos) ? 0 : 1;
                    int deselect_index = (current_model.Expressions[i].Textures[0].FileName.find("default") != std::string::npos) ? 1 : 0;
                    bool is_selected = (current_model.Expressions[i].CurrentIndex == select_index);

                    if (ImGui::Checkbox(label_name.c_str(), &is_selected)) {
                        //gfx_precache_textures();
                        if (is_selected) current_model.Expressions[i].CurrentIndex = select_index;
                        else current_model.Expressions[i].CurrentIndex = deselect_index;
                    }
                    ShowExpressionContextMenu(&current_model.Expressions[i], i);
            } else {
            // Use dropdown
                if (ImGui::BeginCombo(label_name.c_str(), current_model.Expressions[i].Textures[current_model.Expressions[i].CurrentIndex].FileName.c_str(), ImGuiComboFlags_None)) {
                    RecursiveSelector(&current_model.Expressions[i], 0, current_model.Expressions[i].FolderPath);
                    ImGui::EndCombo();
                }
                ShowExpressionContextMenu(&current_model.Expressions[i], i);
            }
            ImGui::PopItemWidth();
        }
        if (current_model.Expressions.size() > 8) ImGui::EndChild();
    }
}