//
// Created by mihail on 3/26/25.
//

#include <filesystem>
#include <fmt/color.h>
#include "LuaScriptExecutor.h"

namespace fs = std::filesystem;

int LuaScriptExecutor::execute_script(lua_State *L, std::string script_path) {
    if (!fs::exists(script_path)) {
        fmt::print(fg(fmt::color::red), "execute_script: ошибка загрузки файла: {}\n", script_path);
    } else {
        int res = luaL_dofile(L, script_path.c_str());
        if (res == LUA_OK) {
            fmt::print(fg(fmt::color::yellow), "Успешно прочитали файл: {} \n", script_path);

            return 1;
        } else {
            const char* error = lua_tostring(L, -1);
            lua_pop(L, 1);

            fmt::print(fg(fmt::color::red), "execute_script: ошибка загрузки файла: {}, error: {}\n", script_path, error);
        }
    }

    return 0;
}
