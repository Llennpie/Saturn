#include "saturn/filesystem/saturn_animfile.h"

#include "saturn/filesystem/saturn_format.h"
#include <vector>

const int curr_ver = 1;

std::vector<int> favorite_anims = {};

void saturn_favorite_anim_data_handler(SaturnFormatStream* stream, int version) {
    int count = saturn_format_read_int32(stream);
    for (int i = 0; i < count; i++) {
        favorite_anims.push_back(saturn_format_read_int32(stream));
    }
}

void saturn_load_favorite_anims() {
    favorite_anims.clear();
    saturn_format_input("dynos/anim_favorites.bin", "STFA", {
        { "DATA", saturn_favorite_anim_data_handler }
    });
}

void saturn_save_favorite_anims() {
    SaturnFormatStream stream = saturn_format_output("STFA", curr_ver);
    saturn_format_new_section(&stream, "DATA");
    saturn_format_write_int32(&stream, favorite_anims.size());
    for (int anim : favorite_anims) {
        saturn_format_write_int32(&stream, anim);
    }
    saturn_format_close_section(&stream);
    saturn_format_write("dynos/anim_favorites.bin", &stream);
}