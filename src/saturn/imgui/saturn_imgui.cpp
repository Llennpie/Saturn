#include "saturn_imgui.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <fstream>
#include <functional>

#include "saturn/imgui/saturn_imgui_dynos.h"
#include "saturn/imgui/saturn_imgui_cc_editor.h"
#include "saturn/imgui/saturn_imgui_machinima.h"
#include "saturn/imgui/saturn_imgui_settings.h"
#include "saturn/imgui/saturn_imgui_chroma.h"
#include "saturn/imgui/saturn_file_browser.h"
#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/libs/imgui/imgui_neo_sequencer.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/saturn_textures.h"
#include "saturn/discord/saturn_discord.h"
#include "pc/controller/controller_keyboard.h"
#include "data/dynos.cpp.h"
#include "icons/IconsForkAwesome.h"
#include "icons/IconsFontAwesome5.h"
#include "saturn/filesystem/saturn_projectfile.h"
#include "saturn/cmd/saturn_cmd.h"
#include "saturn/saturn_json.h"

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
#define PLATFORM_ICON ICON_FK_WINDOWS
#elif defined(OSX_BUILD)
#define PLATFORM "Mac OS"
#define PLATFORM_ICON ICON_FK_APPLE
#else
#define PLATFORM "Linux"
#define PLATFORM_ICON ICON_FK_LINUX
#endif

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/game_init.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "engine/level_script.h"
#include "game/object_list_processor.h"
}


#define CLI_MAX_INPUT_SIZE 4096

using namespace std;

SDL_Window* window = nullptr;
ImGuiIO io;

Array<PackData *> &iDynosPacks = DynOS_Gfx_GetPacks();

// Variables

int currentMenu = 0;
bool showMenu = true;
bool showStatusBars = true;

bool windowStats = true;
bool windowCcEditor;
bool windowAnimPlayer;
bool windowSettings;
bool windowChromaKey;

bool chromaRequireReload;

int windowStartHeight;

bool has_copy_camera;
bool copy_relative = true;

bool paste_forever;

int copiedKeyframeIndex = -1;

bool k_context_popout_open = false;
Keyframe k_context_popout_keyframe = Keyframe();
ImVec2 k_context_popout_pos = ImVec2(0, 0);

bool was_camera_frozen = false;

bool splash_finished = false;

std::string editor_theme = "moon";
std::vector<std::pair<std::string, std::string>> theme_list = {};

char cli_input[CLI_MAX_INPUT_SIZE];

#include "saturn/saturn_timelines.h"

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

    ImGui::TextDisabled(ICON_FA_QUESTION_CIRCLE);
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

void imgui_bundled_window_reset(const char* windowTitle, int width, int height, int x, int y) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Selectable(ICON_FK_UNDO " Reset Window Pos")) {
            ImGui::SetWindowSize(ImGui::FindWindowByName(windowTitle), ImVec2(width, height));
            ImGui::SetWindowPos(ImGui::FindWindowByName(windowTitle), ImVec2(x, y));
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

ImGuiWindowFlags imgui_bundled_window_corner(int corner, int width, int height, float alpha) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar;
    if (corner != -1) {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        if (width != 0) ImGui::SetNextWindowSize(ImVec2(width, height));
        window_flags |= ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowBgAlpha(alpha);
    }

    return window_flags;
}

#define JSON_VEC4(json, entry, dest) if (json.isMember(entry)) dest = ImVec4(json[entry][0].asFloat(), json[entry][1].asFloat(), json[entry][2].asFloat(), json[entry][3].asFloat())
#define JSON_BOOL(json, entry, dest) if (json.isMember(entry)) dest = json[entry].asBool()
#define JSON_FLOAT(json, entry, dest) if (json.isMember(entry)) dest = json[entry].asFloat()
#define JSON_VEC2(json, entry, dest) if (json.isMember(entry)) dest = ImVec2(json[entry][0].asFloat(), json[entry][1].asFloat())
#define JSON_DIR(json, entry, dest) if (json.isMember(entry)) dest = to_dir(json[entry].asString())

ImGuiDir to_dir(std::string dir) {
    if (dir == "up") return ImGuiDir_Up;
    if (dir == "left") return ImGuiDir_Left;
    if (dir == "down") return ImGuiDir_Down;
    if (dir == "right") return ImGuiDir_Right;
    return ImGuiDir_None;
}

void imgui_custom_theme(std::string theme_name) {
    std::filesystem::path path = std::filesystem::path("dynos/themes/" + theme_name + ".json");
    if (!std::filesystem::exists(path)) return;
    std::ifstream file = std::ifstream(path);
    Json::Value json;
    json << file;
    ImGuiStyle* style = &ImGui::GetStyle();
    style->ResetStyle();
    ImVec4* colors = style->Colors;
    if (json.isMember("colors")) {
        Json::Value colorsJson = json["colors"];
        JSON_VEC4(colorsJson, "text", colors[ImGuiCol_Text]);
        JSON_VEC4(colorsJson, "text_disabled", colors[ImGuiCol_TextDisabled]);
        JSON_VEC4(colorsJson, "window_bg", colors[ImGuiCol_WindowBg]);
        JSON_VEC4(colorsJson, "child_bg", colors[ImGuiCol_ChildBg]);
        JSON_VEC4(colorsJson, "popup_bg", colors[ImGuiCol_PopupBg]);
        JSON_VEC4(colorsJson, "border", colors[ImGuiCol_Border]);
        JSON_VEC4(colorsJson, "border_shadow", colors[ImGuiCol_BorderShadow]);
        JSON_VEC4(colorsJson, "frame_bg", colors[ImGuiCol_FrameBg]);
        JSON_VEC4(colorsJson, "frame_bg_hovered", colors[ImGuiCol_FrameBgHovered]);
        JSON_VEC4(colorsJson, "frame_bg_active", colors[ImGuiCol_FrameBgActive]);
        JSON_VEC4(colorsJson, "title_bg", colors[ImGuiCol_TitleBg]);
        JSON_VEC4(colorsJson, "title_bg_active", colors[ImGuiCol_TitleBgActive]);
        JSON_VEC4(colorsJson, "title_bg_collapsed", colors[ImGuiCol_TitleBgCollapsed]);
        JSON_VEC4(colorsJson, "menu_bar_bg", colors[ImGuiCol_MenuBarBg]);
        JSON_VEC4(colorsJson, "scrollbar_bg", colors[ImGuiCol_ScrollbarBg]);
        JSON_VEC4(colorsJson, "scrollbar_grab", colors[ImGuiCol_ScrollbarGrab]);
        JSON_VEC4(colorsJson, "scrollbar_grab_hovered", colors[ImGuiCol_ScrollbarGrabHovered]);
        JSON_VEC4(colorsJson, "scrollbar_grab_active", colors[ImGuiCol_ScrollbarGrabActive]);
        JSON_VEC4(colorsJson, "check_mark", colors[ImGuiCol_CheckMark]);
        JSON_VEC4(colorsJson, "slider_grab", colors[ImGuiCol_SliderGrab]);
        JSON_VEC4(colorsJson, "slider_grab_active", colors[ImGuiCol_SliderGrabActive]);
        JSON_VEC4(colorsJson, "button", colors[ImGuiCol_Button]);
        JSON_VEC4(colorsJson, "button_hovered", colors[ImGuiCol_ButtonHovered]);
        JSON_VEC4(colorsJson, "button_active", colors[ImGuiCol_ButtonActive]);
        JSON_VEC4(colorsJson, "header", colors[ImGuiCol_Header]);
        JSON_VEC4(colorsJson, "header_hovered", colors[ImGuiCol_HeaderHovered]);
        JSON_VEC4(colorsJson, "header_active", colors[ImGuiCol_HeaderActive]);
        JSON_VEC4(colorsJson, "separator", colors[ImGuiCol_Separator]);
        JSON_VEC4(colorsJson, "separator_hovered", colors[ImGuiCol_SeparatorHovered]);
        JSON_VEC4(colorsJson, "separator_active", colors[ImGuiCol_SeparatorActive]);
        JSON_VEC4(colorsJson, "resize_grip", colors[ImGuiCol_ResizeGrip]);
        JSON_VEC4(colorsJson, "resize_grip_hovered", colors[ImGuiCol_ResizeGripHovered]);
        JSON_VEC4(colorsJson, "resize_grip_active", colors[ImGuiCol_ResizeGripActive]);
        JSON_VEC4(colorsJson, "tab", colors[ImGuiCol_Tab]);
        JSON_VEC4(colorsJson, "tab_hovered", colors[ImGuiCol_TabHovered]);
        JSON_VEC4(colorsJson, "tab_active", colors[ImGuiCol_TabActive]);
        JSON_VEC4(colorsJson, "tab_unfocused", colors[ImGuiCol_TabUnfocused]);
        JSON_VEC4(colorsJson, "tab_unfocused_active", colors[ImGuiCol_TabUnfocusedActive]);
        JSON_VEC4(colorsJson, "plot_lines", colors[ImGuiCol_PlotLines]);
        JSON_VEC4(colorsJson, "plot_lines_hovered", colors[ImGuiCol_PlotLinesHovered]);
        JSON_VEC4(colorsJson, "plot_histogram", colors[ImGuiCol_PlotHistogram]);
        JSON_VEC4(colorsJson, "plot_histogram_hovered", colors[ImGuiCol_PlotHistogramHovered]);
        JSON_VEC4(colorsJson, "table_header_bg", colors[ImGuiCol_TableHeaderBg]);
        JSON_VEC4(colorsJson, "table_border_strong", colors[ImGuiCol_TableBorderStrong]);
        JSON_VEC4(colorsJson, "table_border_light", colors[ImGuiCol_TableBorderLight]);
        JSON_VEC4(colorsJson, "table_row_bg", colors[ImGuiCol_TableRowBg]);
        JSON_VEC4(colorsJson, "table_row_bg_alt", colors[ImGuiCol_TableRowBgAlt]);
        JSON_VEC4(colorsJson, "text_selected_bg", colors[ImGuiCol_TextSelectedBg]);
        JSON_VEC4(colorsJson, "drag_drop_target", colors[ImGuiCol_DragDropTarget]);
        JSON_VEC4(colorsJson, "nav_highlight", colors[ImGuiCol_NavHighlight]);
        JSON_VEC4(colorsJson, "nav_windowing_highlight", colors[ImGuiCol_NavWindowingHighlight]);
        JSON_VEC4(colorsJson, "nav_windowing_dim_bg", colors[ImGuiCol_NavWindowingDimBg]);
        JSON_VEC4(colorsJson, "modal_window_dim_bg", colors[ImGuiCol_ModalWindowDimBg]);
    }
    if (json.isMember("style")) {
        Json::Value styleJson = json["style"];
        JSON_BOOL(styleJson, "anti_aliased_lines", style->AntiAliasedLines);
        JSON_BOOL(styleJson, "anti_aliased_lines_use_tex", style->AntiAliasedLinesUseTex);
        JSON_BOOL(styleJson, "anti_aliased_fill", style->AntiAliasedFill);
        JSON_FLOAT(styleJson, "alpha", style->Alpha);
        JSON_FLOAT(styleJson, "disabled_alpha", style->DisabledAlpha);
        JSON_FLOAT(styleJson, "window_rounding", style->WindowRounding);
        JSON_FLOAT(styleJson, "window_border_size", style->WindowBorderSize);
        JSON_FLOAT(styleJson, "child_rounding", style->ChildRounding);
        JSON_FLOAT(styleJson, "child_border_size", style->ChildBorderSize);
        JSON_FLOAT(styleJson, "popup_rounding", style->PopupRounding);
        JSON_FLOAT(styleJson, "popup_border_size", style->PopupBorderSize);
        JSON_FLOAT(styleJson, "frame_rounding", style->FrameRounding);
        JSON_FLOAT(styleJson, "frame_border_size", style->FrameBorderSize);
        JSON_FLOAT(styleJson, "indent_spacing", style->IndentSpacing);
        JSON_FLOAT(styleJson, "columns_min_spacing", style->ColumnsMinSpacing);
        JSON_FLOAT(styleJson, "scrollbar_size", style->ScrollbarSize);
        JSON_FLOAT(styleJson, "scrollbar_rounding", style->ScrollbarRounding);
        JSON_FLOAT(styleJson, "grab_min_size", style->GrabMinSize);
        JSON_FLOAT(styleJson, "grab_rounding", style->GrabRounding);
        JSON_FLOAT(styleJson, "log_slider_deadzone", style->LogSliderDeadzone);
        JSON_FLOAT(styleJson, "tab_rounding", style->TabRounding);
        JSON_FLOAT(styleJson, "tab_border_size", style->TabBorderSize);
        JSON_FLOAT(styleJson, "tab_min_width_for_close_button", style->TabMinWidthForCloseButton);
        JSON_FLOAT(styleJson, "mouse_cursor_scale", style->MouseCursorScale);
        JSON_FLOAT(styleJson, "curve_tessellation_tol", style->CurveTessellationTol);
        JSON_FLOAT(styleJson, "circle_tessellation_max_error", style->CircleTessellationMaxError);
        JSON_VEC2(styleJson, "window_padding", style->WindowPadding);
        JSON_VEC2(styleJson, "window_min_size", style->WindowMinSize);
        JSON_VEC2(styleJson, "window_title_align", style->WindowTitleAlign);
        JSON_VEC2(styleJson, "frame_padding", style->FramePadding);
        JSON_VEC2(styleJson, "item_spacing", style->ItemSpacing);
        JSON_VEC2(styleJson, "item_inner_spacing", style->ItemInnerSpacing);
        JSON_VEC2(styleJson, "cell_padding", style->CellPadding);
        JSON_VEC2(styleJson, "touch_extra_padding", style->TouchExtraPadding);
        JSON_VEC2(styleJson, "button_text_align", style->ButtonTextAlign);
        JSON_VEC2(styleJson, "selectable_text_align", style->SelectableTextAlign);
        JSON_VEC2(styleJson, "display_window_padding", style->DisplayWindowPadding);
        JSON_VEC2(styleJson, "display_safe_area_padding", style->DisplaySafeAreaPadding);
        JSON_DIR(styleJson, "color_button_position", style->ColorButtonPosition);
        JSON_DIR(styleJson, "window_menu_button_position", style->WindowMenuButtonPosition);
    }
}

#undef JSON_VEC4
#undef JSON_BOOL
#undef JSON_FLOAT
#undef JSON_VEC2
#undef JSON_DIR

bool initFont = true;

void imgui_update_theme() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle* style = &ImGui::GetStyle();

    float SCALE = 1.f;

    if (initFont) {
        initFont = false;

        ImFontConfig defaultConfig;
        defaultConfig.SizePixels = 13.0f * SCALE;
        io.Fonts->AddFontDefault(&defaultConfig);

        ImFontConfig symbolConfig;
        symbolConfig.MergeMode = true;
        symbolConfig.SizePixels = 13.0f * SCALE;
        symbolConfig.GlyphMinAdvanceX = 13.0f * SCALE; // Use if you want to make the icon monospaced
        static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
        io.Fonts->AddFontFromFileTTF("fonts/forkawesome-webfont.ttf", symbolConfig.SizePixels, &symbolConfig, icon_ranges);
    }

    // backwards compatibility with older theme settings
    if (configEditorTheme == 0) editor_theme = "legacy";
    else if (configEditorTheme == 1) editor_theme = "moon";
    else if (configEditorTheme == 2) editor_theme = "halflife";
    else if (configEditorTheme == 3) editor_theme = "moviemaker";
    else if (configEditorTheme == 4) editor_theme = "dear";
    else {
        for (const auto& entry : std::filesystem::directory_iterator("dynos/themes")) {
            std::filesystem::path path = entry.path();
            if (path.extension().string() != ".json") continue;
            std::string name = path.filename().string();
            name = name.substr(0, name.length() - 5);
            if (string_hash(name.c_str(), 0, name.length()) == configEditorThemeJson) {
                editor_theme = name;
                break;
            }
        }
    }
    std::cout << "Using theme: " << editor_theme << std::endl;
    configEditorTheme = -1;
    imgui_custom_theme(editor_theme);

    style->ScaleAllSizes(SCALE);
}

// Set up ImGui

void saturn_imgui_init_backend(SDL_Window * sdl_window, SDL_GLContext ctx) {
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

    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, configWindowState?"1":"0");
}

void saturn_load_themes() {
    for (const auto& entry : std::filesystem::directory_iterator("dynos/themes")) {
        std::filesystem::path path = entry.path();
        if (path.extension().string() != ".json") continue;
        std::string id = path.filename().string();
        id = id.substr(0, id.length() - 5);
        std::ifstream stream = std::ifstream(path);
        Json::Value json;
        try {
            json << stream;
        }
        catch (std::runtime_error err) {
            std::cout << "Error parsing theme " << id << std::endl;
            continue;
        }
        if (!json.isMember("name")) continue;
        std::cout << "Found theme " << json["name"].asString() << " (" << id << ")" << std::endl;
        theme_list.push_back({ id, json["name"].asString() });
    }
}

void saturn_imgui_init() {
    saturn_load_themes();
    sdynos_imgui_init();
    smachinima_imgui_init();
    ssettings_imgui_init();
}

void saturn_imgui_handle_events(SDL_Event * event) {
    ImGui_ImplSDL2_ProcessEvent(event);
    switch (event->type){
        case SDL_KEYDOWN:
            if(event->key.keysym.sym == SDLK_F3) {
                saturn_play_keyframe();
            }
            if(event->key.keysym.sym == SDLK_F4) {
                limit_fps = !limit_fps;
                configWindow.fps_changed = true;
            }

            // this is for YOU, baihsense
            // here's your crash key
            //    - bup
            if(event->key.keysym.sym == SDLK_SCROLLLOCK) {
                imgui_update_theme();
            }
            if(event->key.keysym.sym == SDLK_F9) {
                DynOS_Gfx_GetPacks().Clear();
                DynOS_Opt_Init();
                //model_details = "" + std::to_string(DynOS_Gfx_GetPacks().Count()) + " model pack";
                //if (DynOS_Gfx_GetPacks().Count() != 1) model_details += "s";

                if (gCurrLevelNum > 3 || !mario_exists)
                    DynOS_ReturnToMainMenu();
            }

            if(event->key.keysym.sym == SDLK_F6) {
                k_popout_open = !k_popout_open;
            }
        
        break;
    }
    smachinima_imgui_controls(event);
}

void saturn_keyframe_sort(std::vector<Keyframe>* keyframes) {
    for (int i = 0; i < keyframes->size(); i++) {
        for (int j = i + 1; j < keyframes->size(); j++) {
            if ((*keyframes)[i].position < (*keyframes)[j].position) continue;
            Keyframe temp = (*keyframes)[i];
            (*keyframes)[i] = (*keyframes)[j];
            (*keyframes)[j] = temp;
        }
    }
}

uint32_t startFrame = 0;
uint32_t endFrame = 60;
int endFrameText = 60;

void saturn_keyframe_window() {
#ifdef DISCORDRPC
    discord_state = "In-Game // Keyframing";
#endif

    std::string windowLabel = "Timeline###kf_timeline";
    ImGuiWindowFlags timeline_flags = imgui_bundled_window_corner(1, 0, 0, 0.64f);
    ImGui::Begin(windowLabel.c_str(), &k_popout_open, timeline_flags);
    if (!keyframe_playing) {
        if (ImGui::Button(ICON_FK_PLAY " Play###k_t_play")) {
            startFrame = 0;
            saturn_play_keyframe();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Loop###k_t_loop", &k_loop);
    } else {
        if (ImGui::Button(ICON_FK_STOP " Stop###k_t_stop")) {
            keyframe_playing = false;
        }
        ImGui::SameLine();
        ImGui::Checkbox("Loop###k_t_loop", &k_loop);
    }

    ImGui::Separator();

    ImGui::PushItemWidth(35);
    if (ImGui::InputInt("Frames", &endFrameText, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (endFrameText >= 60) {
            endFrame = (uint32_t)endFrameText;
        } else {
            endFrame = 60;
            endFrameText = 60;
        }
    };
    ImGui::PopItemWidth();
            
    // Scrolling
    int scroll = keyframe_playing ? (k_current_frame - startFrame) == 30 ? 1 : 0 : (ImGui::IsWindowHovered() ? (int)(ImGui::GetMouseScrollY() * -2) : 0);
    if (scroll >= 60) scroll = 59;
    startFrame += scroll;
    if (startFrame + 60 > endFrame) startFrame = endFrame - 60;
    if (startFrame > endFrame) startFrame = 0;
    uint32_t end = min(60, endFrame - startFrame) + startFrame;

    // Sequencer
    if (ImGui::BeginNeoSequencer("Sequencer###k_sequencer", (uint32_t*)&k_current_frame, &startFrame, &end, ImVec2((end - startFrame) * 6, 0), ImGuiNeoSequencerFlags_HideZoom)) {
        for (auto& entry : k_frame_keys) {
            std::string name = entry.second.first.name;
            if (ImGui::BeginNeoTimeline(name.c_str(), entry.second.second)) {
                ImGui::EndNeoTimeLine();
            }
        }
        ImGui::EndNeoSequencer();
    }

    // Keyframes
    if (!keyframe_playing) {
        for (auto& entry : k_frame_keys) {
            KeyframeTimeline timeline = entry.second.first;
            std::vector<Keyframe>* keyframes = &entry.second.second;

            if (k_previous_frame == k_current_frame && !saturn_keyframe_matches(entry.first, k_current_frame)) {
                // We create a keyframe here
                int keyframeIndex = 0;
                for (int i = 0; i < keyframes->size(); i++) {
                    if (k_current_frame >= (*keyframes)[i].position) keyframeIndex = i;
                }
                bool create_new = keyframes->size() == 0;
                InterpolationCurve curve = InterpolationCurve::WAIT;
                if (!create_new) {
                    create_new = (*keyframes)[keyframeIndex].position != k_current_frame;
                    curve = (*keyframes)[keyframeIndex].curve;
                }
                if (create_new) saturn_create_keyframe(entry.first, curve);
                else {
                    Keyframe* keyframe = &(*keyframes)[keyframeIndex];
                    if (timeline.type == KFTYPE_BOOL) (*keyframe).value[0] = *(bool*)timeline.dest;
                    if (timeline.type == KFTYPE_FLOAT) (*keyframe).value[0] = *(float*)timeline.dest;
                    if (timeline.type == KFTYPE_ANIM) {
                        AnimationState* anim_state = (AnimationState*)timeline.dest;
                        (*keyframe).value[0] = anim_state->custom;
                        (*keyframe).value[1] = anim_state->loop;
                        (*keyframe).value[2] = anim_state->hang;
                        (*keyframe).value[3] = anim_state->speed;
                        (*keyframe).value[4] = anim_state->id;
                    }
                    if (timeline.type == KFTYPE_EXPRESSION) {
                        Model* model = (Model*)timeline.dest;
                        for (int i = 0; i < model->Expressions.size(); i++) {
                            (*keyframe).value[i] = model->Expressions[i].CurrentIndex;
                        }
                    }
                    if (timeline.type == KFTYPE_COLOR) {
                        float* color = (float*)timeline.dest;
                        (*keyframe).value[0] = color[0];
                        (*keyframe).value[1] = color[1];
                        (*keyframe).value[2] = color[2];
                    }
                    if (timeline.behavior != KFBEH_DEFAULT) (*keyframe).curve = InterpolationCurve::WAIT;
                }
            }
            else saturn_keyframe_apply(entry.first, k_current_frame);
        }
    }
    ImGui::End();

    // Auto focus (use controls without clicking window first)
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && saturn_disable_sm64_input()) {
        ImGui::SetWindowFocus(windowLabel.c_str());
    }

    k_popout_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_None);
    if (k_popout_focused) {
        for (auto& entry : k_frame_keys) {
            if (entry.second.first.name.find(", Main") != string::npos || entry.second.first.name.find(", Shade") != string::npos) {
                // Apply color keyframes
                //apply_cc_from_editor();
            }
        }
    }

    if (camera_frozen) {
        if (keyframe_playing || k_current_frame != k_previous_frame) {
            should_update_cam_from_keyframes = false;
            vec3f_copy(gCamera->pos, freezecamPos);
            vec3f_set_dist_and_angle(gCamera->pos, gCamera->focus, 100, freezecamPitch, freezecamYaw);
            gLakituState.roll = freezecamRoll;
        }
        else {
            float dist;
            s16 yaw;
            s16 pitch;
            vec3f_copy(freezecamPos, gCamera->pos);
            vec3f_get_dist_and_angle(gCamera->pos, gCamera->focus, &dist, &pitch, &yaw);
            freezecamYaw = (float)yaw;
            freezecamPitch = (float)pitch;
            freezecamRoll = (float)gLakituState.roll;
        }
        vec3f_copy(gLakituState.pos, gCamera->pos);
        vec3f_copy(gLakituState.focus, gCamera->focus);
        vec3f_copy(gLakituState.goalPos, gCamera->pos);
        vec3f_copy(gLakituState.goalFocus, gCamera->focus);
        gCamera->yaw = calculate_yaw(gCamera->focus, gCamera->pos);
        gLakituState.yaw = gCamera->yaw;
    }

    k_previous_frame = k_current_frame;
}

char saturnProjectFilename[257] = "Project";
int current_project_id;

std::string selected_macro = "";

void saturn_imgui_update() {
    if (!splash_finished) return;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
    
    windowStartHeight = (showStatusBars) ? 48 : 30;

    camera_savestate_mult = 1.f;

    if (showMenu) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                windowCcEditor = false;

                if (ImGui::MenuItem(ICON_FK_WINDOW_MAXIMIZE " Show UI",      translate_bind_to_name(configKeyShowMenu[0]), showMenu)) showMenu = !showMenu;
                if (ImGui::MenuItem(ICON_FK_WINDOW_MINIMIZE " Show Status Bars",  NULL, showStatusBars)) showStatusBars = !showStatusBars;
                ImGui::Separator();

                if (ImGui::BeginMenu("Open Project")) {
                    saturn_file_browser_filter_extension("spj");
                    saturn_file_browser_scan_directory("dynos/projects");
                    if (saturn_file_browser_show("project")) {
                        std::string str = saturn_file_browser_get_selected().string();
                        str = str.substr(0, str.length() - 4); // trim the extension
                        if (str.length() <= 256) memcpy(saturnProjectFilename, str.c_str(), str.length() + 1);
                        saturnProjectFilename[256] = 0;
                    }
                    ImGui::PushItemWidth(125);
                    ImGui::InputText(".spj###project_file_input", saturnProjectFilename, 256);
                    ImGui::PopItemWidth();
                    if (ImGui::Button(ICON_FA_FILE " Open###project_file_open")) {
                        saturn_load_project((char*)(std::string(saturnProjectFilename) + ".spj").c_str());
                    }
                    ImGui::SameLine(70);
                    bool in_custom_level = gCurrLevelNum == LEVEL_SA && gCurrAreaIndex == 3;
                    if (in_custom_level) ImGui::BeginDisabled();
                    if (ImGui::Button(ICON_FA_SAVE " Save###project_file_save")) {
                        saturn_save_project((char*)(std::string(saturnProjectFilename) + ".spj").c_str());
                    }
                    if (in_custom_level) ImGui::EndDisabled();
                    ImGui::SameLine();
                    imgui_bundled_help_marker("NOTE: Project files are currently EXPERIMENTAL and prone to crashing!");
                    if (in_custom_level) ImGui::Text("Saving in a custom\nlevel isn't supported");
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem(ICON_FA_UNDO " Load Autosaved")) {
                    saturn_load_project("autosave.spj");
                }

                ImGui::Separator();

                if (ImGui::BeginMenu(ICON_FK_CODE " Macros")) {
                    std::filesystem::path path;
                    saturn_file_browser_filter_extension("stm");
                    saturn_file_browser_scan_directory("dynos/macros");
                    if (saturn_file_browser_show("macros")) selected_macro = path.string();
                    if (ImGui::Button("Run")) {
                        saturn_cmd_clear_registers();
                        saturn_cmd_eval_file(selected_macro);
                    }
                    ImGui::SameLine();
                    imgui_bundled_help_marker("EXPERIMENTAL - Allows you to run commands and do stuff in bulk.\nCurrently has no debugging or documentation.");
                    ImGui::Checkbox("Command Line", &configEnableCli);
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Stats",        NULL, windowStats == true)) windowStats = !windowStats;
                if (ImGui::MenuItem(ICON_FK_LINE_CHART " Timeline Editor", "F6", k_popout_open == true)) {
                    k_popout_open = !k_popout_open;
                    windowSettings = false;
                }
                if (ImGui::MenuItem(ICON_FK_COG " Settings",     NULL, windowSettings == true)) {
                    windowSettings = !windowSettings;
                    k_popout_open = false;
                }
                //if (windowStats) imgui_bundled_window_reset("Stats", 250, 125, 10, windowStartHeight);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Camera")) {
                windowCcEditor = false;

                ImGui::Checkbox("Freeze", &camera_frozen);
                if (camera_frozen) {
                    saturn_keyframe_popout({ "k_c_camera_pos0", "k_c_camera_pos1", "k_c_camera_pos2", "k_c_camera_yaw", "k_c_camera_pitch", "k_c_camera_roll" });
                    ImGui::SameLine(200); ImGui::TextDisabled(translate_bind_to_name(configKeyFreeze[0]));

                    if (ImGui::BeginMenu("Options###camera_options")) {
                        camera_savestate_mult = 0.f;
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                        ImGui::BeginChild("###model_metadata", ImVec2(200, 90), true, ImGuiWindowFlags_NoScrollbar);
                        ImGui::TextDisabled("pos %.f, %.f, %.f", gCamera->pos[0], gCamera->pos[1], gCamera->pos[2]);
                        ImGui::TextDisabled("foc %.f, %.f, %.f", gCamera->focus[0], gCamera->focus[1], gCamera->focus[2]);
                        if (ImGui::Button(ICON_FK_FILES_O " Copy###copy_camera")) {
                            saturn_copy_camera(copy_relative);
                            if (copy_relative) saturn_paste_camera();
                            has_copy_camera = 1;
                        } ImGui::SameLine();
                        if (!has_copy_camera) ImGui::BeginDisabled();
                        if (ImGui::Button(ICON_FK_CLIPBOARD " Paste###paste_camera")) {
                            if (has_copy_camera) saturn_paste_camera();
                        }
                        /*ImGui::Checkbox("Loop###camera_paste_forever", &paste_forever);
                        if (paste_forever) {
                            saturn_paste_camera();
                        }*/
                        if (!has_copy_camera) ImGui::EndDisabled();
                        ImGui::Checkbox("Relative to Mario###camera_copy_relative", &copy_relative);

                        ImGui::EndChild();
                        ImGui::PopStyleVar();

                        ImGui::Text(ICON_FK_VIDEO_CAMERA " Speed");
                        ImGui::PushItemWidth(150);
                        ImGui::SliderFloat("Move", &camVelSpeed, 0.0f, 2.0f);
                        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) { camVelSpeed = 1.f; }
                        ImGui::SliderFloat("Rotate", &camVelRSpeed, 0.0f, 2.0f);
                        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) { camVelRSpeed = 1.f; }
                        ImGui::PopItemWidth();
                        ImGui::Text(ICON_FK_KEYBOARD_O " Control Mode");
                        const char* mCameraSettings[] = { "Keyboard", "Keyboard/Gamepad (Old)", "Mouse (Experimental)" };
                        ImGui::PushItemWidth(200);
                        ImGui::Combo("###camera_mode", (int*)&configMCameraMode, mCameraSettings, IM_ARRAYSIZE(mCameraSettings));
                        ImGui::PopItemWidth();
                        if (configMCameraMode == 2) {
                            imgui_bundled_tooltip("Move Camera -> LShift + Mouse Buttons");
                        } else if (configMCameraMode == 1) {
                            imgui_bundled_tooltip("Pan Camera -> R + C-Buttons\nRaise/Lower Camera -> L + C-Buttons\nRotate Camera -> L + Crouch + C-Buttons");
                        } else if (configMCameraMode == 0) {
                            imgui_bundled_tooltip("Move Camera -> Y/G/H/J\nRaise/Lower Camera -> T/U\nRotate Camera -> R + Y/G/H/J");
                        }
                        ImGui::EndMenu();
                    }
                }
                ImGui::Separator();
                ImGui::PushItemWidth(100);
                ImGui::SliderFloat("FOV", &camera_fov, 0.0f, 100.0f);
                if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) { camera_fov = 50.f; }
                imgui_bundled_tooltip("Controls the FOV of the in-game camera; Default is 50, or 40 in Project64.");
                saturn_keyframe_popout("k_fov");
                ImGui::SameLine(200); ImGui::TextDisabled("N/M");
                ImGui::PopItemWidth();
                ImGui::Checkbox("Smooth###fov_smooth", &camera_fov_smooth);

                ImGui::Separator();
                ImGui::PushItemWidth(100);
                ImGui::SliderFloat("Follow", &camera_focus, 0.0f, 1.0f);
                if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) { camera_focus = 1.f; }
                saturn_keyframe_popout("k_focus");
                ImGui::PopItemWidth();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Appearance")) {
                sdynos_imgui_menu();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Game")) {
                imgui_machinima_quick_options();
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem(ICON_FK_EYEDROPPER " CHROMA KEY", NULL, autoChroma)) {
                schroma_imgui_init();
                autoChroma = !autoChroma;
                windowCcEditor = false;
                windowAnimPlayer = false;

                for (int i = 0; i < 960; i++) {
                    gObjectPool[i].header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
                    if (autoChroma && !autoChromaObjects) gObjectPool[i].header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
                }
            }
            ImGui::EndMainMenuBar();
        }

        ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        float height = ImGui::GetFrameHeight();

        if (showStatusBars) {
            if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags)) {
                if (ImGui::BeginMenuBar()) {
                    if (configFps60) ImGui::TextDisabled("%.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
                    else ImGui::TextDisabled("%.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate / 2, 1000.0f / (ImGui::GetIO().Framerate / 2));
                    ImGui::EndMenuBar();
                }
                ImGui::End();
            }

            if (ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags)) {
                if (ImGui::BeginMenuBar()) {
                    ImGui::Text(PLATFORM_ICON " ");
#ifdef GIT_BRANCH
#ifdef GIT_HASH
                    ImGui::SameLine(20);
                    ImGui::TextDisabled(ICON_FK_GITHUB " " GIT_BRANCH " " GIT_HASH);
#endif
#endif
                    if (gCurrLevelNum == LEVEL_SA && gCurrAreaIndex == 3) {
                        ImGui::SameLine(ImGui::GetWindowWidth() - 280);
                        ImGui::Text("Saving in custom level isn't supported");
                    }
                    else {
                        ImGui::SameLine(ImGui::GetWindowWidth() - 140);
                        ImGui::Text("Autosaving in %ds", autosaveDelay / 30);
                    }
                    ImGui::EndMenuBar();
                }
                ImGui::End();
            }
        }

        if (configEnableCli)
        if (ImGui::BeginViewportSideBar("###CliMenuBar", viewport, ImGuiDir_Down, height, window_flags)) {
            if (ImGui::BeginMenuBar()) {
                ImGui::Dummy({ 0, 0 });
                ImGui::SameLine(-7);
                ImGui::PushItemWidth(ImGui::GetWindowWidth());
                if (ImGui::InputText("###cli_input", cli_input, CLI_MAX_INPUT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    is_cli = true;
                    saturn_cmd_eval(std::string(cli_input));
                    cli_input[0] = 0;
                    ImGui::SetKeyboardFocusHere(-1);
                }
                ImGui::PopItemWidth();
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }

        if (windowStats) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGuiWindowFlags stats_flags = imgui_bundled_window_corner(2, 0, 0, 0.64f);
            ImGui::Begin("Stats", &windowStats, stats_flags);

#ifdef DISCORDRPC
            if (has_discord_init && gCurUser.username != "") {
                if (gCurUser.username == NULL || gCurUser.username == "") {
                    ImGui::Text(ICON_FK_DISCORD " Loading...");
                } else {
                    std::string disc = gCurUser.discriminator;
                    if (disc == "0") ImGui::Text(ICON_FK_DISCORD " @%s", gCurUser.username);
                    else ImGui::Text(ICON_FK_DISCORD " %s#%s", gCurUser.username, gCurUser.discriminator);
                }
                ImGui::Separator();
            }
#endif
            ImGui::Text(ICON_FK_FOLDER_OPEN " %i model pack(s)", model_list.size());
            ImGui::Text(ICON_FK_FILE_TEXT " %i color code(s)", color_code_list.size());
            ImGui::TextDisabled(ICON_FK_PICTURE_O " %i textures loaded", preloaded_textures_count);

            ImGui::End();
            ImGui::PopStyleColor();
        }
        if (windowCcEditor && support_color_codes && current_model.ColorCodeSupport) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGuiWindowFlags cce_flags = imgui_bundled_window_corner(0, 0, 0, 1.f);
            ImGui::Begin("Color Code Editor", &windowCcEditor, cce_flags);
            OpenCCEditor();
            ImGui::End();
            ImGui::PopStyleColor();

#ifdef DISCORDRPC
            discord_state = "In-Game // Editing a CC";
#endif
        }
        if (windowAnimPlayer && mario_exists) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGuiWindowFlags anim_flags = imgui_bundled_window_corner(0, 0, 0, 1.f);
            ImGui::Begin("Animation Mixtape", &windowAnimPlayer, anim_flags);
            imgui_machinima_animation_player();
            ImGui::End();
            ImGui::PopStyleColor();
        }
        if (windowSettings) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGuiWindowFlags settings_flags = imgui_bundled_window_corner(1, 0, 0, 0.64f);
            ImGui::Begin("Settings", &windowSettings, settings_flags);
            ssettings_imgui_update();
            ImGui::End();
            ImGui::PopStyleColor();
        }
        if (autoChroma && mario_exists) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGuiWindowFlags chroma_flags = imgui_bundled_window_corner(0, 0, 0, 1.f);
            if (chromaRequireReload) chroma_flags |= ImGuiWindowFlags_UnsavedDocument;
            ImGui::Begin("Chroma Key Settings", &autoChroma, chroma_flags);
            schroma_imgui_update();
            ImGui::End();
            ImGui::PopStyleColor();
        }
        if (k_popout_open) {
            saturn_keyframe_window();
        }
        if (k_context_popout_open) {
            if (ImGui::Begin("###kf_menu", &k_context_popout_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove)) {
                vector<Keyframe>* keyframes = &k_frame_keys[k_context_popout_keyframe.timelineID].second;
                int curve = -1;
                int index = -1;
                for (int i = 0; i < keyframes->size(); i++) {
                    if ((*keyframes)[i].position == k_context_popout_keyframe.position) index = i;
                }
                bool isFirst = k_context_popout_keyframe.position == 0 && k_frame_keys[k_context_popout_keyframe.timelineID].first.behavior != KFBEH_EVENT;
                if (isFirst) ImGui::BeginDisabled();
                bool doDelete = false;
                bool doCopy = false;
                if (ImGui::MenuItem("Delete")) {
                    doDelete = true;
                }
                if (ImGui::MenuItem("Move to Pointer")) {
                    doDelete = true;
                    doCopy = true;
                }
                if (isFirst) ImGui::EndDisabled();
                if (ImGui::MenuItem("Copy to Pointer")) {
                    doCopy = true;
                }
                ImGui::Separator();
                bool forceWait = k_frame_keys[k_context_popout_keyframe.timelineID].first.behavior != KFBEH_DEFAULT;
                if (forceWait) ImGui::BeginDisabled();
                for (int i = 0; i < IM_ARRAYSIZE(curveNames); i++) {
                    if (ImGui::MenuItem(curveNames[i].c_str(), NULL, k_context_popout_keyframe.curve == InterpolationCurve(i))) {
                        curve = i;
                    }
                }
                if (forceWait) ImGui::EndDisabled();
                ImVec2 pos = ImGui::GetWindowPos();
                ImVec2 size = ImGui::GetWindowSize();
                ImVec2 mouse = ImGui::GetMousePos();
                if ((mouse.x < pos.x || mouse.y < pos.y || mouse.x >= pos.x + size.x || mouse.y >= pos.y + size.y) && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))) k_context_popout_open = false;
                ImVec2 window_size = ImGui::GetWindowSize();
                ImVec2 window_pos = k_context_popout_pos;
                window_pos.x -= window_size.x;
                ImGui::SetWindowPos(window_pos);
                ImGui::End();
                std::string timeline = k_context_popout_keyframe.timelineID;
                keyframes = &k_frame_keys[timeline].second;
                if (curve != -1) {
                    (*keyframes)[index].curve = InterpolationCurve(curve);
                    k_context_popout_open = false;
                    k_previous_frame = -1;
                }
                if (doCopy) {
                    int hoverKeyframeIndex = -1;
                    for (int i = 0; i < keyframes->size(); i++) {
                        if ((*keyframes)[i].position == k_current_frame) hoverKeyframeIndex = i;
                    }
                    Keyframe copy = Keyframe();
                    copy.position = k_current_frame;
                    copy.curve = (*keyframes)[index].curve;
                    copy.value = (*keyframes)[index].value;
                    copy.timelineID = (*keyframes)[index].timelineID;
                    if (hoverKeyframeIndex == -1) keyframes->push_back(copy);
                    else (*keyframes)[hoverKeyframeIndex] = copy;
                    k_context_popout_open = false;
                    k_previous_frame = -1;
                }
                if (doDelete) {
                    keyframes->erase(keyframes->begin() + index);
                    k_context_popout_open = false;
                    k_previous_frame = -1;
                }
                saturn_keyframe_sort(keyframes);
            }
        }

        //ImGui::ShowDemoWindow();
    }

    is_cc_editing = windowCcEditor & support_color_codes & current_model.ColorCodeSupport;

    ImGui::Render();
    GLint last_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    glUseProgram(0);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glUseProgram(last_program);

    if (was_camera_frozen && !camera_frozen && saturn_timeline_exists("k_c_camera_pos0")) {
        k_frame_keys.erase("k_c_camera_pos0");
        k_frame_keys.erase("k_c_camera_pos1");
        k_frame_keys.erase("k_c_camera_pos2");
        k_frame_keys.erase("k_c_camera_yaw");
        k_frame_keys.erase("k_c_camera_pitch");
        k_frame_keys.erase("k_c_camera_roll");
    }
    was_camera_frozen = camera_frozen;

    //if (!is_gameshark_open) apply_cc_from_editor();
}

/*
    ======== SATURN ========
    Advanced Keyframe Engine
    ========================
*/

void saturn_keyframe_context_popout(Keyframe keyframe) {
    if (keyframe_playing || k_context_popout_open) return;
    k_context_popout_open = true;
    k_context_popout_pos = ImGui::GetMousePos();
    k_context_popout_keyframe = keyframe;
}

void saturn_keyframe_show_kf_content(Keyframe keyframe) {
    ImGui::BeginTooltip();
    KeyframeTimeline timeline = k_frame_keys[keyframe.timelineID].first;
    if (timeline.type == KFTYPE_BOOL) {
        bool checked = keyframe.value[0] >= 1;
        ImGui::Checkbox("###kf_content_checkbox", &checked);
    }
    if (timeline.type == KFTYPE_FLOAT) {
        char buf[64];
        std::string fmt = timeline.precision >= 0 ? "%d" : ("%." + std::to_string(-timeline.precision) + "f");
        snprintf(buf, 64, fmt.c_str(), keyframe.value[0]);
        buf[63] = 0;
        ImGui::Text(buf);
    }
    if (timeline.type == KFTYPE_ANIM) {
        std::string anim_name;
        bool anim_custom = keyframe.value[0] >= 1;
        bool anim_loop = keyframe.value[1] >= 1;
        bool anim_hang = keyframe.value[2] >= 1;
        float anim_speed = keyframe.value[3];
        int anim_id = keyframe.value[4];
        if (anim_custom) anim_name = canim_array[anim_id];
        else anim_name = saturn_animations_list[anim_id];
        ImGui::Text(anim_name.c_str());
        ImGui::Checkbox("Looping###kf_content_looping", &anim_loop);
        ImGui::Checkbox("Hanging###kf_content_hanging", &anim_hang);
        char buf[64];
        snprintf(buf, 64, "%.2f", anim_speed);
        buf[63] = 0;
        ImGui::Text(("Speed: " + std::string(buf)).c_str());
    }
    if (timeline.type == KFTYPE_EXPRESSION) {
        for (int i = 0; i < keyframe.value.size(); i++) {
            if (current_model.Expressions[i].Textures.size() == 2 && (
                current_model.Expressions[i].Textures[0].FileName.find("default") != std::string::npos ||
                current_model.Expressions[i].Textures[1].FileName.find("default") != std::string::npos
            )) {
                int   select_index = (current_model.Expressions[i].Textures[0].FileName.find("default") != std::string::npos) ? 0 : 1;
                int deselect_index = (current_model.Expressions[i].Textures[0].FileName.find("default") != std::string::npos) ? 1 : 0;
                bool is_selected = (current_model.Expressions[i].CurrentIndex == select_index);
                ImGui::Checkbox((current_model.Expressions[i].Name + "###kf_content_expr").c_str(), &is_selected);
            }
            else {
                ImGui::Text((current_model.Expressions[i].Textures[current_model.Expressions[i].CurrentIndex].FileName + " - " + current_model.Expressions[i].Name).c_str());
            }
        }
    }
    if (timeline.type == KFTYPE_COLOR) {
        ImVec4 color;
        color.x = keyframe.value[0];
        color.y = keyframe.value[1];
        color.z = keyframe.value[2];
        color.w = 1;
        ImGui::ColorEdit4("Col###wlight_col", (float*)&color, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
    }
    ImVec2 window_pos = ImGui::GetMousePos();
    ImVec2 window_size = ImGui::GetWindowSize();
    window_pos.x -= window_size.x - 4;
    window_pos.y += 4;
    ImGui::SetWindowPos(window_pos);
    ImGui::EndTooltip();
}

void saturn_create_keyframe(std::string id, InterpolationCurve curve) {
    Keyframe keyframe = Keyframe();
    keyframe.position = k_current_frame;
    keyframe.curve = curve;
    keyframe.timelineID = id;
    KeyframeTimeline timeline = k_frame_keys[id].first;
    if (timeline.type == KFTYPE_BOOL) keyframe.value.push_back(*(bool*)timeline.dest);
    if (timeline.type == KFTYPE_FLOAT) keyframe.value.push_back(*(float*)timeline.dest);
    if (timeline.type == KFTYPE_ANIM) {
        AnimationState* anim_state = (AnimationState*)timeline.dest;
        keyframe.value.push_back(anim_state->custom);
        keyframe.value.push_back(anim_state->loop);
        keyframe.value.push_back(anim_state->hang);
        keyframe.value.push_back(anim_state->speed);
        keyframe.value.push_back(anim_state->id);
    }
    if (timeline.type == KFTYPE_EXPRESSION) {
        Model* model = (Model*)timeline.dest;
        for (int i = 0; i < model->Expressions.size(); i++) {
            keyframe.value.push_back(model->Expressions[i].CurrentIndex);
        }
    }
    if (timeline.type == KFTYPE_COLOR) {
        float* color = (float*)timeline.dest;
        keyframe.value.push_back(color[0]);
        keyframe.value.push_back(color[1]);
        keyframe.value.push_back(color[2]);
    }
    keyframe.curve = curve;
    k_frame_keys[id].second.push_back(keyframe);
    saturn_keyframe_sort(&k_frame_keys[id].second);
}

void saturn_keyframe_popout(std::string id) {
    ImGui::SameLine();
    saturn_keyframe_popout_next_line(id);
}

void saturn_keyframe_popout_next_line(std::string id) {
    saturn_keyframe_popout_next_line(std::vector<std::string>{ id });
}

void saturn_keyframe_popout(std::vector<std::string> ids) {
    ImGui::SameLine();
    saturn_keyframe_popout_next_line(ids);
}

void saturn_keyframe_popout_next_line(std::vector<std::string> ids) {
    if (ids.size() == 0) return;
    if (timelineDataTable.find(ids[0]) == timelineDataTable.end()) return;
    bool contains = saturn_timeline_exists(ids[0].c_str());
    std::string button_label = std::string(contains ? ICON_FK_TRASH : ICON_FK_LINK) + "###kb_" + ids[0];
    bool event_button = false;
    if (ImGui::Button(button_label.c_str())) {
        k_popout_open = true;
        for (std::string id : ids) {
            if (contains) k_frame_keys.erase(id);
            else {
                auto [ptr, type, behavior, name, precision, num_values] = timelineDataTable[id];
                KeyframeTimeline timeline = KeyframeTimeline();
                timeline.dest = ptr;
                timeline.type = type;
                timeline.behavior = behavior;
                timeline.name = name;
                timeline.precision = precision;
                timeline.numValues = num_values;
                k_current_frame = 0;
                startFrame = 0;
                k_frame_keys.insert({ id, { timeline, {} } });
                if (behavior != KFBEH_EVENT) saturn_create_keyframe(id, behavior == KFBEH_FORCE_WAIT ? InterpolationCurve::WAIT : InterpolationCurve::LINEAR);
            }
        }
    }
    for (std::string id : ids) {
        if (saturn_timeline_exists(id.c_str())) {
            if (k_frame_keys[id].first.behavior == KFBEH_EVENT) event_button = true;
        }
    }
    imgui_bundled_tooltip(contains ? "Remove" : "Animate");
    if (event_button && contains) {
        ImGui::SameLine();
        if (ImGui::Button(ICON_FK_PLUS "###kb_ev_place")) {
            for (std::string id : ids) {
                if (!saturn_timeline_exists(id.c_str())) continue;
                k_frame_keys[id].first.eventPlace = true;
            }
        }
        imgui_bundled_tooltip("Place Keyframe");
    }
}

bool saturn_disable_sm64_input() {
    return ImGui::GetIO().WantTextInput;
}

template <typename T>
void saturn_keyframe_popout(const T &edit_value, s32 data_type, string value_name, string id) {
    /*string buttonLabel = ICON_FK_LINK "###kb_" + id;
    string windowLabel = "Timeline###kw_" + id;

#ifdef DISCORDRPC
    discord_state = "In-Game // Keyframing";
#endif

    ImGui::SameLine();
    if (ImGui::Button(buttonLabel.c_str())) {
        if (active_key_float_value != edit_value) {
            keyframe_playing = false;
            k_last_placed_frame = 0;
            k_frame_keys = {0};
            k_v_float_keys = {0.f};

            k_c_pos1_keys = {0.f};
            k_c_pos2_keys = {0.f};
            k_c_foc0_keys = {0.f};
            k_c_foc1_keys = {0.f};
            k_c_foc2_keys = {0.f};
            k_c_rot0_keys = {0.f};
            k_c_rot1_keys = {0.f};
        }

        k_popout_open = true;
        active_key_float_value = edit_value;
        active_data_type = data_type;
    }

    if (k_popout_open && active_key_float_value == edit_value) {
        if (windowSettings) windowSettings = false;

        ImGuiWindowFlags timeline_flags = imgui_bundled_window_corner(1, 0, 0, 0.64f);
        ImGui::Begin(windowLabel.c_str(), &k_popout_open, timeline_flags);
        if (!keyframe_playing) {
            if (ImGui::Button(ICON_FK_PLAY " Play###k_t_play")) {
                saturn_play_keyframe();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Loop###k_t_loop", &k_loop);
            ImGui::SameLine(180);
            if (ImGui::Button(ICON_FK_TRASH_O " Clear All###k_t_clear")) {
                k_last_placed_frame = 0;
                k_frame_keys = {0};
                k_v_float_keys = {0.f};
                k_v_int_keys = {0};
                
                k_c_pos1_keys = {0.f};
                k_c_pos2_keys = {0.f};
                k_c_foc0_keys = {0.f};
                k_c_foc1_keys = {0.f};
                k_c_foc2_keys = {0.f};
                k_c_rot0_keys = {0.f};
                k_c_rot1_keys = {0.f};
            }
        } else {
            if (ImGui::Button(ICON_FK_STOP " Stop###k_t_stop")) {
                keyframe_playing = false;
            }
            ImGui::SameLine();
            ImGui::Checkbox("Loop###k_t_loop", &k_loop);
        }

        ImGui::Separator();

        ImGui::PushItemWidth(35);
        if (ImGui::InputInt("Frames", &endFrameText, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (endFrameText >= 60) {
                endFrame = (uint32_t)endFrameText;
            } else {
                endFrame = 60;
                endFrameText = 60;
            }
        }
        ImGui::PopItemWidth();
                
        // Popout
        if (ImGui::BeginNeoSequencer("Sequencer###k_sequencer", (uint32_t*)&k_current_frame, &startFrame, &endFrame, ImVec2(endFrame * 6, 0), ImGuiNeoSequencerFlags_HideZoom)) {
            if (ImGui::BeginNeoTimeline(value_name.c_str(), k_frame_keys)) { ImGui::EndNeoTimeLine(); }
            if (data_type == KEY_CAMERA) if(ImGui::BeginNeoTimeline("Rotation", k_frame_keys)) { ImGui::EndNeoTimeLine(); }
            ImGui::EndNeoSequencer();
        }

        // UI Controls
        if (!keyframe_playing) {
            if (k_current_frame != 0) {
                if (std::find(k_frame_keys.begin(), k_frame_keys.end(), k_current_frame) != k_frame_keys.end()) {
                    // We are hovering over a keyframe
                    auto it = std::find(k_frame_keys.begin(), k_frame_keys.end(), k_current_frame);
                    if (it != k_frame_keys.end()) {
                        int key_index = it - k_frame_keys.begin();
                        if (ImGui::Button(ICON_FK_MINUS_SQUARE " Delete Keyframe###k_d_frame")) {
                            // Delete Keyframe
                            k_frame_keys.erase(k_frame_keys.begin() + key_index);
                            if (data_type == KEY_FLOAT || data_type == KEY_CAMERA) k_v_float_keys.erase(k_v_float_keys.begin() + key_index);
                            if (data_type == KEY_INT) k_v_int_keys.erase(k_v_int_keys.begin() + key_index);

                            if (data_type == KEY_CAMERA) {
                                k_c_pos1_keys.erase(k_c_pos1_keys.begin() + key_index);
                                k_c_pos2_keys.erase(k_c_pos2_keys.begin() + key_index);
                                k_c_foc0_keys.erase(k_c_foc0_keys.begin() + key_index);
                                k_c_foc1_keys.erase(k_c_foc1_keys.begin() + key_index);
                                k_c_foc2_keys.erase(k_c_foc2_keys.begin() + key_index);
                                k_c_rot0_keys.erase(k_c_rot0_keys.begin() + key_index);
                                k_c_rot1_keys.erase(k_c_rot1_keys.begin() + key_index);
                            }

                            k_last_placed_frame = k_frame_keys[k_frame_keys.size() - 1];
                        } ImGui::SameLine(); ImGui::Text("at %i", (int)k_current_frame);
                    }
                } else {
                    // No keyframe here
                    if (ImGui::Button(ICON_FK_PLUS_SQUARE " Create Keyframe###k_c_frame")) {
                        if (k_last_placed_frame > k_current_frame) {
                            
                        } else {
                            k_frame_keys.push_back(k_current_frame);
                            if (data_type == KEY_FLOAT || data_type == KEY_CAMERA) k_v_float_keys.push_back(*edit_value);
                            if (data_type == KEY_INT) k_v_int_keys.push_back(*edit_value);

                            if (data_type == KEY_CAMERA) {
                                f32 dist;
                                s16 pitch, yaw;
                                vec3f_get_dist_and_angle(gCamera->pos, gCamera->focus, &dist, &pitch, &yaw);
                                k_c_pos1_keys.push_back(gCamera->pos[1]);
                                k_c_pos2_keys.push_back(gCamera->pos[2]);
                                k_c_foc0_keys.push_back(gCamera->focus[0]);
                                k_c_foc1_keys.push_back(gCamera->focus[1]);
                                k_c_foc2_keys.push_back(gCamera->focus[2]);
                                k_c_rot0_keys.push_back(yaw);
                                k_c_rot1_keys.push_back(pitch);
                                std::cout << pitch << std::endl;
                            }
                            k_last_placed_frame = k_current_frame;
                        }
                    } ImGui::SameLine(); ImGui::Text("at %i", (int)k_current_frame);
                }
            } else {
                // On frame 0
                if (data_type == KEY_CAMERA)
                    ImGui::Text("(Setting initial value)");
            }
        }
        ImGui::End();

        // Auto focus (use controls without clicking window first)
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && accept_text_input == false) {
            ImGui::SetWindowFocus(windowLabel.c_str());
        }
    }*/
}
