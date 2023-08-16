#include "saturn_imgui.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <map>

#include "saturn/imgui/saturn_imgui_dynos.h"
#include "saturn/imgui/saturn_imgui_machinima.h"
#include "saturn/imgui/saturn_imgui_settings.h"
#include "saturn/imgui/saturn_imgui_chroma.h"
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
Keyframe k_context_popout_keyframe = Keyframe(0, InterpolationCurve::LINEAR);
ImVec2 k_context_popout_pos = ImVec2(0, 0);

bool was_camera_frozen = false;

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

void imgui_update_theme() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle* style = &ImGui::GetStyle();

    float SCALE = 1.f;

    ImFontConfig defaultConfig;
    defaultConfig.SizePixels = 13.0f * SCALE;
    io.Fonts->AddFontDefault(&defaultConfig);

    ImFontConfig symbolConfig;
    symbolConfig.MergeMode = true;
    symbolConfig.SizePixels = 13.0f * SCALE;
    symbolConfig.GlyphMinAdvanceX = 13.0f * SCALE; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
    io.Fonts->AddFontFromFileTTF("fonts/forkawesome-webfont.ttf", symbolConfig.SizePixels, &symbolConfig, icon_ranges);

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

    style->ScaleAllSizes(SCALE);
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

    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, configWindowState?"1":"0");
}

void saturn_imgui_handle_events(SDL_Event * event) {
    ImGui_ImplSDL2_ProcessEvent(event);
    switch (event->type){
        case SDL_KEYDOWN:
            if(event->key.keysym.sym == SDLK_F4) {
                limit_fps = !limit_fps;
                configWindow.fps_changed = true;
            }

            if(event->key.keysym.sym == SDLK_F5) {
                imgui_update_theme();
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
    ImGuiWindowFlags timeline_flags = imgui_bundled_window_corner(3, 0, 0, 0.64f);
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
    int scroll = keyframe_playing ? (k_current_frame - startFrame) == 30 ? 1 : 0 : (int)(ImGui::GetMouseScrollY() * -2);
    if (scroll >= 60) scroll = 59;
    startFrame += scroll;
    if (startFrame + 60 > endFrame) startFrame = endFrame - 60;
    if (startFrame > endFrame) startFrame = 0;
    uint32_t end = min(60, endFrame - startFrame) + startFrame;

    // Sequencer
    if (ImGui::BeginNeoSequencer("Sequencer###k_sequencer", (uint32_t*)&k_current_frame, &startFrame, &end, ImVec2((end - startFrame) * 6, 0), ImGuiNeoSequencerFlags_HideZoom)) {
        for (auto& entry : k_frame_keys) {
            if (ImGui::BeginNeoTimeline(entry.second.first.name.c_str(), entry.second.second)) {
                entry.second.first.disabled = ImGui::IsNeoTimelineSelected();
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
                if ((*keyframes)[keyframeIndex].position == k_current_frame) {
                    Keyframe* keyframe = &(*keyframes)[keyframeIndex];
                    if (timeline.bdest != nullptr) (*keyframe).value = *timeline.bdest ? 1 : 0;
                    if (timeline.fdest != nullptr) (*keyframe).value = *timeline.fdest;
                }
                else {
                    Keyframe keyframe = Keyframe(k_current_frame, (*keyframes)[keyframeIndex].curve);
                    keyframe.timelineID = entry.first;
                    if (timeline.bdest != nullptr) keyframe.value = *timeline.bdest ? 1 : 0;
                    if (timeline.fdest != nullptr) keyframe.value = *timeline.fdest;
                    keyframes->push_back(keyframe);
                }
                
                saturn_keyframe_sort(keyframes);
            }
            else saturn_keyframe_apply(entry.first, k_current_frame);
        }
    }
    ImGui::End();

    // Auto focus (use controls without clicking window first)
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && accept_text_input == false) {
        ImGui::SetWindowFocus(windowLabel.c_str());
    }

    if (camera_frozen) {
        if (keyframe_playing || k_current_frame != k_previous_frame) {
            should_update_cam_from_keyframes = false;
            vec3f_copy(gCamera->pos, freezecamPos);
            vec3f_set_dist_and_angle(gCamera->pos, gCamera->focus, 100, freezecamPitch, freezecamYaw);
        }
        else {
            float dist;
            s16 yaw;
            s16 pitch;
            vec3f_copy(freezecamPos, gCamera->pos);
            vec3f_get_dist_and_angle(gCamera->pos, gCamera->focus, &dist, &pitch, &yaw);
            freezecamYaw = (float)yaw;
            freezecamPitch = (float)pitch;
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

void saturn_imgui_update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
    
    windowStartHeight = (showStatusBars) ? 48 : 30;

    camera_savestate_mult = 1.f;

    if (showMenu) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                windowCcEditor = false;

                if (ImGui::MenuItem(ICON_FK_WINDOW_MAXIMIZE " Show UI",      translate_bind_to_name(configKeyShowMenu[0]), showMenu)) {
                    showMenu = !showMenu;
                    if (!showMenu) accept_text_input = true;
                }
                if (ImGui::MenuItem(ICON_FK_WINDOW_MINIMIZE " Show Status Bars",  NULL, showStatusBars)) showStatusBars = !showStatusBars;
                ImGui::Separator();
                if (ImGui::MenuItem("Stats",        NULL, windowStats == true)) windowStats = !windowStats;
                if (ImGui::MenuItem(ICON_FK_LINE_CHART " Timeline Editor", "F6", k_popout_open == true)) {
                    k_popout_open = !k_popout_open;
                }
                if (ImGui::MenuItem(ICON_FK_COG " Settings",     NULL, windowSettings == true)) {
                    windowSettings = !windowSettings;
                }
                //if (windowStats) imgui_bundled_window_reset("Stats", 250, 125, 10, windowStartHeight);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Camera")) {
                windowCcEditor = false;

                ImGui::Checkbox("Freeze", &camera_frozen);
                if (camera_frozen) {
                    saturn_keyframe_camera_popout("Camera", "k_c_camera");
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
                saturn_keyframe_float_popout(&camera_fov, "FOV", "k_fov");
                ImGui::SameLine(200); ImGui::TextDisabled("N/M");
                ImGui::PopItemWidth();
                ImGui::Checkbox("Smooth###fov_smooth", &camera_fov_smooth);

                ImGui::Separator();
                ImGui::PushItemWidth(100);
                ImGui::SliderFloat("Follow", &camera_focus, 0.0f, 1.0f);
                if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) { camera_focus = 1.f; }
                saturn_keyframe_float_popout(&camera_focus, "Follow", "k_focus");
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

                // Auto-chroma
                // Allows any level to become a customizable chroma key stage
                for (int i = 0; i < 960; i++) {
                    if (!autoChroma) gObjectPool[i].header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
                    else gObjectPool[i].header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
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
                    ImGui::EndMenuBar();
                }
                ImGui::End();
            }
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
            ImGui::Text(ICON_FK_FOLDER_OPEN " %s", model_details.c_str());
            ImGui::Text(ICON_FK_FILE_TEXT " %s", cc_details.c_str());
            ImGui::TextDisabled(ICON_FK_PICTURE_O " %i textures loaded", preloaded_textures_count);

            ImGui::End();
            ImGui::PopStyleColor();
        }
        if (windowCcEditor) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGuiWindowFlags cce_flags = imgui_bundled_window_corner(0, 0, 0, 1.f);
            ImGui::Begin("Color Code Editor", &windowCcEditor, cce_flags);
            imgui_dynos_cc_editor();
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
            ImGui::SetNextWindowBgAlpha(0.64f);
            ImGui::SetNextWindowPos(k_context_popout_pos);
            if (ImGui::Begin("###kf_menu", &k_context_popout_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove)) {
                vector<Keyframe>* keyframes = &k_frame_keys[k_context_popout_keyframe.timelineID].second;
                int index = -1;
                for (int i = 0; i < keyframes->size(); i++) {
                    if ((*keyframes)[i].position == k_context_popout_keyframe.position) index = i;
                }
                if (k_context_popout_keyframe.position == 0) ImGui::BeginDisabled();
                bool doDelete = false;
                bool doCopy = false;
                if (ImGui::MenuItem("Delete")) {
                    doDelete = true;
                }
                if (ImGui::MenuItem("Move to Pointer")) {
                    doDelete = true;
                    doCopy = true;
                }
                if (k_context_popout_keyframe.position == 0) ImGui::EndDisabled();
                if (ImGui::MenuItem("Copy to Pointer")) {
                    doCopy = true;
                }
                ImGui::Separator();
                for (int i = 0; i < IM_ARRAYSIZE(curveNames); i++) {
                    if (ImGui::MenuItem(curveNames[i].c_str(), NULL, k_context_popout_keyframe.curve == InterpolationCurve(i))) {
                        (*keyframes)[index].curve = InterpolationCurve(i);
                        k_previous_frame = -1;
                        k_context_popout_open = false;
                    }
                }
                ImVec2 pos = ImGui::GetWindowPos();
                ImVec2 size = ImGui::GetWindowSize();
                ImVec2 mouse = ImGui::GetMousePos();
                if ((mouse.x < pos.x || mouse.y < pos.y || mouse.x >= pos.x + size.x || mouse.y >= pos.y + size.y) && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))) k_context_popout_open = false;
                ImGui::End();
                if (doCopy) {
                    int hoverKeyframeIndex = -1;
                    for (int i = 0; i < keyframes->size(); i++) {
                        if ((*keyframes)[i].position == k_current_frame) hoverKeyframeIndex = i;
                    }
                    Keyframe copy = Keyframe(k_current_frame, (*keyframes)[index].curve);
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

    is_cc_editing = windowCcEditor;

    ImGui::Render();
    GLint last_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    glUseProgram(0);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glUseProgram(last_program);

    if (was_camera_frozen && !camera_frozen && k_frame_keys.find("k_c_camera_pos0") != k_frame_keys.end()) {
        k_frame_keys.erase("k_c_camera_pos0");
        k_frame_keys.erase("k_c_camera_pos1");
        k_frame_keys.erase("k_c_camera_pos2");
        k_frame_keys.erase("k_c_camera_yaw");
        k_frame_keys.erase("k_c_camera_pitch");
    }
    was_camera_frozen = camera_frozen;
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

void saturn_keyframe_float_popout(float* edit_value, string value_name, string id) {
    bool contains = k_frame_keys.find(id) != k_frame_keys.end();

    string buttonLabel = ICON_FK_LINK "###kb_" + id;

    ImGui::SameLine();
    if (ImGui::Button(buttonLabel.c_str())) {
        k_popout_open = true;
        if (contains) k_frame_keys.erase(id);
        else { // Add the timeline
            KeyframeTimeline timeline = KeyframeTimeline();
            timeline.fdest = edit_value;
            timeline.name = value_name;
            timeline.precision = -2; // .01
            Keyframe keyframe = Keyframe(0, InterpolationCurve::LINEAR);
            keyframe.value = *edit_value;
            keyframe.timelineID = id;
            k_frame_keys.insert({ id, std::make_pair(timeline, std::vector<Keyframe>{ keyframe }) });
            k_current_frame = 0;
            startFrame = 0;
        }
    }
    imgui_bundled_tooltip(contains ? "Remove" : "Animate");
}
void saturn_keyframe_bool_popout(bool* edit_value, string value_name, string id) {
    bool contains = k_frame_keys.find(id) != k_frame_keys.end();

    string buttonLabel = ICON_FK_LINK "###kb_" + id;

    ImGui::SameLine();
    if (ImGui::Button(buttonLabel.c_str())) {
        k_popout_open = true;
        if (contains) k_frame_keys.erase(id);
        else { // Add the timeline
            KeyframeTimeline timeline = KeyframeTimeline();
            timeline.bdest = edit_value;
            timeline.name = value_name;
            timeline.precision = -2; // .01
            Keyframe keyframe = Keyframe(0, InterpolationCurve::LINEAR);
            keyframe.value = *edit_value ? 1 : 0;
            keyframe.timelineID = id;
            k_frame_keys.insert({ id, std::make_pair(timeline, std::vector<Keyframe>{ keyframe }) });
            k_current_frame = 0;
            startFrame = 0;
        }
    }
    imgui_bundled_tooltip(contains ? "Remove" : "Animate");
}
void saturn_keyframe_camera_popout(string value_name, string id) {
    bool contains = k_frame_keys.find(id + "_cam_pos0") != k_frame_keys.end();

    string buttonLabel = ICON_FK_LINK "###kb_" + id;

    ImGui::SameLine();
    if (ImGui::Button(buttonLabel.c_str())) {
        float dist;
        s16 pitch, yaw;
        vec3f_copy(freezecamPos, gCamera->pos);
        vec3f_get_dist_and_angle(gCamera->pos, gCamera->focus, &dist, &pitch, &yaw);
        freezecamYaw = (float)yaw;
        freezecamPitch = (float)pitch;
        // ((id, name), (precision, value_ptr))
        std::pair<std::pair<std::string, std::string>, std::pair<int, float*>> values[] = {
            std::make_pair(std::make_pair("pos0", "Pos X"), std::make_pair(0, &freezecamPos[0])),
            std::make_pair(std::make_pair("pos1", "Pos Y"), std::make_pair(0, &freezecamPos[1])),
            std::make_pair(std::make_pair("pos2", "Pos Z"), std::make_pair(0, &freezecamPos[2])),
            std::make_pair(std::make_pair("yaw", "Yaw"), std::make_pair(2, &freezecamYaw)),
            std::make_pair(std::make_pair("pitch", "Pitch"), std::make_pair(2, &freezecamPitch))
        };
        k_popout_open = true;
        if (contains) {
            for (int i = 0; i < IM_ARRAYSIZE(values); i++) {
                k_frame_keys.erase(id + "_" + values[i].first.first);
            }
        }
        else { // Add the timeline
            for (int i = 0; i < IM_ARRAYSIZE(values); i++) {
                KeyframeTimeline timeline = KeyframeTimeline();
                timeline.fdest = values[i].second.second;
                timeline.name = value_name + " " + values[i].first.second;
                timeline.precision = values[i].second.first;
                Keyframe keyframe = Keyframe(0, InterpolationCurve::LINEAR);
                keyframe.value = *values[i].second.second;
                keyframe.timelineID = id + "_" + values[i].first.first;
                k_frame_keys.insert({ id + "_" + values[i].first.first, std::make_pair(timeline, std::vector<Keyframe>{ keyframe }) });
                k_current_frame = 0;
                startFrame = 0;
            }
        }
    }
    imgui_bundled_tooltip(contains ? "Remove" : "Animate");
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
