#include "saturn_discord.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
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

//bool has_init;

// Functions

static void on_current_user_update(UNUSED void* data) {
    struct DiscordUser user = { 0 };
    app.users->get_current_user(app.users, &user);
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
    strcpy(gCurActivity.state, "In-Game [Machinima]");
    strcpy(gCurActivity.details, saturn_get_stage_name(gCurrLevelNum));

    strcpy(gCurActivity.assets.large_image, "saturn-legacy-icon1");
    if (gCurrLevelNum == LEVEL_SA || configEditorAlwaysChroma) strcpy(gCurActivity.assets.small_image, "green");
    else strcpy(gCurActivity.assets.small_image, "circle-512");
#ifdef GIT_HASH
    strcpy(gCurActivity.assets.small_text, GIT_BRANCH " " GIT_HASH);
#else
    strcpy(gCurActivity.assets.small_text, "legacy");
#endif

    app.activities->update_activity(app.activities, &gCurActivity, NULL, on_activity_update_callback);
}