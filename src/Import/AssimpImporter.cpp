#include "AssimpImporter.h"

#include "../Utils/NumericCast.h"
#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

// #define BADGER_ENGINE_ASSIMP_LOGS

namespace BadgerEngine::Import {

namespace {

const auto EmptyMaterial = std::make_shared<Material>();

const char* textureTypeToString(aiTextureType type) noexcept
{
    switch (type) {
    case aiTextureType_NONE:
        return "n/a";
    case aiTextureType_DIFFUSE:
        return "Diffuse";
    case aiTextureType_SPECULAR:
        return "Specular";
    case aiTextureType_AMBIENT:
        return "Ambient";
    case aiTextureType_EMISSIVE:
        return "Emissive";
    case aiTextureType_HEIGHT:
        return "Height";
    case aiTextureType_NORMALS:
        return "Normals";
    case aiTextureType_SHININESS:
        return "Shininess";
    case aiTextureType_OPACITY:
        return "Opacity";
    case aiTextureType_DISPLACEMENT:
        return "Displacement";
    case aiTextureType_LIGHTMAP:
        return "Lightmap";
    case aiTextureType_REFLECTION:
        return "Reflection";
    case aiTextureType_BASE_COLOR:
        return "BaseColor";
    case aiTextureType_NORMAL_CAMERA:
        return "NormalCamera";
    case aiTextureType_EMISSION_COLOR:
        return "EmissionColor";
    case aiTextureType_METALNESS:
        return "Metalness";
    case aiTextureType_DIFFUSE_ROUGHNESS:
        return "DiffuseRoughness";
    case aiTextureType_AMBIENT_OCCLUSION:
        return "AmbientOcclusion";
    case aiTextureType_UNKNOWN:
        return "Sheen";
    case aiTextureType_SHEEN:
        return "Sheen";
    case aiTextureType_CLEARCOAT:
        return "Clearcoat";
    case aiTextureType_TRANSMISSION:
        return "Transmission";
    case aiTextureType_MAYA_BASE:
        return "MayaBase";
    case aiTextureType_MAYA_SPECULAR:
        return "MayaSpecular";
    case aiTextureType_MAYA_SPECULAR_COLOR:
        return "MayaSpecularColor";
    case aiTextureType_MAYA_SPECULAR_ROUGHNESS:
        return "MayaSpecularRoughness";
    case aiTextureType_ANISOTROPY:
        return "Anisotropy";
    case aiTextureType_GLTF_METALLIC_ROUGHNESS:
        return "GLTFMetallicRoughness";
    case _aiTextureType_Force32Bit:
        return "_aiTextureType_Force32Bit";
    }
    std::unreachable();
}

[[maybe_unused]] const char* propertyTypeToString(aiPropertyTypeInfo type) noexcept
{
    switch (type) {
    case aiPTI_Float:
        return "Float";
    case aiPTI_Double:
        return "Double";
    case aiPTI_String:
        return "String";
    case aiPTI_Integer:
        return "Integer";
    case aiPTI_Buffer:
        return "Buffer";
    case _aiPTI_Force32Bit:
        return "_aiPTI_Force32Bit";
    };
    std::unreachable();
}

const char* aiReturnToString(aiReturn type) noexcept
{
    switch (type) {
    case aiReturn_SUCCESS:
        return "Success";
    case aiReturn_FAILURE:
        return "Failure";
    case aiReturn_OUTOFMEMORY:
        return "OutOfMemory";
    case _AI_ENFORCE_ENUM_SIZE:
        return "_AI_ENFORCE_ENUM_SIZE";
    };
    std::unreachable();
}

std::string floatPropertyValueToString(const aiMaterial& material, const aiMaterialProperty& property) noexcept
{
    unsigned int size = 16;
    std::vector<float> data(size, 0.f);
    const auto result = aiGetMaterialFloatArray(&material, property.mKey.C_Str(), property.mSemantic, property.mIndex, data.data(), &size);
    if (result != aiReturn_SUCCESS) {
        std::cerr << "aiGetMaterialFloatArray failed: " << aiReturnToString(result) << std::endl;
        std::abort();
    }
    data.resize(size);

    if (size == 0) {
        return "[]";
    }

    std::ostringstream ss;

    if (size == 1) {
        ss << data[0];
        return ss.str();
    }

    ss << "[ ";
    for (std::size_t i = 0; const auto x : std::move(data)) {
        ss << x;
        if (i++ < size - 1) {
            ss << ", ";
        }
    }
    ss << " ]";

    return std::move(ss).str();
}

std::string stringPropertyValueToString(const aiMaterial& material, const aiMaterialProperty& property) noexcept
{
    aiString string;
    const auto result = aiGetMaterialString(&material, property.mKey.C_Str(), property.mSemantic, property.mIndex, &string);
    if (result != aiReturn_SUCCESS) {
        std::cerr << "aiGetMaterialFloatArray failed: " << aiReturnToString(result) << std::endl;
        std::abort();
    }
    return string.C_Str();
}

std::string bufferPropertyValueToString(const aiMaterialProperty& property) noexcept
{
    std::ostringstream ss;

    for (std::size_t i = 0; i < property.mDataLength; ++i) {
        ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<std::uint16_t>(property.mData[i]);
        if (i < property.mDataLength - 1) {
            ss << ".";
        }
    }

    return ss.str();
}

[[maybe_unused]] std::string propertyValueToString(const aiMaterial& material, const aiMaterialProperty& property) noexcept
{
    switch (property.mType) {
    case aiPTI_Float:
        return floatPropertyValueToString(material, property);
    case aiPTI_Double:
        return floatPropertyValueToString(material, property);
    case aiPTI_String:
        return stringPropertyValueToString(material, property);
    case aiPTI_Integer:
        return floatPropertyValueToString(material, property);
    case aiPTI_Buffer:
        return bufferPropertyValueToString(property);
    case _aiPTI_Force32Bit:
        break;
    }
    std::unreachable();
}

Expected<std::optional<Color>> materialColorProperty(const aiMaterial& material, const char* key, unsigned int type, unsigned int idx) noexcept
{
    aiColor4D c;
    const auto result = material.Get(key, type, idx, c);
    switch (result) {
    case aiReturn_SUCCESS:
        return glm::vec4 { c.r, c.g, c.b, c.a };
    case aiReturn_FAILURE:
        return std::nullopt;
    case aiReturn_OUTOFMEMORY:
        return unexpected("Out of memory");
    case _AI_ENFORCE_ENUM_SIZE:
        break;
    }
    std::unreachable();
}

Expected<std::optional<float>> materialFloatProperty(const aiMaterial& material, const char* key, unsigned int type, unsigned int idx) noexcept
{
    float c;
    const auto result = material.Get(key, type, idx, c);
    switch (result) {
    case aiReturn_SUCCESS:
        return c;
    case aiReturn_FAILURE:
        return std::nullopt;
    case aiReturn_OUTOFMEMORY:
        return unexpected("Out of memory");
    case _AI_ENFORCE_ENUM_SIZE:
        break;
    }
    std::unreachable();
}

Expected<std::optional<aiUVTransform>> materialUVTransformProperty(const aiMaterial& material, const char* key, unsigned int type, unsigned int idx) noexcept
{
    aiUVTransform tr;
    const auto result = aiGetMaterialUVTransform(&material, key, type, idx, &tr);
    switch (result) {
    case aiReturn_SUCCESS:
        return tr;
    case aiReturn_FAILURE:
        return std::nullopt;
    case aiReturn_OUTOFMEMORY:
        return unexpected("Out of memory");
    case _AI_ENFORCE_ENUM_SIZE:
        break;
    }
    std::unreachable();
}

[[nodiscard]] Expected<SharedTexture> loadEmbeddedTexture(const aiTexture* embeddedTexture) noexcept
{
    (void)embeddedTexture;
    return unexpected("TODO");
}

[[nodiscard]] Expected<std::vector<SharedTexture>> loadMaterialTextures(
    const TextureLoader& textureLoader,
    RawPtr<aiMaterial> material,
    aiTextureType type,
    RawPtr<const aiScene> scene,
    const std::optional<std::filesystem::path>& sceneSourcePath) noexcept
{
#ifdef BADGER_ENGINE_ASSIMP_LOGS
    if (material->GetTextureCount(type) != 0) {
        if (sceneSourcePath) {
            std::cout << "loadMaterialTextures (" << textureTypeToString(type) << ") `" << "` required by `" << sceneSourcePath->string() << ": " << material->GetTextureCount(type) << std::endl;
        } else {
            std::cout << "loadMaterialTextures (" << textureTypeToString(type) << "): " << material->GetTextureCount(type) << std::endl;
        }
    }
#endif

    std::vector<SharedTexture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
        aiString texturePath;
        material->GetTexture(type, i, &texturePath);

#ifdef BADGER_ENGINE_ASSIMP_LOGS
        std::cout << "\ttexture: " << texturePath.C_Str() << std::endl;
#endif

        if (const auto texture = textureLoader.texture(texturePath.C_Str())) {
            textures.push_back(*texture);
            continue;
        }

        if (const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
            const auto texture = loadEmbeddedTexture(embeddedTexture);
            if (!texture) {
                return unexpected("Failed to load an embedded texture", texture.error());
            }

            textures.push_back(*texture);
            continue;
        }

        if (sceneSourcePath) {
            std::cerr << "Texture (" << textureTypeToString(type) << ") `" << texturePath.C_Str() << "` required by `" << sceneSourcePath->string() << "` not found nither in texture loader nor in embedded textures." << std::endl;
        } else {
            std::cerr << "Texture (" << textureTypeToString(type) << ") `" << texturePath.C_Str() << "` not found nither in texture loader nor in embedded textures." << std::endl;
        }
    }
    return textures;
}

[[nodiscard]] Expected<SharedMaterial> loadMaterial(
    const TextureLoader& textureLoader,
    RawPtr<aiMaterial> material,
    RawPtr<const aiScene> scene,
    const std::optional<std::filesystem::path>& sceneSourcePath)
{
    const auto baseColor = materialColorProperty(*material, AI_MATKEY_BASE_COLOR);
    if (!baseColor) {
        return unexpected("Failed to get material base color", baseColor.error());
    }

    const auto diffuseColor = materialColorProperty(*material, AI_MATKEY_COLOR_DIFFUSE);
    if (!diffuseColor) {
        return unexpected("Failed to get material diffuse color", diffuseColor.error());
    }

    const auto emissiveColor = materialColorProperty(*material, AI_MATKEY_COLOR_EMISSIVE);
    if (!emissiveColor) {
        return unexpected("Failed to get material emissive color", emissiveColor.error());
    }

    const auto specularColor = materialColorProperty(*material, AI_MATKEY_COLOR_SPECULAR);
    if (!specularColor) {
        return unexpected("Failed to get material specular color", specularColor.error());
    }

    const auto metallness = materialFloatProperty(*material, AI_MATKEY_METALLIC_FACTOR);
    if (!metallness) {
        return unexpected("Failed to get material metallness", metallness.error());
    }

    const auto roughness = materialFloatProperty(*material, AI_MATKEY_ROUGHNESS_FACTOR);
    if (!roughness) {
        return unexpected("Failed to get material roughness", roughness.error());
    }

    const auto shininess = materialFloatProperty(*material, AI_MATKEY_SHININESS);
    if (!shininess) {
        return unexpected("Failed to get material shininess", shininess.error());
    }

    const auto specularFactor = materialFloatProperty(*material, AI_MATKEY_SPECULAR_FACTOR);
    if (!specularFactor) {
        return unexpected("Failed to get material specularFactor", specularFactor.error());
    }

    const auto diffuseMaps = loadMaterialTextures(textureLoader, material, aiTextureType_DIFFUSE, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load diffuse maps", diffuseMaps.error());
    }

    const auto specularMaps = loadMaterialTextures(textureLoader, material, aiTextureType_SPECULAR, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_SPECULAR maps", diffuseMaps.error());
    }

    const auto ambientMaps = loadMaterialTextures(textureLoader, material, aiTextureType_AMBIENT, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_AMBIENT maps", diffuseMaps.error());
    }

    const auto emissiveMaps = loadMaterialTextures(textureLoader, material, aiTextureType_EMISSIVE, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_EMISSIVE maps", diffuseMaps.error());
    }

    const auto heightMaps = loadMaterialTextures(textureLoader, material, aiTextureType_HEIGHT, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_HEIGHT maps", diffuseMaps.error());
    }

    const auto normalsMaps = loadMaterialTextures(textureLoader, material, aiTextureType_NORMALS, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_NORMALS maps", diffuseMaps.error());
    }

    const auto normalsMapsUVTransform = materialUVTransformProperty(*material, AI_MATKEY_UVTRANSFORM(aiTextureType_NORMALS, 0));
    if (!normalsMapsUVTransform) {
        return unexpected("Failde to load aiTextureType_NORMALS maps", diffuseMaps.error());
    }

#ifdef BADGER_ENGINE_ASSIMP_LOGS
    std::cout << "normalsMapsUVTransform: " << normalsMapsUVTransform.value().has_value() << std::endl;
#endif

    const auto shininessMaps = loadMaterialTextures(textureLoader, material, aiTextureType_SHININESS, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_SHININESS maps", diffuseMaps.error());
    }

    const auto opacityMaps = loadMaterialTextures(textureLoader, material, aiTextureType_OPACITY, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_OPACITY maps", diffuseMaps.error());
    }

    const auto displacementMaps = loadMaterialTextures(textureLoader, material, aiTextureType_DISPLACEMENT, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_DISPLACEMENT maps", diffuseMaps.error());
    }

    const auto lightmapMaps = loadMaterialTextures(textureLoader, material, aiTextureType_LIGHTMAP, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_LIGHTMAP maps", diffuseMaps.error());
    }

    const auto reflectionMaps = loadMaterialTextures(textureLoader, material, aiTextureType_REFLECTION, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_REFLECTION maps", diffuseMaps.error());
    }

    const auto baseColorMaps = loadMaterialTextures(textureLoader, material, aiTextureType_BASE_COLOR, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_BASE_COLOR maps", diffuseMaps.error());
    }

    const auto normalCameraMaps = loadMaterialTextures(textureLoader, material, aiTextureType_NORMAL_CAMERA, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_NORMAL_CAMERA maps", diffuseMaps.error());
    }

    const auto emissionColorMaps = loadMaterialTextures(textureLoader, material, aiTextureType_EMISSION_COLOR, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_EMISSION_COLOR maps", diffuseMaps.error());
    }

    const auto metalnessMaps = loadMaterialTextures(textureLoader, material, aiTextureType_METALNESS, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_METALNESS maps", diffuseMaps.error());
    }

    const auto diffuseRoughnessMaps = loadMaterialTextures(textureLoader, material, aiTextureType_DIFFUSE_ROUGHNESS, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_DIFFUSE_ROUGHNESS maps", diffuseMaps.error());
    }

    const auto ambientOcclusionMaps = loadMaterialTextures(textureLoader, material, aiTextureType_AMBIENT_OCCLUSION, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_AMBIENT_OCCLUSION maps", diffuseMaps.error());
    }

    const auto unknownTextures = loadMaterialTextures(textureLoader, material, aiTextureType_UNKNOWN, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_UNKNOWN maps", diffuseMaps.error());
    }

    const auto sheenMaps = loadMaterialTextures(textureLoader, material, aiTextureType_SHEEN, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_SHEEN maps", diffuseMaps.error());
    }

    const auto clearcoatMaps = loadMaterialTextures(textureLoader, material, aiTextureType_CLEARCOAT, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_CLEARCOAT maps", diffuseMaps.error());
    }

    const auto transmissionMaps = loadMaterialTextures(textureLoader, material, aiTextureType_TRANSMISSION, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_TRANSMISSION maps", diffuseMaps.error());
    }

    const auto mayaBaseMaps = loadMaterialTextures(textureLoader, material, aiTextureType_MAYA_BASE, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_MAYA_BASE maps", diffuseMaps.error());
    }

    const auto mayaSpecularMaps = loadMaterialTextures(textureLoader, material, aiTextureType_MAYA_SPECULAR, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_MAYA_SPECULAR maps", diffuseMaps.error());
    }

    const auto mayaSpecularColorMaps = loadMaterialTextures(textureLoader, material, aiTextureType_MAYA_SPECULAR_COLOR, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_MAYA_SPECULAR_COLOR maps", diffuseMaps.error());
    }

    const auto mayaSpecularRoughnessMaps = loadMaterialTextures(textureLoader, material, aiTextureType_MAYA_SPECULAR_ROUGHNESS, scene, sceneSourcePath);
    if (!diffuseMaps) {
        return unexpected("Failde to load aiTextureType_MAYA_SPECULAR_ROUGHNESS maps", diffuseMaps.error());
    }

#ifdef BADGER_ENGINE_ASSIMP_LOGS
    std::cout << "Material `" << material->GetName().C_Str() << "` properties:" << std::endl;
    for (std::size_t i = 0; i < material->mNumProperties; ++i) {
        const auto property = material->mProperties[i];
        assert(property);

        std::cout << "\tkey: " << property->mKey.C_Str() << ": " << propertyTypeToString(property->mType) << " = " << propertyValueToString(*material, *property) << std::endl;
    }
#endif

    return std::make_shared<Material>(Material {
        .name = material->GetName().C_Str(),
        .baseColor = *std::move(baseColor),
        .diffuseColor = *std::move(diffuseColor),
        .emissiveColor = *std::move(emissiveColor),
        .specularColor = *std::move(specularColor),
        .metallness = *std::move(metallness),
        .roughness = *std::move(roughness),
        .shininess = *std::move(shininess),
        .specularFactor = *std::move(specularFactor),
        .diffuseMaps = *std::move(diffuseMaps),
        .specularMaps = *std::move(specularMaps),
        .ambientMaps = *std::move(ambientMaps),
        .emissiveMaps = *std::move(emissiveMaps),
        .heightMaps = *std::move(heightMaps),
        .normalsMaps = *std::move(normalsMaps),
        .shininessMaps = *std::move(shininessMaps),
        .opacityMaps = *std::move(opacityMaps),
        .displacementMaps = *std::move(displacementMaps),
        .lightmapMaps = *std::move(lightmapMaps),
        .reflectionMaps = *std::move(reflectionMaps),
        .baseColorMaps = *std::move(baseColorMaps),
        .normalCameraMaps = *std::move(normalCameraMaps),
        .emissionColorMaps = *std::move(emissionColorMaps),
        .metalnessMaps = *std::move(metalnessMaps),
        .diffuseRoughnessMaps = *std::move(diffuseRoughnessMaps),
        .ambientOcclusionMaps = *std::move(ambientOcclusionMaps),
        .unknownTextures = *std::move(unknownTextures),
        .sheenMaps = *std::move(sheenMaps),
        .clearcoatMaps = *std::move(clearcoatMaps),
        .transmissionMaps = *std::move(transmissionMaps),
        .mayaBaseMaps = *std::move(mayaBaseMaps),
        .mayaSpecularMaps = *std::move(mayaSpecularMaps),
        .mayaSpecularColorMaps = *std::move(mayaSpecularColorMaps),
        .mayaSpecularRoughnessMaps = *std::move(mayaSpecularRoughnessMaps),
    });
}

Expected<std::vector<SharedMaterial>> loadMaterials(
    const TextureLoader& textureLoader,
    RawPtr<const aiScene> scene,
    const std::optional<std::filesystem::path>& sceneSourcePath)
{
    std::vector<SharedMaterial> result;
    result.reserve(numericCast<std::size_t>(scene->mNumMaterials).value());
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        const auto material = loadMaterial(textureLoader, scene->mMaterials[i], scene, sceneSourcePath);
        if (!material) {
            return unexpected("Failed to load scene material", material.error());
        }

        result.push_back(*material);
    }
    return result;
}

[[nodiscard]] bool isVertexValid(const Geometry::Vertex& vertex) noexcept
{
    return !std::isnan(vertex.position.x)
        && !std::isnan(vertex.position.y)
        && !std::isnan(vertex.position.z)
        && !std::isnan(vertex.normal.x)
        && !std::isnan(vertex.normal.y)
        && !std::isnan(vertex.normal.z)
        && !std::isnan(vertex.tangent.x)
        && !std::isnan(vertex.tangent.y)
        && !std::isnan(vertex.tangent.z)
        && !std::isnan(vertex.bitangent.x)
        && !std::isnan(vertex.bitangent.y)
        && !std::isnan(vertex.bitangent.z)
        && !std::isnan(vertex.color.x)
        && !std::isnan(vertex.color.y)
        && !std::isnan(vertex.color.z)
        && !std::isnan(vertex.uv.x)
        && !std::isnan(vertex.uv.y);
}

glm::vec3 glmVec3FromAiVector3D(const aiVector3D& vec) noexcept
{
    return { vec.x, vec.y, vec.z };
}

std::string aiVector3DToString(const aiVector3D& vec) noexcept
{
    std::ostringstream ss;
    ss << "[ " << vec.x << ", " << vec.y << ", " << vec.z << " ]";
    return ss.str();
}

[[nodiscard]] Expected<Shared<Mesh>> processMesh(const std::vector<SharedMaterial>& materials, RawPtr<aiMesh> mesh, glm::mat4 transformation) noexcept
{
    std::vector<Geometry::Vertex> vertices;
    std::vector<Geometry::Mesh::Index> indices;
    SharedMaterial material = EmptyMaterial;

    assert(mesh->mVertices);

    // std::cout << mesh->mName.C_Str() << " -> mTangents: " << mesh->mTangents << ", mBitangents: " << mesh->mBitangents << std::endl;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Geometry::Vertex vertex;

        vertex.position = transformation * glm::vec4(glmVec3FromAiVector3D(mesh->mVertices[i]), 1.);

        if (mesh->mNormals) {
            vertex.normal = transformation * glm::vec4(glmVec3FromAiVector3D(mesh->mNormals[i]), 0.);
        }

        if (mesh->mTangents) {
            vertex.tangent = transformation * glm::vec4(glmVec3FromAiVector3D(mesh->mTangents[i]), 0.);
        }

        if (mesh->mBitangents) {
            vertex.bitangent = transformation * glm::vec4(glmVec3FromAiVector3D(mesh->mBitangents[i]), 0.);
        }

        if (mesh->mNumBones > 0) {
            std::cerr << "Assimp mesh `" << mesh->mName.C_Str() << "` has bones which are not soupported." << std::endl;
        }

        if (mesh->mColors[0]) {
            vertex.color.r = (float)mesh->mColors[0][i].r;
            vertex.color.g = (float)mesh->mColors[0][i].g;
            vertex.color.b = (float)mesh->mColors[0][i].b;
            // vertex.color.a = (float)mesh->mColors[0][i].a;
        }

        if (mesh->HasTextureCoords(0)) {
            vertex.uv = glmVec3FromAiVector3D(mesh->mTextureCoords[0][i]);

            if (mesh->mName.C_Str() == std::string("Main Body")) {
                // std::cout << "uv: " << vertex.uv.x << ", " << vertex.uv.y << std::endl;
            }
        }

        // if (mesh->HasTextureCoords(1)) {
        //     vertex.uv = transformation * glm::vec4(glmVec3FromAiVector3D(mesh->mTextureCoords[1][i]), 0.);
        // }

        if (!isVertexValid(vertex)) {
            return unexpected("Vertex " + std::to_string(i) + " has NANs in it");
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

#ifdef BADGER_ENGINE_ASSIMP_LOGS
    std::cout << "Mesh `" << mesh->mName.C_Str() << "` -> material index: " << mesh->mMaterialIndex << std::endl;
#endif
    if (std::cmp_greater_equal(mesh->mMaterialIndex, 0) && std::cmp_less(mesh->mMaterialIndex, materials.size())) {
        material = materials[mesh->mMaterialIndex];
    }

    return Mesh::create(
        mesh->mName.C_Str(),
        Geometry::Mesh::create(Geometry::Topology::TriangleList, std::move(vertices), std::move(indices)),
        std::move(material));
}

[[nodiscard]] glm::mat4 glmMat4FromAiMatrix4x4(const aiMatrix4x4& m) noexcept
{
    return glm::transpose(glm::mat4 {
        m.a1, m.a2, m.a3, m.a4,
        m.b1, m.b2, m.b3, m.b4,
        m.c1, m.c2, m.c3, m.c4,
        m.d1, m.d2, m.d3, m.d4 });
}

[[maybe_unused]] std::string metadataTypeTpString(aiMetadataType tp) noexcept
{
    switch (tp) {
    case AI_BOOL:
        return "bool";
    case AI_INT32:
        return "int32";
    case AI_UINT64:
        return "uint64";
    case AI_FLOAT:
        return "float";
    case AI_DOUBLE:
        return "double";
    case AI_AISTRING:
        return "string";
    case AI_AIVECTOR3D:
        return "vec3";
    case AI_AIMETADATA:
        return "metadata";
    case AI_INT64:
        return "int64";
    case AI_UINT32:
        return "uint32";
    case AI_META_MAX:
        return "AI_META_MAX";
    case FORCE_32BIT:
        return "FORCE_32BIT";
    }
    std::unreachable();
}

template<typename T>
T metadataEntryValue(const aiMetadataEntry& entry)
{
    assert(GetAiType(T()) != entry.mType);
    return *static_cast<T*>(entry.mData);
};

[[maybe_unused]] std::string metadataValueToString(const aiMetadataEntry& entry) noexcept
{
    std::ostringstream ss;
    switch (entry.mType) {
    case AI_BOOL:
        ss << metadataEntryValue<bool>(entry);
        return ss.str();
    case AI_INT32:
        ss << metadataEntryValue<std::int32_t>(entry);
        return ss.str();
    case AI_UINT64:
        ss << metadataEntryValue<std::uint64_t>(entry);
        return ss.str();
    case AI_FLOAT:
        ss << metadataEntryValue<float>(entry);
        return ss.str();
    case AI_DOUBLE:
        ss << metadataEntryValue<double>(entry);
        return ss.str();
    case AI_AISTRING:
        return metadataEntryValue<aiString>(entry).C_Str();
    case AI_AIVECTOR3D:
        return aiVector3DToString(metadataEntryValue<aiVector3D>(entry));
    case AI_AIMETADATA:
        return "AIMETADATA";
    case AI_INT64:
        ss << metadataEntryValue<std::int64_t>(entry);
        return ss.str();
    case AI_UINT32:
        ss << metadataEntryValue<std::uint32_t>(entry);
        return ss.str();
    case AI_META_MAX:
        return "AI_META_MAX";
    case FORCE_32BIT:
        return "FORCE_32BIT";
    }
    std::unreachable();
}

[[nodiscard]] Expected<void> processNode(
    std::vector<Shared<Mesh>>& meshes,
    const std::vector<SharedMaterial>& materials,
    RawPtr<aiNode> node,
    RawPtr<const aiScene> scene,
    glm::mat4 transformation)
{
#ifdef BADGER_ENGINE_ASSIMP_LOGS
    std::cout << "Entering node: " << node->mName.C_Str() << std::endl;

    if (node->mMetaData) {
        std::cout << "The node metadata has " << node->mMetaData->mNumProperties << " properties:" << std::endl;
        for (unsigned int i = 0; i < node->mMetaData->mNumProperties; ++i) {
            std::cout << "\t" << node->mMetaData->mKeys[i].C_Str() << ": " << metadataTypeTpString(node->mMetaData->mValues[i].mType) << " = " << metadataValueToString(node->mMetaData->mValues[i]) << std::endl;
        }
    } else {
        std::cout << "The node has no metadata." << std::endl;
    }
#endif

    transformation *= glmMat4FromAiMatrix4x4(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        const auto processedMesh = processMesh(materials, mesh, transformation);
        if (!processedMesh) {
            return unexpected("Failed to parse mesh", processedMesh.error());
        }

        meshes.push_back(*processedMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        const auto result = processNode(meshes, materials, node->mChildren[i], scene, transformation);
        if (!result) {
            return result;
        }
    }

    return {};
}

constexpr unsigned int ImportFlags
    = aiProcess_Triangulate
    | aiProcess_TransformUVCoords
    | aiProcess_ConvertToLeftHanded;
}

Expected<Model> AssimpImporter::load(const TextureLoader& textureLoader, const std::filesystem::path& path) const noexcept
{
    if (!std::filesystem::exists(path)) {
        return unexpected("File `" + path.string() + "` does not exist.");
    }

    ::Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.string().c_str(), ImportFlags);

    if (scene == nullptr) {
        return unexpected("Failed to read the file `" + path.string() + "`: " + importer.GetErrorString());
    }

    const auto materials = loadMaterials(textureLoader, scene, path);
    if (!materials) {
        return unexpected("Failed to load scene materials", materials.error());
    }

#ifdef BADGER_ENGINE_ASSIMP_LOGS
    {
        std::cout << "Embedded textures of `" << path << "`" << std::endl;
        for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
            const auto t = scene->mTextures[i];
            std::cout << "t " << i << ": " << t->mFilename.C_Str() << std::endl;
        }

        std::cout << "Materials of `" << path << "`" << std::endl;
        for (std::size_t i = 0; const auto& m : *materials) {
            std::cout << "Material " << i++ << ": " << *m << std::endl;
        }
    }
#endif

    std::vector<Shared<Mesh>> meshes;
    const auto result = processNode(meshes, *materials, scene->mRootNode, scene, glm::mat4(1.));
    if (!result) {
        return unexpected(result.error());
    }

    importer.FreeScene();
    return Model(std::move(meshes));
}

Expected<Model> AssimpImporter::parse(
    const TextureLoader& textureLoader,
    std::span<const std::uint8_t> data,
    const std::map<std::string, std::span<const std::uint8_t>>& additionalData,
    const std::string& hint) const noexcept
{
    (void)additionalData;

    ::Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(data.data(), data.size(), ImportFlags, hint.c_str());

    if (scene == nullptr) {
        return unexpected("Failed to read the data");
    }

    const auto materials = loadMaterials(textureLoader, scene, std::nullopt);
    if (!materials) {
        return unexpected("Failed to load scene materials", materials.error());
    }

    std::vector<Shared<Mesh>> meshes;
    const auto result = processNode(meshes, *materials, scene->mRootNode, scene, glm::mat4(1.));
    if (!result) {
        return unexpected(result.error());
    }

    return Model(meshes);
}

}
