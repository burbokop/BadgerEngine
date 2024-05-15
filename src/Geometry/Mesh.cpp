#include "Mesh.h"

#include "../Utils/Collections.h"
#include "ObjMesh.h"
#include <ranges>

namespace BadgerEngine::Geometry {

Mesh Mesh::fromObjMesh(const ObjMesh& mesh, glm::vec3 color)
{
    // TODO pass normales
    return Mesh(
        mesh.indices()
            | std::views::transform([&mesh, &color](auto i) {
                  return Vertex {
                      .position = mesh.vertices()[i.vertex],
                      .color = color,
                      .uv = mesh.uvs()[i.uv]
                  };
              })
            | Collect<std::vector>,
        std::views::iota(0u, static_cast<std::uint32_t>(mesh.indices().size()))
            | Collect<std::vector>);
}

}
