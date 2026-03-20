#pragma once

#include "../Geometry/Mesh.h"
#include "../Model/TextureLoader.h"
#include "../Utils/Visit.h"
#include <vector>

namespace BadgerEngine::Import {

enum class TextureRole {
    Unknown,
    Diffuse,
    Specular,
    Ambient,
    Emissive,
    Height,
    Normals,
    Shininess,
    Opacity,
    Displacement,
    Lightmap,
    Reflection,
    BaseColor,
    NormalCamera,
    EmissionColor,
    Metalness,
    DiffuseRoughness,
    AmbientOcclusion,
    Sheen,
    Clearcoat,
    Transmission,
    MayaBase,
    MayaSpecular,
    MayaSpecularColor,
    MayaSpecularRoughness,
};

struct Material {
    std::string name;

    std::optional<Color> baseColor;
    std::optional<Color> diffuseColor;
    std::optional<Color> emissiveColor;
    std::optional<Color> specularColor;
    std::optional<float> metallness;
    std::optional<float> roughness;
    std::optional<float> shininess;
    std::optional<float> specularFactor;

    std::vector<SharedTexture> diffuseMaps;
    std::vector<SharedTexture> specularMaps;
    std::vector<SharedTexture> ambientMaps;
    std::vector<SharedTexture> emissiveMaps;
    std::vector<SharedTexture> heightMaps;
    std::vector<SharedTexture> normalsMaps;
    std::vector<SharedTexture> shininessMaps;
    std::vector<SharedTexture> opacityMaps;
    std::vector<SharedTexture> displacementMaps;
    std::vector<SharedTexture> lightmapMaps;
    std::vector<SharedTexture> reflectionMaps;
    std::vector<SharedTexture> baseColorMaps;
    std::vector<SharedTexture> normalCameraMaps;
    std::vector<SharedTexture> emissionColorMaps;
    std::vector<SharedTexture> metalnessMaps;
    std::vector<SharedTexture> diffuseRoughnessMaps;
    std::vector<SharedTexture> ambientOcclusionMaps;
    std::vector<SharedTexture> unknownTextures;
    std::vector<SharedTexture> sheenMaps;
    std::vector<SharedTexture> clearcoatMaps;
    std::vector<SharedTexture> transmissionMaps;
    std::vector<SharedTexture> mayaBaseMaps;
    std::vector<SharedTexture> mayaSpecularMaps;
    std::vector<SharedTexture> mayaSpecularColorMaps;
    std::vector<SharedTexture> mayaSpecularRoughnessMaps;
};

inline std::ostream& operator<<(std::ostream& stream, const Material& material)
{
    std::map<const char*, std::string> counts = {
        { "diffuseMaps", std::to_string(material.diffuseMaps.size()) },
        { "specularMaps", std::to_string(material.specularMaps.size()) },
        { "ambientMaps", std::to_string(material.ambientMaps.size()) },
        { "emissiveMaps", std::to_string(material.emissiveMaps.size()) },
        { "heightMaps", std::to_string(material.heightMaps.size()) },
        { "normalsMaps", std::to_string(material.normalsMaps.size()) },
        { "shininessMaps", std::to_string(material.shininessMaps.size()) },
        { "opacityMaps", std::to_string(material.opacityMaps.size()) },
        { "displacementMaps", std::to_string(material.displacementMaps.size()) },
        { "lightmapMaps", std::to_string(material.lightmapMaps.size()) },
        { "reflectionMaps", std::to_string(material.reflectionMaps.size()) },
        { "baseColorMaps", std::to_string(material.baseColorMaps.size()) },
        { "normalCameraMaps", std::to_string(material.normalCameraMaps.size()) },
        { "emissionColorMaps", std::to_string(material.emissionColorMaps.size()) },
        { "metalnessMaps", std::to_string(material.metalnessMaps.size()) },
        { "diffuseRoughnessMaps", std::to_string(material.diffuseRoughnessMaps.size()) },
        { "ambientOcclusionMaps", std::to_string(material.ambientOcclusionMaps.size()) },
        { "unknownTextures", std::to_string(material.unknownTextures.size()) },
        { "sheenMaps", std::to_string(material.sheenMaps.size()) },
        { "clearcoatMaps", std::to_string(material.clearcoatMaps.size()) },
        { "transmissionMaps", std::to_string(material.transmissionMaps.size()) },
        { "mayaBaseMaps", std::to_string(material.mayaBaseMaps.size()) },
        { "mayaSpecularMaps", std::to_string(material.mayaSpecularMaps.size()) },
        { "mayaSpecularColorMaps", std::to_string(material.mayaSpecularColorMaps.size()) },
        { "mayaSpecularRoughnessMaps", std::to_string(material.mayaSpecularRoughnessMaps.size()) },
    };

    for (auto it = counts.begin(); it != counts.end();) {
        if (it->second == "0") {
            it = counts.erase(it);
        } else {
            ++it;
        }
    }

    const auto toString = Overloaded {
        [](float x) {
            std::ostringstream ss;
            ss << x;
            return ss.str();
        },
        [](glm::vec4 x) {
            std::ostringstream ss;
            ss << "[ " << x[0] << ", " << x[1] << ", " << x[2] << ", " << x[3] << " ]";
            return ss.str();
        },
    };

    const auto append = [&counts, &toString](const char* k, const auto& x) {
        if (x) {
            counts.insert({ k, toString(*x) });
        }
    };

    append("baseColor", material.baseColor);
    append("diffuseColor", material.diffuseColor);
    append("emissiveColor", material.emissiveColor);
    append("specularColor", material.specularColor);
    append("metallness", material.metallness);
    append("roughness", material.roughness);
    append("shininess", material.shininess);
    append("specularFactor", material.specularFactor);

    if (counts.empty()) {
        return stream << "{ name: " << material.name << " }";
    }

    stream << "{ name: " << material.name << ", ";
    for (std::size_t i = 0; const auto& count : counts) {
        stream << count.first << ": " << count.second;
        if (i++ < counts.size() - 1) {
            stream << ", ";
        }
    }
    return stream << " }";
}

using MaterialIndex = std::size_t;
using SharedMaterial = Shared<Material>;

class Mesh {
    struct Private {};

public:
    Mesh& operator=(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh(const Mesh&) = delete;

    static Shared<Mesh> create(
        std::string name,
        Shared<Geometry::Mesh> mesh,
        SharedMaterial material)
    {
        return std::make_shared<Mesh>(std::move(name), std::move(mesh), std::move(material), Private {});
    }

    const auto& name() const { return m_name; }
    const auto& mesh() const { return m_mesh; }
    const auto& material() const { return m_material; }

    Mesh(
        std::string name,
        Shared<Geometry::Mesh> mesh,
        SharedMaterial material,
        Private)
        : m_name(std::move(name))
        , m_mesh(std::move(mesh))
        , m_material(std::move(material))
    {
    }

private:
    std::string m_name;
    Shared<Geometry::Mesh> m_mesh;
    SharedMaterial m_material;
};

class Model {
public:
    const auto& meshes() const { return m_meshes; }

    Model(std::vector<Shared<Mesh>> meshes)
        : m_meshes(std::move(meshes))
    {
    }

private:
    std::vector<Shared<Mesh>> m_meshes;
};

}
