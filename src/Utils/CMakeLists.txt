cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Utils)

target_sources(
  BadgerEngine
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/NoNull.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/NoNull.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Fs.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Fs.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Error.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Error.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Collections.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Collections.h>
  PRIVATE Fs.cpp Error.cpp)
