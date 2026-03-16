#include "Primitives.h"

namespace BadgerEngine::Geometry::Primitives {

const Shared<Mesh> plane = Mesh::create(
    Topology::TriangleList,
    {
        { .position = { -0.5f, -0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = { 0.0f, 0.0f } },
        { .position = { 0.5f, -0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 0.0f, 1.0f, 0.0f }, .uv = { 1.0f, 0.0f } },
        { .position = { 0.5f, 0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 0.0f, 0.0f, 1.0f }, .uv = { 1.0f, 1.0f } },
        { .position = { -0.5f, 0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 1.0f, 1.0f }, .uv = { 0.0f, 1.0f } },
    },
    { 0, 1, 2,
        2, 3, 0 });

const Shared<Mesh> ufo = Mesh::create(
    Topology::TriangleList,
    {
        { .position = { 4.5f, -0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { 4.5f, 0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { 0.5f, 1.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { 0.5f, 0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { 0.5f, -0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { 0.5f, -1.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -2.5f, -5.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -4.5f, -5.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -2.5f, -1.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -2.5f, -0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -3.5f, -0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -4.5f, -0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -4.5f, 0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -3.5f, 0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -2.5f, 0.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -2.5f, 1.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -4.5f, 5.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
        { .position = { -2.5f, 5.5f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = {} },
    },
    { 3, 0, 1,
        1, 2, 3,
        3, 4, 0,
        0, 4, 5,
        5, 8, 7,
        7, 6, 5,
        8, 9, 10,
        10, 12, 11,
        9, 12, 10,
        9, 12, 13,
        9, 14, 12,
        9, 13, 14,
        13, 14, 15,
        15, 8, 5,
        15, 5, 2,
        15, 2, 16,
        16, 2, 17 });
}
