#include "saturn/saturn_colors.h"

#include <string>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "saturn/saturn.h"
#include "saturn/imgui/saturn_imgui.h"

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

// CometSPARK
unsigned int sparkColorShirtRLight = 255;
unsigned int sparkColorShirtRDark = 127;
unsigned int sparkColorShirtGLight = 255;
unsigned int sparkColorShirtGDark = 127;
unsigned int sparkColorShirtBLight = 0;
unsigned int sparkColorShirtBDark = 0;

unsigned int sparkColorShouldersRLight = 0;
unsigned int sparkColorShouldersRDark = 0;
unsigned int sparkColorShouldersGLight = 255;
unsigned int sparkColorShouldersGDark = 127;
unsigned int sparkColorShouldersBLight = 255;
unsigned int sparkColorShouldersBDark = 127;

unsigned int sparkColorArmsRLight = 0;
unsigned int sparkColorArmsRDark = 0;
unsigned int sparkColorArmsGLight = 255;
unsigned int sparkColorArmsGDark = 127;
unsigned int sparkColorArmsBLight = 127;
unsigned int sparkColorArmsBDark = 64;

unsigned int sparkColorOverallsBottomRLight = 255;
unsigned int sparkColorOverallsBottomRDark = 127;
unsigned int sparkColorOverallsBottomGLight = 0;
unsigned int sparkColorOverallsBottomGDark = 0;
unsigned int sparkColorOverallsBottomBLight = 255;
unsigned int sparkColorOverallsBottomBDark = 127;

unsigned int sparkColorLegTopRLight = 255;
unsigned int sparkColorLegTopRDark = 127;
unsigned int sparkColorLegTopGLight = 0;
unsigned int sparkColorLegTopGDark = 0;
unsigned int sparkColorLegTopBLight = 127;
unsigned int sparkColorLegTopBDark = 64;

unsigned int sparkColorLegBottomRLight = 127;
unsigned int sparkColorLegBottomRDark = 64;
unsigned int sparkColorLegBottomGLight = 0;
unsigned int sparkColorLegBottomGDark = 0;
unsigned int sparkColorLegBottomBLight = 255;
unsigned int sparkColorLegBottomBDark = 127;

// Color Codes

bool cc_model_support = true;
bool cc_spark_support = false;

std::vector<string> cc_array;
string colorCodeDir;

bool modelCcLoaded;

/*
    The currently active GameShark code in string format.
*/
std::string global_gs_code() {
    std::string gameshark;

    char col1char[64];
    ImFormatString(col1char, IM_ARRAYSIZE(col1char), "%02X%02X%02X", ImClamp((int)defaultColorHatRLight, 0, 255), ImClamp((int)defaultColorHatGLight, 0, 255), ImClamp((int)defaultColorHatBLight, 0, 255));
    std::string col1 = col1char;
    char col2char[64];
    ImFormatString(col2char, IM_ARRAYSIZE(col2char), "%02X%02X%02X", ImClamp((int)defaultColorHatRDark, 0, 255), ImClamp((int)defaultColorHatGDark, 0, 255), ImClamp((int)defaultColorHatBDark, 0, 255));
    std::string col2 = col2char;
    char col3char[64];
    ImFormatString(col3char, IM_ARRAYSIZE(col3char), "%02X%02X%02X", ImClamp((int)defaultColorOverallsRLight, 0, 255), ImClamp((int)defaultColorOverallsGLight, 0, 255), ImClamp((int)defaultColorOverallsBLight, 0, 255));
    std::string col3 = col3char;
    char col4char[64];
    ImFormatString(col4char, IM_ARRAYSIZE(col4char), "%02X%02X%02X", ImClamp((int)defaultColorOverallsRDark, 0, 255), ImClamp((int)defaultColorOverallsGDark, 0, 255), ImClamp((int)defaultColorOverallsBDark, 0, 255));
    std::string col4 = col4char;
    char col5char[64];
    ImFormatString(col5char, IM_ARRAYSIZE(col5char), "%02X%02X%02X", ImClamp((int)defaultColorGlovesRLight, 0, 255), ImClamp((int)defaultColorGlovesGLight, 0, 255), ImClamp((int)defaultColorGlovesBLight, 0, 255));
    std::string col5 = col5char;
    char col6char[64];
    ImFormatString(col6char, IM_ARRAYSIZE(col6char), "%02X%02X%02X", ImClamp((int)defaultColorGlovesRDark, 0, 255), ImClamp((int)defaultColorGlovesGDark, 0, 255), ImClamp((int)defaultColorGlovesBDark, 0, 255));
    std::string col6 = col6char;
    char col7char[64];
    ImFormatString(col7char, IM_ARRAYSIZE(col7char), "%02X%02X%02X", ImClamp((int)defaultColorShoesRLight, 0, 255), ImClamp((int)defaultColorShoesGLight, 0, 255), ImClamp((int)defaultColorShoesBLight, 0, 255));
    std::string col7 = col7char;
    char col8char[64];
    ImFormatString(col8char, IM_ARRAYSIZE(col8char), "%02X%02X%02X", ImClamp((int)defaultColorShoesRDark, 0, 255), ImClamp((int)defaultColorShoesGDark, 0, 255), ImClamp((int)defaultColorShoesBDark, 0, 255));
    std::string col8 = col8char;
    char col9char[64];
    ImFormatString(col9char, IM_ARRAYSIZE(col9char), "%02X%02X%02X", ImClamp((int)defaultColorSkinRLight, 0, 255), ImClamp((int)defaultColorSkinGLight, 0, 255), ImClamp((int)defaultColorSkinBLight, 0, 255));
    std::string col9 = col9char;
    char col10char[64];
    ImFormatString(col10char, IM_ARRAYSIZE(col10char), "%02X%02X%02X", ImClamp((int)defaultColorSkinRDark, 0, 255), ImClamp((int)defaultColorSkinGDark, 0, 255), ImClamp((int)defaultColorSkinBDark, 0, 255));
    std::string col10 = col10char;
    char col11char[64];
    ImFormatString(col11char, IM_ARRAYSIZE(col11char), "%02X%02X%02X", ImClamp((int)defaultColorHairRLight, 0, 255), ImClamp((int)defaultColorHairGLight, 0, 255), ImClamp((int)defaultColorHairBLight, 0, 255));
    std::string col11 = col11char;
    char col12char[64];
    ImFormatString(col12char, IM_ARRAYSIZE(col12char), "%02X%02X%02X", ImClamp((int)defaultColorHairRDark, 0, 255), ImClamp((int)defaultColorHairGDark, 0, 255), ImClamp((int)defaultColorHairBDark, 0, 255));
    std::string col12 = col12char;

    gameshark += "8107EC40 " + col1.substr(0, 2) + col1.substr(2, 2) + "\n";
    gameshark += "8107EC42 " + col1.substr(4, 2) + "00\n";
    gameshark += "8107EC38 " + col2.substr(0, 2) + col2.substr(2, 2) + "\n";
    gameshark += "8107EC3A " + col2.substr(4, 2) + "00\n";
    gameshark += "8107EC28 " + col3.substr(0, 2) + col3.substr(2, 2) + "\n";
    gameshark += "8107EC2A " + col3.substr(4, 2) + "00\n";
    gameshark += "8107EC20 " + col4.substr(0, 2) + col4.substr(2, 2) + "\n";
    gameshark += "8107EC22 " + col4.substr(4, 2) + "00\n";
    gameshark += "8107EC58 " + col5.substr(0, 2) + col5.substr(2, 2) + "\n";
    gameshark += "8107EC5A " + col5.substr(4, 2) + "00\n";
    gameshark += "8107EC50 " + col6.substr(0, 2) + col6.substr(2, 2) + "\n";
    gameshark += "8107EC52 " + col6.substr(4, 2) + "00\n";
    gameshark += "8107EC70 " + col7.substr(0, 2) + col7.substr(2, 2) + "\n";
    gameshark += "8107EC72 " + col7.substr(4, 2) + "00\n";
    gameshark += "8107EC68 " + col8.substr(0, 2) + col8.substr(2, 2) + "\n";
    gameshark += "8107EC6A " + col8.substr(4, 2) + "00\n";
    gameshark += "8107EC88 " + col9.substr(0, 2) + col9.substr(2, 2) + "\n";
    gameshark += "8107EC8A " + col9.substr(4, 2) + "00\n";
    gameshark += "8107EC80 " + col10.substr(0, 2) + col10.substr(2, 2) + "\n";
    gameshark += "8107EC82 " + col10.substr(4, 2) + "00\n";
    gameshark += "8107ECA0 " + col11.substr(0, 2) + col11.substr(2, 2) + "\n";
    gameshark += "8107ECA2 " + col11.substr(4, 2) + "00\n";
    gameshark += "8107EC98 " + col12.substr(0, 2) + col12.substr(2, 2) + "\n";
    if (!cc_spark_support) {
        gameshark += "8107EC9A " + col12.substr(4, 2) + "00";
    } else {
        gameshark += "8107EC9A " + col12.substr(4, 2) + "00\n";

        char col13char[64];
        ImFormatString(col13char, IM_ARRAYSIZE(col13char), "%02X%02X%02X", ImClamp((int)sparkColorShirtRLight, 0, 255), ImClamp((int)sparkColorShirtGLight, 0, 255), ImClamp((int)sparkColorShirtBLight, 0, 255));
        std::string col13 = col13char;
        char col14char[64];
        ImFormatString(col14char, IM_ARRAYSIZE(col14char), "%02X%02X%02X", ImClamp((int)sparkColorShirtRDark, 0, 255), ImClamp((int)sparkColorShirtGDark, 0, 255), ImClamp((int)sparkColorShirtBDark, 0, 255));
        std::string col14 = col14char;
        char col15char[64];
        ImFormatString(col15char, IM_ARRAYSIZE(col15char), "%02X%02X%02X", ImClamp((int)sparkColorShouldersRLight, 0, 255), ImClamp((int)sparkColorShouldersGLight, 0, 255), ImClamp((int)sparkColorShouldersBLight, 0, 255));
        std::string col15 = col15char;
        char col16char[64];
        ImFormatString(col16char, IM_ARRAYSIZE(col16char), "%02X%02X%02X", ImClamp((int)sparkColorShouldersRDark, 0, 255), ImClamp((int)sparkColorShouldersGDark, 0, 255), ImClamp((int)sparkColorShouldersBDark, 0, 255));
        std::string col16 = col16char;
        char col17char[64];
        ImFormatString(col17char, IM_ARRAYSIZE(col17char), "%02X%02X%02X", ImClamp((int)sparkColorArmsRLight, 0, 255), ImClamp((int)sparkColorArmsGLight, 0, 255), ImClamp((int)sparkColorArmsBLight, 0, 255));
        std::string col17 = col17char;
        char col18char[64];
        ImFormatString(col18char, IM_ARRAYSIZE(col18char), "%02X%02X%02X", ImClamp((int)sparkColorArmsRDark, 0, 255), ImClamp((int)sparkColorArmsGDark, 0, 255), ImClamp((int)sparkColorArmsBDark, 0, 255));
        std::string col18 = col18char;
        char col19char[64];
        ImFormatString(col19char, IM_ARRAYSIZE(col19char), "%02X%02X%02X", ImClamp((int)sparkColorOverallsBottomRLight, 0, 255), ImClamp((int)sparkColorOverallsBottomGLight, 0, 255), ImClamp((int)sparkColorOverallsBottomBLight, 0, 255));
        std::string col19 = col19char;
        char col20char[64];
        ImFormatString(col20char, IM_ARRAYSIZE(col20char), "%02X%02X%02X", ImClamp((int)sparkColorOverallsBottomRDark, 0, 255), ImClamp((int)sparkColorOverallsBottomGDark, 0, 255), ImClamp((int)sparkColorOverallsBottomBDark, 0, 255));
        std::string col20 = col20char;
        char col21char[64];
        ImFormatString(col21char, IM_ARRAYSIZE(col21char), "%02X%02X%02X", ImClamp((int)sparkColorLegTopRLight, 0, 255), ImClamp((int)sparkColorLegTopGLight, 0, 255), ImClamp((int)sparkColorLegTopBLight, 0, 255));
        std::string col21 = col21char;
        char col22char[64];
        ImFormatString(col22char, IM_ARRAYSIZE(col22char), "%02X%02X%02X", ImClamp((int)sparkColorLegTopRDark, 0, 255), ImClamp((int)sparkColorLegTopGDark, 0, 255), ImClamp((int)sparkColorLegTopBDark, 0, 255));
        std::string col22 = col22char;
        char col23char[64];
        ImFormatString(col23char, IM_ARRAYSIZE(col23char), "%02X%02X%02X", ImClamp((int)sparkColorLegBottomRLight, 0, 255), ImClamp((int)sparkColorLegBottomGLight, 0, 255), ImClamp((int)sparkColorLegBottomBLight, 0, 255));
        std::string col23 = col23char;
        char col24char[64];
        ImFormatString(col24char, IM_ARRAYSIZE(col24char), "%02X%02X%02X", ImClamp((int)sparkColorLegBottomRDark, 0, 255), ImClamp((int)sparkColorLegBottomGDark, 0, 255), ImClamp((int)sparkColorLegBottomBDark, 0, 255));
        std::string col24 = col24char;

        gameshark += "8107ECB8 " + col13.substr(0, 2) + col13.substr(2, 2) + "\n";
        gameshark += "8107ECBA " + col13.substr(4, 2) + "00\n";
        gameshark += "8107ECB0 " + col14.substr(0, 2) + col14.substr(2, 2) + "\n";
        gameshark += "8107ECB2 " + col14.substr(4, 2) + "00\n";
        gameshark += "8107ECD0 " + col15.substr(0, 2) + col15.substr(2, 2) + "\n";
        gameshark += "8107ECD2 " + col15.substr(4, 2) + "00\n";
        gameshark += "8107ECC8 " + col16.substr(0, 2) + col16.substr(2, 2) + "\n";
        gameshark += "8107ECCA " + col16.substr(4, 2) + "00\n";
        gameshark += "8107ECE8 " + col17.substr(0, 2) + col17.substr(2, 2) + "\n";
        gameshark += "8107ECEA " + col17.substr(4, 2) + "00\n";
        gameshark += "8107ECE0 " + col18.substr(0, 2) + col18.substr(2, 2) + "\n";
        gameshark += "8107ECE2 " + col18.substr(4, 2) + "00\n";
        gameshark += "8107ED00 " + col19.substr(0, 2) + col19.substr(2, 2) + "\n";
        gameshark += "8107ED02 " + col19.substr(4, 2) + "00\n";
        gameshark += "8107ECF8 " + col20.substr(0, 2) + col20.substr(2, 2) + "\n";
        gameshark += "8107ECFA " + col20.substr(4, 2) + "00\n";
        gameshark += "8107ED18 " + col21.substr(0, 2) + col21.substr(2, 2) + "\n";
        gameshark += "8107ED1A " + col21.substr(4, 2) + "00\n";
        gameshark += "8107ED10 " + col22.substr(0, 2) + col22.substr(2, 2) + "\n";
        gameshark += "8107ED12 " + col22.substr(4, 2) + "00\n";
        gameshark += "8107ED30 " + col23.substr(0, 2) + col23.substr(2, 2) + "\n";
        gameshark += "8107ED32 " + col23.substr(4, 2) + "00\n";
        gameshark += "8107ED28 " + col24.substr(0, 2) + col24.substr(2, 2) + "\n";
        gameshark += "8107ED2A " + col24.substr(4, 2) + "00";
    }

    return gameshark;
}

/*
    Returns true if a defined address is for the CometSPARK format.
*/
bool is_spark_address(string address) {
    // The unholy address table
    if (address == "07ECB8" || address == "07ECBA" || address == "07ECB0" || address == "07ECB2")
        return true;
    if (address == "07ECD0" || address == "07ECD2" || address == "07ECC8" || address == "07ECCA")
        return true;
    if (address == "07ECE8" || address == "07ECEA" || address == "07ECE0" || address == "07ECE2")
        return true;
    if (address == "07ED00" || address == "07ED02" || address == "07ECF8"|| address == "07ECFA")
        return true;
    if (address == "07ED18" || address == "07ED1A" || address == "07ED10" || address == "07ED12")
        return true;
    if (address == "07ED30" || address == "07ED32" || address == "07ED28" || address == "07ED2A")
        return true;

    // Nope
    return false;
}

/*
    Loads the dynos/colorcodes/ folder into the CC array.
*/
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
        if (entry.path().filename().u8string() != "Mario")
            cc_array.push_back(entry.path().filename().u8string());
}

/*
    Preferably used in a while loop, sets a global color with a defined address and value.
*/
void run_cc_replacement(string address, int value1, int value2) {
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

    // --------
    // EXPERIMENTAL: CometSPARK Support
    // --------
    // Things MIGHT break. This is unfinished and not fully compatible with Saturn yet.
    // Pull requests and detailed GH issues are much appreciated...
    // --------

    if (!cc_spark_support)
        return;

    // Shirt
    if (address == "07ECB8") {
        sparkColorShirtRLight = value1;
        sparkColorShirtGLight = value2;
    }
    if (address == "07ECBA") {
        sparkColorShirtBLight = value1;
    }
    if (address == "07ECB0") {
        sparkColorShirtRDark = value1;
        sparkColorShirtGDark = value2;
    }
    if (address == "07ECB2") {
        sparkColorShirtBDark = value1;
    }

    // Shoulders
    if (address == "07ECD0") {
        sparkColorShouldersRLight = value1;
        sparkColorShouldersGLight = value2;
    }
    if (address == "07ECD2") {
        sparkColorShouldersBLight = value1;
    }
    if (address == "07ECC8") {
        sparkColorShouldersRDark = value1;
        sparkColorShouldersGDark = value2;
    }
    if (address == "07ECCA") {
        sparkColorShouldersBDark = value1;
    }

    // Arms
    if (address == "07ECE8") {
        sparkColorArmsRLight = value1;
        sparkColorArmsGLight = value2;
    }
    if (address == "07ECEA") {
        sparkColorArmsBLight = value1;
    }
    if (address == "07ECE0") {
        sparkColorArmsRDark = value1;
        sparkColorArmsGDark = value2;
    }
    if (address == "07ECE2") {
        sparkColorArmsBDark = value1;
    }

    // OverallsBottom
    if (address == "07ED00") {
        sparkColorOverallsBottomRLight = value1;
        sparkColorOverallsBottomGLight = value2;
    }
    if (address == "07ED02") {
        sparkColorOverallsBottomBLight = value1;
    }
    if (address == "07ECF8") {
        sparkColorOverallsBottomRDark = value1;
        sparkColorOverallsBottomGDark = value2;
    }
    if (address == "07ECFA") {
        sparkColorOverallsBottomBDark = value1;
    }

    // LegTop
    if (address == "07ED18") {
        sparkColorLegTopRLight = value1;
        sparkColorLegTopGLight = value2;
    }
    if (address == "07ED1A") {
        sparkColorLegTopBLight = value1;
    }
    if (address == "07ED10") {
        sparkColorLegTopRDark = value1;
        sparkColorLegTopGDark = value2;
    }
    if (address == "07ED12") {
        sparkColorLegTopBDark = value1;
    }

    // LegBottom
    if (address == "07ED30") {
        sparkColorLegBottomRLight = value1;
        sparkColorLegBottomGLight = value2;
    }
    if (address == "07ED32") {
        sparkColorLegBottomBLight = value1;
    }
    if (address == "07ED28") {
        sparkColorLegBottomRDark = value1;
        sparkColorLegBottomGDark = value2;
    }
    if (address == "07ED2A") {
        sparkColorLegBottomBDark = value1;
    }
}

/*
    Resets Mario's colors to default.
*/
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

    // CometSPARK
    sparkColorShirtRLight = 255;
    sparkColorShirtRDark = 127;
    sparkColorShirtGLight = 255;
    sparkColorShirtGDark = 127;
    sparkColorShirtBLight = 0;
    sparkColorShirtBDark = 0;

    sparkColorShouldersRLight = 0;
    sparkColorShouldersRDark = 0;
    sparkColorShouldersGLight = 255;
    sparkColorShouldersGDark = 127;
    sparkColorShouldersBLight = 255;
    sparkColorShouldersBDark = 127;

    sparkColorArmsRLight = 0;
    sparkColorArmsRDark = 0;
    sparkColorArmsGLight = 255;
    sparkColorArmsGDark = 127;
    sparkColorArmsBLight = 127;
    sparkColorArmsBDark = 64;

    sparkColorOverallsBottomRLight = 255;
    sparkColorOverallsBottomRDark = 127;
    sparkColorOverallsBottomGLight = 0;
    sparkColorOverallsBottomGDark = 0;
    sparkColorOverallsBottomBLight = 255;
    sparkColorOverallsBottomBDark = 127;

    sparkColorLegTopRLight = 255;
    sparkColorLegTopRDark = 127;
    sparkColorLegTopGLight = 0;
    sparkColorLegTopGDark = 0;
    sparkColorLegTopBLight = 127;
    sparkColorLegTopBDark = 64;

    sparkColorLegBottomRLight = 127;
    sparkColorLegBottomRDark = 64;
    sparkColorLegBottomGLight = 0;
    sparkColorLegBottomGDark = 0;
    sparkColorLegBottomBLight = 255;
    sparkColorLegBottomBDark = 127;
}

/*
    Sets Mario's colors using a GameShark code string.
*/
void paste_gs_code(string content) {
    std::istringstream f(content);
    std::string line;
        
    while (std::getline(f, line)) {
        std::string address = line.substr(2, 6);
        int value1 = std::stoi(line.substr(9, 2), 0, 16);
        int value2 = std::stoi(line.substr(11, 2), 0, 16);

        run_cc_replacement(address, value1, value2);
    }
}

/*
    Saves the global_gs_code() to a GS file.
*/
void save_cc_file(std::string name) {
#ifdef __MINGW32__
    std::ofstream file("dynos\\colorcodes\\" + name + ".gs");
#else
    std::ofstream file("dynos/colorcodes/" + name + ".gs");
#endif
    file << global_gs_code();
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
        load_cc_directory();
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
    load_cc_directory();
}

/*
    Sets Mario's colors using a defined model path.
*/
void set_cc_from_model(std::string modelPath) {
    // We're using "default.gs" to set a default palette
#ifdef __MINGW32__
    string filePath = modelPath + "\\default.gs";
#else
    string filePath = modelPath + "/default.gs";
#endif
    std::ifstream file(filePath, std::ios::in | std::ios::binary);

    if (!file.good()) {
        // No file exists, cancel
        return;
    }

    const std::size_t& size = std::filesystem::file_size(filePath);
    std::string content(size, '\0');
    file.read(content.data(), size);
    file.close();

    std::istringstream f(content);
    std::string line;
        
    while (std::getline(f, line)) {
        std::string address = line.substr(2, 6);
        int value1 = std::stoi(line.substr(9, 2), 0, 16);
        int value2 = std::stoi(line.substr(11, 2), 0, 16);

        if (!cc_spark_support) cc_spark_support = is_spark_address(address);
        run_cc_replacement(address, value1, value2);
    }
}