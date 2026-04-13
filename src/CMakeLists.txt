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
    PUBLIC # cmake-format: sort
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Camera.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/PointLight.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Renderer.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/RenderingOptions.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/VertexObject.h>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Window.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Camera.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/PointLight.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Renderer.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/RenderingOptions.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/VertexObject.h>
           $<INSTALL_INTERFACE:${INSTALLDIR}/Window.h>
    PRIVATE # cmake-format: sort
            Camera.cpp Renderer.cpp VertexObject.cpp)
