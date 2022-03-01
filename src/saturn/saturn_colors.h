#ifndef SaturnColors
#define SaturnColors

#include <stdio.h>
#include <stdbool.h>

extern unsigned int defaultColorHatRLight;
extern unsigned int defaultColorHatRDark;
extern unsigned int defaultColorHatGLight;
extern unsigned int defaultColorHatGDark;
extern unsigned int defaultColorHatBLight;
extern unsigned int defaultColorHatBDark;

extern unsigned int defaultColorOverallsRLight;
extern unsigned int defaultColorOverallsRDark;
extern unsigned int defaultColorOverallsGLight;
extern unsigned int defaultColorOverallsGDark;
extern unsigned int defaultColorOverallsBLight;
extern unsigned int defaultColorOverallsBDark;

extern unsigned int defaultColorGlovesRLight;
extern unsigned int defaultColorGlovesRDark;
extern unsigned int defaultColorGlovesGLight;
extern unsigned int defaultColorGlovesGDark;
extern unsigned int defaultColorGlovesBLight;
extern unsigned int defaultColorGlovesBDark;

extern unsigned int defaultColorShoesRLight;
extern unsigned int defaultColorShoesRDark;
extern unsigned int defaultColorShoesGLight;
extern unsigned int defaultColorShoesGDark;
extern unsigned int defaultColorShoesBLight;
extern unsigned int defaultColorShoesBDark;

extern unsigned int defaultColorSkinRLight;
extern unsigned int defaultColorSkinRDark;
extern unsigned int defaultColorSkinGLight;
extern unsigned int defaultColorSkinGDark;
extern unsigned int defaultColorSkinBLight;
extern unsigned int defaultColorSkinBDark;

extern unsigned int defaultColorHairRLight;
extern unsigned int defaultColorHairRDark;
extern unsigned int defaultColorHairGLight;
extern unsigned int defaultColorHairGDark;
extern unsigned int defaultColorHairBLight;
extern unsigned int defaultColorHairBDark;

extern bool cc_model_support;

#ifdef __cplusplus
#include <string>
#include <vector>

extern std::vector<std::string> cc_array;
extern std::string global_gs_code();

void paste_gs_code(std::string content);
void save_cc_file(std::string name);
void delete_cc_file(std::string name);

extern "C" {
#endif
    void load_cc_directory();
    void reset_cc_colors();
    void load_cc_file(char*);
#ifdef __cplusplus
}
#endif

#endif