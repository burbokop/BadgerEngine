cmake_minimum_required(VERSION 3.27.2)

project(BadgerEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BIN2H_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/build_tools/bin2h)
set(BIN2H_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build_tools/bin2h)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/bin2h.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/shaders.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/utils.cmake)
set(BIN2H_EXECUTABLE
    ${BIN2H_EXECUTABLE}
    PARENT_SCOPE)

add_library(BadgerEngine SHARED)
add_subdirectory(src)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SDL2 REQUIRED)

target_link_libraries(BadgerEngine PRIVATE Vulkan::Vulkan SDL2::SDL2 glfw freetype)

target_include_directories(BadgerEngine PUBLIC $<BUILD_INTERFACE:/usr/include/freetype2>)

badger_engine_install_public_headers(
    BadgerEngine ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine)

badger_engine_add_embedded_shader_target(
    BadgerEngineEmbeddedShaders
    OUTPUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine/Shaders
    SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/shaders/normal_debug.frag
    ${CMAKE_CURRENT_LIST_DIR}/shaders/normal_debug.vert)

add_dependencies(BadgerEngine BadgerEngineEmbeddedShaders)

target_include_directories(
    BadgerEngine
    INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include
    PRIVATE /usr/include/freetype2
            ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine/Shaders)
