#include "saturn_discord.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn/saturn_colors.h"
#include "saturn/saturn_models.h"
#include "pc/controller/controller_keyboard.h"
#include "data/dynos.cpp.h"
#include <SDL2/SDL.h>

extern "C" {
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/level_update.h"
#include "engine/level_script.h"
#include "game/memory.h"
#include "game/area.h"
}

using namespace std;

static int64_t applicationId = 862767511208460318;
struct DiscordApplication app = { 0 };

struct DiscordActivity gCurActivity;
struct DiscordUser gCurUser = { 0 };

std::string model_details;
std::string cc_details;

std::string discord_state;
std::string discord_details;
int model_count;
int cc_count;

bool has_set_time;

//bool has_init;

// Functions

static void on_current_user_update(UNUSED void* data) {
    app.users->get_current_user(app.users, &gCurUser);
}

// Callbacks

static void on_activity_update_callback(UNUSED void* data, enum EDiscordResult result) {
    //std::cout << "test" << std::endl;
}
struct IDiscordUserEvents* discord_user_initialize(void) {
    static struct IDiscordUserEvents events = { 0 };
    events.on_current_user_update = on_current_user_update;
    return &events;
}

// Discord

void sdiscord_init() {
    if (!configDiscordRPC) return;

    struct DiscordCreateParams params = { 0 };
    DiscordCreateParamsSetDefault(&params);
    params.client_id = applicationId;
    params.flags = DiscordCreateFlags_NoRequireDiscord;
    params.event_data = &app;
    params.user_events = discord_user_initialize();

    int rc = DiscordCreate(DISCORD_VERSION, &params, &app.core);
    if (rc == 0) {
        std::cout << "Connected to Discord" << std::endl;

        if (app.core != NULL) {
            app.activities = app.core->get_activity_manager(app.core);
            app.users = app.core->get_user_manager(app.core);
        }

        has_discord_init = true;
    } else {
        // Discord didn't initialize, disable feature
        has_discord_init = false;
        return;
    }
}

void sdiscord_update() {
    if (!configDiscordRPC || !has_discord_init) return;
    
    app.core->run_callbacks(app.core);

    gCurActivity.type = DiscordActivityType_Playing;

    if (model_list.size() > 0 && color_code_list.size() > 0) {
        strcpy(gCurActivity.details,    (std::to_string(model_list.size()) + " model pack(s), " +
                                        std::to_string(color_code_list.size()) + " color code(s)").c_str());
    }

    switch(gCurrLevelNum) {
        case LEVEL_SA:
            strcpy(gCurActivity.assets.large_image, "lvl-chroma");
            strcpy(gCurActivity.assets.small_image, "play-icon-bg");
            break;

        case LEVEL_CASTLE_GROUNDS:
            strcpy(gCurActivity.assets.large_image, "lvl-castle");
            if (autoChroma) strcpy(gCurActivity.assets.small_image, "green");
            else strcpy(gCurActivity.assets.small_image, "play-icon-bg");
            break;

        case LEVEL_CASTLE:
            strcpy(gCurActivity.assets.large_image, "lvl-castle-inside");
            if (autoChroma) strcpy(gCurActivity.assets.small_image, "green");
            else strcpy(gCurActivity.assets.small_image, "play-icon-bg");
            break;

        case LEVEL_CASTLE_COURTYARD:
            strcpy(gCurActivity.assets.large_image, "lvl-castle");
            if (autoChroma) strcpy(gCurActivity.assets.small_image, "green");
            else strcpy(gCurActivity.assets.small_image, "play-icon-bg");
            break;

        default:
            strcpy(gCurActivity.assets.large_image, "play-icon-bg");
            if (autoChroma) strcpy(gCurActivity.assets.small_image, "green");
            else strcpy(gCurActivity.assets.small_image, "circle-512");
            break;
    }
    strcpy(gCurActivity.assets.large_text, saturn_get_stage_name(gCurrLevelNum));

#ifdef GIT_HASH
    strcpy(gCurActivity.assets.small_text, GIT_BRANCH " " GIT_HASH);
#else
    strcpy(gCurActivity.assets.small_text, "legacy");
#endif

    if (!has_set_time) {
        gCurActivity.timestamps.start = (int64_t)time(0);
        has_set_time = true;
    }

    if (!k_popout_open && !is_cc_editing)
        discord_state = "In-Game // Animating";

    strcpy(gCurActivity.state, discord_state.c_str());

    app.activities->update_activity(app.activities, &gCurActivity, NULL, on_activity_update_callback);
}

void sdiscord_shutdown() {
    app.activities->clear_activity(app.activities, NULL, NULL);
}