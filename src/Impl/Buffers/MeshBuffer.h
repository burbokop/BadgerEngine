#pragma once

#include "../Geometry/Mesh.h"
#include "../Utils/Error.h"
#include "../Utils/NoNull.h"

namespace vk {
class CommandBuffer;
}

namespace e172vp {
class GraphicsObject;
}

namespace BadgerEngine {

class UploadedMesh {
public:
    static Expected<UploadedMesh> upload(Shared<e172vp::GraphicsObject> device, const Geometry::Mesh& mesh);

    Geometry::Topology topology() const;
    std::size_t indexCount() const;

    void bindTo(const vk::CommandBuffer&) const;

private:
    struct Impl;

private:
    UploadedMesh(Shared<Impl>&& impl)
        : m_impl(std::move(impl))
    {
    }

private:
    Shared<Impl> m_impl;
};

}
