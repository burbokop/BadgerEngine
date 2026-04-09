cmake_minimum_required(VERSION 3.27.2)

add_subdirectory(Buffers)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Impl)

target_sources(
    BadgerEngine
    PRIVATE # cmake-format: sort
            BSDFVertexObject.cpp
            BSDFVertexObject.h
            ColorRenderPass.cpp
            ColorRenderPass.h
            commandpool.cpp
            commandpool.h
            descriptorsetlayout.cpp
            descriptorsetlayout.h
            extensiontools.cpp
            extensiontools.h
            font.cpp
            font.h
            graphicsobject.cpp
            graphicsobject.h
            hardware.cpp
            hardware.h
            ImageViewVertexObject.cpp
            ImageViewVertexObject.h
            logicdevicefactory.cpp
            logicdevicefactory.h
            pipeline.cpp
            pipeline.h
            ShadowMapRenderPass.cpp
            ShadowMapRenderPass.h
            stringvector.cpp
            stringvector.h
            SwapChain.cpp
            SwapChain.h
            UploadedModel.cpp
            UploadedModel.h
            UploadedTexture.cpp
            UploadedTexture.h
            validation.cpp
            validation.h
            vulkaninstancefactory.cpp
            vulkaninstancefactory.h)
