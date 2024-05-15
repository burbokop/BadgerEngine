#pragma once

#include "../Utils/Error.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

namespace BadgerEngine::Geometry {

/// .obj format
class ObjMesh {
public:
    struct Index {
        std::uint32_t vertex;
        std::uint32_t uv;
        std::uint32_t normal;
    };

    static Expected<ObjMesh> load(const std::filesystem::path& path);

    ObjMesh(const ObjMesh&) = delete;
    ObjMesh& operator=(const ObjMesh&) = delete;
    ObjMesh(ObjMesh&&) = default;
    ObjMesh& operator=(ObjMesh&&) = default;

    ObjMesh(
        std::vector<glm::vec3> vertices,
        std::vector<glm::vec2> uvs,
        std::vector<glm::vec3> normals,
        std::vector<Index> indices)
        : m_vertices(std::move(vertices))
        , m_uvs(std::move(uvs))
        , m_normals(std::move(normals))
        , m_indices(std::move(indices))
    {
    }

    const auto& vertices() const { return m_vertices; }
    const auto& uvs() const { return m_uvs; }
    const auto& normals() const { return m_normals; }
    const auto& indices() const { return m_indices; }

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals;
    std::vector<Index> m_indices;
};

}
