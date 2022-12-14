#include "saturn_imgui.h"

#include <string>
#include <iostream>

#include "saturn/imgui/saturn_imgui_dynos.h"
#include "saturn/imgui/saturn_imgui_machinima.h"
#include "saturn/imgui/saturn_imgui_settings.h"
#include "saturn/imgui/saturn_imgui_chroma.h"
#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "pc/controller/controller_keyboard.h"
#include <SDL2/SDL.h>

#ifdef __MINGW32__
# define FOR_WINDOWS 1
#else
# define FOR_WINDOWS 0
#endif

#if FOR_WINDOWS || defined(OSX_BUILD)
# define GLEW_STATIC
# include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#ifdef USE_GLES
# include <SDL2/SDL_opengles2.h>
# define RAPI_NAME "OpenGL ES"
#else
# include <SDL2/SDL_opengl.h>
# define RAPI_NAME "OpenGL"
#endif

#if FOR_WINDOWS
#define PLATFORM "Windows"
#elif defined(OSX_BUILD)
#define PLATFORM "MacOS"
#else
#define PLATFORM "Linux"
#endif

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/game_init.h"
#include "game/camera.h"
#include "engine/level_script.h"
}

using namespace std;

SDL_Window* window = nullptr;
ImGuiIO io;

// Variables

int currentMenu = 0;
bool showMenu = true;

// Bundled Components

void imgui_bundled_tooltip(const char* text) {
    if (!configEditorShowTips) return;

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(450.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void imgui_bundled_help_marker(const char* desc) {
    if (!configEditorShowTips) {
        ImGui::TextDisabled("");
        return;
    }

    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void imgui_bundled_space(float size, const char* title = NULL, const char* help_marker = NULL) {
    ImGui::Dummy(ImVec2(0, size/2));
    ImGui::Separator();
    if (title == NULL) {
        ImGui::Dummy(ImVec2(0, size/2));
    } else {
        ImGui::Dummy(ImVec2(0, size/4));
        ImGui::Text(title);
        if (help_marker != NULL) {
            ImGui::SameLine(); imgui_bundled_help_marker(help_marker);
        }
        ImGui::Dummy(ImVec2(0, size/4));
    }
}

void imgui_update_theme() {
    if (configEditorTheme == 0) {
        ImGui::StyleColorsDark();
    } else if (configEditorTheme == 1) {
        ImGui::StyleColorsMoon();
    } else if (configEditorTheme == 2) {
        ImGui::StyleColorsHalfLife();
    } else if (configEditorTheme == 3) {
        ImGui::StyleColorsM64MM();
    } else if (configEditorTheme == 4) {
        ImGui::StyleColorsClassic();
    }
}

// Set up ImGui

void saturn_imgui_init(SDL_Window * sdl_window, SDL_GLContext ctx) {
    window = sdl_window;

    const char* glsl_version = "#version 120";
    ImGuiContext* imgui = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui);
    io = ImGui::GetIO(); (void)io;
    io.WantSetMousePos = false;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    imgui_update_theme();

    ImGui_ImplSDL2_InitForOpenGL(window, ctx);
    ImGui_ImplOpenGL3_Init(glsl_version);

    sdynos_imgui_init();
    smachinima_imgui_init();
    ssettings_imgui_init();
}

void saturn_imgui_handle_events(SDL_Event * event) {
    ImGui_ImplSDL2_ProcessEvent(event);
    switch (event->type){
        case SDL_KEYDOWN:
            if(event->key.keysym.sym == SDLK_F4) {
                limit_fps = !limit_fps;
                configWindow.settings_changed = true;
            }
        
        break;
    }
    smachinima_imgui_controls(event);
}

void saturn_imgui_update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    if(showMenu){
        ImGui::BeginMainMenuBar();
        if (ImGui::MenuItem("Stats"))
            if (currentMenu != 0) { currentMenu = 0; }
            else {currentMenu = -1; }
        if (ImGui::MenuItem("Machinima"))
            if (currentMenu != 1) { currentMenu = 1; }
            else {currentMenu = -1; }
        if (ImGui::BeginMenu("Appearance")) {
            currentMenu = -1;
            sdynos_imgui_menu();
        } else {
            if (currentMenu == -1 && !accept_text_input)
                accept_text_input = true;
        }
        if (ImGui::MenuItem("Settings"))
            if (currentMenu != 4) { currentMenu = 4; }
            else {currentMenu = -1; }
        if (gCurrLevelNum == LEVEL_SA || configEditorAlwaysChroma) {
            if (ImGui::MenuItem("CHROMA KEY"))
                if (currentMenu != 5) { currentMenu = 5; }
                else {currentMenu = -1; }
        }
        ImGui::EndMainMenuBar();

        // Stats
        if (currentMenu == 0) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Stats", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(300, 125));
            ImGui::Text("Platform: " PLATFORM " (" RAPI_NAME ")");
            ImGui::Text("Status: %.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
#ifdef GIT_BRANCH
#ifdef GIT_HASH
            ImGui::Text("Version: " GIT_BRANCH " " GIT_HASH);
#endif
#endif
            imgui_bundled_space(20);
            ImGui::Text("Press %s to hide/show menu", translate_bind_to_name(configKeyShowMenu[0]));

            ImGui::End();
            ImGui::PopStyleColor();
        }

        // Machinima
        if (currentMenu == 1) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Machinima", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(275, 435));
            
            smachinima_imgui_update();

            ImGui::End();
            ImGui::PopStyleColor();
        }

        // DynOS
        sdynos_imgui_update();
    
        // Settings
        if (currentMenu == 4) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(325, 435));

            ssettings_imgui_update();

            ImGui::End();
            ImGui::PopStyleColor();
        }

        // Chroma Key
        if (currentMenu == 5 && (gCurrLevelNum == LEVEL_SA || configEditorAlwaysChroma) && mario_exists) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Chroma Key", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(300, 200));

            schroma_imgui_update();

            ImGui::End();
            ImGui::PopStyleColor();
        }

        //ImGui::ShowDemoWindow();
    }

    ImGui::Render();
    GLint last_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    glUseProgram(0);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glUseProgram(last_program);
}