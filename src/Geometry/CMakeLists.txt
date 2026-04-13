cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Geometry)

target_sources(
    BadgerEngine
    PUBLIC # cmake-format: sort
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Mesh.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/ObjMesh.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Primitives.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Topology.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Vertex.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Mesh.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/ObjMesh.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Primitives.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Topology.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Vertex.h>
    PRIVATE # cmake-format: sort
            Mesh.cpp ObjMesh.cpp Primitives.cpp Vertex.cpp)
