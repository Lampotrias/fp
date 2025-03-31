#include <lua.hpp>
#include <iostream>
#include <fstream>
#include "LuaScriptExecutor.h"
#include <nlohmann/json.hpp>
#include "lua/LuaContext.h"

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

int main() {
    LuaContext luaContext = LuaContext(
            "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/data",
            "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/mods"
    );

    luaContext.parse();
    return 0;
}