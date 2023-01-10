#ifndef SaturnColors
#define SaturnColors

#include <stdio.h>
#include <stdbool.h>
#ifdef __cplusplus
#include <array>

extern std::array<std::array<unsigned int, 2>, 3> defaultColorHat;
extern std::array<std::array<unsigned int, 2>, 3> defaultColorOveralls;
extern std::array<std::array<unsigned int, 2>, 3> defaultColorGloves;
extern std::array<std::array<unsigned int, 2>, 3> defaultColorShoes;
extern std::array<std::array<unsigned int, 2>, 3> defaultColorSkin;
extern std::array<std::array<unsigned int, 2>, 3> defaultColorHair;

// CometSPARK
extern std::array<std::array<unsigned int, 2>, 3> sparkColorShirt;
extern std::array<std::array<unsigned int, 2>, 3> sparkColorShoulders;
extern std::array<std::array<unsigned int, 2>, 3> sparkColorArms;
extern std::array<std::array<unsigned int, 2>, 3> sparkColorOverallsBottom;
extern std::array<std::array<unsigned int, 2>, 3> sparkColorLegTop;
extern std::array<std::array<unsigned int, 2>, 3> sparkColorLegBottom;

extern std::array<std::array<unsigned int, 2>, 3> chromaColor;

#else
extern unsigned int defaultColorHat[3][2];
extern unsigned int defaultColorOveralls[3][2];
extern unsigned int defaultColorGloves[3][2];
extern unsigned int defaultColorShoes[3][2];
extern unsigned int defaultColorSkin[3][2];
extern unsigned int defaultColorHair[3][2];

extern unsigned int sparkColorShirt[3][2];
extern unsigned int sparkColorShoulders[3][2];
extern unsigned int sparkColorArms[3][2];
extern unsigned int sparkColorOverallsBottom[3][2];
extern unsigned int sparkColorLegTop[3][2];
extern unsigned int sparkColorLegBottom[3][2];

extern unsigned int chromaColor[3][2];
#endif

extern bool cc_model_support;
extern bool cc_spark_support;

#ifdef __cplusplus
#include <string>
#include <vector>

extern std::vector<std::string> cc_array;
extern std::string global_gs_code();

extern bool is_default_cc(std::string gameshark);

void paste_gs_code(std::string content);
void save_cc_file(std::string name, std::string gameshark);
void save_cc_model_file(std::string name, std::string gameshark, std::string modelFolder);
void delete_cc_file(std::string name);
void delete_model_cc_file(std::string name, std::string modelFolder);
void set_cc_from_model(std::string ccPath);
extern std::vector<std::string> model_cc_array;
void get_ccs_from_model(std::string modelPath);

extern "C" {
#endif
    void saturn_load_cc_directory();
    void reset_cc_colors();
    void load_cc_file(char*);
#ifdef __cplusplus
}
#endif

#endif
