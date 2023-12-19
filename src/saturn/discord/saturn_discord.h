#ifndef SaturnDiscord
#define SaturnDiscord

#include "SDL2/SDL.h"

#ifdef __cplusplus
#include <string>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#pragma pack(push, 8)
#include "saturn/discord/discord_game_sdk.h"
#pragma pack(pop)

struct DiscordApplication {
    struct IDiscordCore* core;
    struct IDiscordUserManager* users;
    struct IDiscordAchievementManager* achievements;
    struct IDiscordActivityManager* activities;
    struct IDiscordRelationshipManager* relationships;
    struct IDiscordApplicationManager* application;
    struct IDiscordLobbyManager* lobbies;
    DiscordUserId userId;
};
extern struct DiscordApplication app;
extern struct DiscordActivity gCurActivity;
extern struct DiscordUser gCurUser;

extern std::string discord_state;

extern std::string model_details;
extern std::string cc_details;

extern "C" {
#endif
    void sdiscord_init(void);
    void sdiscord_update(void);
    void sdiscord_shutdown(void);
#ifdef __cplusplus
}
#endif

#endif