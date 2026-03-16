#pragma once

#include "../Utils/Error.h"
#include "../Utils/NoNull.h"
#include "Topology.h"
#include "Vertex.h"
#include <filesystem>
#include <optional>
#include <vector>

namespace BadgerEngine::Geometry {

class ObjMesh;

/// Internal engine mesh format
class Mesh {
    struct Private {};

public:
    using Index = std::uint32_t;

    Mesh& operator=(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh(const Mesh&) = delete;

    static Shared<Mesh> create(
        Topology topology,
        std::vector<Vertex> vertices,
        std::vector<Index> indices)
    {
        return std::make_shared<Mesh>(topology, std::move(vertices), std::move(indices), Private {});
    }

    static Shared<Mesh> fromObjMesh(const ObjMesh& mesh, glm::vec3 color);

    Topology topology() const { return m_topology; }
    const auto& vertices() const { return m_vertices; }
    const auto& indices() const { return m_indices; }

    std::optional<Shared<Mesh>> polygonNormalsMesh(float len) const;
    std::optional<Shared<Mesh>> vertexNormalsMesh(float len, glm::vec3 color) const;
    std::optional<Shared<Mesh>> vertexTangentsMesh(float len, glm::vec3 color) const;
    std::optional<Shared<Mesh>> vertexBitangentsMesh(float len, glm::vec3 color) const;

    Mesh(
        Topology topology,
        std::vector<Vertex> vertices,
        std::vector<Index> indices,
        Private)
        : m_topology(topology)
        , m_vertices(std::move(vertices))
        , m_indices(std::move(indices))
    {
    }

private:
    Topology m_topology;
    std::vector<Vertex> m_vertices;
    std::vector<Index> m_indices;
};
}
