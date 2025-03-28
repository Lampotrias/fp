//
// Created by mihail on 3/27/25.
//

#include <string>
#include <expected>
#include "../Error.h"

class IZipReader {
public:
    virtual ~IZipReader() = default;
    virtual std::expected<std::string, Error> readFile(const std::string& zipPath, const std::string& innerFilePath) = 0;
};