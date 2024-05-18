#pragma once

#include "../Buffers/MeshBuffer.h"
#include "../pipeline.h"

namespace BadgerEngine {

class UploadedModel {
public:
    UploadedModel(
        MeshBuffer meshBuffer,
        Shared<e172vp::Pipeline> pipeline)
        : m_meshBuffer(std::move(meshBuffer))
        , m_pipeline(std::move(pipeline))
    {
        assert(m_meshBuffer.topology() == m_pipeline->topology());
    }

    std::size_t indexCount() const
    {
        return m_meshBuffer.indexCount();
    }

    Shared<const e172vp::Pipeline> pipeline() const
    {
        return m_pipeline;
    }

    void bindTo(const vk::CommandBuffer& commandBuffer) const
    {
        m_pipeline->bindTo(commandBuffer);
        m_meshBuffer.bindTo(commandBuffer);
    }

private:
    MeshBuffer m_meshBuffer;
    Shared<e172vp::Pipeline> m_pipeline;
};

}
