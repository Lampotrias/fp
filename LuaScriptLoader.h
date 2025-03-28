#ifndef FP_LUASCRIPTLOADER_H
#define FP_LUASCRIPTLOADER_H

#include <lua.hpp>
#include "LuaScriptPathResolver.h"

class LuaScriptLoader {

public:
    LuaScriptLoader();

    static int custom_searcher(lua_State *L, LuaScriptPathResolver resolver);

private:
    static std::string_view get_current_lua_script_path(lua_State *L);
};


#endif //FP_LUASCRIPTLOADER_H
