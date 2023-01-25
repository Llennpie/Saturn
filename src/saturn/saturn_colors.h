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

// CometSPARK
extern unsigned int sparkColorShirtRLight;
extern unsigned int sparkColorShirtRDark;
extern unsigned int sparkColorShirtGLight;
extern unsigned int sparkColorShirtGDark;
extern unsigned int sparkColorShirtBLight;
extern unsigned int sparkColorShirtBDark;

extern unsigned int sparkColorShouldersRLight;
extern unsigned int sparkColorShouldersRDark;
extern unsigned int sparkColorShouldersGLight;
extern unsigned int sparkColorShouldersGDark;
extern unsigned int sparkColorShouldersBLight;
extern unsigned int sparkColorShouldersBDark;

extern unsigned int sparkColorArmsRLight;
extern unsigned int sparkColorArmsRDark;
extern unsigned int sparkColorArmsGLight;
extern unsigned int sparkColorArmsGDark;
extern unsigned int sparkColorArmsBLight;
extern unsigned int sparkColorArmsBDark;

extern unsigned int sparkColorOverallsBottomRLight;
extern unsigned int sparkColorOverallsBottomRDark;
extern unsigned int sparkColorOverallsBottomGLight;
extern unsigned int sparkColorOverallsBottomGDark;
extern unsigned int sparkColorOverallsBottomBLight;
extern unsigned int sparkColorOverallsBottomBDark;

extern unsigned int sparkColorLegTopRLight;
extern unsigned int sparkColorLegTopRDark;
extern unsigned int sparkColorLegTopGLight;
extern unsigned int sparkColorLegTopGDark;
extern unsigned int sparkColorLegTopBLight;
extern unsigned int sparkColorLegTopBDark;

extern unsigned int sparkColorLegBottomRLight;
extern unsigned int sparkColorLegBottomRDark;
extern unsigned int sparkColorLegBottomGLight;
extern unsigned int sparkColorLegBottomGDark;
extern unsigned int sparkColorLegBottomBLight;
extern unsigned int sparkColorLegBottomBDark;

extern unsigned int chromaColorRLight;
extern unsigned int chromaColorRDark;
extern unsigned int chromaColorGLight;
extern unsigned int chromaColorGDark;
extern unsigned int chromaColorBLight;
extern unsigned int chromaColorBDark;

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
extern std::string last_model_cc_address;

extern "C" {
#endif
    void saturn_load_cc_directory();
    void reset_cc_colors();
    void load_cc_file(char*);
#ifdef __cplusplus
}
#endif

#endif