//
// Created by mihail on 3/26/25.
//

#include <filesystem>
#include <iostream>
#include <fstream>
#include <fmt/color.h>
#include "ModFetcher.h"
#include "zip/MinizZipReader.cpp"

namespace fs = std::filesystem;

ModFetcher::ModFetcher() = default;

std::vector<ModInfo> ModFetcher::fetch_mods(std::string mods_path, std::string factorio_path) {
    std::vector<ModInfo> mods = {};
    nlohmann::json j;
    MinizZipReader zip_reader;

    std::ifstream file(mods_path + "/mod-list.json");
    if (!file) {
        return mods;
    }
    std::ostringstream content;
    content << file.rdbuf();

    std::vector<std::string> enabled_mods = {};

    nlohmann::json jsonData = nlohmann::json::parse(content.str());
    if (jsonData.contains("mods") && jsonData["mods"].is_array()) {
        nlohmann::json modsArray = jsonData["mods"];

        for (const auto &mod: modsArray) {
            std::string name = mod.value("name", "");
            bool enabled = mod.value("enabled", false);
            if (enabled) {
                enabled_mods.push_back(name);
            }
        }
    } else {
        std::cerr << "Ключ 'mods' отсутствует или не является массивом." << std::endl;
    }


    for (std::string global_mod_folder: std::vector<std::string>{mods_path, factorio_path}) {
        auto mods_folder_path = fs::path(global_mod_folder);
        if (!fs::exists(mods_folder_path)) {
            continue;
        }

        try {
            for (const auto &mod_entity: fs::directory_iterator(mods_folder_path)) {
                ModInfo modInfo;
                if (fs::is_directory(mod_entity)) {
                    auto info_file_path = mod_entity.path() / "info.json";
                    if (fs::exists(info_file_path)) {
                        modInfo.name = mod_entity.path().filename().string();
                        modInfo.folder = global_mod_folder;
                    }
                } else if (mod_entity.is_regular_file() && mod_entity.path().extension() == ".zip") {
                    auto folder_name = mod_entity.path().filename().string().substr(0,
                                                                                    mod_entity.path().filename().string().size() -
                                                                                    4);
                    auto result = zip_reader.readFile(mod_entity.path(), "info.json");

                    if (result) {
                        ModInfo::parseJsonToModInfo(result.value(), modInfo);
                    } else {
                        fmt::print(fg(fmt::color::red), result.error().message + "\n");
                    }

                    std::cout << "Found mod: " << mod_entity.path() << std::endl;
                }

                if (std::find(enabled_mods.begin(), enabled_mods.end(), modInfo.name) != enabled_mods.end() || modInfo.name == "base" || modInfo.name == "core") {
                    mods.push_back(modInfo);
                }
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }

    return mods;
}
