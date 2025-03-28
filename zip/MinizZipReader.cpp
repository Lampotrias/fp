//
// Created by mihail on 3/27/25.
//

#include <fmt/color.h>
#include <iostream>
#include "IZipReader.h"
#include "../libs/miniz/miniz.h"

class MinizZipReader : public IZipReader {
public:
    MinizZipReader() = default;

    std::expected<std::string, Error> readFile(const std::string &zipPath, const std::string &innerFilePath) override {
        mz_zip_archive zip_archive = {};
        if (!mz_zip_reader_init_file(&zip_archive, zipPath.c_str(), 0)) {
            return std::unexpected{Error{"Не удалось открыть ZIP-архив: " + zipPath}};
        }

        size_t file_size;
        char *p = static_cast<char *>(mz_zip_reader_extract_file_to_heap(&zip_archive, innerFilePath.c_str(),
                                                                         &file_size, 0));

        if (!p) {
            int file_count = (int) mz_zip_reader_get_num_files(&zip_archive);
            if (file_count > 0) {
                for (int i = 0; i < file_count; i++) {
                    mz_zip_archive_file_stat file_stat;
                    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
                        fmt::print(fg(fmt::color::blue_violet),
                                   "Не удалось получить информацию о файле с индексом " + std::to_string(i) + "\n");
                        continue;
                    }
                    std::string new_path = file_stat.m_filename;
                    if (new_path.ends_with(innerFilePath)) {
                        p = static_cast<char *>(mz_zip_reader_extract_file_to_heap(&zip_archive, new_path.c_str(),
                                                                                   &file_size, 0));
                        break;
                    }
                }


            }

            if (!p) {

//                fmt::print(fg(fmt::color::blue_violet), std::to_string(file_count) + "\n");
//                for (int i = 0; i < file_count; i++) {
//                    mz_zip_archive_file_stat file_stat;
//                    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
//                        fmt::print(fg(fmt::color::blue_violet),
//                                   "Не удалось получить информацию о файле с индексом " + std::to_string(i) + "\n");
//                        continue;
//                    }
//                    fmt::print(fg(fmt::color::blue_violet), file_stat.m_filename);
//                    fmt::print(fg(fmt::color::blue_violet), "\n");
//                }


                mz_zip_reader_end(&zip_archive);
                return std::unexpected{Error{"Не удалось извлечь файл " + innerFilePath + " из архива: " + zipPath}};
            }

        }

        std::string file_data(p, file_size);
        mz_free(p);
        mz_zip_reader_end(&zip_archive);
        return file_data;
    }
};