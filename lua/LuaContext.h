//
// Created by mihail on 3/31/25.
//

#ifndef FP_LUACONTEXT_H
#define FP_LUACONTEXT_H

#include <lua.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include "models/RequireModeFile.h"
#include "../LuaScriptPathResolver.h"

class LuaContext {
private:
    lua_State *L;

    std::unordered_map<RequireModeFile, int> required;

    std::string data_folder_path;
    std::string mods_folder_path;

    LuaScriptPathResolver resolver;

    void setup_lua();

    int Require(lua_State *L);

    static int RequireWrapper(lua_State *L);

    void print_table(lua_State *L, int index, int depth = 0);

    void dump_mods(std::vector<std::string> names);

public:
    LuaContext(std::string data_folder_path, std::string mods_folder_path);

    void parse();

    ~LuaContext();

    std::string get_current_lua_script_path();
};


#endif //FP_LUACONTEXT_H
