#pragma once

#include "../Geometry/Mesh.h"
#include "../Utils/Error.h"
#include "../Utils/NoNull.h"

namespace e172vp {
class GraphicsObject;
}

namespace BadgerEngine {

class MeshBuffer {
public:
    static Expected<MeshBuffer> upload(Shared<e172vp::GraphicsObject> device, const Geometry::Mesh& mesh);

    Geometry::Topology topology() const;
    std::size_t indexCount() const;

    void bindTo(const vk::CommandBuffer&) const;

private:
    class Impl;

private:
    MeshBuffer(Shared<Impl>&& impl)
        : m_impl(std::move(impl))
    {
    }

private:
    Shared<Impl> m_impl;
};

}
