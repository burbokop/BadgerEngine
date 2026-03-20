cmake_minimum_required(VERSION 3.27.2)

project(BadgerEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_BADGER_ENGINE_GLFW3 "Compile glfw3 based window" ON)
option(ENABLE_BADGER_ENGINE_SDL2 "Compile SDL2 based window" ON)
option(ENABLE_BADGER_ENGINE_PEDANTIC_DIAGNOSTIC
       "Enable all compiler warnings. Treat warnings as errors" ON)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(Freetype REQUIRED)
find_program(
    GLSLC_EXECUTABLE
    NAMES glslc
    HINTS Vulkan::glslc)

if (ENABLE_BADGER_ENGINE_GLFW3)
    find_package(glfw3 REQUIRED)
endif ()

if (ENABLE_BADGER_ENGINE_SDL2)
    find_package(SDL2 REQUIRED)
endif ()

set(BIN2H_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/build_tools/bin2h)
set(BIN2H_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build_tools/bin2h)
include(ExternalProject)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/bin2h.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/shaders.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/diagnostic.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/assimp.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/tinygltf.cmake)
set(BIN2H_EXECUTABLE
    ${BIN2H_EXECUTABLE}
    PARENT_SCOPE)

if (WIN32)
    add_library(BadgerEngine STATIC)
else ()
    add_library(BadgerEngine SHARED)
endif ()

add_subdirectory(src)

target_link_libraries(BadgerEngine PRIVATE Vulkan::Vulkan Freetype::Freetype assimp::assimp
                                           tinygltf::tinygltf)

badger_engine_target_pedantic_diagnostic(BadgerEngine)

if (ENABLE_BADGER_ENGINE_GLFW3)
    target_link_libraries(BadgerEngine PRIVATE glfw)
endif ()

if (ENABLE_BADGER_ENGINE_SDL2)
    target_link_libraries(BadgerEngine PRIVATE SDL2::SDL2)
endif ()

badger_engine_install_public_headers(
    BadgerEngine ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine)

badger_engine_add_embedded_shader_target(
    BadgerEngineEmbeddedShaders
    OUTPUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine/Shaders
    SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/shaders/normal_debug.frag
    ${CMAKE_CURRENT_LIST_DIR}/shaders/normal_debug.vert
    ${CMAKE_CURRENT_LIST_DIR}/shaders/BSDF.vert
    ${CMAKE_CURRENT_LIST_DIR}/shaders/BSDF.frag)

add_dependencies(BadgerEngine BadgerEngineEmbeddedShaders)

target_include_directories(
    BadgerEngine
    INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine/Shaders)
