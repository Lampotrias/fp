//
// Created by mihail on 3/31/25.
//

#include "LuaContext.h"

#include <utility>
#include <algorithm>
#include <fmt/color.h>
#include <iostream>
#include "../LuaScriptExecutor.h"
#include "../ModFetcher.h"

LuaContext::LuaContext(std::string data_folder_path, std::string mods_folder_path)
        : L{luaL_newstate()},
          data_folder_path(std::move(
                  data_folder_path)),
          mods_folder_path(std::move(
                  mods_folder_path)),
          resolver(this->data_folder_path, this->mods_folder_path) {

    setup_lua();
}


// Вспомогательная функция
inline int PushNilAndReturn(lua_State *L) {
    lua_pushnil(L);
    return 1;
}

int LuaContext::Require(lua_State *L) {
    // 1. Получаем имя модуля
    const std::string module_name = luaL_checkstring(L, 1);
    fmt::print(fg(fmt::color::yellow_green), "Requiring module: {}\n", module_name);

    // 2. Получаем путь к текущему скрипту
    const std::string current_path = get_current_lua_script_path();
    if (current_path.empty()) {
        fmt::print(fg(fmt::color::red), "Cannot determine path for module: {}\n", module_name);
        return PushNilAndReturn(L);
    }

    // 3. Разрешаем полный путь к модулю
    const auto resolved_path = resolver.resolve_require(module_name, fs::path(current_path).parent_path());
    if (!resolved_path) {
        fmt::print(fg(fmt::color::red), "Module {} not found: {}\n",
                   module_name, resolved_path.error().message);
        return PushNilAndReturn(L);
    }

    // 4. Проверяем кеш
    static std::unordered_map<std::string, int> module_cache;
    if (auto it = module_cache.find(resolved_path.value()); it != module_cache.end()) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, it->second);
        fmt::print(fg(fmt::color::green), "Module {} loaded from cache\n", module_name);
        return 1;
    }

    // 5. Загружаем и выполняем файл одним вызовом
    if (luaL_dofile(L, resolved_path.value().c_str()) != LUA_OK) {
        const std::string error = lua_tostring(L, -1);
        fmt::print(fg(fmt::color::red), "Error loading {}: {}\n", module_name, error);
        lua_pop(L, 1); // Remove error message
        return PushNilAndReturn(L);
    }

    // 6. Кешируем результат (если модуль что-то вернул)
    if (!lua_isnil(L, -1)) {
        module_cache[resolved_path.value()] = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_rawgeti(L, LUA_REGISTRYINDEX, module_cache[resolved_path.value()]);
    }

    fmt::print(fg(fmt::color::green), "Module {} successfully loaded\n", module_name);
    return 1;
}

std::string LuaContext::get_current_lua_script_path() {
    lua_Debug ar;
    if (lua_getstack(L, 1, &ar)) {
        if (lua_getinfo(L, "Sl", &ar)) {
            if (ar.source[0] == '@') {
                return ar.source + 1;
            }
        }
    }

    return "";
}

LuaContext::~LuaContext() {
    lua_close(L);
    L = nullptr;
}

void LuaContext::setup_lua() {
    luaL_openlibs(L);
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, &LuaContext::RequireWrapper, 1);
    lua_setglobal(L, "require");
}

void LuaContext::parse() {
    LuaScriptExecutor executor = LuaScriptExecutor();
    executor.execute_script(L, "Data/Sandbox.lua");
    executor.execute_script(L, "Data/Postprocess.lua");

    auto ss = ModFetcher();
    auto all_mods = ss.fetch_mods(mods_folder_path, data_folder_path);

    std::vector<std::string> keys;
    std::transform(all_mods.begin(), all_mods.end(), std::back_inserter(keys),
                   [](const auto &pair) { return pair.first; });

//    dump_mods(keys);

    std::vector<std::string> mods_to_load = keys;
    std::erase(mods_to_load, "core");
    mods_to_load.reserve(all_mods.size());

    std::vector<std::string> mod_load_order = {};
    mod_load_order.reserve(all_mods.size());
    mod_load_order.emplace_back("core");


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

    while (!mods_to_load.empty()) {
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

        std::erase_if(sorted_mods, [&](const std::string &mod) {
            return std::find(mods_to_load.begin(), mods_to_load.end(), mod) == mods_to_load.end();
        });

    }

    for (auto s: mod_load_order) {
        std::cout << s << std::endl;
    }

    executor.execute_script(L, data_folder_path + "/" + mod_load_order[0] + "/data.lua");
    executor.execute_script(L, data_folder_path + "/" + mod_load_order[1] + "/data.lua");

//    lua_getglobal(L, "data");
//    if (!lua_istable(L, -1)) {
//        std::cerr << "Ошибка: 'data' не является таблицей" << std::endl;
//    } else {
//        print_table(L, lua_gettop(L));
//    }
}

void LuaContext::dump_mods(std::vector<std::string> names) {
    for (auto name: names) {
        fmt::print(fg(fmt::color::green), name + "\n");
    }
}

int LuaContext::RequireWrapper(lua_State *L) {
    // Достаём контекст из замыкания
    void *ptr = lua_touserdata(L, lua_upvalueindex(1));
    LuaContext *ctx = static_cast<LuaContext *>(ptr);
    return ctx->Require(L);
}

void LuaContext::print_table(lua_State *L, int index, int depth) {
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

