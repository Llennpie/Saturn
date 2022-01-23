#ifndef ModEngineTextureModule
#define ModEngineTextureModule

#include "moon/addons/interfaces/file-entry.h"
#include "moon/addons/interfaces/bit-module.h"
#include "libs/nlohmann/json.hpp"
#include <string>
#include <vector>
#include <map>

extern "C" {
#include "pc/gfx/gfx_pc.h"
}

extern std::map<std::string, TextureData*> textureMap;

namespace Moon {
    void saveAddonTexture(BitModule *addon, std::string texturePath, EntryFileData* data);
    void bindTextureModifier(std::string texture, std::string modName, nlohmann::json data);

    void precacheBaseTexture(char* data, long size, std::string texturePath);
    TextureData *getCachedTexture(std::string texturePath);
}

namespace MoonInternal {
    EntryFileData *getTextureData(const char *fullpath);
    void loadTexture(int tile, const char *fullpath, const char *rawpath, struct GfxRenderingAPI *gfx_rapi);
    void saveTexture(TextureData *data, std::string texturePath);
    void buildTextureCache(std::vector<int> order);
    void setupTextureEngine( std::string state );
}

#endif