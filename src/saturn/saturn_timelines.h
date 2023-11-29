#ifndef SaturnTimelineGroups
#define SaturnTimelineGroups

#include "saturn/saturn.h"
#include "saturn/imgui/saturn_imgui_cc_editor.h"
#include "saturn/imgui/saturn_imgui_machinima.h"
#include "saturn/imgui/saturn_imgui_chroma.h"
#include <map>
#include <string>
#include <vector>
#include <tuple>

extern "C" {
#include "game/camera.h"
#include "game/level_update.h"
#include "pc/cheats.h"
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
}

extern void saturn_fill_data_table();
extern std::map<std::string, std::tuple<void*, KeyframeType, bool, std::string, int, int>> timelineDataTable;

#define SATURN_KFGROUP_COLOR(id, name) { name, { id "_r", id "_g", id "_b" } }
inline std::map<std::string, std::vector<std::string>> k_groups = {
    SATURN_KFGROUP_COLOR("k_light_col", "Light Color"),
    SATURN_KFGROUP_COLOR("k_color", "Skybox Color"),
    SATURN_KFGROUP_COLOR("k_1/2###hat_half_1", "Hat, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###hat_half_2", "Hat, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###overalls_half_1", "Overalls, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###overalls_half_2", "Overalls, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###gloves_half_1", "Gloves, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###gloves_half_2", "Gloves, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###shoes_half_1", "Shoes, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###shoes_half_2", "Shoes, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###skin_half_1", "Skin, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###skin_half_2", "Skin, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###hair_half_1", "Hair, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###hair_half_2", "Hair, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###shirt_half_1", "Shirt, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###shirt_half_2", "Shirt, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###shoulders_half_1", "Shoulders, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###shoulders_half_2", "Shoulders, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###arms_half_1", "Arms, Main"),
    SATURN_KFGROUP_COLOR("k_1/2###arms_half_2", "Arms, Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###overalls_bottom_half_1", "Overalls (Bottom), Main"),
    SATURN_KFGROUP_COLOR("k_1/2###overalls_bottom_half_2", "Overalls (Bottom), Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###leg_top_half_1", "Leg (Top), Main"),
    SATURN_KFGROUP_COLOR("k_1/2###leg_top_half_2", "Leg (Top), Shade"),
    SATURN_KFGROUP_COLOR("k_1/2###leg_bottom_half_1", "Leg (Bottom), Main"),
    SATURN_KFGROUP_COLOR("k_1/2###leg_bottom_half_2", "Leg (Bottom), Shade"),
    { "Mario Head", { "k_mario_headrot_yaw", "k_mario_headrot_pitch" } },
};

#endif