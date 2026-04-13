cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Model)

target_sources(
    BadgerEngine
    PUBLIC # cmake-format: sort
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Material.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Model.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Texture.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/TextureLoader.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Material.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Model.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Texture.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/TextureLoader.h>
    PRIVATE # cmake-format: sort
            Material.cpp Model.cpp Texture.cpp TextureLoader.cpp)
