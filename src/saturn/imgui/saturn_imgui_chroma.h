#ifndef SaturnImGuiChroma
#define SaturnImGuiChroma

#include "saturn/libs/imgui/imgui_internal.h"
#include "SDL2/SDL.h"

extern bool renderFloor;
extern int currentChromaArea;
extern ImVec4 uiChromaColor;

#ifdef __cplusplus
extern "C" {
#endif
    void schroma_imgui_init(void);
    void schroma_imgui_update(void);
#ifdef __cplusplus
}
#endif

#endif