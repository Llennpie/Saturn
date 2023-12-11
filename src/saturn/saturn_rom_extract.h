#ifndef SaturnRomExtract
#define SaturnRomExtract

#define EXTRACT_TYPE_SOUND      (1 << 0)
#define EXTRACT_TYPE_SATURN     (1 << 1)
#define EXTRACT_TYPE_TEXTURES   (1 << 2)
#define EXTRACT_TYPE_FONT       (1 << 3)
#define EXTRACT_TYPE_TRANSITION (1 << 4)
#define EXTRACT_TYPE_ALL      0xFFFFFFFF

#ifdef __cplusplus

#include <string>

extern std::string currently_extracting;

extern "C" {
#endif
    bool saturn_extract_rom(int type);
#ifdef __cplusplus
}
#endif

#endif