#ifndef SaturnFileBrowser
#define SaturnFileBrowser

#include <string>
#include <filesystem>

void saturn_file_browser_item(std::string item);
void saturn_file_browser_tree_node(std::string name);
void saturn_file_browser_tree_node_end();
void saturn_file_browser_scan_directory(std::filesystem::path path, bool recursive = true);
void saturn_file_browser_filter_extension(std::string extension);
void saturn_file_browser_height(int height);
bool saturn_file_browser_show(std::string id);
std::filesystem::path saturn_file_browser_get_selected();

#endif