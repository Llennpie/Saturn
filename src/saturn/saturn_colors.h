#ifndef SaturnColors
#define SaturnColors

#include <stdio.h>
#include <stdbool.h>

// Struct definition:
/*    
    struct ColorTemplate {
        unsigned int red[2];
        unsigned int green[2];
        unsigned int blue[2];
    };
*/
// [0] for light
// [1] for dark

struct ColorTemplate {
    unsigned int red[2];
    unsigned int green[2];
    unsigned int blue[2];
};

extern struct ColorTemplate defaultColorHat;
extern struct ColorTemplate defaultColorOveralls;
extern struct ColorTemplate defaultColorGloves;
extern struct ColorTemplate defaultColorShoes;
extern struct ColorTemplate defaultColorSkin;
extern struct ColorTemplate defaultColorHair;

extern struct ColorTemplate sparkColorShirt;
extern struct ColorTemplate sparkColorShoulders;
extern struct ColorTemplate sparkColorArms;
extern struct ColorTemplate sparkColorOverallsBottom; 
extern struct ColorTemplate sparkColorLegTop;
extern struct ColorTemplate sparkColorLegBottom;

extern struct ColorTemplate chromaColor;

#ifdef __cplusplus

#include <vector>
#include <string>
#include "saturn/libs/imgui/imgui_internal.h"

extern std::string HexifyColorTemplate(ColorTemplate &colorBodyPart);

class ColorCode {
    private:
        class ColorPart {
            int MainR, MainG, MainB;
            int ShadeR, ShadeG, ShadeB;

            // Initializer
            public:
                ColorPart(int x, int y, int z, int x1, int y1, int z1) : MainR(x1), MainG(y1), MainB(z1), ShadeR(x1), ShadeG(y1), ShadeB(z1) {}
        };

        class ColorPalette {
            ColorPart Cap =         ColorPart(255, 0, 0,        127, 0, 0);
            ColorPart Overalls =    ColorPart(0, 0, 255,        0, 0, 127);
            ColorPart Gloves =      ColorPart(0, 255, 0,        0, 127, 0);
            ColorPart Shoes =       ColorPart(114, 28, 14,      57, 14, 7);
            ColorPart Skin =        ColorPart(254, 193, 121,    127, 96, 60);
            ColorPart Hair =        ColorPart(115, 6, 0,        57, 3, 0);
        };

    public:
        std::string Name = "Sample";
        std::string GameShark = "8107EC40 FF00\n8107EC42 0000\n8107EC38 7F00\n8107EC3A 0000\n"
                                "8107EC28 0000\n8107EC2A FF00\n8107EC20 0000\n8107EC22 7F00\n"
                                "8107EC58 FFFF\n8107EC5A FF00\n8107EC50 7F7F\n8107EC52 7F00\n"
                                "8107EC70 721C\n8107EC72 0E00\n8107EC68 390E\n8107EC6A 0700\n"
                                "8107EC88 FEC1\n8107EC8A 7900\n8107EC80 7F60\n8107EC82 3C00\n"
                                "8107ECA0 7306\n8107ECA2 0000\n8107EC98 3903\n8107EC9A 0000";

        void ParseGameShark() {
            std::string gameshark;

            std::string col1 = HexifyColorTemplate(defaultColorHat);
            std::string col2 = HexifyColorTemplate(defaultColorOveralls);
            std::string col3 = HexifyColorTemplate(defaultColorGloves);
            std::string col4 = HexifyColorTemplate(defaultColorShoes);
            std::string col5 = HexifyColorTemplate(defaultColorSkin);
            std::string col6 = HexifyColorTemplate(defaultColorHair);

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

            if (this->IsSpark()) {
                gameshark += "\n";

                std::string col7 = HexifyColorTemplate(sparkColorShirt);
                std::string col8 = HexifyColorTemplate(sparkColorShoulders);
                std::string col9 = HexifyColorTemplate(sparkColorArms);
                std::string col10 = HexifyColorTemplate(sparkColorOverallsBottom);
                std::string col11 = HexifyColorTemplate(sparkColorLegTop);
                std::string col12 = HexifyColorTemplate(sparkColorLegBottom);

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

            this->GameShark = gameshark;
        }

        ColorPalette Colors;

        /* Returns true if the CC is in SPARK format */
        bool IsSpark() {
            // SPARK GS codes are twice the size as regular GS codes
            return (this->GameShark.length() >= 718);
        }

        bool IsModel;

        /* Returns true if the CC colors match the vanilla palette */
        bool IsDefaultColors() {
            if (!this->IsSpark()) {
                // Default Color Code
                return (this->GameShark ==  "8107EC40 FF00\n8107EC42 0000\n8107EC38 7F00\n8107EC3A 0000\n"
                                            "8107EC28 0000\n8107EC2A FF00\n8107EC20 0000\n8107EC22 7F00\n"
                                            "8107EC58 FFFF\n8107EC5A FF00\n8107EC50 7F7F\n8107EC52 7F00\n"
                                            "8107EC70 721C\n8107EC72 0E00\n8107EC68 390E\n8107EC6A 0700\n"
                                            "8107EC88 FEC1\n8107EC8A 7900\n8107EC80 7F60\n8107EC82 3C00\n"
                                            "8107ECA0 7306\n8107ECA2 0000\n8107EC98 3903\n8107EC9A 0000");
            } else {
                // Default SPARK Color Code
                return (this->GameShark ==  "8107EC40 FF00\n8107EC42 0000\n8107EC38 7F00\n8107EC3A 0000\n"
                                            "8107EC28 0000\n8107EC2A FF00\n8107EC20 0000\n8107EC22 7F00\n"
                                            "8107EC58 FFFF\n8107EC5A FF00\n8107EC50 7F7F\n8107EC52 7F00\n"
                                            "8107EC70 721C\n8107EC72 0E00\n8107EC68 390E\n8107EC6A 0700\n"
                                            "8107EC88 FEC1\n8107EC8A 7900\n8107EC80 7F60\n8107EC82 3C00\n"
                                            "8107ECA0 7306\n8107ECA2 0000\n8107EC98 3903\n8107EC9A 0000\n"
                                            "8107ECB8 FFFF\n8107ECBA 0000\n8107ECB0 7F7F\n8107ECB2 0000\n"
                                            "8107ECD0 00FF\n8107ECD2 FF00\n8107ECC8 007F\n8107ECCA 7F00\n"
                                            "8107ECE8 00FF\n8107ECEA 7F00\n8107ECE0 007F\n8107ECE2 4000\n"
                                            "8107ED00 FF00\n8107ED02 FF00\n8107ECF8 7F00\n8107ECFA 7F00\n"
                                            "8107ED18 FF00\n8107ED1A 7F00\n8107ED10 7F00\n8107ED12 4000\n"
                                            "8107ED30 7F00\n8107ED32 FF00\n8107ED28 4000\n8107ED2A 7F00");
            }
        }
};

extern void PasteGameShark(std::string);
extern void ApplyColorCode(ColorCode);

extern std::vector<std::string> GetColorCodeList(std::string);
extern ColorCode LoadGSFile(std::string, std::string);
extern void SaveGSFile(ColorCode, std::string);

extern std::vector<std::string> color_code_list;
extern std::vector<std::string> model_color_code_list;

extern ColorCode current_color_code;

#endif

extern bool cc_model_support;
extern bool cc_spark_support;

#ifdef __cplusplus
#include <string>
#include <vector>

extern std::string last_model_cc_address;
void saturn_refresh_cc_count();

#endif

#endif
