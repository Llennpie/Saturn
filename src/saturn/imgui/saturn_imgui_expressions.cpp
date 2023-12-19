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
            expression->Textures = LoadExpressionTextures(expression->FolderPath);
            expression->Folders = LoadExpressionFolders(expression->FolderPath);
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

/* Recursively loads an Expression's folders and textures into a selection tree */
void RecursiveSelector(Expression* expression, int depth, std::string parent_path) {
    // Regenerate folder contents
    expression->Folders = LoadExpressionFolders(parent_path);
    expression->Textures = LoadExpressionTextures(parent_path);

    // Runs for all subfolders
    for (int i = 0; i < expression->Folders.size(); i++) {
        TexturePath Folder = expression->Folders[i];
        // Only show folders in our current subfolder
        if (Folder.ParentPath() == parent_path || Folder.ParentPath() == parent_path + "/") {
            // Runs when the tree is open
            if (ImGui::TreeNode(Folder.FileName.c_str())) {
                if (ImGui::IsItemClicked())
                    expression->CurrentIndex = 0;
                // Repeat
                RecursiveSelector(expression, depth + 1, Folder.FilePath);
                ImGui::TreePop();
            }
        }
    }

    // Runs for all textures
    for (int i = 0; i < expression->Textures.size(); i++) {
        TexturePath Texture = expression->Textures[i];
        // Failsafe if the index was going out of bounds
        if (expression->CurrentIndex > expression->Textures.size())
            expression->CurrentIndex = 0;

        // Only show textures in our current subfolder
        if (Texture.ParentPath() == parent_path || Texture.ParentPath() == parent_path + "/") {
            // On select
            if (ImGui::Selectable(Texture.FileName.c_str(), expression->CurrentIndex == i)) {
                std::cout << "Clicked texture #" << i << " under folder #" << depth << std::endl; 
                expression->CurrentIndex = i;
            }
        }
    }
}

/* A selection menu for a single Expression; A larger, one-click child window */
void OpenEyeSelector(Expression* expression) {
    //if (expression->Textures.size() <= 0) {
    //    ImGui::TextWrapped("No eye textures found. Place PNG files in /%s.", expression->FolderPath.c_str());
    //    return;
    //};

    ImGui::BeginChild(("###menu_eye_%s", expression->Name.c_str()), ImVec2(200, 150), true);
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