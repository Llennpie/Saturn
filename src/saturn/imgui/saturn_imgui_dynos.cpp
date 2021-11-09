#include "saturn_imgui_dynos.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn_imgui.h"
#include "data/dynos.cpp.h"
#include <SDL2/SDL.h>

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/level_update.h"
#include <mario_animation_ids.h>
}

const Array<PackData *> &sDynosPacks = DynOS_Gfx_GetPacks();

using namespace std;



void sdynos_imgui_init() {
    load_cc_directory();
}

void sdynos_imgui_update() {

    if (cc_array.size() > 0) {
        ImGui::Text("Select Color Code");
        static int current_cc_id = 0;
        string cc_name = cc_array[current_cc_id].substr(0, cc_array[current_cc_id].size() - 3);
        if (ImGui::BeginCombo(".gs", cc_name.c_str()))
        {
            for (int n = 0; n < cc_array.size(); n++)
            {
                const bool is_selected = (current_cc_id == n);
                cc_name = cc_array[n].substr(0, cc_array[n].size() - 3);
                if (ImGui::Selectable(cc_name.c_str(), is_selected)) {
                    current_cc_id = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine(); imgui_bundled_help_marker("These are GameShark color codes, which overwrite Mario's lights. Place in dynos/colorcodes.");
        if (ImGui::Button("Load CC")) {
            load_cc_file((char*)cc_array[current_cc_id].c_str());
        }
    } else {
        ImGui::Text("Could not load color code directory...");
    }

    imgui_bundled_space(20, "Model Packs","These are DynOS model packs, used for live model loading.\nPlace packs in dynos/packs.");
    if (ImGui::BeginListBox("", ImVec2(200, 150))) {
        for (int i = 0; i < sDynosPacks.Count(); i++) {
            u64 _DirSep1 = sDynosPacks[i]->mPath.find_last_of('\\');
            u64 _DirSep2 = sDynosPacks[i]->mPath.find_last_of('/');
            if (_DirSep1++ == SysPath::npos) _DirSep1 = 0;
            if (_DirSep2++ == SysPath::npos) _DirSep2 = 0;

            std::string label = sDynosPacks[i]->mPath.substr(MAX(_DirSep1, _DirSep2));
            bool selected = DynOS_Opt_GetValue(String("dynos_pack_%d", i));

            if (ImGui::Selectable(label.c_str(), &selected)) {
                DynOS_Opt_SetValue(String("dynos_pack_%d", i), selected);
            }
        }
        ImGui::EndListBox();
    }
}