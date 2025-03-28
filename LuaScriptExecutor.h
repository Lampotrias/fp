//
// Created by mihail on 3/26/25.
//

#ifndef FP_LUASCRIPTEXECUTOR_H
#define FP_LUASCRIPTEXECUTOR_H

#include <lua.hpp>
#include <string>

class LuaScriptExecutor {
public:
    int execute_script(lua_State *L, std::string script_path);
};


#endif //FP_LUASCRIPTEXECUTOR_H
