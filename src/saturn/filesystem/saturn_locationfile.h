#ifndef SaturnLocationFile
#define SaturnLocationFile

#include <string>
#include <map>
#include <utility>
#include <array>
extern "C" {
#include "include/types.h"
}

void saturn_load_locations();
void saturn_save_locations();
std::map<std::string, std::pair<s16, std::array<float, 3>>>* saturn_get_locations();
void saturn_add_location(char* name);

#endif