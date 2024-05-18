#pragma once

#include "Topology.h"
#include "Vertex.h"
#include <optional>
#include <vector>

namespace BadgerEngine::Geometry {

class ObjMesh;

/// Internal engine mesh format
class Mesh {
public:
    Mesh(
        Topology topology,
        std::vector<Vertex> vertices,
        std::vector<std::uint32_t> indices)
        : m_topology(topology)
        , m_vertices(std::move(vertices))
        , m_indices(std::move(indices))
    {
    }

    static Mesh fromObjMesh(const ObjMesh& mesh, glm::vec3 color);

    Topology topology() const { return m_topology; }
    const auto& vertices() const { return m_vertices; }
    const auto& indices() const { return m_indices; }

    std::optional<Mesh> polygonNormalsMesh(float len) const;
    std::optional<Mesh> vertexNormalsMesh(float len) const;

private:
    Topology m_topology;
    std::vector<Vertex> m_vertices;
    std::vector<std::uint32_t> m_indices;
};
}
