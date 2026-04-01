#pragma once

#include "Texture.h"

namespace BadgerEngine {

using MaterialColorChannel = std::variant<SharedTexture, RGBAColor>;
using MaterialFloatChannel = std::variant<SharedTexture, float>;

struct BSDFMaterial {
    MaterialColorChannel baseColor;
    MaterialColorChannel normalMap;
    MaterialColorChannel ambientOclusion;
    MaterialFloatChannel metallness;
    MaterialFloatChannel roughness;
    MaterialFloatChannel indexOfRefration;
    bool castShadow;
};

/// Uses frame buffer as texture
struct RecursiveMaterial {
    std::vector<std::uint8_t> vert;
    std::vector<std::uint8_t> frag;
};

struct ShadowMapMaterial {
    std::vector<std::uint8_t> vert;
    std::vector<std::uint8_t> frag;
};

struct CustomMaterial {
    std::vector<SharedTexture> textures;
    std::vector<std::uint8_t> vert;
    std::vector<std::uint8_t> frag;
};

using Material = std::variant<BSDFMaterial, RecursiveMaterial, ShadowMapMaterial, CustomMaterial>;

using SharedMaterial = Shared<Material>;

}
