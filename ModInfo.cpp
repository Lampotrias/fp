//
// Created by mihail on 3/26/25.
//

#include <iostream>
#include <regex>
#include "ModInfo.h"

ModInfo::ModInfo() = default;

void ModInfo::parseJsonToModInfo(const std::string &jsonString, ModInfo &modInfo) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);

        // Извлечение и присвоение значений
        if (jsonData.contains("name")) {
            modInfo.name = jsonData.at("name").get<std::string>();
        }
        if (jsonData.contains("version")) {
            modInfo.version = jsonData.at("version").get<std::string>();
        }
        if (jsonData.contains("factorio_version")) {
            modInfo.factorio_version = jsonData.at("factorio_version").get<std::string>();
        }
        if (jsonData.contains("title")) {
            modInfo.title = jsonData.at("title").get<std::string>();
        }
        if (jsonData.contains("author")) {
            modInfo.author = jsonData.at("author").get<std::string>();
        }
        if (jsonData.contains("contact")) {
            modInfo.contact = jsonData.at("contact").get<std::string>();
        }
        if (jsonData.contains("homepage")) {
            modInfo.homepage = jsonData.at("homepage").get<std::string>();
        }
        if (jsonData.contains("description")) {
            modInfo.description = jsonData.at("description").get<std::string>();
        }
        if (jsonData.contains("folder")) {
            modInfo.folder = jsonData.at("folder").get<std::string>();
        }
        if (jsonData.contains("quality_required")) {
            modInfo.quality_required = jsonData.at("quality_required").get<bool>();
        }
        if (jsonData.contains("space_travel_required")) {
            modInfo.space_travel_required = jsonData.at("space_travel_required").get<bool>();
        }
        if (jsonData.contains("spoiling_required")) {
            modInfo.spoiling_required = jsonData.at("spoiling_required").get<bool>();
        }
        if (jsonData.contains("freezing_required")) {
            modInfo.freezing_required = jsonData.at("freezing_required").get<bool>();
        }
        if (jsonData.contains("segmented_units_required")) {
            modInfo.segmented_units_required = jsonData.at("segmented_units_required").get<bool>();
        }
        if (jsonData.contains("dependencies")) {
            modInfo.dependencies = jsonData.at("dependencies").get<std::vector<std::string>>();
        } else {
            modInfo.dependencies = {"base"};
        }

        std::regex depRegex(R"(^\(?([?!~]|\(\?\))?\)?\s*([\w-]+)(?:\s*[><=]+\s*[\d.]+)?$)");

        for (const auto& dep : modInfo.dependencies) {
            std::smatch match;
            if (std::regex_match(dep, match, depRegex)) {
                std::string type = match[1];
                std::string mod_name = match[2];

                if (type == "!") {
                    modInfo.incompatibilities.push_back(mod_name);
                }

                if (type == "~") {
                    continue;
                }

                modInfo.parsed_dependencies[mod_name] = (type == "?");
            }
        }


    } catch (const nlohmann::json::exception &e) {
        std::cerr << "Ошибка при парсинге JSON: " << e.what() << std::endl;
    }
}

bool ModInfo::CanLoad(std::vector<std::string> notLoadedMods) {
    for (const auto &dependency: parsed_dependencies) {
      if (std::find(notLoadedMods.begin(), notLoadedMods.end(), dependency.first) != notLoadedMods.end()) {
        return false;
      }
    }
    return true;
}
