cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Impl/Uploaded)

target_sources(BadgerEngine PRIVATE
    # cmake-format: sort
    UploadedMesh.h
    UploadedMesh.cpp
    UploadedModel.cpp
    UploadedModel.h
    UploadedTexture.cpp
    UploadedTexture.h)
