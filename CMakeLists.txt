cmake_minimum_required(VERSION 3.25)

project(
    Radinue
    VERSION 0.1.0
    DESCRIPTION "A minimal radio recording player"
    HOMEPAGE_URL "https://github.com/t-chov/radinue"
    LANGUAGES CXX
)

include(CTest)

option(RADINUE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(RadinueWarnings)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Widgets)
find_package(Mpv REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(src)

if(BUILD_TESTING)
    find_package(Qt6 6.8 REQUIRED COMPONENTS Test)
    add_subdirectory(tests)
endif()
