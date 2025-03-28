//
// Created by mihail on 3/26/25.
//

#ifndef FP_MODFETCHER_H
#define FP_MODFETCHER_H


#include <vector>
#include "ModInfo.h"

class ModFetcher {
public:
    ModFetcher();

    std::vector<ModInfo> fetch_mods(std::string mods_path, std::string factorio_path);
};


#endif //FP_MODFETCHER_H
