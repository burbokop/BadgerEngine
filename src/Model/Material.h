#pragma once

#include "Texture.h"

namespace BadgerEngine {

using MaterialChannel = std::variant<SharedTexture, Color>;

struct BSDFMaterial {
    MaterialChannel baseColor;
    MaterialChannel metallness;
    MaterialChannel roughness;
    MaterialChannel indexOfRefration;
    MaterialChannel normalMap;
    MaterialChannel ambientOclusion;
};

/// Uses frame buffer as texture
struct RecursiveMaterial {
    std::vector<std::uint8_t> vert;
    std::vector<std::uint8_t> frag;
};

struct CustomMaterial {
    std::vector<SharedTexture> textures;
    std::vector<std::uint8_t> vert;
    std::vector<std::uint8_t> frag;
};

using Material = std::variant<BSDFMaterial, RecursiveMaterial, CustomMaterial>;

using SharedMaterial = Shared<Material>;

}
