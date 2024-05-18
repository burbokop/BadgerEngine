cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Buffers)

target_sources(
  BadgerEngine
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/MeshBuffer.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/MeshBuffer.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/BufferUtils.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/BufferUtils.h>
  PRIVATE MeshBuffer.cpp BufferUtils.cpp)
