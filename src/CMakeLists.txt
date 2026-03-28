cmake_minimum_required(VERSION 3.27.2)

add_subdirectory(Utils)
add_subdirectory(Impl)
add_subdirectory(Geometry)
add_subdirectory(Model)
add_subdirectory(Windows)
add_subdirectory(Import)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

target_sources(
    BadgerEngine
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Renderer.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Renderer.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/VertexObject.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/VertexObject.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Camera.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Camera.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Window.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Window.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/PointLight.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/PointLight.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/RenderingOptions.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/RenderingOptions.h>
    PRIVATE Renderer.cpp VertexObject.cpp Camera.cpp)
