#pragma once

#include "../Geometry/Mesh.h"
#include "Texture.h"

namespace BadgerEngine {

enum class PolygonMode {
    Fill,
    Line,
    Point,
    FillRectangleNV,
};

class Model {
public:
    Model(
        Shared<Geometry::Mesh> mesh,
        std::vector<SharedTexture> textures,
        std::vector<std::uint8_t> vert,
        std::vector<std::uint8_t> frag,
        PolygonMode polygonMode)
        : m_mesh(std::move(mesh))
        , m_textures(std::move(textures))
        , m_vert(std::move(vert))
        , m_frag(std::move(frag))
        , m_polygonMode(std::move(polygonMode))
    {
    }

    const auto& mesh() const { return m_mesh; }
    const auto& textures() const { return m_textures; }
    const auto& vert() const { return m_vert; }
    const auto& frag() const { return m_frag; }
    const auto& polygonMode() const { return m_polygonMode; }

private:
    Shared<Geometry::Mesh> m_mesh;
    std::vector<SharedTexture> m_textures;
    std::vector<std::uint8_t> m_vert;
    std::vector<std::uint8_t> m_frag;
    PolygonMode m_polygonMode;
};

}
