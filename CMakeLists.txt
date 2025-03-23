cmake_minimum_required(VERSION 3.24)
project(fp)

set(CMAKE_CXX_STANDARD 20)

add_executable(fp main.cpp)

include(GNUInstallDirs)
install(TARGETS fp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)
pkg_check_modules(GTK REQUIRED gtkmm-4.0)

find_package(lua CONFIG REQUIRED)

include_directories(PRIVATE ${GTK_INCLUDE_DIRS})
target_link_directories(fp PRIVATE ${GTK_LIBRARY_DIRS})

target_compile_options(fp PRIVATE ${GTK_CFLAGS_OTHER})

target_link_libraries(fp PRIVATE  ${GTK_LIBRARIES} lua)