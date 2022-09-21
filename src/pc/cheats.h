#ifndef _CHEATS_H
#define _CHEATS_H

#include <stdbool.h>
typedef unsigned int uint;

struct CheatList {
    bool         EnableCheats;
    bool         MoonJump;
    bool         GodMode;
    bool         InfiniteLives;
    bool         SuperSpeed;
    bool         Responsive;
    bool         ExitAnywhere;
    bool         HugeMario;
    bool         TinyMario;
    bool         CustomMarioScale;
    int          PlayAs;
};

extern struct CheatList Cheats;

extern float marioScaleSizeX;
extern float marioScaleSizeY;
extern float marioScaleSizeZ;

#endif // _CHEATS_H
