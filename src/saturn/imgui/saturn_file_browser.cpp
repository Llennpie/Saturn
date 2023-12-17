#include "saturn_file_browser.h"

#include <string>
#include <filesystem>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include "saturn/libs/imgui/imgui.h"
#include "saturn/imgui/icons/IconsForkAwesome.h"

class FileBrowserEntry {
private:
    std::string _name;
    bool _dir = false;
    std::vector<FileBrowserEntry> entries = {};
    FileBrowserEntry* _parent;
public:
    FileBrowserEntry(std::string name, bool dir, FileBrowserEntry* parent) {
        _name = name;
        _dir = dir;
        _parent = parent;
    }
    bool is_dir() { return _dir; }
    std::vector<FileBrowserEntry> dir() { return entries; }
    FileBrowserEntry* parent() { return _parent; }
    std::string name() { return _name; }
    void clear() { entries.clear(); }
    void add_file(std::string name) { entries.push_back(FileBrowserEntry(name, false, this)); }
    FileBrowserEntry* add_dir(std::string name) {
        entries.push_back(FileBrowserEntry(name, true, this));
        return &entries[entries.size() - 1];
    }
    void merge(FileBrowserEntry* entry) {
        for (FileBrowserEntry& child : entry->entries) {
            entries.push_back(child);
        }
    }
};

FileBrowserEntry root = FileBrowserEntry("root", true, nullptr);
FileBrowserEntry* curr = &root;
int browser_height = 75;
std::filesystem::path selected_path;
std::string extension_filter = "";
std::map<std::string, char*> search_terms = {};
std::map<std::string, std::string> selected_paths = {};
std::map<std::filesystem::path, FileBrowserEntry*> scanned_folders = {};

void saturn_file_browser_item(std::string item) {
    curr->add_file(item);
}

void saturn_file_browser_tree_node(std::string item) {
    curr = curr->add_dir(item);
}

void saturn_file_browser_tree_node_end() {
    curr = curr->parent();
}

void saturn_file_browser_refresh_directory(std::filesystem::path dir) {
    if (scanned_folders.find(dir) == scanned_folders.end()) return;
    delete scanned_folders[dir];
    scanned_folders.erase(dir);
}

void saturn_file_browser_scan_directory_internal(FileBrowserEntry* ftb, std::filesystem::path dir, bool recursive) {
    std::vector<std::string> dirs = {};
    std::vector<std::string> files = {};
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        std::filesystem::path path = entry.path();
        if (std::filesystem::is_directory(path)) {
            if (!recursive) continue;
            dirs.push_back(path.filename().string());
            continue;
        }
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (extension_filter != "" && ext != ("." + extension_filter)) continue;
        files.push_back(path.filename().string());
    }
    auto stringcomp = [](std::string a, std::string b) {
        return a < b;
    };
    std::sort(dirs.begin(), dirs.end(), stringcomp);
    std::sort(files.begin(), files.end(), stringcomp);
    for (std::string dirname : dirs) {
        saturn_file_browser_scan_directory_internal(ftb->add_dir(dirname), dir / dirname, true);
    }
    for (std::string filename : files) {
        ftb->add_file(filename);
    }
}

void saturn_file_browser_scan_directory(std::filesystem::path dir, bool recursive) {
    if (scanned_folders.find(dir) == scanned_folders.end()) {
        FileBrowserEntry* entry = new FileBrowserEntry("root", true, nullptr);
        saturn_file_browser_scan_directory_internal(entry, dir, recursive);
        scanned_folders.insert({ dir, entry });
    }
    curr->merge(scanned_folders[dir]);
}

void saturn_file_browser_filter_extension(std::string extension) {
    extension_filter = std::string(extension);
    std::transform(extension_filter.begin(), extension_filter.end(), extension_filter.begin(), ::tolower);
}

void saturn_file_browser_height(int height) {
    browser_height = height;
}

void saturn_file_browser_clear() {
    root.clear();
    browser_height = 75;
    extension_filter = "";
}

bool saturn_file_browser_create_imgui(FileBrowserEntry dir, std::string path, std::string browser_id) {
    bool clicked = false;
    for (FileBrowserEntry& entry : dir.dir()) {
        if (entry.is_dir()) {
            if (ImGui::TreeNode(entry.name().c_str())) {
                clicked |= saturn_file_browser_create_imgui(entry, path + entry.name() + "/", browser_id);
                ImGui::TreePop();
            }
        }
        else {
            std::string filename = entry.name();
            std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
            if (filename.find(search_terms[browser_id]) == std::string::npos) continue;
            std::string fullpath = path + entry.name();
            bool selected = selected_paths[browser_id] == fullpath;
            if (ImGui::Selectable(entry.name().c_str(), &selected)) {
                selected_paths[browser_id] = fullpath;
                selected_path = fullpath;
                clicked = true;
            }
        }
    }
    return clicked;
}

bool saturn_file_browser_show(std::string id) {
    selected_path = "";
    if (selected_paths.find(id) == selected_paths.end()) selected_paths.insert({ id, "" });
    if (search_terms.find(id) == search_terms.end()) {
        char* search = (char*)malloc(256);
        search[0] = 0;
        search_terms.insert({ id, search });
    }
    ImGui::InputTextWithHint(("###searchbar_file_browser_" + id).c_str(), ICON_FK_SEARCH " Search...", search_terms[id], 256);
    ImGui::BeginChild(("###file_browser_" + id).c_str(), ImVec2(0, browser_height), true);
    bool result = saturn_file_browser_create_imgui(root, "", id);
    ImGui::EndChild();
    saturn_file_browser_clear();
    return result;
}

std::filesystem::path saturn_file_browser_get_selected() {
    return selected_path;
}