cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Import)

target_sources(
    BadgerEngine
    PUBLIC # cmake-format: sort
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/AssimpImporter.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Data.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Importer.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/TinyGLTFImporter.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/AssimpImporter.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Data.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Importer.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/TinyGLTFImporter.h>
    PRIVATE # cmake-format: sort
            AssimpImporter.cpp
            AssimpImporter.h
            Importer.cpp
            Importer.h
            TinyGLTFImporter.cpp
            TinyGLTFImporter.h)
