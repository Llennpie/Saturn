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

bool showMenu = true;
bool showWindowStats = false;
bool showWindowMachinima = false;
bool showWindowDynOS = false;
bool showWindowSettings = false;
bool showWindowChromaKey = false;

// Bundled Components

void imgui_bundled_tooltip(const char* text) {
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(450.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void imgui_bundled_help_marker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
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

// Set up ImGui

void saturn_imgui_init(SDL_Window * sdl_window, SDL_GLContext ctx) {
    window = sdl_window;

    const char* glsl_version = "#version 120";
    ImGuiContext* imgui = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui);
    io = ImGui::GetIO(); (void)io;
    io.WantSetMousePos = false;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();

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
            if(event->key.keysym.sym == SDLK_F1)
                showMenu = !showMenu;

        case SDL_CONTROLLERBUTTONDOWN:
            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
                showMenu = !showMenu;
        
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
        if (ImGui::MenuItem("Stats")) {
            showWindowStats = !showWindowStats;
            showWindowMachinima = false;
            showWindowDynOS = false;
            showWindowSettings = false;
            showWindowChromaKey = false;
        }
        if (ImGui::MenuItem("Machinima")) {
            showWindowStats = false;
            showWindowMachinima = !showWindowMachinima;
            showWindowDynOS = false;
            showWindowSettings = false;
            showWindowChromaKey = false;
        }
        if (ImGui::MenuItem("Appearance")) {
            showWindowStats = false;
            showWindowMachinima = false;
            showWindowDynOS = !showWindowDynOS;
            showWindowSettings = false;
            showWindowChromaKey = false;
        }
        if (ImGui::MenuItem("Settings")) {
            showWindowStats = false;
            showWindowMachinima = false;
            showWindowDynOS = false;
            showWindowSettings = !showWindowSettings;
            showWindowChromaKey = false;
        }
        if (gCurrLevelNum == LEVEL_SA) {
            if (ImGui::MenuItem("CHROMA KEY")) {
                showWindowStats = false;
                showWindowMachinima = false;
                showWindowDynOS = false;
                showWindowSettings = false;
                showWindowChromaKey = !showWindowChromaKey;
            }
        }
        ImGui::EndMainMenuBar();

        // Stats
        if (showWindowStats) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Stats", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(300, 125));
            ImGui::Text("Platform: " PLATFORM " (" RAPI_NAME ")");
            ImGui::Text("Status: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
#ifdef GIT_BRANCH
#ifdef GIT_HASH
            ImGui::Text("Version: " GIT_BRANCH " " GIT_HASH);
#endif
#endif
            imgui_bundled_space(20);
            ImGui::Text("Press F1 to hide/show menu");

            ImGui::End();
            ImGui::PopStyleColor();
        }
        // Machinima
        if (showWindowMachinima) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Machinima", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(275, 425));
            
            smachinima_imgui_update();

            ImGui::End();
            ImGui::PopStyleColor();
        }
        // DynOS
        if (showWindowDynOS) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("DynOS", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(275, 475));

            sdynos_imgui_update();

            ImGui::End();
            ImGui::PopStyleColor();
        }
        // Settings
        if (showWindowSettings) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(275, 400));

            ssettings_imgui_update();

            ImGui::End();
            ImGui::PopStyleColor();
        }

        // Chroma Key
        if (showWindowChromaKey && gCurrLevelNum == LEVEL_SA && mario_exists) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::Begin("Chroma Key", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(10, 30));
            ImGui::SetWindowSize(ImVec2(300, 125));

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