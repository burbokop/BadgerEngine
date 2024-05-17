#include "Mesh.h"

#include "../Utils/Collections.h"
#include "ObjMesh.h"
#include <ranges>

namespace BadgerEngine::Geometry {

Mesh Mesh::fromObjMesh(const ObjMesh& mesh, glm::vec3 color)
{
    // TODO pass normales
    return Mesh(
        Mesh::Topology::TriangleList,
        mesh.indices()
            | std::views::transform([&mesh, &color](auto i) {
                  return Vertex {
                      .position = mesh.vertices()[i.vertex],
                      .normal = mesh.normals()[i.normal],
                      .color = color,
                      .uv = mesh.uvs()[i.uv]
                  };
              })
            | Collect<std::vector>,
        std::views::iota(0u, static_cast<std::uint32_t>(mesh.indices().size()))
            | Collect<std::vector>);
}

std::optional<Mesh> Mesh::polygonNormalsMesh(float len) const
{

    switch (m_topology) {
    case Topology::LineList:
        return std::nullopt;
    case Topology::TriangleList: {
        std::vector<Vertex> vertices;
        vertices.reserve((m_indices.size() + 1) * 2 / 3);

        for (std::size_t i = 0; i < m_indices.size() - 2; ++i) {
            const std::array v = {
                m_vertices[m_indices[i + 0]],
                m_vertices[m_indices[i + 1]],
                m_vertices[m_indices[i + 2]]
            };

            const auto center = (v[0].position + v[1].position + v[2].position) / 3.f;
            const auto averageNormal = (v[0].normal + v[1].normal + v[2].normal) / 3.f;
            const auto averageColor = (v[0].color + v[1].color + v[2].color) / 3.f;
            const auto averageUV = (v[0].uv + v[1].uv + v[2].uv) / 3.f;

            vertices.push_back(Vertex { .position = center, .normal = averageNormal, .color = averageColor, .uv = averageUV });
            vertices.push_back(Vertex { .position = center + averageNormal * len, .normal = averageNormal, .color = averageColor, .uv = averageUV });
        }

        return Mesh(
            Topology::LineList,
            vertices,
            std::views::iota(0u, static_cast<std::uint32_t>(vertices.size())) | Collect<std::vector>);
    }
    }
    std::abort();
}

std::optional<Mesh> Mesh::vertexNormalsMesh(float len) const
{
    switch (m_topology) {
    case Topology::LineList:
        return std::nullopt;
    case Topology::TriangleList: {
        std::vector<Vertex> vertices;
        vertices.reserve(m_indices.size() * 2);

        for (std::size_t i = 0; i < m_indices.size(); ++i) {
            const auto v = m_vertices[m_indices[i]];
            vertices.push_back(Vertex { .position = v.position, .normal = v.normal, .color = v.color, .uv = v.uv });
            vertices.push_back(Vertex { .position = v.position + v.normal * len, .normal = v.normal, .color = v.color, .uv = v.uv });
        }

        return Mesh(
            Topology::LineList,
            vertices,
            std::views::iota(0u, static_cast<std::uint32_t>(vertices.size())) | Collect<std::vector>);
    }
    }
    std::abort();
}
}
