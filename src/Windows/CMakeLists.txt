cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Windows)

if (ENABLE_BADGER_ENGINE_GLFW3)
    target_sources(
        BadgerEngine
        PUBLIC # cmake-format: sort
               $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/GLFWWindow.h>
               $<INSTALL_INTERFACE:${INSTALLDIR}/GLFWWindow.h>
        PRIVATE # cmake-format: sort
                GLFWWindow.cpp)
endif ()

if (ENABLE_BADGER_ENGINE_SDL2)
    target_sources(
        BadgerEngine
        PUBLIC # cmake-format: sort
               $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/SDL2Window.h>
               $<INSTALL_INTERFACE:${INSTALLDIR}/SDL2Window.h>
        PRIVATE # cmake-format: sort
                SDL2Window.cpp)
endif ()
