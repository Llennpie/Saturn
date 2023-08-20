#ifndef SaturnProjectFile
#define SaturnProjectFile

#include <vector>
#include <string>

void saturn_load_project(char* filename);
void saturn_save_project(char* filename);
std::vector<std::string> saturn_list_projects();

#endif