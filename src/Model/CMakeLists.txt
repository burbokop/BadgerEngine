cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Model)

target_sources(
    BadgerEngine
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Model.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Model.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Texture.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Texture.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/TextureLoader.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/TextureLoader.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/AssimpModel.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/AssimpModel.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Material.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Material.h>
    PRIVATE Model.cpp
            Texture.cpp
            TextureLoader.cpp
            AssimpModel.cpp
            Material.cpp)
