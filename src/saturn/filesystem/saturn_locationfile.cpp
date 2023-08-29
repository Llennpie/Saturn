#include "saturn_format.h"

#include <string>
#include <map>
#include <utility>
#include <array>
extern "C" {
#include "include/types.h"
#include "game/area.h"
#include "game/level_update.h"
}

std::map<int, std::map<std::string, std::pair<s16, std::array<float, 3>>>> locations = {};

void saturn_add_location_internal(int level, float x, float y, float z, s16 angle, char* name) {
    if (locations.find(level) == locations.end()) {
        std::map<std::string, std::pair<s16, std::array<float, 3>>> levelLocations;
        levelLocations.insert({ std::string(name), { angle, { x, y, z } } });
        locations.insert({ level, levelLocations });
    }
    else {
        std::string locationName = std::string(name);
        if (locations[level].find(name) == locations[level].end()) locations[level].insert({ locationName, { angle, { x, y, z } } });
        else {
            locations[level][locationName].first = angle;
            locations[level][locationName].second[0] = x;
            locations[level][locationName].second[1] = y;
            locations[level][locationName].second[2] = z;
        }
    }
}
void saturn_location_data_handler(SaturnFormatStream* stream, int version) {
    int count = saturn_format_read_int32(stream);
    for (int i = 0; i < count; i++) {
        int level = saturn_format_read_int16(stream);
        s16 angle = saturn_format_read_int16(stream);
        float x = saturn_format_read_float(stream);
        float y = saturn_format_read_float(stream);
        float z = saturn_format_read_float(stream);
        char name[257];
        saturn_format_read_string(stream, name);
        name[256] = 0;
        saturn_add_location_internal(level, x, y, z, angle, name);
    }
}
void saturn_load_locations() {
    saturn_format_input("dynos/locations.bin", "STLC", {
        { "DATA", saturn_location_data_handler }
    });
}
void saturn_save_locations() {
    SaturnFormatStream stream = saturn_format_output("STLC", 1);
    saturn_format_new_section(&stream, "DATA");
    int count = 0;
    for (auto& levelEntry : locations) {
        for (auto& locationEntry : levelEntry.second) {
            count++;
        }
    }
    saturn_format_write_int32(&stream, count);
    for (auto& levelEntry : locations) {
        for (auto& locationEntry : levelEntry.second) {
            saturn_format_write_int16(&stream, levelEntry.first);
            saturn_format_write_int16(&stream, locationEntry.second.first);
            saturn_format_write_float(&stream, locationEntry.second.second[0]);
            saturn_format_write_float(&stream, locationEntry.second.second[1]);
            saturn_format_write_float(&stream, locationEntry.second.second[2]);
            saturn_format_write_string(&stream, (char*)locationEntry.first.c_str());
        }
    }
    saturn_format_close_section(&stream);
    saturn_format_write("dynos/locations.bin", &stream);
}
std::map<std::string, std::pair<s16, std::array<float, 3>>>* saturn_get_locations() {
    int level = (gCurrLevelNum << 8) | gCurrAreaIndex;
    return &locations[level];
}
void saturn_add_location(char* name) {
    saturn_add_location_internal((gCurrLevelNum << 8) | gCurrAreaIndex, gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], gMarioState->faceAngle[1], name);
}