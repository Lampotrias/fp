//
// Created by mihail on 3/31/25.
//

#ifndef FP_REQUIREMODEFILE_H
#define FP_REQUIREMODEFILE_H


#include <string>

class RequireModeFile {
public:
    std::string mod_name;
    std::string full_file_path;

    // Оператор сравнения
    bool operator==(const RequireModeFile &other) const = default; // C++20

    // Хэш-функция как friend-функция
    friend struct std::hash<RequireModeFile>;
};

namespace std {
    template<>
    struct hash<RequireModeFile> {
        size_t operator()(const RequireModeFile &key) const {
            size_t h1 = hash<string>{}(key.mod_name);
            size_t h2 = hash<string>{}(key.full_file_path);
            return h1 ^ (h2 << 1); // Комбинируем хэши
        }
    };
}

#endif //FP_REQUIREMODEFILE_H
