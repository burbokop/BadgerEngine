cmake_minimum_required(VERSION 3.27.2)

project(BadgerEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(__cpp_concepts=202002L)

add_library(BadgerEngine SHARED)

add_subdirectory(src)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(BadgerEngine PRIVATE Vulkan::Vulkan glfw freetype)

target_include_directories(BadgerEngine
                           PUBLIC $<BUILD_INTERFACE:/usr/include/freetype2>)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/shaders.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/utils.cmake)

badger_engine_install_public_headers(
  BadgerEngine ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine)

target_include_directories(
  BadgerEngine
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include
  PRIVATE /usr/include/freetype2)
