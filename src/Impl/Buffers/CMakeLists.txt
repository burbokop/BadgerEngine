cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Impl/Buffers)

target_sources(BadgerEngine PRIVATE MeshBuffer.h BufferUtils.h MeshBuffer.cpp BufferUtils.cpp)
