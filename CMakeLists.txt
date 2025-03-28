cmake_minimum_required(VERSION 3.24)
project(fp)

set(CMAKE_CXX_STANDARD 23)

add_executable(fp main.cpp LuaScriptPathResolver.cpp LuaScriptPathResolver.h LuaScriptLoader.cpp LuaScriptLoader.h LuaScriptExecutor.cpp LuaScriptExecutor.h ModInfo.cpp ModInfo.h ModFetcher.cpp ModFetcher.h zip/IZipReader.h zip/MinizZipReader.cpp Error.h)

add_library(miniz STATIC libs/miniz/miniz.c)

include(GNUInstallDirs)
install(TARGETS fp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)
pkg_check_modules(GTK REQUIRED gtkmm-4.0)

find_package(nlohmann_json CONFIG REQUIRED)

target_include_directories(miniz PUBLIC libs/miniz)

find_package(lua CONFIG REQUIRED)

include_directories(PRIVATE ${GTK_INCLUDE_DIRS})
target_link_directories(fp PRIVATE ${GTK_LIBRARY_DIRS})

target_compile_options(fp PRIVATE ${GTK_CFLAGS_OTHER})

target_link_libraries(fp PRIVATE ${GTK_LIBRARIES} lua fmt nlohmann_json::nlohmann_json miniz)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})