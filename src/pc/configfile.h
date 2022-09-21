#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <stdbool.h>

#define CONFIGFILE_DEFAULT "sm64config.txt"

#define MAX_BINDS  3
#define MAX_VOLUME 127

typedef struct {
    unsigned int x, y, w, h;
    bool vsync;
    bool enable_antialias;
    unsigned int antialias_level;
    bool jabo_mode;
    bool reset;
    bool fullscreen;
    bool exiting_fullscreen;
    bool settings_changed;
} ConfigWindow;

extern ConfigWindow configWindow;
extern unsigned int configFiltering;
extern unsigned int configMasterVolume;
extern unsigned int configMusicVolume;
extern unsigned int configSfxVolume;
extern unsigned int configEnvVolume;
//extern bool         configVoicesEnabled;
extern unsigned int configKeyA[];
extern unsigned int configKeyB[];
extern unsigned int configKeyStart[];
extern unsigned int configKeyL[];
extern unsigned int configKeyR[];
extern unsigned int configKeyZ[];
extern unsigned int configKeyCUp[];
extern unsigned int configKeyCDown[];
extern unsigned int configKeyCLeft[];
extern unsigned int configKeyCRight[];
extern unsigned int configKeyStickUp[];
extern unsigned int configKeyStickDown[];
extern unsigned int configKeyStickLeft[];
extern unsigned int configKeyStickRight[];
extern unsigned int configStickDeadzone;
extern unsigned int configRumbleStrength;
// Saturn
extern unsigned int configKeyFreeze[];
extern unsigned int configKeyPlayAnim[];
extern unsigned int configKeyLoopAnim[];
extern unsigned int configKeyShowMenu[];
#ifdef EXTERNAL_DATA
extern bool         configPrecacheRes;
#endif
extern unsigned int configEditorTheme;
extern unsigned int configMCameraMode;
extern bool         configEditorFastApply;
extern bool         configEditorAutoModelCc;
extern bool         configEditorAutoSpark;
extern bool         configEditorNearClipping;
#ifdef BETTERCAMERA
extern unsigned int configCameraXSens;
extern unsigned int configCameraYSens;
extern unsigned int configCameraAggr;
extern unsigned int configCameraPan;
extern unsigned int configCameraDegrade;
extern bool         configCameraInvertX;
extern bool         configCameraInvertY;
extern bool         configEnableCamera;
extern bool         configCameraMouse;
extern bool         configCameraAnalog;
#endif
extern bool         configHUD;
extern bool         configSkipIntro;
#ifdef DISCORDRPC
extern bool         configDiscordRPC;
#endif

void configfile_load(const char *filename);
void configfile_save(const char *filename);
const char *configfile_name(void);

#ifdef __cplusplus
extern "C" {
#endif
    extern bool         configVoicesEnabled;
#ifdef __cplusplus
}
#endif

#endif
