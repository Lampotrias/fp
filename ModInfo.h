//
// Created by mihail on 3/26/25.
//

#ifndef FP_MODINFO_H
#define FP_MODINFO_H


#include <string>
#include <nlohmann/json.hpp>

class ModInfo {

public:
    ModInfo();

    std::string name;
    std::string version;
    std::string factorio_version;
    std::string title;
    std::string author;
    std::string contact;
    std::string homepage;
    std::string description;
    std::string folder;
    bool quality_required;
    bool space_travel_required;
    bool spoiling_required;
    bool freezing_required;
    bool segmented_units_required;
    std::vector<std::string> dependencies;
    std::vector<std::string> incompatibilities;
    std::map<std::string, bool> parsed_dependencies;

    static void parseJsonToModInfo(const std::string &jsonString, ModInfo &modInfo);

    bool CanLoad(std::vector<std::string> notLoadedMods);
};


#endif //FP_MODINFO_H
