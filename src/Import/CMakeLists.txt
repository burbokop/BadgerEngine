cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Import)

target_sources(
    BadgerEngine
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Data.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Data.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Importer.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Importer.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/AssimpImporter.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/AssimpImporter.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/TinyGLTFImporter.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/TinyGLTFImporter.h>
    PRIVATE Importer.h
            Importer.cpp
            AssimpImporter.h
            AssimpImporter.cpp
            TinyGLTFImporter.h
            TinyGLTFImporter.cpp)
