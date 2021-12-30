#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <iostream>

#include "macros.h"
#include "PR/ultratypes.h"
#include "game/memory.h"
#include "game/save_file.h"
#include "pc/configfile.h"
#include "discordrpc.h"
#include "moon/texts/moon-loader.h"
#include "moon/utils/moon-env.h"

#include "moon/achievements/achievements.h"

extern "C" {
#include "pc/platform.h"
#include "game/level_update.h"
#include "game/area.h"
}

#define DISCORDLIBFILE "libdiscord-rpc"

// Thanks Microsoft for being non posix compliant
#if defined(_WIN32)
# include <windows.h>
# define DISCORDLIBEXT ".dll"
# define dlopen(lib, flag) LoadLibrary(TEXT(lib))
# define dlerror() ""
# define dlsym(handle, func) (void *)GetProcAddress(reinterpret_cast<HMODULE>(handle), func);
# define dlclose(handle) FreeLibrary(reinterpret_cast<HMODULE>(handle))
#elif defined(__APPLE__)
# include <dlfcn.h>
# define DISCORDLIBEXT ".dylib"
#elif defined(__linux__) || defined(__FreeBSD__) // lets make the bold assumption for FreeBSD
# include <dlfcn.h>
# define DISCORDLIBEXT ".so"
#else
# error Unknown System
#endif

#define DISCORDLIB DISCORDLIBFILE DISCORDLIBEXT
#define DISCORD_APP_ID  "895788060518793226"
#define DISCORD_UPDATE_RATE 3

using namespace std;

extern s16 gCurrCourseNum;
extern s16 gCurrActNum;

static time_t lastUpdatedTime;

static DiscordRichPresence discordRichPresence;
static bool initd = false;

static void* handle;

typedef void (*Discord_Initialize)(const char *, DiscordEventHandlers *, int, const char *);
typedef void (*Discord_Shutdown)(void);
typedef void (*Discord_ClearPresence)(void);
typedef void (*Discord_UpdatePresence)(DiscordRichPresence *);

Discord_Initialize discordInit;
Discord_Shutdown discordShutdown;
Discord_ClearPresence discordClearPresence;
Discord_UpdatePresence discordUpdatePresence;

static s16 lastHealth = -1;

static s16 lastStarAmount = 0;
static s16 lastAchievements = 0;


static s16 lastCourseNum = -1;
static s16 lastActNum = -1;
bool reloadRPC = false;
static char stage[188];
static char act[188];

static char smallImageKey[5];
static char largeImageKey[5];

static const char charset[0xFF+1] = {
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 7
    ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',  // 15
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',  // 23
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',  // 31
    'w', 'x', 'y', 'z', ' ', ' ', ' ', ' ',  // 39
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 49
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 55
    ' ', ' ', ' ', ' ', ' ', ' ', '\'', ' ', // 63
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 71
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 79
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 87
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 95
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 103
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ',',  // 111
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 119
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 127
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 135
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 143
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 151
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', '-',  // 159
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 167
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 175
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 183
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 192
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 199
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 207
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 215
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 223
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 231
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 239
    ' ', ' ', '!', ' ', ' ', ' ', ' ', ' ',  // 247
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '   // 255
};

static void convertstring(const u8 *str, char* output) {
    s32 strPos = 0;
    bool capitalizeChar = true;

    while (str[strPos] != 0xFF)  {
        if (str[strPos] < 0xFF) {
            output[strPos] = charset[str[strPos]];

            // if the char is a letter we can capatalize it
            if (capitalizeChar && 0x0A <= str[strPos] && str[strPos] <= 0x23) {
                output[strPos] -= ('a' - 'A');
                capitalizeChar = false;
            }

        } else {
            output[strPos] = ' ';
        }

        // decide if the next character should be capitalized
        switch (output[strPos]) {
            case ' ':
                if (str[strPos] != 158)
                    fprintf(stdout, "Unknown Character (%i)\n", str[strPos]); // inform that an unknown char was found
            case '-':
                capitalizeChar = true;
                break;
            default:
                capitalizeChar = false;
                break;
        }

        strPos++;
    }

    output[strPos] = '\0';
}

static void on_ready(UNUSED const DiscordUser* user) {
    discord_reset();
}

static void init_discord(void) {
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = on_ready;

    discordInit(DISCORD_APP_ID, &handlers, false, "");

    initd = true;
}

static void set_time(void){
    if (lastCourseNum != gCurrCourseNum) {
        discordRichPresence.startTimestamp = time(0);
    }
}

static void set_details(void) {
    if (gCurrLevelNum == LEVEL_SA) {
        lastCourseNum = gCurrCourseNum;
        strcpy(stage, "Chroma Keying");
        return;
    }
    if (lastCourseNum != gCurrCourseNum || reloadRPC) {
        // If we are in in Course 0 we are in the castle which doesn't have a string
        if (gCurrCourseNum) {
            u8 *courseName = Moon::current->courses[gCurrCourseNum - 1];

            convertstring(&courseName[3], stage);
        } else {
            strcpy(stage, "Peach's Castle");
        }

        lastCourseNum = gCurrCourseNum;
    }
}

static void set_state(void) {
    if (lastActNum != gCurrActNum || lastCourseNum != gCurrCourseNum || reloadRPC) {
        // when exiting a stage the act doesn't get reset
        if (gCurrActNum && gCurrCourseNum) {
            // any stage over 19 is a special stage without acts
            if (gCurrCourseNum < COURSE_STAGES_MAX) {
                u8 *actName = actName = Moon::current->acts[(gCurrCourseNum - 1) * 6 + gCurrActNum - 1];

                convertstring(actName, act);
            } else {
                act[0] = '\0';
                gCurrActNum = 0;
            }
        } else {
            act[0] = '\0';
        }

        lastActNum = gCurrActNum;
    }
}

string getLevelLogo(){
    switch(lastCourseNum){
        case 0:
            return "level-peach";
        case 6:
            return "moon64-logo";
        case 9:
            return "level-3";
        case 16:
        case 17:
        case 18:
            return "level-bowser";
        default:
            return "level-"+to_string(lastCourseNum);
    }
}

void set_logo(void) {
    discordRichPresence.largeImageKey = "saturn-moon-full";
    discordRichPresence.largeImageText = "https://github.com/Llennpie/Saturn";
}

void DiscordReloadPresence() {
    reloadRPC = true;
}

void set_platform(){
#ifdef __MINGW32__
    discordRichPresence.smallImageKey = "windows";
    discordRichPresence.smallImageText = "Windows";
#else
    discordRichPresence.smallImageKey = "linux";
    discordRichPresence.smallImageText = "Linux";
#endif
}

void DiscordUpdatePresence(){
    if (!configDiscordRPC || !initd) return;
    if (time(NULL) < lastUpdatedTime + DISCORD_UPDATE_RATE) return;

    lastUpdatedTime = time(NULL);
    set_time();
    set_state();
    set_details();
    set_logo();
    set_platform();
    discordUpdatePresence(&discordRichPresence);
    reloadRPC = false;
}

extern "C" {
void discord_update_rich_presence() {
    DiscordUpdatePresence();
}

void discord_shutdown(void) {
    if (handle) {
        discordClearPresence();
        discordShutdown();
        dlclose(handle);
    }
}

void discord_init(void) {
    if (configDiscordRPC) {
        string cwd = MoonInternal::getEnvironmentVar("MOON_CWD");
        string libPath = cwd.substr(0, cwd.find_last_of("/\\")) + "/lib/discord/" + DISCORDLIB;

        cout << "Loading discord lib from " << libPath << endl;

        handle = dlopen(libPath.c_str(), RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "Unable to load Discord\n%s\n", dlerror());
            return;
        }

        discordInit =           (Discord_Initialize)     dlsym(handle, "Discord_Initialize");
        discordShutdown =       (Discord_Shutdown)       dlsym(handle, "Discord_Shutdown");
        discordClearPresence =  (Discord_ClearPresence)  dlsym(handle, "Discord_ClearPresence");
        discordUpdatePresence = (Discord_UpdatePresence) dlsym(handle, "Discord_UpdatePresence");
        init_discord();

        discordRichPresence.details = "Making Machinima";
        discordRichPresence.state = stage;

        lastUpdatedTime = 0;
    }
}
}

void discord_reset(void) {
    memset( &discordRichPresence, 0, sizeof( discordRichPresence ) );

    set_state();
    set_details();
    set_logo();
    discordUpdatePresence(&discordRichPresence);
}