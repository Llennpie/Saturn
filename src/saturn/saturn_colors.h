#ifndef SaturnColors
#define SaturnColors

#include <stdio.h>
#include <stdbool.h>
extern unsigned int defaultColorHat[6];
extern unsigned int defaultColorOveralls[6];
extern unsigned int defaultColorGloves[6];
extern unsigned int defaultColorShoes[6];
extern unsigned int defaultColorSkin[6];
extern unsigned int defaultColorHair[6];

extern unsigned int sparkColorShirt[6];
extern unsigned int sparkColorShoulders[6];
extern unsigned int sparkColorArms[6];
extern unsigned int sparkColorOverallsBottom[6];
extern unsigned int sparkColorLegTop[6];
extern unsigned int sparkColorLegBottom[6];

extern unsigned int chromaColor[6];

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
