cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Geometry)

target_sources(
  BadgerEngine
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Vertex.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Vertex.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Mesh.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Mesh.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/ObjMesh.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/ObjMesh.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Primitives.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Primitives.h>
  PRIVATE Vertex.cpp Mesh.cpp ObjMesh.cpp Primitives.cpp)
