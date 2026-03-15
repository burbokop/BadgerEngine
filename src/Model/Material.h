#pragma once

#include "Texture.h"

namespace BadgerEngine {

using MaterialColorChannel = std::variant<SharedTexture, Color>;
using MaterialFloatChannel = std::variant<SharedTexture, float>;

struct BSDFMaterial {
    MaterialColorChannel baseColor;
    MaterialColorChannel normalMap;
    MaterialColorChannel ambientOclusion;
    MaterialFloatChannel metallness;
    MaterialFloatChannel roughness;
    MaterialFloatChannel indexOfRefration;
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
