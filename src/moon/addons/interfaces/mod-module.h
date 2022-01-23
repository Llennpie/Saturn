#ifndef ModEngineModule
#define ModEngineModule

#include <string>
#include <vector>

extern "C" {
#include "libs/lua/lualib.h"
#include "libs/lua/lauxlib.h"
#include "libs/lua/lua.h"
}

class ModModule {
public:
    std::string moduleName;
    std::vector<luaL_Reg> functions;
    ModModule(std::string moduleName){
        this->moduleName = moduleName;
    }
};

#endif