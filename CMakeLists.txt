cmake_minimum_required(VERSION 3.22)
project(pong VERSION 1.0.1 LANGUAGES C)

include(FetchContent)
# Keep older dependency projects configurable with CMake 4 as well.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
# Linux uses the desktop's X11/XWayland. The game needs no audio/devices.
foreach(feature AUDIO JOYSTICK HAPTIC SENSOR WAYLAND KMSDRM)
    set(SDL_${feature} OFF CACHE BOOL "" FORCE)
endforeach()
set(SDL2TTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2TTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL2TTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
FetchContent_Declare(sdl2
    URL https://www.libsdl.org/release/SDL2-2.32.10.tar.gz
    URL_HASH SHA256=5f5993c530f084535c65a6879e9b26ad441169b3e25d789d83287040a9ca5165)
FetchContent_Declare(sdl2_ttf
    URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-2.24.0.tar.gz
    URL_HASH SHA256=0b2bf1e7b6568adbdbc9bb924643f79d9dedafe061fa1ed687d1d9ac4e453bfd)
FetchContent_MakeAvailable(sdl2 sdl2_ttf)

# Store assets and license notices in the binary; never load from the cwd.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    font/DejaVuSans-Bold.ttf font/LICENSE-DejaVu.txt LICENSE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/font/DejaVuSans-Bold.ttf" font_hex HEX)
string(REGEX REPLACE "(..)" "0x\\1," font_bytes "${font_hex}")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated/embedded_font.h"
    "static const unsigned char embedded_font[] = {${font_bytes}};\n")
set(notices "Pong ${PROJECT_VERSION}\n\nThis software uses SDL2, SDL2_ttf, FreeType and DejaVu Sans Bold.\nPortions of this software are copyright (c) The FreeType Project (www.freetype.org). All rights reserved.\n")
foreach(license "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        "${CMAKE_CURRENT_SOURCE_DIR}/font/LICENSE-DejaVu.txt"
        "${sdl2_SOURCE_DIR}/LICENSE.txt"
        "${sdl2_ttf_SOURCE_DIR}/LICENSE.txt"
        "${sdl2_ttf_SOURCE_DIR}/external/freetype/docs/FTL.TXT")
    file(READ "${license}" license_text)
    string(APPEND notices "\n${license_text}\n")
endforeach()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/THIRD-PARTY-NOTICES.txt" "${notices}")
file(READ "${CMAKE_CURRENT_BINARY_DIR}/THIRD-PARTY-NOTICES.txt" notices_hex HEX)
string(REGEX REPLACE "(..)" "0x\\1," notices_bytes "${notices_hex}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated/embedded_licenses.h"
    "static const unsigned char embedded_licenses[] = {${notices_bytes}0};\n")

add_executable(pong src/main.c)
target_compile_features(pong PRIVATE c_std_99)
target_compile_definitions(pong PRIVATE SDL_MAIN_HANDLED)
target_include_directories(pong PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_link_libraries(pong PRIVATE SDL2_ttf::SDL2_ttf-static SDL2::SDL2-static)
if(MINGW)
    target_link_options(pong PRIVATE -static -mwindows)
endif()
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pong PRIVATE -Wall -Wextra -Wpedantic)
endif()
include(CTest)
if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_test(NAME standalone-smoke COMMAND pong --smoke-test)
    set_tests_properties(standalone-smoke PROPERTIES
        ENVIRONMENT "SDL_VIDEODRIVER=dummy;SDL_RENDER_DRIVER=software" TIMEOUT 20
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()
