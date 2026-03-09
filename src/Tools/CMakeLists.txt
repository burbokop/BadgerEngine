cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Tools)

target_sources(
    BadgerEngine
    PRIVATE hardware.h
            extensiontools.h
            validation.h
            stringvector.h
            vulkaninstancefactory.h
            logicdevicefactory.h
            UploadedModel.h
            UploadedTexture.h
            hardware.cpp
            extensiontools.cpp
            validation.cpp
            stringvector.cpp
            vulkaninstancefactory.cpp
            logicdevicefactory.cpp
            UploadedModel.cpp
            UploadedTexture.h
            UploadedTexture.cpp)
