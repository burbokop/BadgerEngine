cmake_minimum_required(VERSION 3.27.2)

add_subdirectory(Utils)
add_subdirectory(Tools)
add_subdirectory(Geometry)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

target_sources(
  BadgerEngine
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/graphicsobject.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/graphicsobject.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/graphicsobjectcreateinfo.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/graphicsobjectcreateinfo.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/renderer.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/renderer.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/swapchain.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/swapchain.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/renderpass.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/renderpass.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/commandpool.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/commandpool.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/vertexobject.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/vertexobject.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/descriptorsetlayout.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/descriptorsetlayout.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pipeline.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/pipeline.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/font.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/font.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Camera.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Camera.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Window.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Window.h>
  PRIVATE graphicsobject.cpp
          graphicsobjectcreateinfo.cpp
          renderer.cpp
          swapchain.cpp
          renderpass.cpp
          commandpool.cpp
          vertexobject.cpp
          descriptorsetlayout.cpp
          pipeline.cpp
          font.cpp
          Camera.cpp
          Window.cpp)
