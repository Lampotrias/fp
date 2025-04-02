//
// Created by mihail on 3/24/25.
//

#include "LuaScriptPathResolver.h"

#include <fmt/color.h>
#include <algorithm>

LuaScriptPathResolver::LuaScriptPathResolver(std::string data_folder_path, std::string mods_folder_path)
        : data_folder_path(std::move(data_folder_path)), mods_folder_path(std::move(mods_folder_path)), replace_paths{
        {"__core__", "core"},
        {"__base__", "base"}
} {}

std::expected<std::string, Error>
LuaScriptPathResolver::resolve_require(std::string_view module_name, const fs::path &parent_script_folder) {
    std::string lua_path;

    if (module_name.ends_with(".lua")) {
        lua_path = module_name.substr(0, module_name.size() - 4);
    } else {
        lua_path = module_name;
    }

    std::ranges::replace(lua_path, '.', '/');


    std::string lua_full_path = lua_path;
    if (lua_full_path[0] == '/') {
        lua_full_path += ".lua";
    } else if (lua_full_path.starts_with("__")) {
        replacePrefix(lua_full_path);
        lua_full_path = lua_full_path + ".lua";
    } else {
        lua_full_path = parent_script_folder / (lua_path + ".lua");
    }

    if (fs::exists(lua_full_path)) {
        fmt::print(fg(fmt::color::white_smoke), "Нашли модуль '{}' по пути: '{}'\n", module_name, lua_full_path);
        return lua_full_path;
    } else {
        fmt::print(fg(fmt::color::white_smoke), "Не удалось найти модуль '{}' по пути: '{}'\n", module_name, lua_full_path);

        std::vector<std::string> fallback_paths = {
                data_folder_path + "/core/lualib/" + lua_path + ".lua",
                parent_script_folder.parent_path().parent_path() / (lua_path + ".lua"),
                parent_script_folder.parent_path() / (lua_path + ".lua")
        };

        for (auto path: fallback_paths) {
            if (fs::exists(path)) {
                fmt::print(fg(fmt::color::white_smoke), "Нашли модуль '{}' по пути: '{}'\n", module_name, path);
                return path;
            }
        }


        auto error = "Не найден скрипт";
        return std::unexpected{Error{error}};
    }
}

void LuaScriptPathResolver::replacePrefix(std::string &lua_path) {
    for (const auto &rp: replace_paths) {
        if (lua_path.starts_with(rp.macros)) {
            lua_path.replace(0, rp.macros.length(), rp.path);

            lua_path = data_folder_path + "/" + lua_path;
            break;
        }
    }
}
