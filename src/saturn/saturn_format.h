#ifndef SaturnFormat
#define SaturnFormat

#include <string>
#include <map>
#include <functional>
extern "C" {
#include "include/types.h"
}

struct SaturnFormatStream {
    char* data;
    int pointer;
    int sectionBeginning;
};

typedef std::function<void(SaturnFormatStream*, int)> SaturnFormatSectionHandler;

void saturn_format_input(char* filename, char* identifier, std::map<std::string, SaturnFormatSectionHandler> handlers);
SaturnFormatStream saturn_format_output(char* identifier, int version);
u8 saturn_format_read_int8(SaturnFormatStream* stream);
u16 saturn_format_read_int16(SaturnFormatStream* stream);
u32 saturn_format_read_int32(SaturnFormatStream* stream);
float saturn_format_read_float(SaturnFormatStream* stream);
bool saturn_format_read_bool(SaturnFormatStream* stream);
void saturn_format_read_string(SaturnFormatStream* stream, char* dest);
void saturn_format_read_any(SaturnFormatStream* stream, void* dest, int length);
void saturn_format_new_section(SaturnFormatStream* stream, char* identifier);
void saturn_format_close_section(SaturnFormatStream* stream);
void saturn_format_write_int8(SaturnFormatStream* stream, u8 value);
void saturn_format_write_int16(SaturnFormatStream* stream, u16 value);
void saturn_format_write_int32(SaturnFormatStream* stream, u32 value);
void saturn_format_write_float(SaturnFormatStream* stream, float value);
void saturn_format_write_bool(SaturnFormatStream* stream, bool value);
void saturn_format_write_string(SaturnFormatStream* stream, char* value);
void saturn_format_write_any(SaturnFormatStream* stream, void* value, int length);
void saturn_format_write(char* filename, SaturnFormatStream* stream);

#endif