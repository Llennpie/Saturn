#include "saturn_imgui_settings.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn_imgui.h"
#include <SDL2/SDL.h>

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "pc/cheats.h"
}

using namespace std;

// Variables

int windowWidth, windowHeight;

void ssettings_imgui_init() {
    windowWidth = configWindow.w;
    windowHeight = configWindow.h;
}

void ssettings_imgui_update() {
    ImGui::Checkbox("Limit FPS", &limit_fps);
    imgui_bundled_tooltip("(F4) Helpful for speeding up slow in-game events. Works like Project64.");

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader("Graphics")) {
        if (ImGui::Checkbox("Fullscreen", &configWindow.fullscreen))
            configWindow.settings_changed = true;
        if (!configWindow.fullscreen) {
            ImGui::Text("Resolution");
            static int screen_res[2] = { windowWidth, windowHeight };
            ImGui::InputInt2("###screen_resolution", screen_res); ImGui::SameLine();
            if (ImGui::Button("Set###screen_apply")) {
                windowWidth = screen_res[0];
                windowHeight = screen_res[1];
                if (windowWidth < 640 || windowHeight < 480) {
                    windowWidth = screen_res[0] = 640;
                    windowHeight = screen_res[1] = 480;
                }
                SDL_SetWindowSize(window, windowWidth, windowHeight);
                SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                configWindow.w = windowWidth;
                configWindow.h = windowHeight;
            }
        }

        ImGui::Dummy(ImVec2(0, 5));

        if (ImGui::Checkbox("VSync", &configWindow.vsync))
            configWindow.settings_changed = true;
        imgui_bundled_tooltip("Enable/disable this if your game speed is too fast or slow.");

        ImGui::Text("Texture Filtering");
        const char* texture_filters[] = { "Nearest", "Linear", "Three-point" };
        ImGui::Combo("###texture_filters", (int*)&configFiltering, texture_filters, IM_ARRAYSIZE(texture_filters));

        if (ImGui::Checkbox("Anti-aliasing", &configWindow.enable_antialias))
            configWindow.settings_changed = true;
        imgui_bundled_tooltip("EXPERIMENTAL. Enables/disables anti-aliasing with OpenGL.");
    }
    if (ImGui::CollapsingHeader("Audio")) {
        ImGui::Text("Volume");
        ImGui::SliderInt("Master", (int*)&configMasterVolume, 0, MAX_VOLUME);
        ImGui::SliderInt("SFX", (int*)&configSfxVolume, 0, MAX_VOLUME);
        ImGui::SliderInt("Music", (int*)&configMusicVolume, 0, MAX_VOLUME);
        ImGui::SliderInt("Fanfares", (int*)&configEnvVolume, 0, MAX_VOLUME);
        ImGui::Checkbox("Enable Voices", &configVoicesEnabled);
    }
    if (ImGui::CollapsingHeader("Gameplay")) {
#ifdef DISCORDRPC
        ImGui::Checkbox("Discord Activity Status", &configDiscordRPC);
        imgui_bundled_tooltip("Enables/disables Discord Rich Presence. Requires restart.");
        ImGui::Dummy(ImVec2(0, 5));
#endif
        ImGui::Text("Rumble Strength");
        ImGui::SliderInt("###rumble_strength", (int*)&configRumbleStrength, 0, 50);
#ifdef EXTERNAL_DATA
        ImGui::Checkbox("Precache Textures", &configPrecacheRes);
#endif
        ImGui::Checkbox("Skip Intro", &configSkipIntro);
        ImGui::Checkbox("Exit Anywhere", &Cheats.ExitAnywhere);
        imgui_bundled_tooltip("Allows the level to be exited from any state.");
    }
    if (ImGui::CollapsingHeader("Editor")) {
        const char* mThemeSettings[] = { "Legacy", "Moon Edition", "Half-Life" };
        ImGui::Text("Theme");
        ImGui::Combo("###theme", (int*)&configEditorTheme, mThemeSettings, IM_ARRAYSIZE(mThemeSettings));
        ImGui::SameLine(); imgui_bundled_help_marker("Changes the UI theme. Requires restart.");

        ImGui::Checkbox("Auto-apply CC color editor", &configEditorFastApply);
        imgui_bundled_tooltip("If enabled, color codes will automatically apply in the CC editor. May cause lag on low-end machines.");
        ImGui::Checkbox("Auto-enable CometSPARK support", &configEditorAutoSpark);
        imgui_bundled_tooltip("If enabled, CometSPARK support will automatically turn on when a SPARK GS code is loaded.");

        ImGui::Separator();

        const char* mCameraSettings[] = { "Mouse", "Keyboard/Gamepad" };
        ImGui::Text("Machinima Camera Mode");
        ImGui::Combo("###machinima_camera_mode", (int*)&configMCameraMode, mCameraSettings, IM_ARRAYSIZE(mCameraSettings));
        if (configMCameraMode == 0) {
            ImGui::SameLine(); imgui_bundled_help_marker("LShift + Mouse Buttons = Move Camera");
        } else if (configMCameraMode == 1) {
            ImGui::SameLine(); imgui_bundled_help_marker("R + C-Buttons = Pan Camera, L + C-Buttons = Raise/Lower Camera");
        }
    }
}