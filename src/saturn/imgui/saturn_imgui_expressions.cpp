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

void OpenEyeSelector(Texpression* expression) {
    if (expression->Textures.size() <= 0) {
        ImGui::TextWrapped("No eye textures found. Place PNG files in /%s.", expression->FolderPath.c_str());
        return;
    };

    ImGui::BeginChild(("###menu_eye_%s", expression->Name.c_str()), ImVec2(-FLT_MIN, 75), true);
    for (int n = 0; n < expression->Textures.size(); n++) {
        bool is_selected = (expression->CurrentIndex == n);
        std::string entry_name = expression->Textures[n].FileName;

        if (ImGui::Selectable(entry_name.c_str(), is_selected)) {
            gfx_precache_textures();
            expression->CurrentIndex = n;
        }
    }
    ImGui::EndChild();
}

void OpenExpressionSelector() {
    // Custom Eyes
    if (custom_eyes_enabled) {
        if (current_model.UsingVanillaEyes())
            OpenEyeSelector(&VanillaEyes);
        else {
            // Models
            if (current_model.Expressions[0].Name == "eyes")
                OpenEyeSelector(&current_model.Expressions[0]);
        }
    }

    // Other Expressions
    for (int i = 0; i < current_model.Expressions.size(); i++) {
        if (current_model.Expressions[i].Name == "eyes") continue;

        std::string label_name = "###exp_menu_" + current_model.Expressions[i].Name;
        ImGui::Text(current_model.Expressions[i].Name.c_str());
        ImGui::SameLine(75);
        ImGui::PushItemWidth(150);
        if (ImGui::BeginCombo(label_name.c_str(), current_model.Expressions[i].Textures[current_model.Expressions[i].CurrentIndex].FileName.c_str(), ImGuiComboFlags_None)) {
            for (int n = 0; n < current_model.Expressions[i].Textures.size(); n++) {
                bool is_selected = (current_model.Expressions[i].CurrentIndex == n);
                std::string entry_name = current_model.Expressions[i].Textures[n].FileName;

                if (ImGui::Selectable(entry_name.c_str(), is_selected)) {
                    gfx_precache_textures();
                    current_model.Expressions[i].CurrentIndex = n;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
    }
}