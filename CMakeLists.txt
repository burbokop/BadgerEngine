cmake_minimum_required(VERSION 3.27.2)

project(
    BadgerEngine
    VERSION 0.0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_BADGER_ENGINE_GLFW3 "Compile glfw3 based window" ON)
option(ENABLE_BADGER_ENGINE_SDL2 "Compile SDL2 based window" ON)
option(ENABLE_BADGER_ENGINE_FREETYPE "Use freetype. Fonts are unavailable if OFF" ON)
option(ENABLE_BADGER_ENGINE_PEDANTIC_DIAGNOSTIC
       "Enable all compiler warnings. Treat warnings as errors" ON)
option(ENABLE_BADGER_ENGINE_RENDERDOC "Enable renderdoc api" OFF)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(glm REQUIRED)
find_program(
    GLSLC_EXECUTABLE
    NAMES glslc
    HINTS Vulkan::glslc)

if (ENABLE_BADGER_ENGINE_FREETYPE)
    find_package(Freetype REQUIRED)
endif ()

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
include(${CMAKE_CURRENT_LIST_DIR}/cmake/spng.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/embed.cmake)
set(BIN2H_EXECUTABLE
    ${BIN2H_EXECUTABLE}
    PARENT_SCOPE)

if (WIN32)
    add_library(BadgerEngine STATIC)
else ()
    add_library(BadgerEngine SHARED)
endif ()

add_subdirectory(src)

target_link_libraries(
    BadgerEngine
    PUBLIC glm::glm
    PRIVATE Vulkan::Vulkan assimp::assimp tinygltf::tinygltf spng::spng)

if (LINUX)
    target_link_libraries(BadgerEngine PRIVATE tbb)
endif ()

target_compile_definitions(
    BadgerEngine
    PUBLIC GLM_ENABLE_EXPERIMENTAL
    PRIVATE VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

badger_engine_target_pedantic_diagnostic(BadgerEngine)

if (ENABLE_BADGER_ENGINE_GLFW3)
    target_link_libraries(BadgerEngine PRIVATE glfw)
endif ()

if (ENABLE_BADGER_ENGINE_SDL2)
    target_link_libraries(BadgerEngine PRIVATE SDL2::SDL2)
endif ()

if (ENABLE_BADGER_ENGINE_FREETYPE)
    target_link_libraries(BadgerEngine PRIVATE Freetype::Freetype)
    target_compile_definitions(BadgerEngine PRIVATE BADGER_ENGINE_FREETYPE)
endif ()

if (ENABLE_BADGER_ENGINE_RENDERDOC)
    target_compile_definitions(BadgerEngine PRIVATE BADGER_ENGINE_RENDERDOC)
endif ()

badger_engine_install_public_headers(
    BadgerEngine ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine)

badger_engine_add_embedded_shader_target(
    BadgerEngineEmbeddedShaders
    OUTPUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine/Shaders
    SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/shaders/BSDF.vert
    ${CMAKE_CURRENT_LIST_DIR}/shaders/BSDF.frag
    ${CMAKE_CURRENT_LIST_DIR}/shaders/Normal.frag
    ${CMAKE_CURRENT_LIST_DIR}/shaders/Normal.vert
    ${CMAKE_CURRENT_LIST_DIR}/shaders/ShadowMap.vert)

add_dependencies(BadgerEngine BadgerEngineEmbeddedShaders)

target_include_directories(
    BadgerEngine
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include>
              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/install_prefix/include/BadgerEngine/Shaders)

# target_include_directories( ${PROJECT_NAME} INTERFACE
# $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
# $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

include(GNUInstallDirs)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/src/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
    FILES_MATCHING
    PATTERN "*.h")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(
    EXPORT ${PROJECT_NAME}_targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

set(CPACK_PACKAGE_FILE_NAME libbadger_engine-${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(DEBIAN_PACKAGE_DEPENDENCIES "libtbb12 >= 2022.1.0-1+deb13u1" "libvulkan1 >= 1.4.309.0-1")

if (ENABLE_BADGER_ENGINE_FREETYPE)
    list(APPEND DEBIAN_PACKAGE_DEPENDENCIES "libfreetype6 >= 2.13.3+dfsg-1+deb13u1")
endif ()

if (ENABLE_BADGER_ENGINE_GLFW3)
    list(APPEND DEBIAN_PACKAGE_DEPENDENCIES "libglfw3 >= 3.4-3")
endif ()

if (ENABLE_BADGER_ENGINE_SDL2)
    list(APPEND DEBIAN_PACKAGE_DEPENDENCIES "libsdl2-2.0-0 >= 2.32.4+dfsg-1")
endif ()

string(REPLACE ";" ", " CPACK_DEBIAN_PACKAGE_DEPENDS "${DEBIAN_PACKAGE_DEPENDENCIES}")

if (UNIX)
    set(CPACK_GENERATOR "DEB;STGZ")
elseif (WIN32)
    set(CPACK_GENERATOR ZIP)
endif ()

set(CPACK_PACKAGE_NAME "lib${PROJECT_NAME}")
set(CPACK_PACKAGE_CONTACT "io.github.burbokop")
include(CPack)
