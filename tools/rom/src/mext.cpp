#include <iostream>
#include "types.h"
#include "utils.h"
#include "sm64.hpp"
#include "json.hpp"

using namespace std;
namespace sm64 = MoonUtils;

#define STR(x) #x

string main_folder = "assets";
vector<string> defaults;

ExtractRule rules[] = {
    { "graphics", { "actors", "levels", "textures" }},
    { "/",        { "sound", "fonts", "langs" }}
};

Transformer transformers[] = {
    { "aiff", [](string filename, string fullpath) {
        string dir   = sm64::dirname(fullpath);
        string aiff  = fullpath;
        string aifc  = sm64::join(dir, filename + ".aifc");
        string table = sm64::join(dir, filename + ".table");
        string codebook = sm64::exec("./tools/aiff_extract_codebook " + aiff);
        sm64::writeFile(table, codebook);

        sm64::exec(sm64::format("./tools/vadpcm_enc -c %s %s %s", table.c_str(), aiff.c_str(), aifc.c_str()));
    }}
};

void remove_unused(string dir){
    sm64::rm(sm64::join(sm64::join(dir, main_folder), "demos"));
}

void copy_assets(string dir){
    for (auto& rule : rules){
        string parent = sm64::join(sm64::join(dir, main_folder), rule.name);
        if(!sm64::exists(parent))
            sm64::mkdir(parent);

        for (auto& folder : rule.rules){
            string from = sm64::join(dir, folder);
            string to   = sm64::join(parent, folder);
            sm64::move(from, to);
        }
    }
}

void copy_defaults(string dir) {
    if(defaults.empty()) return;
    cout << "Copying defaults" << endl;
    for(auto& path : defaults){
        sm64::copy(path, sm64::join(dir, sm64::relative(main_folder, path)));
    }
}

void write_manifest(string dir) {
    nlohmann::json addon = {{
        "bit", {
            { "name", "Moon64" },
            { "authors", { "UnderVolt", "Nintendo" } },
            { "description", "SM64 Default Addon" },
            { "version", 1.0 },
            { "readOnly", true }
        }}
    };
    sm64::writeFile(sm64::join(dir, "properties.json"), addon.dump(4));
}

void build_addon(const char* rom, const char* dir){
    string rom_path(rom);
    string temp_dir(dir);

    cout << "Extracting to " << temp_dir << endl;

    string assets = sm64::exec(sm64::format("python3 %s/tools/extract_assets.py %s %s", CWD, temp_dir.c_str(), rom_path.c_str()));
    if( !nlohmann::json::accept(assets) ) return;

    nlohmann::json json = nlohmann::json::parse(assets);

    if(json["status"]){
        vector<string> files;
        sm64::dirscan(temp_dir, files);

        for(auto& file : files){
            string ext = sm64::extname(file);
            for(auto& transformer : transformers)
                if(ext == transformer.name)
                    transformer.callback(sm64::basename(file), file);
        }

        copy_defaults(temp_dir);
        compile_sound(temp_dir);
        copy_assets(temp_dir);
        remove_unused(temp_dir);
        write_manifest(temp_dir);
        cout << "Done!" << endl;
    }
}

int main(int argc, char *argv[]) {
    if(argc < 3){
        cout << "Usage: " << argv[0] << " <rom> <output> [...defaults]" << endl;
        return 1;
    }

    for(int i = 3; i < argc; i++){
        std::string path = argv[i];
        if(fs::exists(path)){
            defaults.push_back(path);
        }
    }

    build_addon(argv[1], argv[2]);

    return 0;
}