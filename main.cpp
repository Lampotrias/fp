#include <lua.hpp>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "LuaScriptLoader.h"
#include "LuaScriptExecutor.h"
#include "ModFetcher.h"
#include <nlohmann/json.hpp>
#include <fmt/color.h>

namespace fs = std::filesystem;

static int custom_searcher(lua_State *L) {
    LuaScriptPathResolver resolver = LuaScriptPathResolver(
            "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/data",
            "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/mods");
    LuaScriptLoader loader = LuaScriptLoader();
    return loader.custom_searcher(L, resolver);
}

void setup_lua(lua_State *L) {
    luaL_openlibs(L);
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "searchers");

    lua_pushcfunction(L, custom_searcher);

    lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);

    // Убираем таблицу searchers из стека
    lua_pop(L, 1);

    // Убираем таблицу package из стека
    lua_pop(L, 1);
}

void print_table(lua_State *L, int index, int depth = 0) {
    lua_pushnil(L);  // Первый ключ
    while (lua_next(L, index) != 0) {
        // Получаем ключ и значение из стека
        int value_type = lua_type(L, -1);
        int key_type = lua_type(L, -2);

        // Отступ для наглядности структуры таблицы
        std::string indent(depth * 2, ' ');

        // Обработка ключа
        std::cout << indent;
        if (key_type == LUA_TSTRING) {
            std::cout << lua_tostring(L, -2);
        } else if (key_type == LUA_TNUMBER) {
            std::cout << lua_tonumber(L, -2);
        } else {
            std::cout << "unknown key type";
        }
        std::cout << " = ";

        // Обработка значения
        if (value_type == LUA_TSTRING) {
            std::cout << lua_tostring(L, -1) << std::endl;
        } else if (value_type == LUA_TNUMBER) {
            std::cout << lua_tonumber(L, -1) << std::endl;
        } else if (value_type == LUA_TTABLE) {
            std::cout << std::endl;
            print_table(L, lua_gettop(L), depth + 1);  // Рекурсивный вызов для вложенной таблицы
        } else if (value_type == LUA_TBOOLEAN) {
            std::cout << (lua_toboolean(L, -1) ? "true" : "false") << std::endl;
        } else {
            std::cout << "unknown value type" << std::endl;
        }

        lua_pop(L, 1);  // Удаляем значение, оставляем ключ для следующего lua_next
    }
}

using json = nlohmann::json;
#define MINIZ_HEADER_FILE_ONLY

#include "libs/miniz/miniz.h"

static void dump_mods(std::vector<std::string> names) {

    for (auto name: names) {
        fmt::print(fg(fmt::color::green), name + "\n");
    }

// All mods found! Loading order: core, base, pyalienlifegraphics2, pyalienlifegraphics3, pyalternativeenergygraphics, pyfusionenergygraphics, pyhightechgraphics,
// pyindustrygraphics, pypetroleumhandlinggraphics, pyraworesgraphics, squeak-through-2, aai-loaders, Custom-Production-UI-Fixed, even-distribution, even-pickier-dollies,
// Fill4Me, flib, FNEI, GhostPlacerXpress, helmod, inventory-repair, Kruise_Kontrol_Updated, Nearby-Ammo-Count, pyalienlifegraphics, pycoalprocessing, pycoalprocessinggraphics,
// show-max-underground-distance, Teleporters, TimeTools, BottleneckLite, earlier-teleporters_redux, factoryplanner, Lighted-Poles-Plus, Milestones, pyfusionenergy, RateCalculator,
// StatsGui, GUI_Unifyer, pyindustry, pyrawores, pyhightech, pypetroleumhandling, pyalienlife, pyalternativeenergy, InserterCranes-for-pymods, Lighted-Poles-Plus-pyalternativeenergy,
// pyhardmode, pypostprocessing, cybersyn, cybersyn-combinator
}

int main() {
    lua_State *L = luaL_newstate();
    if (!L) {
        std::cerr << "Failed to create Lua state" << std::endl;
        return 1;
    }

    setup_lua(L);
    LuaScriptExecutor executor = LuaScriptExecutor();
    executor.execute_script(L, "Data/Sandbox.lua");
    executor.execute_script(L, "Data/Postprocess.lua");

    auto ss = ModFetcher();
    auto all_mods = ss.fetch_mods("/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/mods",
                                  "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/data");

    std::vector<std::string> keys;
    std::transform(all_mods.begin(), all_mods.end(), std::back_inserter(keys),
                   [](const auto &pair) { return pair.first; });

    dump_mods(keys);

    std::vector<std::string> mods_to_load = keys;
    std::erase(mods_to_load, "core");
    mods_to_load.reserve(all_mods.size());

    std::vector<std::string> mod_load_order = {};
    mod_load_order.reserve(all_mods.size());
    mod_load_order.push_back("core");


    std::vector<std::string> sorted_mods = {};
    sorted_mods.reserve(all_mods.size());

    std::vector<std::string> current_load_batch = {};
    current_load_batch.reserve(all_mods.size());

    sorted_mods = mods_to_load;
    std::sort(sorted_mods.begin(), sorted_mods.end(), [&](const std::string &a, const std::string &b) {
        return std::lexicographical_compare(
                a.begin(), a.end(),
                b.begin(), b.end(),
                [](unsigned char c1, unsigned char c2) { return std::tolower(c1) < std::tolower(c2); }
        );
    });
    int index = 1;
    while (mods_to_load.size() > 0) {
        current_load_batch.clear();

        for (auto &mod: sorted_mods) {
            if (all_mods[mod].CanLoad(mods_to_load)) {
                current_load_batch.push_back(mod);
            }
        }

        if (current_load_batch.empty()) {
            std::cout << "No more mods to load" << std::endl;
            break;
        }

        for (auto mod: current_load_batch) {
            mod_load_order.push_back(mod);
            std::erase(mods_to_load, mod);
        }

        std::erase_if(sorted_mods, [&](const std::string& mod) {
            return std::find(mods_to_load.begin(), mods_to_load.end(), mod) == mods_to_load.end();
        });

    }

    for (auto s: mod_load_order) {
        std::cout << s << std::endl;
    }


//    std::cout << std::setw(4) << json::meta() << std::endl;

    lua_close(L);
    return 0;
}