#include "saturn_format.h"
#include "saturn/cmd/saturn_cmd.h"

const int curr_ver = 1;

int regcounts[] = {
    128
};

void saturn_cmd_registers_data_handler(SaturnFormatStream* stream, int version) {
    for (int i = 0; i < regcounts[version - 1]; i++) {
        registers[i] = saturn_format_read_int32(stream);
    }
}

void saturn_cmd_alias_data_handler(SaturnFormatStream* stream, int version) {
    int value = saturn_format_read_int32(stream);
    char name[256];
    saturn_format_read_string(stream, name, 256);
    name[255] = 0;
    aliases.insert({ name, value });
}

void saturn_cmd_registers_load() {
    aliases.clear();
    saturn_format_input("dynos/command_data.bin", "SCMD", {
        { "REGI", saturn_cmd_registers_data_handler },
        { "ALIS", saturn_cmd_alias_data_handler },
    });
}

void saturn_cmd_registers_save() {
    SaturnFormatStream stream = saturn_format_output("SCMD", curr_ver);
    saturn_format_new_section(&stream, "REGI");
    for (int i = 0; i < regcounts[curr_ver - 1]; i++) {
        saturn_format_write_int32(&stream, registers[i]);
    }
    saturn_format_close_section(&stream);
    for (auto& alias : aliases) {
        saturn_format_new_section(&stream, "ALIS");
        saturn_format_write_int32(&stream, alias.second);
        saturn_format_write_string(&stream, (char*)alias.first.c_str());
        saturn_format_close_section(&stream);
    }
    saturn_format_write("dynos/command_data.bin", &stream);
}