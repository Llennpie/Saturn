#include "saturn/saturn_format.h"

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <cstring>
extern "C" {
#include "include/types.h"
}

#define SATURN_FORMAT_FINISH_IDENTIFIER "DONE"
#define SATURN_FORMAT_BLOCK_SIZE        0x10
#define SATURN_FORMAT_IDENTIFIER_LENGTH 0x04
#define SATURN_FORMAT_MAX_CONTENT_SIZE  0x200000 // 2 MB, max 131072 blocks

void align(SaturnFormatStream* stream) {
    int alignment = SATURN_FORMAT_BLOCK_SIZE - (stream->pointer % SATURN_FORMAT_BLOCK_SIZE);
    if (alignment == SATURN_FORMAT_BLOCK_SIZE) return;
    stream->pointer += alignment;
}
void pad(SaturnFormatStream* stream) {
    int padding = SATURN_FORMAT_BLOCK_SIZE - (stream->pointer % SATURN_FORMAT_BLOCK_SIZE);
    if (padding == SATURN_FORMAT_BLOCK_SIZE) return;
    for (int i = 0; i < padding; i++) {
        saturn_format_write_int8(stream, rand() % 256);
    }
}
u32 hash(char* data, int length, int offset = 0) { // See Java's java.lang.String#hashCode() method
    u32 hash = 0;
    for (int i = 0; i < length; i++) {
        hash += 31 * hash + (int)(u8)(data[i + offset]);
    }
    return hash;
}
void saturn_format_input(char* filename, char* id, std::map<std::string, SaturnFormatSectionHandler> handlers) {
    std::ifstream file = std::ifstream(filename, std::ios::binary);
    if (!file.good()) return;
    char* header = (char*)malloc(SATURN_FORMAT_BLOCK_SIZE);
    file.read(header, SATURN_FORMAT_BLOCK_SIZE);
    SaturnFormatStream headerStream;
    headerStream.data = header;
    headerStream.pointer = 0;
    headerStream.sectionBeginning = -1;
    char identifier[SATURN_FORMAT_IDENTIFIER_LENGTH + 1];
    saturn_format_read_any(&headerStream, identifier, SATURN_FORMAT_IDENTIFIER_LENGTH);
    identifier[SATURN_FORMAT_IDENTIFIER_LENGTH] = 0;
    int blockSize = saturn_format_read_int32(&headerStream);
    int checksum = saturn_format_read_int32(&headerStream);
    int version = saturn_format_read_int32(&headerStream);
    char* content = (char*)malloc(SATURN_FORMAT_BLOCK_SIZE * blockSize);
    file.read(content, SATURN_FORMAT_BLOCK_SIZE * blockSize);
    SaturnFormatStream contentStream;
    contentStream.data = content;
    contentStream.pointer = 0;
    contentStream.sectionBeginning = -1;
    int contentHash = hash(content, SATURN_FORMAT_BLOCK_SIZE * blockSize);
    if (std::strcmp(identifier, id) != 0) std::cout << "Identifier " << identifier << " doesn't match " << id << std::endl;
    if (checksum != contentHash) std::cout << "Checksum failed! Corruptions or crash might happen." << std::endl;
    while (true) {
        int beginning = contentStream.pointer;
        char sectionIdentifier[SATURN_FORMAT_IDENTIFIER_LENGTH + 1];
        saturn_format_read_any(&contentStream, sectionIdentifier, SATURN_FORMAT_IDENTIFIER_LENGTH);
        sectionIdentifier[SATURN_FORMAT_IDENTIFIER_LENGTH] = 0;
        int sectionBlockSize = saturn_format_read_int32(&contentStream);
        align(&contentStream);
        if (std::strcmp(sectionIdentifier, SATURN_FORMAT_FINISH_IDENTIFIER) == 0) break;
        handlers[sectionIdentifier](&contentStream, version);
        contentStream.pointer = beginning + sectionBlockSize * SATURN_FORMAT_BLOCK_SIZE;
    }
    free(header);
    free(content);
    file.close();
}
SaturnFormatStream saturn_format_output(char* identifier, int version) {
    char* content = (char*)malloc(SATURN_FORMAT_MAX_CONTENT_SIZE);
    SaturnFormatStream stream;
    stream.data = content;
    stream.pointer = 0;
    stream.sectionBeginning = -1;
    saturn_format_write_any(&stream, identifier, SATURN_FORMAT_IDENTIFIER_LENGTH);
    stream.pointer += 8; // we write to this later
    saturn_format_write_int32(&stream, version);
    pad(&stream);
    return stream;
}
u8 saturn_format_read_int8(SaturnFormatStream* stream) {
    std::cout << (int)(u8)stream->data[stream->pointer] << " at " << stream->pointer << std::endl;
    return stream->data[stream->pointer++];
}
u16 saturn_format_read_int16(SaturnFormatStream* stream) {
    u8 high = saturn_format_read_int8(stream);
    u8 low = saturn_format_read_int8(stream);
    return (high << 8) | low;
}
u32 saturn_format_read_int32(SaturnFormatStream* stream) {
    u16 high = saturn_format_read_int16(stream);
    u16 low = saturn_format_read_int16(stream);
    return (high << 16) | low;
}
float saturn_format_read_float(SaturnFormatStream* stream) {
    union {
        u32 x;
        float y;
    } u;
    u.x = saturn_format_read_int32(stream);
    return u.y;
}
bool saturn_format_write_bool(SaturnFormatStream* stream) {
    saturn_format_read_int8(stream) != 0;
}
void saturn_format_read_string(SaturnFormatStream* stream, char* dest) {
    int i = -1;
    do {
        i++;
        dest[i] = saturn_format_read_int8(stream);
    }
    while (dest[i] != '\0');
}
void saturn_format_read_any(SaturnFormatStream* stream, void* dest, int length) {
    memcpy(dest, stream->data + stream->pointer, length);
    stream->pointer += length;
}
void saturn_format_new_section(SaturnFormatStream* stream, char* identifier) {
    if (std::strcmp(identifier, SATURN_FORMAT_FINISH_IDENTIFIER) == 0) {
        std::cout << "Trying to create section with reserved identifier: " << identifier << std::endl;
        return;
    }
    pad(stream);
    stream->sectionBeginning = stream->pointer;
    saturn_format_write_any(stream, identifier, SATURN_FORMAT_IDENTIFIER_LENGTH);
    pad(stream);
}
void saturn_format_close_section(SaturnFormatStream* stream) {
    pad(stream);
    int end = stream->pointer;
    int blocks = (stream->pointer - stream->sectionBeginning) / SATURN_FORMAT_BLOCK_SIZE;
    stream->pointer = stream->sectionBeginning + SATURN_FORMAT_IDENTIFIER_LENGTH;
    saturn_format_write_int32(stream, blocks);
    stream->pointer = end;
}
void saturn_format_write_int8(SaturnFormatStream* stream, u8 value) {
    stream->data[stream->pointer++] = value;
}
void saturn_format_write_int16(SaturnFormatStream* stream, u16 value) {
    saturn_format_write_int8(stream, (value >> 8) & 0xFF);
    saturn_format_write_int8(stream, value & 0xFF);
}
void saturn_format_write_int32(SaturnFormatStream* stream, u32 value) {
    saturn_format_write_int8(stream, (value >> 24) & 0xFF);
    saturn_format_write_int8(stream, (value >> 16) & 0xFF);
    saturn_format_write_int8(stream, (value >> 8) & 0xFF);
    saturn_format_write_int8(stream, value & 0xFF);
}
void saturn_format_write_float(SaturnFormatStream* stream, float value) {
    union {
        float x;
        u32 y;
    } u;
    u.x = value;
    saturn_format_write_int32(stream, u.y);
}
void saturn_format_write_bool(SaturnFormatStream* stream, bool value) {
    saturn_format_write_int8(stream, value ? 1 : 0);
}
void saturn_format_write_string(SaturnFormatStream* stream, char* value) {
    int i = -1;
    do {
        i++;
        saturn_format_write_int8(stream, value[i]);
    }
    while (value[i] != '\0');
}
void saturn_format_write_any(SaturnFormatStream* stream, void* value, int length) {
    memcpy(stream->data + stream->pointer, value, length);
    stream->pointer += length;
}
void saturn_format_write(char* filename, SaturnFormatStream* stream) {
    std::ofstream file = std::ofstream(filename, std::ios::binary);
    pad(stream);
    saturn_format_write_any(stream, (char*)SATURN_FORMAT_FINISH_IDENTIFIER, SATURN_FORMAT_IDENTIFIER_LENGTH);
    pad(stream);
    int size = stream->pointer;
    stream->pointer = SATURN_FORMAT_IDENTIFIER_LENGTH;
    saturn_format_write_int32(stream, size / SATURN_FORMAT_BLOCK_SIZE - 1);
    saturn_format_write_int32(stream, hash(stream->data, size - SATURN_FORMAT_BLOCK_SIZE, SATURN_FORMAT_BLOCK_SIZE));
    file.write(stream->data, size);
    free(stream->data);
    file.close();
}