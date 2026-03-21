#include "TinyGLTFImporter.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "../Utils/NumericCast.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <tiny_gltf.h>

// #define BADGER_ENGINE_TINY_GLTF_LOGS

namespace BadgerEngine::Import {

namespace {

const auto EmptyMaterial = std::make_shared<Material>();

[[nodiscard, maybe_unused]] std::size_t sz(int mode) noexcept
{
    return numericCast<std::size_t>(mode).value();
}

[[nodiscard, maybe_unused]] std::string modeToString(int mode) noexcept
{
    if (mode == TINYGLTF_MODE_POINTS) {
        return "POINTS";
    } else if (mode == TINYGLTF_MODE_LINE) {
        return "LINE";
    } else if (mode == TINYGLTF_MODE_LINE_LOOP) {
        return "LINE_LOOP";
    } else if (mode == TINYGLTF_MODE_TRIANGLES) {
        return "TRIANGLES";
    } else if (mode == TINYGLTF_MODE_TRIANGLE_FAN) {
        return "TRIANGLE_FAN";
    } else if (mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
        return "TRIANGLE_STRIP";
    }
    return "**UNKNOWN**";
}

[[nodiscard, maybe_unused]] std::string targetToString(int target) noexcept
{
    if (target == 34962) {
        return "GL_ARRAY_BUFFER";
    } else if (target == 34963) {
        return "GL_ELEMENT_ARRAY_BUFFER";
    } else {
        return "**UNKNOWN**";
    }
}

[[nodiscard]] std::string typeToString(int ty) noexcept
{
    if (ty == TINYGLTF_TYPE_SCALAR) {
        return "SCALAR";
    } else if (ty == TINYGLTF_TYPE_VECTOR) {
        return "VECTOR";
    } else if (ty == TINYGLTF_TYPE_VEC2) {
        return "VEC2";
    } else if (ty == TINYGLTF_TYPE_VEC3) {
        return "VEC3";
    } else if (ty == TINYGLTF_TYPE_VEC4) {
        return "VEC4";
    } else if (ty == TINYGLTF_TYPE_MATRIX) {
        return "MATRIX";
    } else if (ty == TINYGLTF_TYPE_MAT2) {
        return "MAT2";
    } else if (ty == TINYGLTF_TYPE_MAT3) {
        return "MAT3";
    } else if (ty == TINYGLTF_TYPE_MAT4) {
        return "MAT4";
    }
    return "**UNKNOWN**";
}

[[nodiscard, maybe_unused]] std::string componentTypeToString(int ty) noexcept
{
    if (ty == TINYGLTF_COMPONENT_TYPE_BYTE) {
        return "BYTE";
    } else if (ty == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
        return "UNSIGNED_BYTE";
    } else if (ty == TINYGLTF_COMPONENT_TYPE_SHORT) {
        return "SHORT";
    } else if (ty == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        return "UNSIGNED_SHORT";
    } else if (ty == TINYGLTF_COMPONENT_TYPE_INT) {
        return "INT";
    } else if (ty == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
        return "UNSIGNED_INT";
    } else if (ty == TINYGLTF_COMPONENT_TYPE_FLOAT) {
        return "FLOAT";
    } else if (ty == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
        return "DOUBLE";
    }

    return "**UNKNOWN**";
}

[[nodiscard, maybe_unused]] std::string parameterTypeToString(int ty) noexcept
{
    if (ty == TINYGLTF_PARAMETER_TYPE_BYTE) {
        return "BYTE";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) {
        return "UNSIGNED_BYTE";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_SHORT) {
        return "SHORT";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
        return "UNSIGNED_SHORT";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_INT) {
        return "INT";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
        return "UNSIGNED_INT";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_FLOAT) {
        return "FLOAT";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC2) {
        return "FLOAT_VEC2";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3) {
        return "FLOAT_VEC3";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC4) {
        return "FLOAT_VEC4";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_INT_VEC2) {
        return "INT_VEC2";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_INT_VEC3) {
        return "INT_VEC3";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_INT_VEC4) {
        return "INT_VEC4";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_BOOL) {
        return "BOOL";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_BOOL_VEC2) {
        return "BOOL_VEC2";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_BOOL_VEC3) {
        return "BOOL_VEC3";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_BOOL_VEC4) {
        return "BOOL_VEC4";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_FLOAT_MAT2) {
        return "FLOAT_MAT2";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_FLOAT_MAT3) {
        return "FLOAT_MAT3";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_FLOAT_MAT4) {
        return "FLOAT_MAT4";
    } else if (ty == TINYGLTF_PARAMETER_TYPE_SAMPLER_2D) {
        return "SAMPLER_2D";
    }

    return "**UNKNOWN**";
}

[[nodiscard, maybe_unused]] std::string wrapModeToString(int mode) noexcept
{
    if (mode == TINYGLTF_TEXTURE_WRAP_REPEAT) {
        return "REPEAT";
    } else if (mode == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE) {
        return "CLAMP_TO_EDGE";
    } else if (mode == TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT) {
        return "MIRRORED_REPEAT";
    }

    return "**UNKNOWN**";
}

[[nodiscard, maybe_unused]] std::string filterModeToString(int mode) noexcept
{
    if (mode == TINYGLTF_TEXTURE_FILTER_NEAREST) {
        return "NEAREST";
    } else if (mode == TINYGLTF_TEXTURE_FILTER_LINEAR) {
        return "LINEAR";
    } else if (mode == TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST) {
        return "NEAREST_MIPMAP_NEAREST";
    } else if (mode == TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR) {
        return "NEAREST_MIPMAP_LINEAR";
    } else if (mode == TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST) {
        return "LINEAR_MIPMAP_NEAREST";
    } else if (mode == TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR) {
        return "LINEAR_MIPMAP_LINEAR";
    }
    return "**UNKNOWN**";
}

[[nodiscard, maybe_unused]] std::string intArrayToString(const std::vector<int>& arr) noexcept
{
    if (arr.size() == 0) {
        return "";
    }

    std::stringstream ss;
    ss << "[ ";
    for (size_t i = 0; i < arr.size(); i++) {
        ss << arr[i];
        if (i != arr.size() - 1) {
            ss << ", ";
        }
    }
    ss << " ]";

    return ss.str();
}

[[nodiscard, maybe_unused]] std::string floatArrayToString(const std::vector<double>& arr) noexcept
{
    if (arr.size() == 0) {
        return "";
    }

    std::stringstream ss;
    ss << "[ ";
    for (size_t i = 0; i < arr.size(); i++) {
        ss << arr[i];
        if (i != arr.size() - 1) {
            ss << ", ";
        }
    }
    ss << " ]";

    return ss.str();
}

[[nodiscard]] std::string indentToString(const int indent)
{
    std::string s;
    for (int i = 0; i < indent; i++) {
        s += "  ";
    }

    return s;
}

[[nodiscard, maybe_unused]] std::string valueToString(
    const std::string& name,
    const tinygltf::Value& value, const int indent,
    const bool tag = true) noexcept
{
    std::stringstream ss;

    if (value.IsObject()) {
        const tinygltf::Value::Object& o = value.Get<tinygltf::Value::Object>();
        tinygltf::Value::Object::const_iterator it(o.begin());
        tinygltf::Value::Object::const_iterator itEnd(o.end());
        for (; it != itEnd; it++) {
            ss << valueToString(it->first, it->second, indent + 1) << std::endl;
        }
    } else if (value.IsString()) {
        if (tag) {
            ss << indentToString(indent) << name << " : " << value.Get<std::string>();
        } else {
            ss << indentToString(indent) << value.Get<std::string>() << " ";
        }
    } else if (value.IsBool()) {
        if (tag) {
            ss << indentToString(indent) << name << " : " << value.Get<bool>();
        } else {
            ss << indentToString(indent) << value.Get<bool>() << " ";
        }
    } else if (value.IsNumber()) {
        if (tag) {
            ss << indentToString(indent) << name << " : " << value.Get<double>();
        } else {
            ss << indentToString(indent) << value.Get<double>() << " ";
        }
    } else if (value.IsInt()) {
        if (tag) {
            ss << indentToString(indent) << name << " : " << value.Get<int>();
        } else {
            ss << indentToString(indent) << value.Get<int>() << " ";
        }
    } else if (value.IsArray()) {
        // TODO(syoyo): Better pretty printing of array item
        ss << indentToString(indent) << name << " [ \n";
        for (std::size_t i = 0; i < value.Size(); i++) {
            ss << valueToString("", value.Get(i), indent + 1, /* tag */ false);
            if (i != (value.ArrayLen() - 1)) {
                ss << ", \n";
            }
        }
        ss << "\n"
           << indentToString(indent) << "] ";
    }

    // @todo { binary }

    return ss.str();
}

[[nodiscard]] Expected<void> parseIndexBuffer(std::vector<Geometry::Mesh::Index>& indices, const tinygltf::Model& model, const tinygltf::Accessor& accessor) noexcept
{
    if (indices.size() < accessor.count) {
        indices.resize(accessor.count);
    }

    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        return unexpected("Unsupported component type `" + componentTypeToString(accessor.componentType) + "`");
    }

    if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        return unexpected("Unsupported type `" + typeToString(accessor.type) + "`");
    }

    const std::span<const std::uint8_t> bs = std::span(
        buffer.data.data() + bufferView.byteOffset,
        bufferView.byteLength);

    std::size_t i = 0;
    for (const std::uint8_t* elem = bs.data(); elem < bs.data() + bs.size(); elem += byteStride) {
        indices[i] = *reinterpret_cast<const std::uint16_t*>(elem);
        ++i;
    }

    return {};
}

using ParseAttributeBufferFunction = Expected<void> (*)(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor, const glm::mat4& transformation);

[[nodiscard]] Expected<void> parsePositionAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor, const glm::mat4& transformation) noexcept
{
    if (vertices.size() < accessor.count) {
        vertices.resize(accessor.count);
    }

    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        return unexpected("Unsupported component type `" + componentTypeToString(accessor.componentType) + "`");
    }

    if (accessor.type != TINYGLTF_TYPE_VEC3) {
        return unexpected("Unsupported type `" + typeToString(accessor.type) + "`");
    }

    const std::span<const std::uint8_t> bs = std::span(
        buffer.data.data() + bufferView.byteOffset,
        bufferView.byteLength);

    std::size_t i = 0;
    for (const std::uint8_t* elem = bs.data(); elem < bs.data() + bs.size(); elem += byteStride) {
        const auto e = reinterpret_cast<const float*>(elem);

        const float x = e[0];
        const float y = e[1];
        const float z = e[2];

        vertices[i].position = transformation * glm::vec4 { x, y, z, 1 };
        ++i;
    }

    return {};
}

[[nodiscard]] Expected<void> parseColorAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor, const glm::mat4&) noexcept
{
    if (vertices.size() < accessor.count) {
        vertices.resize(accessor.count);
    }

    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
        return unexpected("Unsupported component type `" + componentTypeToString(accessor.componentType) + "`");
    }

    if (accessor.type != TINYGLTF_TYPE_VEC4) {
        return unexpected("Unsupported type `" + typeToString(accessor.type) + "`");
    }

    const std::span<const std::uint8_t> bs = std::span(
        buffer.data.data() + bufferView.byteOffset,
        bufferView.byteLength);

    std::size_t i = 0;
    for (const std::uint8_t* elem = bs.data(); elem < bs.data() + bs.size(); elem += byteStride) {
        const std::uint8_t r = elem[0];
        const std::uint8_t g = elem[1];
        const std::uint8_t b = elem[2];
        const std::uint8_t a = elem[3];

        vertices[i].color = glm::vec4 {
            static_cast<float>(r) / 255.f,
            static_cast<float>(g) / 255.f,
            static_cast<float>(b) / 255.f,
            static_cast<float>(a) / 255.f,
        };
        ++i;
    }

    return {};
}

[[nodiscard]] Expected<void> parseNormalAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor, const glm::mat4& transformation) noexcept
{
    if (vertices.size() < accessor.count) {
        vertices.resize(accessor.count);
    }

    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        return unexpected("Unsupported component type `" + componentTypeToString(accessor.componentType) + "`");
    }

    if (accessor.type != TINYGLTF_TYPE_VEC3) {
        return unexpected("Unsupported type `" + typeToString(accessor.type) + "`");
    }

    const std::span<const std::uint8_t> bs = std::span(
        buffer.data.data() + bufferView.byteOffset,
        bufferView.byteLength);

    std::size_t i = 0;
    for (const std::uint8_t* elem = bs.data(); elem < bs.data() + bs.size(); elem += byteStride) {
        const auto e = reinterpret_cast<const float*>(elem);

        const float x = e[0];
        const float y = e[1];
        const float z = e[2];

        vertices[i].normal = glm::normalize(transformation * glm::vec4 { x, y, z, 0 });
        ++i;
    }

    return {};
}

[[nodiscard]] Expected<void> parseTangentAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor, const glm::mat4& transformation) noexcept
{
    if (vertices.size() < accessor.count) {
        vertices.resize(accessor.count);
    }

    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        return unexpected("Unsupported component type `" + componentTypeToString(accessor.componentType) + "`");
    }

    if (accessor.type != TINYGLTF_TYPE_VEC4) {
        return unexpected("Unsupported type `" + typeToString(accessor.type) + "`");
    }

    const std::span<const std::uint8_t> bs = std::span(
        buffer.data.data() + bufferView.byteOffset,
        bufferView.byteLength);

    std::size_t i = 0;
    for (const std::uint8_t* elem = bs.data(); elem < bs.data() + bs.size(); elem += byteStride) {
        const auto e = reinterpret_cast<const float*>(elem);

        const float x = e[0];
        const float y = e[1];
        const float z = e[2];
        const float w = e[3];

        if (std::abs(vertices[i].normal.x) <= std::numeric_limits<float>::epsilon()
            && std::abs(vertices[i].normal.y) <= std::numeric_limits<float>::epsilon()
            && std::abs(vertices[i].normal.z) <= std::numeric_limits<float>::epsilon()) {
            return unexpected("Can not calculate bitangent without a normal");
        }

        vertices[i].tangent = glm::normalize(transformation * glm::vec4 { x, y, z, 0 });
        vertices[i].bitangent = glm::normalize(glm::cross(vertices[i].normal, vertices[i].tangent) * w);
        ++i;
    }

    return {};
}

[[nodiscard]] Expected<void> parseUVAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor, const glm::mat4&) noexcept
{
    if (vertices.size() < accessor.count) {
        vertices.resize(accessor.count);
    }

    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        return unexpected("Unsupported component type `" + componentTypeToString(accessor.componentType) + "`");
    }

    if (accessor.type != TINYGLTF_TYPE_VEC2) {
        return unexpected("Unsupported type `" + typeToString(accessor.type) + "`");
    }

    const std::span<const std::uint8_t> bs = std::span(
        buffer.data.data() + bufferView.byteOffset,
        bufferView.byteLength);

    std::size_t i = 0;
    for (const std::uint8_t* elem = bs.data(); elem < bs.data() + bs.size(); elem += byteStride) {
        const auto e = reinterpret_cast<const float*>(elem);

        const float x = e[0];
        const float y = e[1];

        vertices[i].uv = glm::vec2 { x, y };
        ++i;
    }

    return {};
}

[[nodiscard]] Expected<std::pair<Shared<Geometry::Mesh>, SharedMaterial>> parsePrimitive(
    const tinygltf::Model& model,
    const tinygltf::Primitive& primitive,
    const std::vector<SharedMaterial>& materials,
    const glm::mat4& transformation) noexcept
{
#ifdef BADGER_ENGINE_TINY_GLTF_LOGS
    std::cout << "\tPrimitive:" << std::endl;
#endif

    const tinygltf::Accessor indexAccessor = model.accessors[sz(primitive.indices)];

#ifdef BADGER_ENGINE_TINY_GLTF_LOGS
    std::cout << "\t\tIndexAccessor: " << typeToString(indexAccessor.type) << "<" << componentTypeToString(indexAccessor.componentType) << ">" << "[" << indexAccessor.count << "]" << std::endl;
#endif

    std::vector<Geometry::Mesh::Index> indices;
    std::vector<Geometry::Vertex> vertices;

    {
        const auto result = parseIndexBuffer(indices, model, indexAccessor);
        if (!result) {
            return unexpected("Failed to parse index buffer", result.error());
        }
    }

    std::map<std::string, ParseAttributeBufferFunction> attributesBufferParseFunctions = {
        { "POSITION", parsePositionAttributeBuffer },
        { "COLOR_0", parseColorAttributeBuffer },
        { "NORMAL", parseNormalAttributeBuffer },
        { "TANGENT", parseTangentAttributeBuffer },
        { "TEXCOORD_0", parseUVAttributeBuffer },
    };

    for (auto& attrib : primitive.attributes) {
        tinygltf::Accessor accessor = model.accessors[sz(attrib.second)];

#ifdef BADGER_ENGINE_TINY_GLTF_LOGS
        std::cout << "\t\tAttribute: " << attrib.first << " " << typeToString(accessor.type) << "<" << componentTypeToString(accessor.componentType) << ">" << "[" << accessor.count << "]" << std::endl;
#endif

        const auto it = attributesBufferParseFunctions.find(attrib.first);
        if (it != attributesBufferParseFunctions.end()) {
            const auto result = it->second(vertices, model, accessor, transformation);
            if (!result) {
                return unexpected("Failed to parse attribute buffer `" + attrib.first + "`", result.error());
            }
        }
    }

    SharedMaterial material = EmptyMaterial;
    if (primitive.material >= 0) {
        material = materials[sz(primitive.material)];
    }

    return std::pair {
        Geometry::Mesh::create(Geometry::Topology::TriangleList, std::move(vertices), std::move(indices)),
        material
    };
}

[[nodiscard]] Expected<void> parseMesh(
    std::vector<Shared<Mesh>>& meshes,
    const tinygltf::Model& model,
    const tinygltf::Mesh& mesh,
    const std::vector<SharedMaterial>& materials,
    const glm::mat4& transformation) noexcept
{
#ifdef BADGER_ENGINE_TINY_GLTF_LOGS
    std::cout << "Mesh: " << mesh.name << std::endl;
#endif

    for (std::size_t i = 0; i < mesh.primitives.size(); ++i) {
        const auto result = parsePrimitive(model, mesh.primitives[i], materials, transformation);
        if (!result) {
            return unexpected("Failed to parse primitive", result.error());
        }

        meshes.push_back(Mesh::create(
            mesh.name,
            result->first,
            result->second));
    }

    return {};
}

[[nodiscard]] glm::mat4 nodeTransformation(const tinygltf::Node& node)
{
    if (!node.matrix.empty()) {
        assert(false && "Check that the matrix is correct. maybe it needs to be transposed");
        assert(node.matrix.size() == 16);
        const auto& m = node.matrix;
        return glm::mat4(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
    } else {
        glm::mat4 result = glm::mat4(1);

        if (!node.translation.empty()) {
            assert(node.translation.size() == 3);
            result = glm::translate(result, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }

        if (!node.rotation.empty()) {
            assert(node.rotation.size() == 4);

            const auto quat = glm::quat {
                static_cast<float>(node.rotation[3]),
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]),
                static_cast<float>(node.rotation[2]),
            };

            result = result * glm::toMat4(quat);
        }

        if (!node.scale.empty()) {
            assert(node.scale.size() == 3);
            result = glm::scale(result, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
        }

        return result;
    }
}

[[nodiscard]] Expected<void> parseNode(
    std::vector<Shared<Mesh>>& meshes,
    const tinygltf::Model& model,
    const tinygltf::Node& node,
    const std::vector<SharedMaterial>& materials,
    glm::mat4 transformation) noexcept
{
    transformation *= nodeTransformation(node);

    if (node.mesh >= 0 && std::cmp_less(node.mesh, model.meshes.size())) {
        const auto result = parseMesh(meshes, model, model.meshes[numericCast<std::size_t>(node.mesh).value()], materials, transformation);
        if (!result) {
            return unexpected("Failed to parse mesh", result.error());
        }
    }

    for (std::size_t i = 0; i < node.children.size(); i++) {
        assert(node.children[i] >= 0 && std::cmp_less(node.children[i], model.nodes.size()));
        const auto result = parseNode(meshes, model, model.nodes[numericCast<std::size_t>(node.children[i]).value()], materials, transformation);
        if (!result) {
            return unexpected("Failed to parse node", result.error());
        }
    }

    return {};
}

[[nodiscard]] Expected<SharedTexture> parseTexture(
    const TextureLoader& textureLoader,
    const tinygltf::Texture& texture,
    const tinygltf::Model& model)
{
    const auto image = model.images[sz(texture.source)];
    const auto result = textureLoader.texture(image.uri);
    if (!result) {
        return unexpected("Texture `" + image.uri + "` not found");
    }

    return *result;
}

[[nodiscard]] Expected<std::vector<SharedTexture>> parseTextures(
    const TextureLoader& textureLoader,
    const std::vector<tinygltf::Texture>& textures,
    const tinygltf::Model& model)
{
    std::vector<SharedTexture> result;
    result.reserve(textures.size());

    for (const auto& texture : textures) {
        const auto t = parseTexture(textureLoader, texture, model);
        if (!t) {
            return unexpected("Failed to parse texture", t.error());
        }

        result.push_back(*t);
    }

    return result;
}

Expected<glm::vec3> parseVec3(const std::vector<double>& vec) noexcept
{
    if (vec.size() == 3) {
        return glm::vec3 { vec[0], vec[1], vec[2] };
    } else {
        return unexpected("Vec size doesn't match");
    }
}

Expected<glm::vec4> parseVec4(const std::vector<double>& vec) noexcept
{
    if (vec.size() == 4) {
        return glm::vec4 { vec[0], vec[1], vec[2], vec[3] };
    } else {
        return unexpected("Vec size doesn't match");
    }
}

[[nodiscard]] Expected<SharedMaterial> parseMaterial(
    const tinygltf::Material& material,
    const std::vector<SharedTexture>& textures)
{
    const auto baseColor = parseVec4(material.pbrMetallicRoughness.baseColorFactor);
    if (!baseColor) {
        return unexpected("Failed to parse base color", baseColor.error());
    }

    const auto emissiveColor = parseVec3(material.emissiveFactor);
    if (!emissiveColor) {
        return unexpected("Failed to parse emissive color", emissiveColor.error());
    }

    const auto metallness = material.pbrMetallicRoughness.metallicFactor;
    const auto roughness = material.pbrMetallicRoughness.roughnessFactor;

    std::vector<SharedTexture> normalsMaps;
    if (material.normalTexture.index >= 0) {
        normalsMaps.push_back(textures[sz(material.normalTexture.index)]);
    }

    std::vector<SharedTexture> baseColorMaps;
    if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
        baseColorMaps.push_back(textures[sz(material.pbrMetallicRoughness.baseColorTexture.index)]);
    }

    std::vector<SharedTexture> metalnessMaps;
    if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
        metalnessMaps.push_back(textures[sz(material.pbrMetallicRoughness.metallicRoughnessTexture.index)]);
    }

    std::vector<SharedTexture> ambientOcclusionMaps;
    if (material.occlusionTexture.index >= 0) {
        ambientOcclusionMaps.push_back(textures[sz(material.occlusionTexture.index)]);
    }

    return std::make_shared<Material>(Material {
        .name = material.name,
        .baseColor = *std::move(baseColor),
        .diffuseColor = std::nullopt,
        .emissiveColor = glm::vec4(*emissiveColor, 1),
        .specularColor = std::nullopt,
        .metallness = std::move(metallness),
        .roughness = std::move(roughness),
        .shininess = std::nullopt,
        .specularFactor = std::nullopt,
        .diffuseMaps = {},
        .specularMaps = {},
        .ambientMaps = {},
        .emissiveMaps = {},
        .heightMaps = {},
        .normalsMaps = std::move(normalsMaps),
        .shininessMaps = {},
        .opacityMaps = {},
        .displacementMaps = {},
        .lightmapMaps = {},
        .reflectionMaps = {},
        .baseColorMaps = std::move(baseColorMaps),
        .normalCameraMaps = {},
        .emissionColorMaps = {},
        .metalnessMaps = std::move(metalnessMaps),
        .diffuseRoughnessMaps = {},
        .ambientOcclusionMaps = std::move(ambientOcclusionMaps),
        .unknownTextures = {},
        .sheenMaps = {},
        .clearcoatMaps = {},
        .transmissionMaps = {},
        .mayaBaseMaps = {},
        .mayaSpecularMaps = {},
        .mayaSpecularColorMaps = {},
        .mayaSpecularRoughnessMaps = {},
    });
}

Expected<std::vector<SharedMaterial>> parseMaterials(
    const std::vector<tinygltf::Material>& materials,
    const std::vector<SharedTexture>& textures) noexcept
{
    std::vector<SharedMaterial> result;
    result.reserve(materials.size());

    for (const auto& material : materials) {
        const auto m = parseMaterial(material, textures);
        if (!m) {
            return unexpected("Failed to load scene material", m.error());
        }

        result.push_back(*m);
    }

    return result;
}

[[nodiscard]] Expected<void> parseModel(std::vector<Shared<Mesh>>& meshes, const TextureLoader& textureLoader, const tinygltf::Model& model) noexcept
{

    const auto textures = parseTextures(textureLoader, model.textures, model);
    if (!textures) {
        return unexpected("Failed to load scene textures", textures.error());
    }

    const auto materials = parseMaterials(model.materials, *textures);
    if (!materials) {
        return unexpected("Failed to load scene materials", materials.error());
    }

    const tinygltf::Scene& scene = model.scenes[numericCast<std::size_t>(model.defaultScene).value()];

    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        assert((scene.nodes[i] >= 0) && std::cmp_less(scene.nodes[i], model.nodes.size()));
        const auto result = parseNode(meshes, model, model.nodes[numericCast<std::size_t>(scene.nodes[i]).value()], *materials, glm::mat4(1));
        if (!result) {
            return unexpected("Failed to parse root nodes", result.error());
        }
    }

    return {};
}
}

Expected<Model> TinyGLTFImporter::load(const TextureLoader& textureLoader, const std::filesystem::path& path) const noexcept
{
    tinygltf::TinyGLTF loader;

    std::string err;
    std::string warn;

    tinygltf::Model model;
    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());
    if (!warn.empty()) {
        return unexpected("LoadASCIIFromFile.warn: " + warn);
    }

    if (!err.empty()) {
        return unexpected("LoadASCIIFromFile.err: " + err);
    }

    if (!res) {
        return unexpected("LoadASCIIFromFile failed: " + path.string());
    }

    std::vector<Shared<Mesh>> meshes;

    {
        const auto result = parseModel(meshes, textureLoader, model);
        if (!result) {
            return unexpected("Failed to parse model", result.error());
        }
    }

    return Model(meshes);
}

Expected<Model> TinyGLTFImporter::parse(const TextureLoader&, std::span<std::uint8_t> data, const std::string& hint) const noexcept
{
    (void)data;
    (void)hint;
    return unexpected("TODO");
}
}
