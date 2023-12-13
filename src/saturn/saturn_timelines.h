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
extern std::map<std::string, std::tuple<void*, KeyframeType, char, std::string, int, int>> timelineDataTable;

#endif