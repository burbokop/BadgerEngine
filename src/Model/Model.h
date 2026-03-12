#pragma once

#include "../Geometry/Mesh.h"
#include "Material.h"
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
        SharedMaterial material,
        PolygonMode polygonMode)
        : m_mesh(std::move(mesh))
        , m_material(std::move(material))
        , m_polygonMode(std::move(polygonMode))
    {
    }

    Model(
        Shared<Geometry::Mesh> mesh,
        Material material,
        PolygonMode polygonMode)
        : m_mesh(std::move(mesh))
        , m_material(std::make_shared<Material>(std::move(material)))
        , m_polygonMode(std::move(polygonMode))
    {
    }

    const auto& mesh() const { return m_mesh; }
    const auto& material() const { return m_material; }
    const auto& polygonMode() const { return m_polygonMode; }

private:
    Shared<Geometry::Mesh> m_mesh;
    SharedMaterial m_material;
    PolygonMode m_polygonMode;
};

}
