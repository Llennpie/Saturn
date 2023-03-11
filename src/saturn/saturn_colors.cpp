#include "saturn/saturn_colors.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "saturn/saturn.h"
#include "saturn/imgui/saturn_imgui.h"
#include "saturn/saturn_textures.h"

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"

#include "pc/configfile.h"

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
#include <array>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

// arrays are ordered like this: [{RLight, RDark}, {GLight, GDark}, {BLight, BDark}]
// NOTE: This will only work with C++ standards above C++14!
// There are most likely ways to further minimize repetition and overall complexity,
// However i will not be doing that here
// Structs might help, Will I use them here? Nope!

// Default
unsigned int defaultColorHat[6]             {255, 127, 0, 0, 0, 0};
unsigned int defaultColorOveralls[6]        {0, 0, 0, 0, 255, 127};
unsigned int defaultColorGloves[6]          {255, 127, 255, 127, 255, 127};
unsigned int defaultColorShoes[6]           {114, 57, 28, 14, 14, 7};
unsigned int defaultColorSkin[6]            {254, 127, 193, 96, 121, 60};
unsigned int defaultColorHair[6]            {115, 57, 6, 3, 0, 0};

// CometSPARK
unsigned int sparkColorShirt[6]             {255, 127, 255, 127, 0, 0};
unsigned int sparkColorShoulders[6]         {0, 0, 255, 127, 255, 127};
unsigned int sparkColorArms[6]              {0, 0, 255, 127, 127, 64};
unsigned int sparkColorOverallsBottom[6]    {255, 127, 0, 0, 255, 127}; 
unsigned int sparkColorLegTop[6]            {255, 127, 0, 0, 127, 64};
unsigned int sparkColorLegBottom[6]         {127, 64, 0, 0, 255, 127};

unsigned int chromaColor[6]                 {0, 0, 255, 127, 0, 0};

// Color Codes

bool cc_model_support = true;
bool cc_spark_support = false;

std::vector<string> cc_array;
string colorCodeDir;

bool modelCcLoaded;

std::vector<string> model_cc_array;
string modelColorCodeDir;

std::string formatColour(unsigned int *colorBodyPart) {
    char colour[64];
    ImFormatString(colour, IM_ARRAYSIZE(colour), "%02X%02X%02X%02X%02X%02X"
            , ImClamp((int)colorBodyPart[0], 0, 255)
            , ImClamp((int)colorBodyPart[2], 0, 255)
            , ImClamp((int)colorBodyPart[4], 0, 255)
            , ImClamp((int)colorBodyPart[1], 0, 255)
            , ImClamp((int)colorBodyPart[3], 0, 255)
            , ImClamp((int)colorBodyPart[5], 0, 255));
    std::string col = colour;

    return col;
} // this should help remove a lot of the repetition,
  // however it's still very repetitive and should be worked on further

/*
    The currently active GameShark code in string format.
*/
std::string global_gs_code() {
    std::string gameshark;

    // TODO: Clean this code up, I dont think it needs 36 similar blocks of code lol

    std::string col1 = formatColour(defaultColorHat);
    std::string col2 = formatColour(defaultColorOveralls);
    std::string col3 = formatColour(defaultColorGloves);
    std::string col4 = formatColour(defaultColorShoes);
    std::string col5 = formatColour(defaultColorSkin);
    std::string col6 = formatColour(defaultColorHair);

    gameshark += "8107EC40 " + col1.substr(0, 4) + "\n";
    gameshark += "8107EC42 " + col1.substr(4, 2) + "00\n";
    gameshark += "8107EC38 " + col1.substr(6, 4) + "\n";
    gameshark += "8107EC3A " + col1.substr(10, 2) + "00\n";
    gameshark += "8107EC28 " + col2.substr(0, 4) + "\n";
    gameshark += "8107EC2A " + col2.substr(4, 2) + "00\n";
    gameshark += "8107EC20 " + col2.substr(6, 4) + "\n";
    gameshark += "8107EC22 " + col2.substr(10, 2) + "00\n";
    gameshark += "8107EC58 " + col3.substr(0, 4) + "\n";
    gameshark += "8107EC5A " + col3.substr(4, 2) + "00\n";
    gameshark += "8107EC50 " + col3.substr(6, 4) + "\n";
    gameshark += "8107EC52 " + col3.substr(10, 2) + "00\n";
    gameshark += "8107EC70 " + col4.substr(0, 4) + "\n";
    gameshark += "8107EC72 " + col4.substr(4, 2) + "00\n";
    gameshark += "8107EC68 " + col4.substr(6, 4) + "\n";
    gameshark += "8107EC6A " + col4.substr(10, 2) + "00\n";
    gameshark += "8107EC88 " + col5.substr(0, 4) + "\n";
    gameshark += "8107EC8A " + col5.substr(4, 2) + "00\n";
    gameshark += "8107EC80 " + col5.substr(6, 4) + "\n";
    gameshark += "8107EC82 " + col5.substr(10, 2) + "00\n";
    gameshark += "8107ECA0 " + col6.substr(0, 4)+ "\n";
    gameshark += "8107ECA2 " + col6.substr(4, 2) + "00\n";
    gameshark += "8107EC98 " + col6.substr(6, 4)+ "\n";
    gameshark += "8107EC9A " + col6.substr(10, 2) + "00";

    if (cc_spark_support) {
        gameshark += "\n";

        std::string col7 = formatColour(sparkColorShirt);
        std::string col8 = formatColour(sparkColorShoulders);
        std::string col9 = formatColour(sparkColorArms);
        std::string col10 = formatColour(sparkColorOverallsBottom);
        std::string col11 = formatColour(sparkColorLegTop);
        std::string col12 = formatColour(sparkColorLegBottom);

        gameshark += "8107ECB8 " + col7.substr(0, 4) + "\n";
        gameshark += "8107ECBA " + col7.substr(4, 2) + "00\n";
        gameshark += "8107ECB0 " + col7.substr(6, 4) + "\n";
        gameshark += "8107ECB2 " + col7.substr(10, 2) + "00\n";
        gameshark += "8107ECD0 " + col8.substr(0, 4) + "\n";
        gameshark += "8107ECD2 " + col8.substr(4, 2) + "00\n";
        gameshark += "8107ECC8 " + col8.substr(6, 4) + "\n";
        gameshark += "8107ECCA " + col8.substr(10, 2) + "00\n";
        gameshark += "8107ECE8 " + col9.substr(0, 4) + "\n";
        gameshark += "8107ECEA " + col9.substr(4, 2) + "00\n";
        gameshark += "8107ECE0 " + col9.substr(6, 4) + "\n";
        gameshark += "8107ECE2 " + col9.substr(10, 2) + "00\n";
        gameshark += "8107ED00 " + col10.substr(0, 4) + "\n";
        gameshark += "8107ED02 " + col10.substr(4, 2) + "00\n";
        gameshark += "8107ECF8 " + col10.substr(6, 4) + "\n";
        gameshark += "8107ECFA " + col10.substr(10, 2) + "00\n";
        gameshark += "8107ED18 " + col11.substr(0, 4) + "\n";
        gameshark += "8107ED1A " + col11.substr(4, 2) + "00\n";
        gameshark += "8107ED10 " + col11.substr(6, 4) + "\n";
        gameshark += "8107ED12 " + col11.substr(10, 2) + "00\n";
        gameshark += "8107ED30 " + col12.substr(0, 4) + "\n";
        gameshark += "8107ED32 " + col12.substr(4, 2) + "00\n";
        gameshark += "8107ED28 " + col12.substr(6, 4) + "\n";
        gameshark += "8107ED2A " + col12.substr(10, 2) + "00";
    }

    return gameshark;
} // :(

/*
    Returns true if a defined address is for the CometSPARK format.
*/
bool is_spark_address(string address) {
    if (!current_model_data.spark_support)
        return false;;
        
    // The unholy address table
    if (address == 0x07ECB8 || address == 0x07ECBA || address == 0x07ECB0 || address == 0x07ECB2
     || address == 0x07ECD0 || address == 0x07ECD2 || address == 0x07ECC8 || address == 0x07ECCA
     || address == 0x07ECE8 || address == 0x07ECEA || address == 0x07ECE0 || address == 0x07ECE2
     || address == 0x07ED00 || address == 0x07ED02 || address == 0x07ECF8 || address == 0x07ECFA
     || address == 0x07ED18 || address == 0x07ED1A || address == 0x07ED10 || address == 0x07ED12
     || address == 0x07ED30 || address == 0x07ED32 || address == 0x07ED28 || address == 0x07ED2A)
        return true;

    // Nope
    return false;
}

string last_model_cc_address;

/*
    Loads the dynos/colorcodes/ folder into the CC array.
*/
void saturn_load_cc_directory() {
    cc_array.clear();
    cc_array.push_back("Mario.gs");

#ifdef __MINGW32__
    // windows moment
    colorCodeDir = "dynos\\colorcodes\\";
#else
    colorCodeDir = "dynos/colorcodes/";
#endif

    if (!fs::exists(colorCodeDir))
        return;

    for (const auto & entry : fs::directory_iterator(colorCodeDir)) {
        fs::path path = entry.path();

        if (path.filename().u8string() != "Mario") {
            if (path.extension().u8string() == ".gs")
                cc_array.push_back(path.filename().u8string());
        }
    }

    cc_details = "" + std::to_string(cc_array.size()) + " color code";
    if (cc_array.size() != 1) cc_details += "s";
}

/*
    Preferably used in a while loop, sets a global color with a defined address and value.
*/

void run_cc_replacement(int address, int value1, int value2) {
    // NOTE: There are better ways to do this, please optimize further! 
    
    switch(address) {
            //Hat
        case 0x07EC40:  
            defaultColorHat[0] = value1;
            defaultColorHat[2] = value2;
            break;
        case 0x07EC42:
            defaultColorHat[4] = value1;
            break;
        case 0x07EC38:
            defaultColorHat[1] = value1;
            defaultColorHat[3] = value2;
            break;
        case 0x07EC3A:
            defaultColorHat[5] = value1;
            break;

            //Overalls
        case 0x07EC28:
            defaultColorOveralls[0] = value1;
            defaultColorOveralls[2] = value2;
            break;
        case 0x07EC2A:
            defaultColorOveralls[4] = value1;
            break;
        case 0x07EC20:
            defaultColorOveralls[3] = value2;
            break;
        case 0x07EC22:
            defaultColorOveralls[5] = value1;
            break;

            //Gloves
        case 0x07EC58:
            defaultColorGloves[0] = value1;
            defaultColorGloves[2] = value2;
            break;
        case 0x07EC5A:
            defaultColorGloves[4] = value1;
            break;
        case 0x07EC50:
            defaultColorGloves[1] = value1;
            defaultColorGloves[3] = value2;
            break;
        case 0x08EC52:
            defaultColorGloves[5] = value1;
            break;

            //Shoes
        case 0x07EC70:
            defaultColorShoes[0] = value1;
            defaultColorShoes[2] = value2;
            break;
        case 0x07EC72:
            defaultColorShoes[4] = value1;
            break;
        case 0x07EC68:
            defaultColorShoes[1] = value1;
            defaultColorShoes[3] = value2;
            break;
        case 0x07EC6A:
            defaultColorShoes[5] = value1;
            break;

            //Skin
        case 0x07EC88:
            defaultColorSkin[0] = value1;
            defaultColorSkin[2] = value2;
            break;
        case 0x07EC8A:
            defaultColorSkin[4] = value1;
            break;
        case 0x07EC80:
            defaultColorSkin[1] = value1;
            defaultColorSkin[3] = value2;
            break;
        case 0x07EC82:
            defaultColorSkin[5] = value1;
            break;

            //Hair
        case 0x07ECA0:
            defaultColorHair[0] = value1;
            defaultColorHair[2] = value2;
            break;
        case 0x07ECA2:
            defaultColorHair[4] = value1;
            break;
        case 0x07EC98:
            defaultColorHair[1] = value1;
            defaultColorHair[3] = value2;
            break;
        case 0x07EC9A:
            defaultColorHair[5] = value1;
            break;
    }

    // --------
    // EXPERIMENTAL: CometSPARK Support
    // --------
    // Things MIGHT break. This is unfinished and not fully compatible with Saturn yet.
    // Pull requests and detailed GH issues are much appreciated...
    // --------

    if (!cc_spark_support)
        return;

    switch(address) {
        //Shirt
        case 0x07ECB8:  
            sparkColorShirt[0] = value1;
            sparkColorShirt[2] = value2;
            break;
        case 0x07ECBA:
            sparkColorShirt[4] = value1;
            break;
        case 0x07ECB0:
            sparkColorShirt[1] = value1;
            sparkColorShirt[3] = value2;
            break;
        case 0x07ECB2:
            sparkColorShirt[1] = value1;
            sparkColorShirt[3] = value2;
            break;

            //Shoulders
        case 0x07ECD0:
            sparkColorShoulders[0] = value1;
            sparkColorShoulders[2] = value2;
            break;
        case 0x07ECD2:
            sparkColorShoulders[4] = value1;
            break;
        case 0x07ECC8:
            sparkColorShoulders[3] = value2;
            break;
        case 0x07ECCA:
            sparkColorShoulders[5] = value1;
            break;

            //Arms
        case 0x07ECE8:
            sparkColorArms[0] = value1;
            sparkColorArms[2] = value2;
            break;
        case 0x07ECEA:
            sparkColorArms[4] = value1;
            break;
        case 0x07ECE0:
            sparkColorArms[1] = value1;
            sparkColorArms[3] = value2;
            break;
        case 0x08ECE2:
            sparkColorArms[5] = value1;
            break;

            //OverallsBottom
        case 0x07ED00:
            sparkColorOverallsBottom[0] = value1;
            sparkColorOverallsBottom[2] = value2;
            break;
        case 0x07ED02:
            sparkColorOverallsBottom[4] = value1;
            break;
        case 0x07ECF8:
            sparkColorOverallsBottom[1] = value1;
            sparkColorOverallsBottom[3] = value2;
            break;
        case 0x07ECFA:
            sparkColorOverallsBottom[5] = value1;
            break;

            //LegTop
        case 0x07ED18:
            sparkColorLegTop[0] = value1;
            sparkColorLegTop[2] = value2;
            break;
        case 0x07ED1A:
            sparkColorLegTop[4] = value1;
            break;
        case 0x07ED10:
            sparkColorLegTop[1] = value1;
            sparkColorLegTop[3] = value2;
            break;
        case 0x07ED12:
            sparkColorLegTop[5] = value1;
            break;

            //LegBottom
        case 0x07ED30:
            sparkColorLegBottom[0] = value1;
            sparkColorLegBottom[2] = value2;
            break;
        case 0x07ED32:
            sparkColorLegBottom[4] = value1;
            break;
        case 0x07ED28:
            sparkColorLegBottom[1] = value1;
            sparkColorLegBottom[3] = value2;
            break;
        case 0x07ED2A:
            sparkColorLegBottom[5] = value1;
            break;
    }
}

/*
    Resets Mario's colors to default.
*/
void reset_colors(unsigned int* ccColor, int val1, int val2, int val3, int val4, int val5, int val6) {
    ccColor[0] = val1;  // R
    ccColor[1] = val2;

    ccColor[2] = val3;  // G
    ccColor[3] = val4;

    ccColor[4] = val5;  // B
    ccColor[5] = val6;
}
// Please save me

void reset_cc_colors() {
    reset_colors(defaultColorHat, 255, 127, 0, 0, 0, 0);
    reset_colors(defaultColorOveralls, 0, 0, 0, 0, 255, 127);
    reset_colors(defaultColorGloves, 255, 127, 255, 127, 255, 127);
    reset_colors(defaultColorShoes, 114, 57, 28, 14, 14, 7);
    reset_colors(defaultColorSkin, 254, 127, 193, 96, 121, 60);
    reset_colors(defaultColorHair, 115, 57, 6, 3, 0, 0);

    // CometSPARK
    reset_colors(sparkColorShirt, 255, 127, 255, 127, 0, 0);
    reset_colors(sparkColorShoulders, 0, 0, 255, 127, 255, 127);
    reset_colors(sparkColorArms, 0, 0, 255, 127, 127, 64);
    reset_colors(sparkColorOverallsBottom, 255, 127, 0, 0, 255, 127);
    reset_colors(sparkColorLegTop, 255, 127, 0, 0, 127, 64);
    reset_colors(sparkColorLegBottom, 127, 64, 0, 0, 255, 127);
}

bool is_default_cc(string gameshark) {
    std::string default_cc =    "8107EC40 FF00\n"
                                "8107EC42 0000\n"
                                "8107EC38 7F00\n"
                                "8107EC3A 0000\n"
                                "8107EC28 0000\n"
                                "8107EC2A FF00\n"
                                "8107EC20 0000\n"
                                "8107EC22 7F00\n"
                                "8107EC58 FFFF\n"
                                "8107EC5A FF00\n"
                                "8107EC50 7F7F\n"
                                "8107EC52 7F00\n"
                                "8107EC70 721C\n"
                                "8107EC72 0E00\n"
                                "8107EC68 390E\n"
                                "8107EC6A 0700\n"
                                "8107EC88 FEC1\n"
                                "8107EC8A 7900\n"
                                "8107EC80 7F60\n"
                                "8107EC82 3C00\n"
                                "8107ECA0 7306\n"
                                "8107ECA2 0000\n"
                                "8107EC98 3903\n"
                                "8107EC9A 0000";
    std::string default_spark_cc =  "8107EC40 FF00\n"
                                    "8107EC42 0000\n"
                                    "8107EC38 7F00\n"
                                    "8107EC3A 0000\n"
                                    "8107EC28 0000\n"
                                    "8107EC2A FF00\n"
                                    "8107EC20 0000\n"
                                    "8107EC22 7F00\n"
                                    "8107EC58 FFFF\n"
                                    "8107EC5A FF00\n"
                                    "8107EC50 7F7F\n"
                                    "8107EC52 7F00\n"
                                    "8107EC70 721C\n"
                                    "8107EC72 0E00\n"
                                    "8107EC68 390E\n"
                                    "8107EC6A 0700\n"
                                    "8107EC88 FEC1\n"
                                    "8107EC8A 7900\n"
                                    "8107EC80 7F60\n"
                                    "8107EC82 3C00\n"
                                    "8107ECA0 7306\n"
                                    "8107ECA2 0000\n"
                                    "8107EC98 3903\n"
                                    "8107EC9A 0000\n"
                                    "8107ECB8 FFFF\n"
                                    "8107ECBA 0000\n"
                                    "8107ECB0 7F7F\n"
                                    "8107ECB2 0000\n"
                                    "8107ECD0 00FF\n"
                                    "8107ECD2 FF00\n"
                                    "8107ECC8 007F\n"
                                    "8107ECCA 7F00\n"
                                    "8107ECE8 00FF\n"
                                    "8107ECEA 7F00\n"
                                    "8107ECE0 007F\n"
                                    "8107ECE2 4000\n"
                                    "8107ED00 FF00\n"
                                    "8107ED02 FF00\n"
                                    "8107ECF8 7F00\n"
                                    "8107ECFA 7F00\n"
                                    "8107ED18 FF00\n"
                                    "8107ED1A 7F00\n"
                                    "8107ED10 7F00\n"
                                    "8107ED12 4000\n"
                                    "8107ED30 7F00\n"
                                    "8107ED32 FF00\n"
                                    "8107ED28 4000\n"
                                    "8107ED2A 7F00";

    if (gameshark == default_cc || gameshark == default_spark_cc || gameshark == last_model_cc_address)
        return true;

    return false;
}

/*
    Sets Mario's colors using a GameShark code string.
*/
void paste_gs_code(string content) {
    std::istringstream f(content);
    std::string line;
        
    while (std::getline(f, line)) {
        if (line.rfind("81", 0) == 0) {
            std::string address = line.substr(2, 6);
            int value1 = std::stoi(line.substr(9, 2), 0, 16);
            int value2 = std::stoi(line.substr(11, 2), 0, 16);

            run_cc_replacement(address, value1, value2);
        }
    }
}

/*
    Saves the global_gs_code() to a GS file.
*/
void save_cc_file(std::string name, std::string gameshark) {
#ifdef __MINGW32__
    std::ofstream file("dynos\\colorcodes\\" + name + ".gs");
#else
    std::ofstream file("dynos/colorcodes/" + name + ".gs");
#endif
    file << gameshark;
}

/*
    Saves the global_gs_code() to a GS file.
*/
void save_cc_model_file(std::string name, std::string gameshark, std::string modelFolder) {
    // Create colorcodes folder if it doesn't already exist
    fs::create_directory("dynos/packs/" + modelFolder + "/colorcodes");

#ifdef __MINGW32__
    std::ofstream file("dynos\\packs\\" + modelFolder + "\\colorcodes\\" + name + ".gs");
#else
    std::ofstream file("dynos/packs/" + modelFolder + "/colorcodes/" + name + ".gs");
#endif
    file << gameshark;
}

/*
    Loads a GS file and sets Mario's colors.
*/
void load_cc_file(char* cc_char_filename) {
    string cc_filename = cc_char_filename;
    if (cc_filename == "Mario.gs") {
        reset_cc_colors();
        return;
    }

    std::ifstream file(colorCodeDir + cc_filename, std::ios::in | std::ios::binary);

    // If the color code was previously deleted, reload the list and cancel.
    if (!file.good()) {
        saturn_load_cc_directory();
        return;
    }

    const std::size_t& size = std::filesystem::file_size(colorCodeDir + cc_filename);
    std::string content(size, '\0');
    file.read(content.data(), size);

    file.close();

    paste_gs_code(content);
    modelCcLoaded = false;
}

/*
    Deletes a GS file.
*/
void delete_cc_file(std::string name) {
    if (name == "Mario")
        name = "Sample";

#ifdef __MINGW32__
    string cc_path = "dynos\\colorcodes\\" + name + ".gs";
#else
    string cc_path = "dynos/colorcodes/" + name + ".gs";
#endif

    remove(cc_path.c_str());
    saturn_load_cc_directory();
}

void delete_model_cc_file(std::string name, std::string modelFolder) {
    if (name == "Mario")
        name = "Sample";

#ifdef __MINGW32__
    string cc_path = "dynos\\packs\\" + modelFolder + "\\colorcodes\\" + name + ".gs";
#else
    string cc_path = "dynos/packs/" + modelFolder + "/colorcodes/" + name + ".gs";
#endif

    remove(cc_path.c_str());
    saturn_load_cc_directory();
}

void get_ccs_from_model(std::string modelPath) {
    model_cc_array.clear();

    // Old path
    if (fs::exists(modelPath + "\\default.gs")) {
        model_cc_array.push_back("../default.gs");
    }

#ifdef __MINGW32__
    // windows moment
    modelColorCodeDir = modelPath + "\\colorcodes\\";
#else
    modelColorCodeDir = modelPath + "/colorcodes/";
#endif

    if (!fs::exists(modelColorCodeDir))
        return;

    for (const auto & entry : fs::directory_iterator(modelColorCodeDir)) {
        fs::path path = entry.path();

        if (path.filename().u8string() != "Mario") {
            if (path.extension().u8string() == ".gs")
                model_cc_array.push_back(path.filename().u8string());
        }
    }
}

/*
    Sets Mario's colors using a defined model path.
*/
void set_cc_from_model(std::string ccPath) {
    std::ifstream file(ccPath, std::ios::in | std::ios::binary);

    if (!file.good()) {
        // No file exists, cancel
        return;
    }

    const std::size_t& size = std::filesystem::file_size(ccPath);
    std::string content(size, '\0');
    file.read(content.data(), size);
    file.close();

    std::istringstream f(content);
    std::string line;
        
    while (std::getline(f, line)) {
        int address = std::stoi(line.substr(2, 6), 0, 16);
        int value1 = std::stoi(line.substr(9, 2), 0, 16);
        int value2 = std::stoi(line.substr(11, 2), 0, 16);

        if (!cc_spark_support) cc_spark_support = is_spark_address(address);
        run_cc_replacement(address, value1, value2);
    }
}
