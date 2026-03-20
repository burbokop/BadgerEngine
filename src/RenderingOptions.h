#pragma once

#include <cstdint>

namespace BadgerEngine {

enum class DisplayNormals : std::uint8_t {
    NoNormals,
    VertexNormals,
    PolygonNormals
};

struct RenderingOptions {
    bool backfaceCulling = true;
    DisplayNormals displayNormals = DisplayNormals::NoNormals;
};

}
