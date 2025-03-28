//
// Created by mihail on 3/24/25.
//

#include "LuaScriptLoader.h"
#include <filesystem>
#include <fmt/color.h>
#include <iostream>

namespace fs = std::filesystem;


LuaScriptLoader::LuaScriptLoader() {

}

int LuaScriptLoader::custom_searcher(lua_State *L, LuaScriptPathResolver resolver) {
    std::string module_name = luaL_checkstring(L, 1);
    fmt::print(fg(fmt::color::yellow_green), "Требуется модуль: {}\n", module_name);

    auto current_lua_script_path = get_current_lua_script_path(L);
    if (!current_lua_script_path.empty()) {
        auto current_lua_script_folder = fs::path(current_lua_script_path).parent_path();

        auto result = resolver.resolve_require(module_name, current_lua_script_folder);
        if (result) {
            if (luaL_loadfile(L, result.value().c_str()) == LUA_OK) {
                fmt::print(fg(fmt::color::green), "Lua подключил модуль: {}.\n", module_name);
                return 1;
            } else {
                fmt::print(fg(fmt::color::red), "Lua не подключила модуль: {}.\n", module_name);
                return 0;
            }
        } else {
            fmt::print(fg(fmt::color::red), "Не удалось подключить найти скрипт модуля: {}. Ошибка: {}\n", module_name,
                       result.error().message);
        }

    } else {
        fmt::print(fg(fmt::color::red), "не удалось получить путь к: {}\n", module_name);
    }


    return 0;
}

std::string_view LuaScriptLoader::get_current_lua_script_path(lua_State *L) {
    lua_Debug ar;
    if (lua_getstack(L, 2, &ar)) {
        if (lua_getinfo(L, "Sl", &ar) && ar.source[0] == '@') {
            return ar.source + 1;
        }
    }

    return "";
}