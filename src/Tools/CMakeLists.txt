cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Tools)

target_sources(
  BadgerEngine
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/hardware.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/hardware.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/extensiontools.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/extensiontools.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/validation.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/validation.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/stringvector.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/stringvector.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/vulkaninstancefactory.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/vulkaninstancefactory.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/logicdevicefactory.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/logicdevicefactory.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Model.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/Model.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/UploadedModel.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/UploadedModel.h>
  PRIVATE hardware.cpp
          extensiontools.cpp
          validation.cpp
          stringvector.cpp
          vulkaninstancefactory.cpp
          logicdevicefactory.cpp
          Model.cpp
          UploadedModel.cpp)
