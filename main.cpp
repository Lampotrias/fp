#include <lua.hpp>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>

namespace fs = std::filesystem;

int my_module_loader(lua_State *L) {
    std::string  module_name = luaL_checkstring(L, 1);
    std::cout << "Требуется модуль: " << module_name << std::endl;

    lua_Debug ar;
    if (lua_getstack(L, 2, &ar)) {
        if (lua_getinfo(L, "Sl", &ar)) {
            if (ar.source[0] == '@') {
                std::cout << "Вызов из файла: " << (ar.source + 1)
                          << " на строке: " << ar.currentline << std::endl;

                std::string full_path = ar.source + 1; // пропускаем '@'
                fs::path path_obj(full_path);
                fs::path folder = path_obj.parent_path();

                std::cout << "Папка: " << folder << std::endl;

                std::replace(module_name.begin(), module_name.end(), '.', '/');

                fs::path target_file = folder / (module_name + ".lua");
                std::cout << "Будем читать файл: " << target_file << std::endl;

                std::ifstream file(target_file);
                if (!file) {
                    std::cerr << "Не удалось открыть файл: " << target_file << std::endl;
                } else {
                    std::string content((std::istreambuf_iterator<char>(file)),
                                        std::istreambuf_iterator<char>());
                    file.close();

                    // Выполняем код в Lua
                    if (luaL_loadstring(L, content.c_str()) != LUA_OK) {
                        std::cerr << "Ошибка загрузки Lua: " << lua_tostring(L, -1) << std::endl;
                    } else {
                        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                            std::cerr << "Ошибка выполнения Lua: " << lua_tostring(L, -1) << std::endl;
                        } else {
                            std::cout << "Файл выполнен успешно." << std::endl;
                            return 1;
                        }
                    }
                }

            } else {
                std::cout << "Вызов из: " << ar.source
                          << " на строке: " << ar.currentline << std::endl;
            }
        }
    } else {
        std::cout << "Не удалось получить стек вызова уровня 2" << std::endl;
    }

    // Пример: обработаем "prototypes.fonts"
    if (std::string(module_name) == "prototypes.fonts") {
        const char *lua_code = "print('Загружен prototypes.fonts'); return { font_size = 12 }";
        if (luaL_loadstring(L, lua_code) != LUA_OK) {
            return luaL_error(L, "Ошибка загрузки модуля: %s", lua_tostring(L, -1));
        }
        return 1; // Вернули функцию-загрузчик
    }

    // Вернём "not found"
    lua_pushstring(L, ("Модуль " + std::string(module_name) + " не найден").c_str());
    return 1; // Вернули сообщение об ошибке
}

void setup_loader(lua_State *L) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "searchers"); // Lua 5.2+
    int len = lua_rawlen(L, -1);

    // Вставим нашу функцию в начало
    lua_pushcfunction(L, my_module_loader);
    for (int i = len; i >= 1; i--) {
        lua_rawgeti(L, -2, i);
        lua_rawseti(L, -3, i + 1);
    }
    lua_rawseti(L, -2, 1);

    lua_pop(L, 2); // Очистили стек
}

int main() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    setup_loader(L);
    if (luaL_dofile(L, "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/data/core/lualib/dataloader.lua") != LUA_OK) {
        std::cerr << "Lua error1: " << lua_tostring(L, -1) << std::endl;
    } else{
        std::cerr << "dataloader is ok" << std::endl;
    }

//    if (luaL_dofile(L, "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/data/core/data.lua") !=
//        LUA_OK) {
//        std::cerr << "Lua error1: " << lua_tostring(L, -1) << std::endl;
//    } else {
//        lua_getglobal(L, "result");
//        if (lua_isnumber(L, -1)) {
//            std::cout << "Lua result: " << lua_tonumber(L, -1) << std::endl;
//        }
//        lua_pop(L, 1);
//    }

    lua_close(L);
    return 0;
}