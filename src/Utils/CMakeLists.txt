cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Utils)

target_sources(
    BadgerEngine
    PUBLIC # cmake-format: sort
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Collections.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Error.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Fs.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/NoNull.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/NumericCast.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Visit.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Collections.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Error.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Fs.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/NoNull.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/NumericCast.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Visit.h>
    PRIVATE # cmake-format: sort
            Error.cpp Fs.cpp)
