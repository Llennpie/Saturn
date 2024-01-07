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

#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
#include <array>
namespace fs = std::filesystem;
#include "pc/fs/fs.h"

std::vector<std::string> color_code_list;
std::vector<std::string> model_color_code_list;

ColorCode current_color_code;

// Palette as used by GFX
ColorTemplate defaultColorHat               {255, 127, 0,   0,   0,   0  };
ColorTemplate defaultColorOveralls          {0,   0,   0,   0,   255, 127};
ColorTemplate defaultColorGloves            {255, 127, 255, 127, 255, 127};
ColorTemplate defaultColorShoes             {114, 57,  28,  14,  14,  7  };
ColorTemplate defaultColorSkin              {254, 127, 193, 96,  121, 60 };
ColorTemplate defaultColorHair              {115, 57,  6,   3,   0,   0  };

ColorTemplate sparkColorShirt               {255, 127, 255, 127, 0,   0  };
ColorTemplate sparkColorShoulders           {0,   0,   255, 127, 255, 127};
ColorTemplate sparkColorArms                {0,   0,   255, 127, 127, 64 };
ColorTemplate sparkColorOverallsBottom      {255, 127, 0,   0,   255, 127}; 
ColorTemplate sparkColorLegTop              {255, 127, 0,   0,   127, 64 };
ColorTemplate sparkColorLegBottom           {127, 64,  0,   0,   255, 127};

std::string HexifyColorTemplate(ColorTemplate &colorBodyPart) {
    char colour[64];
    ImFormatString(colour, IM_ARRAYSIZE(colour), "%02X%02X%02X%02X%02X%02X"
            , ImClamp((int)colorBodyPart.red[0], 0, 255)
            , ImClamp((int)colorBodyPart.green[0], 0, 255)
            , ImClamp((int)colorBodyPart.blue[0], 0, 255)
            , ImClamp((int)colorBodyPart.red[1], 0, 255)
            , ImClamp((int)colorBodyPart.green[1], 0, 255)
            , ImClamp((int)colorBodyPart.blue[1], 0, 255));
    std::string col = colour;
    return col;
}

ColorTemplate chromaColor                   {0,   0,   255, 255, 0,   0  };

std::string last_model_cc_address;

/* Global toggle for Saturn's color code compatibility. True by default */
bool support_color_codes = true;
/* Global toggle for Saturn's SPARK extension. False by default */
bool support_spark = true;

void PasteGameShark(std::string GameShark) {
    std::istringstream f(GameShark);
    std::string line;
        
    while (std::getline(f, line)) {
        if (line.rfind("81", 0) == 0) {
            int address = std::stoi(line.substr(2, 6), 0, 16);
            int value1 = std::stoi(line.substr(9, 2), 0, 16);
            int value2 = std::stoi(line.substr(11, 2), 0, 16);

            switch(address) {

                /* Address (80000000 + Offset)  RED     BLUE
                                                GREEN           */

                // Cap
                case 0x07EC40:  defaultColorHat.red[0] = value1;        defaultColorHat.green[0] = value2;          break;
                case 0x07EC42:  defaultColorHat.blue[0] = value1;                                                   break;
                case 0x07EC38:  defaultColorHat.red[1] = value1;        defaultColorHat.green[1] = value2;          break;
                case 0x07EC3A:  defaultColorHat.blue[1] = value1;                                                   break;
                // Overalls
                case 0x07EC28:  defaultColorOveralls.red[0] = value1;   defaultColorOveralls.green[0] = value2;     break;
                case 0x07EC2A:  defaultColorOveralls.blue[0] = value1;                                              break;
                case 0x07EC20:  defaultColorOveralls.red[1] = value1;   defaultColorOveralls.green[1] = value2;     break;
                case 0x07EC22:  defaultColorOveralls.blue[1] = value1;                                              break;
                // Gloves
                case 0x07EC58:  defaultColorGloves.red[0] = value1;     defaultColorGloves.green[0] = value2;       break;
                case 0x07EC5A:  defaultColorGloves.blue[0] = value1;                                                break;
                case 0x07EC50:  defaultColorGloves.red[1] = value1;     defaultColorGloves.green[1] = value2;       break;
                case 0x07EC52:  defaultColorGloves.blue[1] = value1;                                                break;
                // Shoes
                case 0x07EC70:  defaultColorShoes.red[0] = value1;      defaultColorShoes.green[0] = value2;        break;
                case 0x07EC72:  defaultColorShoes.blue[0] = value1;                                                 break;
                case 0x07EC68:  defaultColorShoes.red[1] = value1;      defaultColorShoes.green[1] = value2;        break;
                case 0x07EC6A:  defaultColorShoes.blue[1] = value1;                                                 break;
                // Skin
                case 0x07EC88:  defaultColorSkin.red[0] = value1;       defaultColorSkin.green[0] = value2;         break;
                case 0x07EC8A:  defaultColorSkin.blue[0] = value1;                                                  break;
                case 0x07EC80:  defaultColorSkin.red[1] = value1;       defaultColorSkin.green[1] = value2;         break;
                case 0x07EC82:  defaultColorSkin.blue[1] = value1;                                                  break;
                // Hair
                case 0x07ECA0:  defaultColorHair.red[0] = value1;       defaultColorHair.green[0] = value2;         break;
                case 0x07ECA2:  defaultColorHair.blue[0] = value1;                                                  break;
                case 0x07EC98:  defaultColorHair.red[1] = value1;       defaultColorHair.green[1] = value2;         break;
                case 0x07EC9A:  defaultColorHair.blue[1] = value1;                                                  break;

                // (Optional) SPARK Addresses

                // Shirt
                case 0x07ECB8:  sparkColorShirt.red[0] = value1;            sparkColorShirt.green[0] = value2;          break;
                case 0x07ECBA:  sparkColorShirt.blue[0] = value1;                                                       break;
                case 0x07ECB0:  sparkColorShirt.red[1] = value1;            sparkColorShirt.green[1] = value2;          break;
                case 0x07ECB2:  sparkColorShirt.blue[1] = value1;                                                       break;
                // Shoulders
                case 0x07ECD0:  sparkColorShoulders.red[0] = value1;        sparkColorShoulders.green[0] = value2;      break;
                case 0x07ECD2:  sparkColorShoulders.blue[0] = value1;                                                   break;
                case 0x07ECC8:  sparkColorShoulders.red[1] = value1;        sparkColorShoulders.green[1] = value2;      break;
                case 0x07ECCA:  sparkColorShoulders.blue[1] = value1;                                                   break;
                // Arms
                case 0x07ECE8:  sparkColorArms.red[0] = value1;             sparkColorArms.green[0] = value2;           break;
                case 0x07ECEA:  sparkColorArms.blue[0] = value1;                                                        break;
                case 0x07ECE0:  sparkColorArms.red[1] = value1;             sparkColorArms.green[1] = value2;           break;
                case 0x07ECE2:  sparkColorArms.blue[1] = value1;                                                        break;
                // Pelvis
                case 0x07ED00:  sparkColorOverallsBottom.red[0] = value1;   sparkColorOverallsBottom.green[0] = value2; break;
                case 0x07ED02:  sparkColorOverallsBottom.blue[0] = value1;                                              break;
                case 0x07ECF8:  sparkColorOverallsBottom.red[1] = value1;   sparkColorOverallsBottom.green[1] = value2; break;
                case 0x07ECFA:  sparkColorOverallsBottom.blue[1] = value1;                                              break;
                // Thighs
                case 0x07ED18:  sparkColorLegTop.red[0] = value1;           sparkColorLegTop.green[0] = value2;         break;
                case 0x07ED1A:  sparkColorLegTop.blue[0] = value1;                                                      break;
                case 0x07ED10:  sparkColorLegTop.red[1] = value1;           sparkColorLegTop.green[1] = value2;         break;
                case 0x07ED12:  sparkColorLegTop.blue[1] = value1;                                                      break;
                // Calves
                case 0x07ED30:  sparkColorLegBottom.red[0] = value1;        sparkColorLegBottom.green[0] = value2;      break;
                case 0x07ED32:  sparkColorLegBottom.blue[0] = value1;                                                   break;
                case 0x07ED28:  sparkColorLegBottom.red[1] = value1;        sparkColorLegBottom.green[1] = value2;      break;
                case 0x07ED2A:  sparkColorLegBottom.blue[1] = value1;                                                   break;
            }
        }
    }
}

/*
    Applies a ColorCode to the game, overwriting the vanilla palette.
*/
void ApplyColorCode(ColorCode colorCode) {
    current_color_code = colorCode;
    PasteGameShark(colorCode.GameShark);
}

std::vector<std::string> GetColorCodeList(std::string folderPath) {
    std::vector<std::string> cc_list;

    if (folderPath.find("dynos/packs") != std::string::npos) {
        if (fs::exists(folderPath + "/../default.gs")) {
            cc_list.push_back("../default.gs");
        }
        
        if (fs::exists(folderPath)) {
            for (const auto & entry : fs::directory_iterator(folderPath)) {
                fs::path path = entry.path();

                if (path.filename().u8string() != "Mario") {
                    if (path.extension().u8string() == ".gs")
                        cc_list.push_back(path.filename().u8string());
                }
            }
        }
    } else {
        if (fs::exists(folderPath)) {
            cc_list.push_back("Mario.gs");

            for (const auto & entry : fs::directory_iterator(folderPath)) {
                fs::path path = entry.path();
                
                if (fs::is_directory(path)) continue;
                if (path.filename().u8string() != "Mario") {
                    if (path.extension().u8string() == ".gs")
                        cc_list.push_back(path.filename().u8string());
                }
            }
        }
    }

    return cc_list;
}

ColorCode LoadGSFile(std::string fileName, std::string filePath) {
    ColorCode colorCode;

    if (fileName == "../default.gs") {
        filePath = fs::path(filePath).parent_path().u8string();
        fileName = "default.gs";
    }

    if (fileName != "Mario.gs") {
        std::ifstream file(filePath + "/" + fileName, std::ios::in | std::ios::binary);
        if (file.good()) {
            std::cout << "Loaded CC file: " << filePath << "/" << fileName << std::endl;

            // Read GS File
            const std::size_t& size = std::filesystem::file_size(filePath + "/" + fileName);
            std::string content(size, '\0');
            file.read(content.data(), size);
            file.close();

            // Write to CC
            colorCode.Name = fileName.substr(0, fileName.size() - 3);
            colorCode.GameShark = content;
            colorCode.IsModel = (filePath.find("dynos/packs") != std::string::npos);

        } else {
            // Load failed; Refresh active list
            std::cout << "Failed to load " << fileName.c_str() << std::endl;
            if (filePath.find("dynos/packs") != std::string::npos) model_color_code_list = GetColorCodeList(filePath);
            else color_code_list = GetColorCodeList(filePath);
        }
    }

    // Change conflicting file names
    if (colorCode.Name == "Mario" || colorCode.Name == "default")
        colorCode.Name = "Sample";

    return colorCode;
}

void SaveGSFile(ColorCode colorCode, std::string filePath) {
    // Change conflicting file names
    if (colorCode.Name == "Mario" || colorCode.Name == "default")
        colorCode.Name = "Sample";

    // Create "/colorcodes" directory if it doesn't already exist
    if (!fs::exists(filePath))
        fs::create_directory(filePath + "/../colorcodes");

    std::ofstream file(filePath + "/" + colorCode.Name + ".gs");
    file << colorCode.GameShark;
}

void DeleteGSFile(std::string filePath) {
    // Disallow paths that reach out of bounds
    if (filePath.find("/colorcodes/") == std::string::npos ||
        filePath.find("../") != std::string::npos)
            return;

    if (fs::exists(filePath))
        fs::remove(filePath);
}

void saturn_refresh_cc_count() {
    //cc_details = "" + std::to_string(model_color_code_list.size()) + " color code";
    //if (model_color_code_list.size() != 1) cc_details += "s";
}