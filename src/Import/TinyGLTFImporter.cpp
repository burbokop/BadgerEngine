#include "TinyGLTFImporter.h"

#include "../Utils/NumericCast.h"
#include <iostream>
#include <tiny_gltf.h>

namespace BadgerEngine::Import {

namespace {

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

[[nodiscard]] std::string valueToString(
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

using ParseAttributeBuffer = Expected<void> (*)(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor);

[[nodiscard]] Expected<void> parsePositionAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor) noexcept
{
    vertices.resize(accessor.count);
    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    (void)byteStride;

    return unexpected("TODO");
}

[[nodiscard]] Expected<void> parseColorAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor) noexcept
{
    vertices.resize(accessor.count);
    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    (void)byteStride;
    return unexpected("TODO");
}

[[nodiscard]] Expected<void> parseNormalAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor) noexcept
{
    vertices.resize(accessor.count);
    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    (void)byteStride;
    return unexpected("TODO");
}

[[nodiscard]] Expected<void> parseTangentAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor) noexcept
{
    vertices.resize(accessor.count);
    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    (void)byteStride;
    return unexpected("TODO");
}

[[nodiscard]] Expected<void> parseUVAttributeBuffer(std::vector<Geometry::Vertex>& vertices, const tinygltf::Model& model, const tinygltf::Accessor& accessor) noexcept
{
    vertices.resize(accessor.count);
    const auto bufferView = model.bufferViews[sz(accessor.bufferView)];
    const auto buffer = model.buffers[sz(bufferView.buffer)];
    const auto byteStride = accessor.ByteStride(bufferView);

    (void)byteStride;
    return unexpected("TODO");
}

[[nodiscard]] Expected<Shared<Mesh>> parsePrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive) noexcept
{
    std::cout << "\tPrimitive:" << std::endl;

    const tinygltf::Accessor indexAccessor = model.accessors[sz(primitive.indices)];
    const auto indexBufferView = model.bufferViews[sz(indexAccessor.bufferView)];
    const auto indexBuffer = model.buffers[sz(indexBufferView.buffer)];

    std::cout << "\t\tIndexAccessor: " << typeToString(indexAccessor.type) << "<" << componentTypeToString(indexAccessor.componentType) << ">" << "[" << indexAccessor.count << "]" << " " << indexBufferView.byteLength << " (" << indexBuffer.data.size() << ")" << std::endl;

    std::vector<Geometry::Vertex> vertices;

    std::map<std::string, ParseAttributeBuffer> attributesBufferParseFunctions = {
        { "POSITION", parsePositionAttributeBuffer },
        { "COLOR_0", parseColorAttributeBuffer },
        { "NORMAL", parseNormalAttributeBuffer },
        { "TANGENT", parseTangentAttributeBuffer },
        { "TEXCOORD_0", parseUVAttributeBuffer },
    };

    for (auto& attrib : primitive.attributes) {
        tinygltf::Accessor accessor = model.accessors[sz(attrib.second)];

        std::cout << "\t\tAttribute: " << attrib.first << " " << typeToString(accessor.type) << "<" << componentTypeToString(accessor.componentType) << ">" << "[" << accessor.count << "]" << std::endl;

        const auto result = attributesBufferParseFunctions.at(attrib.first)(vertices, model, accessor);
        if (!result) {
            return unexpected("Failed to parse attribute buffer `" + attrib.first + "`", result.error());
        }

        // glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

        // int size = 1;
        // if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        //     size = accessor.type;
        // }

        // int vaa = -1;
        // if (attrib.first.compare("POSITION") == 0)
        //     vaa = 0;
        // if (attrib.first.compare("NORMAL") == 0)
        //     vaa = 1;
        // if (attrib.first.compare("TEXCOORD_0") == 0)
        //     vaa = 2;
        // if (vaa > -1) {
        //     glEnableVertexAttribArray(vaa);
        //     glVertexAttribPointer(vaa, size, accessor.componentType,
        //         accessor.normalized ? GL_TRUE : GL_FALSE,
        //         byteStride, BUFFER_OFFSET(accessor.byteOffset));
        // } else
        //     std::cout << "vaa missing: " << attrib.first << std::endl;
    }

    // TODO
    // if (model.textures.size() > 0) {
    //     // fixme: Use material's baseColor
    //     tinygltf::Texture& tex = model.textures[0];

    //     if (tex.source > -1) {

    //         GLuint texid;
    //         glGenTextures(1, &texid);

    //         tinygltf::Image& image = model.images[tex.source];

    //         glBindTexture(GL_TEXTURE_2D, texid);
    //         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //         GLenum format = GL_RGBA;

    //         if (image.component == 1) {
    //             format = GL_RED;
    //         } else if (image.component == 2) {
    //             format = GL_RG;
    //         } else if (image.component == 3) {
    //             format = GL_RGB;
    //         } else {
    //             // ???
    //         }

    //         GLenum type = GL_UNSIGNED_BYTE;
    //         if (image.bits == 8) {
    //             // ok
    //         } else if (image.bits == 16) {
    //             type = GL_UNSIGNED_SHORT;
    //         } else {
    //             // ???
    //         }

    //         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
    //             format, type, &image.image.at(0));
    //     }
    // }

    return unexpected("TODO");
}

[[nodiscard]] Expected<void> parseMesh(std::vector<Shared<Mesh>>& meshes, const tinygltf::Model& model, const tinygltf::Mesh& mesh) noexcept
{
    std::cout << "Mesh: " << mesh.name << std::endl;

    for (std::size_t i = 0; i < mesh.primitives.size(); ++i) {
        const auto result = parsePrimitive(model, mesh.primitives[i]);
        if (!result) {
            return unexpected("Failed to parse primitive", result.error());
        }

        meshes.push_back(*result);
    }

    return {};
}

[[nodiscard]] Expected<void> parseNode(std::vector<Shared<Mesh>>& meshes, const tinygltf::Model& model,
    const tinygltf::Node& node) noexcept
{
    if (node.mesh >= 0 && std::cmp_less(node.mesh, model.meshes.size())) {
        const auto result = parseMesh(meshes, model, model.meshes[numericCast<std::size_t>(node.mesh).value()]);
        if (!result) {
            return unexpected("Failed to parse mesh", result.error());
        }
    }

    for (std::size_t i = 0; i < node.children.size(); i++) {
        assert(node.children[i] >= 0 && std::cmp_less(node.children[i], model.nodes.size()));
        const auto result = parseNode(meshes, model, model.nodes[numericCast<std::size_t>(node.children[i]).value()]);
        if (!result) {
            return unexpected("Failed to parse node", result.error());
        }
    }

    return {};
}

[[nodiscard]] Expected<void> parseModel(std::vector<Shared<Mesh>>& meshes, const tinygltf::Model& model) noexcept
{
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {
        const tinygltf::BufferView& bufferView = model.bufferViews[i];
        if (bufferView.target == 0) { // TODO impl drawarrays
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue; // Unsupported bufferView.
            /*
              From spec2.0 readme:
              https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                       ... drawArrays function should be used with a count equal to
              the count            property of any of the accessors referenced by the
              attributes            property            (they are all equal for a given
              primitive).
            */
        }

        const tinygltf::Buffer& buffer = model.buffers[numericCast<std::size_t>(bufferView.buffer).value()];
        std::cout << "bufferview `" << bufferView.name
                  << "` target: " << targetToString(bufferView.target)
                  << ", buffer.data.size = " << buffer.data.size()
                  << ", bufferview.byteOffset = " << bufferView.byteOffset
                  << std::endl;

        // TODO
        // glBufferData(bufferView.target, bufferView.byteLength,
        //     &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    const tinygltf::Scene& scene = model.scenes[numericCast<std::size_t>(model.defaultScene).value()];

    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        assert((scene.nodes[i] >= 0) && std::cmp_less(scene.nodes[i], model.nodes.size()));
        const auto result = parseNode(meshes, model, model.nodes[numericCast<std::size_t>(scene.nodes[i]).value()]);
        if (!result) {
            return unexpected("Failed to parse root nodes", result.error());
        }
    }

    // glBindVertexArray(0);
    // // cleanup vbos but do not delete index buffers yet
    // for (auto it = vbos.cbegin(); it != vbos.cend();) {
    //     tinygltf::BufferView bufferView = model.bufferViews[it->first];
    //     if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
    //         glDeleteBuffers(1, &vbos[it->first]);
    //         vbos.erase(it++);
    //     } else {
    //         ++it;
    //     }
    // }

    return {};
}

}

Expected<Model> TinyGLTFImporter::load(const TextureLoader&, const std::filesystem::path& path) const noexcept
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
        const auto result = parseModel(meshes, model);
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
