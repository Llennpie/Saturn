#ifndef SM64Utils_H
#define SM64Utils_H

#include "utils.h"

namespace sm64 = MoonUtils;

void compile_sound(std::string dir){
    const char* cflags = "-DVERSION_US -D_LANGUAGE_C -DNON_MATCHING -DAVOID_UB";
    const char* basedir = dir.c_str();
    const char* cwd     = sm64::cwd;
    sm64::exec(sm64::format(
        "python3 %s/assemble_sound.py %s/sound/samples/ ./assets/sound/sound_banks %s/sound/sound_data.ctl %s/sound/sound_data.tbl %s --endian %s --bitwidth %s",
        cwd, basedir, basedir, basedir, cflags, sm64::endian, sm64::bitwidth
    ));

    sm64::exec(sm64::format(
        "as -I include -I./assets/sound/sequences/ --defsym AVOID_UB=1 -MD %s/sound/sequences/us/00_sound_player.d -o %s/sound/sequences/us/00_sound_player.o ./assets/sound/sequences/00_sound_player.s",
        basedir, basedir
    ));

    sm64::exec(sm64::format(
        "objcopy -j .rodata %s/sound/sequences/us/00_sound_player.o -O binary %s/sound/sequences/us/00_sound_player.m64",
        basedir, basedir
    ));

    std::vector<std::string> dirfiles;
    std::string m64files;
    sm64::dirscan(dir, dirfiles);
    for( auto &file : dirfiles )
        if( sm64::extname(file) == "m64" )  m64files += ( file + " " );

    sm64::exec(sm64::format(
        "python3 %s/assemble_sound.py --sequences %s/sound/sequences.bin %s/sound/bank_sets ./assets/sound/sound_banks/ ./assets/sound/sequences.json %s %s --endian %s --bitwidth %s",
        cwd, basedir, basedir, m64files.c_str(), cflags, sm64::endian, sm64::bitwidth
    ));

    sm64::rm(sm64::join(dir, "sound/samples"));
    sm64::rm(sm64::join(dir, "sound/sequences"));
}

#endif