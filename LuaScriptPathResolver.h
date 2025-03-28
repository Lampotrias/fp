//
// Created by mihail on 3/24/25.
//

#ifndef FP_LUASCRIPTPATHRESOLVER_H
#define FP_LUASCRIPTPATHRESOLVER_H

#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include <expected>
#include "Error.h"

namespace fs = std::filesystem;

struct ReplacePath {
    std::string macros;
    std::string path;
};

class LuaScriptPathResolver {

public:
    LuaScriptPathResolver(std::string data_folder_path, std::string mods_folder_path);

    std::expected<std::string, Error> resolve_require(std::string_view module_name, const fs::path& parent_script_folder);



private:
    std::string data_folder_path;
    std::string mods_folder_path;
    std::vector<ReplacePath> replace_paths;

    void replacePrefix(std::string &lua_path);
};


#endif //FP_LUASCRIPTPATHRESOLVER_H
