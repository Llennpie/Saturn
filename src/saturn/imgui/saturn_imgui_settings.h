#ifndef SaturnImGuiSettings
#define SaturnImGuiSettings

#include "SDL2/SDL.h"

extern const char* translate_bind_to_name(int);

#ifdef __cplusplus
extern "C" {
#endif
    void ssettings_imgui_init(void);
    void ssettings_imgui_update(void);
#ifdef __cplusplus
}
#endif

#endif