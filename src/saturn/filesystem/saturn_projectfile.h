#ifndef SaturnProjectFile
#define SaturnProjectFile

#include <vector>
#include <string>

void saturn_load_project(char* filename);
void saturn_save_project(char* filename);

extern std::string project_dir;
extern std::vector<std::string> project_array;
extern void saturn_load_project_list();

#endif