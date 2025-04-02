#include <fstream>
#include <nlohmann/json.hpp>
#include "lua/LuaContext.h"


int main() {
    LuaContext luaContext = LuaContext(
            "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/data",
            "/home/mihail/Downloads/Factorio_Linux/factorio_linux_2.0.39/factorio/mods"
    );

    luaContext.parse();
    return 0;
}