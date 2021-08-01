#include "saturn_imgui_dynos.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn_imgui.h"
#include "saturn/saturn_animations.h"
#include "data/dynos.cpp.h"
#include <SDL2/SDL.h>

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/level_update.h"
#include <mario_animation_ids.h>
}

int anim_index = 113;
const Array<PackData *> &sDynosPacks = DynOS_Gfx_GetPacks();

using namespace std;



void sdynos_imgui_init() {

}

void sdynos_imgui_update() {
    ImGui::Text("Animations");
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Combo("", &anim_index, saturn_animations, IM_ARRAYSIZE(saturn_animations));
    selected_animation = (MarioAnimID)anim_index;
    if (ImGui::Button("Play")) {
        saturn_play_animation(selected_animation);
    }
    imgui_bundled_space(20, "Model Packs","These are DynOS model packs, used for live model loading.\nPlace packs in dynos/packs.");
    if (ImGui::BeginListBox("", ImVec2(200, 100))) {
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