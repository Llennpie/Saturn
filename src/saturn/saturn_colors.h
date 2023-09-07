#ifndef SaturnColors
#define SaturnColors

#include <stdio.h>
#include <stdbool.h>

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

extern bool cc_model_support;
extern bool cc_spark_support;

#ifdef __cplusplus
#include <string>
#include <vector>

extern std::vector<std::string> cc_array;
extern std::string global_gs_code();

extern std::string current_cc_path;
extern int current_cc_path_dirs;

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
void saturn_refresh_cc_count();

extern "C" {
#endif
    void saturn_load_cc_directory();
    void reset_cc_colors();
    void load_cc_file(char*);
#ifdef __cplusplus
}
#endif

#endif
