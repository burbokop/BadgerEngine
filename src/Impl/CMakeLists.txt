cmake_minimum_required(VERSION 3.27.2)

add_subdirectory(Buffers)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Impl)

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
            UploadedTexture.cpp
            ColorRenderPass.h
            ColorRenderPass.cpp
            ShadowMapRenderPass.cpp
            ShadowMapRenderPass.h
            graphicsobject.h
            swapchain.h
            commandpool.h
            descriptorsetlayout.h
            pipeline.h
            font.h
            swapchain.cpp
            commandpool.cpp
            pipeline.cpp
            font.cpp
            graphicsobject.cpp
            ImageViewVertexObject.cpp
            ImageViewVertexObject.h
            BSDFVertexObject.cpp
            BSDFVertexObject.h
            descriptorsetlayout.cpp)
