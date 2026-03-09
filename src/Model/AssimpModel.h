#pragma once

#include "../Geometry/Mesh.h"
#include "TextureLoader.h"
#include <filesystem>
#include <vector>

namespace BadgerEngine {

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
    return stream << "{ diffuseMaps: " << material.diffuseMaps.size()
                  << ", specularMaps: " << material.specularMaps.size()
                  << ", ambientMaps: " << material.ambientMaps.size()
                  << ", emissiveMaps: " << material.emissiveMaps.size()
                  << ", heightMaps: " << material.heightMaps.size()
                  << ", normalsMaps: " << material.normalsMaps.size()
                  << ", shininessMaps: " << material.shininessMaps.size()
                  << ", opacityMaps: " << material.opacityMaps.size()
                  << ", displacementMaps: " << material.displacementMaps.size()
                  << ", lightmapMaps: " << material.lightmapMaps.size()
                  << ", reflectionMaps: " << material.reflectionMaps.size()
                  << ", baseColorMaps: " << material.baseColorMaps.size()
                  << ", normalCameraMaps: " << material.normalCameraMaps.size()
                  << ", emissionColorMaps: " << material.emissionColorMaps.size()
                  << ", metalnessMaps: " << material.metalnessMaps.size()
                  << ", diffuseRoughnessMaps: " << material.diffuseRoughnessMaps.size()
                  << ", ambientOcclusionMaps: " << material.ambientOcclusionMaps.size()
                  << ", unknownTextures: " << material.unknownTextures.size()
                  << ", sheenMaps: " << material.sheenMaps.size()
                  << ", clearcoatMaps: " << material.clearcoatMaps.size()
                  << ", transmissionMaps: " << material.transmissionMaps.size()
                  << ", mayaBaseMaps: " << material.mayaBaseMaps.size()
                  << ", mayaSpecularMaps: " << material.mayaSpecularMaps.size()
                  << ", mayaSpecularColorMaps: " << material.mayaSpecularColorMaps.size()
                  << ", mayaSpecularRoughnessMaps: " << material.mayaSpecularRoughnessMaps.size()
                  << " }";
}

using MaterialIndex = std::size_t;
using SharedMaterial = Shared<Material>;

class AssimpMesh {
    struct Private {};

public:
    AssimpMesh& operator=(AssimpMesh&&) = delete;
    AssimpMesh& operator=(const AssimpMesh&) = delete;
    AssimpMesh(AssimpMesh&&) = delete;
    AssimpMesh(const AssimpMesh&) = delete;

    static Shared<AssimpMesh> create(
        Shared<Geometry::Mesh> mesh,
        SharedMaterial material)
    {
        return std::make_shared<AssimpMesh>(std::move(mesh), std::move(material), Private {});
    }

    const auto& mesh() const { return m_mesh; }
    const auto& material() const { return m_material; }

    AssimpMesh(
        Shared<Geometry::Mesh> mesh,
        SharedMaterial material,
        Private)
        : m_mesh(std::move(mesh))
        , m_material(std::move(material))
    {
    }

private:
    Shared<Geometry::Mesh> m_mesh;
    SharedMaterial m_material;
};

class AssimpModel {
public:
    /**
     * @brief load
     * @param path
     * @param additionalTextures - to be used when model file is broken and materials don't have links to the textures
     * @return
     */
    static Expected<AssimpModel> load(
        const TextureLoader&,
        const std::filesystem::path& path,
        const std::map<std::pair<MaterialIndex, TextureRole>, TextureLoader::VirtualTexturePath>& additionalTextures = {});

    /**
     * @brief parse
     * @param data
     * @param hint
     * @param additionalTextures - to be used when model file is broken and materials don't have links to the textures
     * @return
     */
    static Expected<AssimpModel> parse(
        const TextureLoader&,
        std::span<std::uint8_t> data,
        const std::string& hint,
        const std::map<std::pair<MaterialIndex, TextureRole>, TextureLoader::VirtualTexturePath>& additionalTextures = {});

    const auto& meshes() const { return m_meshes; }

    AssimpModel(std::vector<Shared<AssimpMesh>> meshes)
        : m_meshes(std::move(meshes))
    {
    }

private:
    std::vector<Shared<AssimpMesh>> m_meshes;
};

}
