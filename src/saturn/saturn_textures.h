#ifndef SaturnTextures
#define SaturnTextures

#include <stdio.h>
#include <stdbool.h>


#ifdef __cplusplus
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

class TexturePath {
public:
    std::string FileName;
    std::string FilePath;
    /* Relative path from res/gfx, as used by EXTERNAL_DATA */
    std::string GetRelativePath() {
        return "../../" + this->FilePath;//.substr(0, this->FilePath.size() - 4);
    }
    /* Parent directory, used for subfolders */
    std::string ParentPath() {
        return FilePath.substr(0, this->FilePath.length() - this->FileName.length());
    }
};

class Expression {
private:
    std::string ReplaceKey() {
        return "saturn_" + this->Name;
    }
public:
    std::string Name;
    std::string FolderPath;
    std::vector<TexturePath> Textures;
    std::vector<TexturePath> Folders;
    /* The index of the current selected texture */
    int CurrentIndex = 0;

    /* Returns true if a given path contains the expression's replacement keywords
       For example, "/eye" can be "saturn_eye" or "saturn_eyes" */
    bool PathHasReplaceKey(std::string path) {
        // We also append a _ or . to the end, so "saturn_eyebrow" doesn't get read as "saturn_eye"
        return (path.find("saturn_" + this->Name + "_") != std::string::npos ||
                path.find("saturn_" + this->Name + "_s") != std::string::npos ||
                path.find("saturn_" + this->Name.substr(0, this->Name.size() - 1) + "_") != std::string::npos);
    }
};

extern bool custom_eyes_enabled;

extern Expression VanillaEyes;
extern void LoadEyesFolder();

std::vector<TexturePath> LoadExpressionTextures(std::string);
std::vector<TexturePath> LoadExpressionFolders(std::string);
std::vector<Expression> LoadExpressions(std::string);

void saturn_copy_file(std::string from, std::string to);
void saturn_delete_file(std::string file);
std::size_t number_of_files_in_directory(std::filesystem::path path);

extern bool show_vmario_emblem;

extern "C" {
#endif
    const void* saturn_bind_texture(const void*);
#ifdef __cplusplus
}
#endif

#endif