#pragma once

#include "../Geometry/Mesh.h"
#include "../Utils/Error.h"
#include "../Utils/NoNull.h"

#include <map>

namespace vk {
class CommandBuffer;
}

namespace e172vp {
class GraphicsObject;
}

namespace BadgerEngine {

class UploadedMeshCache;

class UploadedMesh {
    friend UploadedMeshCache;

public:
    static Expected<UploadedMesh> upload(Shared<e172vp::GraphicsObject> device, UploadedMeshCache& cache, const Shared<Geometry::Mesh>& mesh);

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

class UploadedMeshCache {
    friend UploadedMesh;

public:
    UploadedMeshCache() = default;

    UploadedMeshCache& operator=(UploadedMeshCache&&) = delete;
    UploadedMeshCache& operator=(const UploadedMeshCache&) = delete;
    UploadedMeshCache(UploadedMeshCache&&) = delete;
    UploadedMeshCache(const UploadedMeshCache&) = delete;

private:
    struct MeshCmpLess {
        bool operator()(const std::weak_ptr<Geometry::Mesh>& a, const std::weak_ptr<Geometry::Mesh>& b) const
        {
            return a.lock() < b.lock();
        }
    };

private:
    std::map<std::weak_ptr<Geometry::Mesh>, std::weak_ptr<UploadedMesh::Impl>, MeshCmpLess> m_cache;
};

}
