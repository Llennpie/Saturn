#include "saturn/saturn_colors.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "saturn/saturn.h"
#include "saturn/imgui/saturn_imgui.h"

extern "C" {
#include "game/camera.h"
#include "game/level_update.h"
#include "sm64.h"
}

using namespace std;
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

unsigned int defaultColorHatRLight = 255;
unsigned int defaultColorHatRDark = 127;
unsigned int defaultColorHatGLight = 0;
unsigned int defaultColorHatGDark = 0;
unsigned int defaultColorHatBLight = 0;
unsigned int defaultColorHatBDark = 0;

unsigned int defaultColorOverallsRLight = 0;
unsigned int defaultColorOverallsRDark = 0;
unsigned int defaultColorOverallsGLight = 0;
unsigned int defaultColorOverallsGDark = 0;
unsigned int defaultColorOverallsBLight = 255;
unsigned int defaultColorOverallsBDark = 127;

unsigned int defaultColorGlovesRLight = 255;
unsigned int defaultColorGlovesRDark = 127;
unsigned int defaultColorGlovesGLight = 255;
unsigned int defaultColorGlovesGDark = 127;
unsigned int defaultColorGlovesBLight = 255;
unsigned int defaultColorGlovesBDark = 127;

unsigned int defaultColorShoesRLight = 114;
unsigned int defaultColorShoesRDark = 57;
unsigned int defaultColorShoesGLight = 28;
unsigned int defaultColorShoesGDark = 14;
unsigned int defaultColorShoesBLight = 14;
unsigned int defaultColorShoesBDark = 7;

unsigned int defaultColorSkinRLight = 254;
unsigned int defaultColorSkinRDark = 127;
unsigned int defaultColorSkinGLight = 193;
unsigned int defaultColorSkinGDark = 96;
unsigned int defaultColorSkinBLight = 121;
unsigned int defaultColorSkinBDark = 60;

unsigned int defaultColorHairRLight = 115;
unsigned int defaultColorHairRDark = 57;
unsigned int defaultColorHairGLight = 6;
unsigned int defaultColorHairGDark = 3;
unsigned int defaultColorHairBLight = 0;
unsigned int defaultColorHairBDark = 0;

// Color Codes

std::vector<string> cc_array;
string colorCodeDir;

void load_cc_directory() {
    cc_array.clear();
    cc_array.push_back("Mario.gs");

#ifdef __MINGW32__
    // windows moment
    colorCodeDir = "dynos\\colorcodes\\";
#else
    colorCodeDir = "dynos/colorcodes/";
#endif

    for (const auto & entry : fs::directory_iterator(colorCodeDir))
        cc_array.push_back(entry.path().filename().u8string());
}

void reset_cc_colors() {
    defaultColorHatRLight = 255;
    defaultColorHatRDark = 127;
    defaultColorHatGLight = 0;
    defaultColorHatGDark = 0;
    defaultColorHatBLight = 0;
    defaultColorHatBDark = 0;

    defaultColorOverallsRLight = 0;
    defaultColorOverallsRDark = 0;
    defaultColorOverallsGLight = 0;
    defaultColorOverallsGDark = 0;
    defaultColorOverallsBLight = 255;
    defaultColorOverallsBDark = 127;

    defaultColorGlovesRLight = 255;
    defaultColorGlovesRDark = 127;
    defaultColorGlovesGLight = 255;
    defaultColorGlovesGDark = 127;
    defaultColorGlovesBLight = 255;
    defaultColorGlovesBDark = 127;

    defaultColorShoesRLight = 114;
    defaultColorShoesRDark = 57;
    defaultColorShoesGLight = 28;
    defaultColorShoesGDark = 14;
    defaultColorShoesBLight = 14;
    defaultColorShoesBDark = 7;

    defaultColorSkinRLight = 254;
    defaultColorSkinRDark = 127;
    defaultColorSkinGLight = 193;
    defaultColorSkinGDark = 96;
    defaultColorSkinBLight = 121;
    defaultColorSkinBDark = 60;

    defaultColorHairRLight = 115;
    defaultColorHairRDark = 57;
    defaultColorHairGLight = 6;
    defaultColorHairGDark = 3;
    defaultColorHairBLight = 0;
    defaultColorHairBDark = 0;
}

void load_cc_file(char* cc_char_filename) {
    string cc_filename = cc_char_filename;
    if (cc_filename == "Mario.gs") {
        reset_cc_colors();
        return;
    }

    std::ifstream file(colorCodeDir + cc_filename, std::ios::in | std::ios::binary);

    // If the color code was previously deleted, reload the list and cancel.
    if (!file.good()) {
        load_cc_directory();
        return;
    }

    const std::size_t& size = std::filesystem::file_size(colorCodeDir + cc_filename);
    std::string content(size, '\0');
    file.read(content.data(), size);

    file.close();

    std::istringstream f(content);
    std::string line;
        
    while (std::getline(f, line)) {
        std::string address = line.substr(2, 6);
        int value1 = std::stoi(line.substr(9, 2), 0, 16);
        int value2 = std::stoi(line.substr(11, 2), 0, 16);

        // Hat
        if (address == "07EC40") {
            defaultColorHatRLight = value1;
            defaultColorHatGLight = value2;
        }
        if (address == "07EC42") {
            defaultColorHatBLight = value1;
        }
        if (address == "07EC38") {
            defaultColorHatRDark = value1;
            defaultColorHatGDark = value2;
        }
        if (address == "07EC3A") {
            defaultColorHatBDark = value1;
        }

        // Overalls
        if (address == "07EC28") {
            defaultColorOverallsRLight = value1;
            defaultColorOverallsGLight = value2;
        }
        if (address == "07EC2A") {
            defaultColorOverallsBLight = value1;
        }
        if (address == "07EC20") {
            defaultColorOverallsRDark = value1;
            defaultColorOverallsGDark = value2;
        }
        if (address == "07EC22") {
            defaultColorOverallsBDark = value1;
        }

        // Gloves
        if (address == "07EC58") {
            defaultColorGlovesRLight = value1;
            defaultColorGlovesGLight = value2;
        }
        if (address == "07EC5A") {
            defaultColorGlovesBLight = value1;
        }
        if (address == "07EC50") {
            defaultColorGlovesRDark = value1;
            defaultColorGlovesGDark = value2;
        }
        if (address == "07EC52") {
            defaultColorGlovesBDark = value1;
        }

        // Shoes
        if (address == "07EC70") {
            defaultColorShoesRLight = value1;
            defaultColorShoesGLight = value2;
        }
        if (address == "07EC72") {
            defaultColorShoesBLight = value1;
        }
        if (address == "07EC68") {
            defaultColorShoesRDark = value1;
            defaultColorShoesGDark = value2;
        }
        if (address == "07EC6A") {
            defaultColorShoesBDark = value1;
        }

        // Skin
        if (address == "07EC88") {
            defaultColorSkinRLight = value1;
            defaultColorSkinGLight = value2;
        }
        if (address == "07EC8A") {
            defaultColorSkinBLight = value1;
        }
        if (address == "07EC80") {
            defaultColorSkinRDark = value1;
            defaultColorSkinGDark = value2;
        }
        if (address == "07EC82") {
            defaultColorSkinBDark = value1;
        }

        // Hair
        if (address == "07ECA0") {
            defaultColorHairRLight = value1;
            defaultColorHairGLight = value2;
        }
        if (address == "07ECA2") {
            defaultColorHairBLight = value1;
        }
        if (address == "07EC98") {
            defaultColorHairRDark = value1;
            defaultColorHairGDark = value2;
        }
        if (address == "07EC9A") {
            defaultColorHairBDark = value1;
        }
    }
}