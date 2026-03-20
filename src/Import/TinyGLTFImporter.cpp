#include "TinyGLTFImporter.h"

#include "../Utils/NumericCast.h"
#include <iostream>
#include <tiny_gltf.h>

namespace BadgerEngine::Import {

namespace {

[[nodiscard]] Expected<Shared<Mesh>> bindMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh) noexcept
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
        std::cout << "bufferview.target " << bufferView.target << std::endl;

        std::cout << "buffer.data.size = " << buffer.data.size()
                  << ", bufferview.byteOffset = " << bufferView.byteOffset
                  << std::endl;

        // TODO
        // glBufferData(bufferView.target, bufferView.byteLength,
        //     &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    for (std::size_t i = 0; i < mesh.primitives.size(); ++i) {
        const tinygltf::Primitive primitive = mesh.primitives[i];
        const tinygltf::Accessor indexAccessor = model.accessors[numericCast<std::size_t>(primitive.indices).value()];

        for (auto& attrib : primitive.attributes) {
            tinygltf::Accessor accessor = model.accessors[numericCast<std::size_t>(attrib.second).value()];
            int byteStride = accessor.ByteStride(model.bufferViews[numericCast<std::size_t>(accessor.bufferView).value()]);

            (void)byteStride;
            // TODO
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
    }

    return unexpected("TODO");
}

[[nodiscard]] Expected<void> bindModelNodes(std::vector<Shared<Mesh>>& meshes, const tinygltf::Model& model,
    const tinygltf::Node& node) noexcept
{
    if (node.mesh >= 0 && std::cmp_less(node.mesh, model.meshes.size())) {
        const auto mesh = bindMesh(model, model.meshes[numericCast<std::size_t>(node.mesh).value()]);
        if (!mesh) {
            return unexpected("bindMesh failed", mesh.error());
        }
        meshes.push_back(*mesh);
    }

    for (std::size_t i = 0; i < node.children.size(); i++) {
        assert(node.children[i] >= 0 && std::cmp_less(node.children[i], model.nodes.size()));
        const auto result = bindModelNodes(meshes, model, model.nodes[numericCast<std::size_t>(node.children[i]).value()]);
        if (!result) {
            return unexpected("bindModelNodes failed", result.error());
        }
    }

    return {};
}

[[nodiscard]] Expected<void> bindModel(std::vector<Shared<Mesh>>& meshes, const tinygltf::Model& model) noexcept
{
    const tinygltf::Scene& scene = model.scenes[numericCast<std::size_t>(model.defaultScene).value()];

    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        assert((scene.nodes[i] >= 0) && std::cmp_less(scene.nodes[i], model.nodes.size()));
        const auto result = bindModelNodes(meshes, model, model.nodes[numericCast<std::size_t>(scene.nodes[i]).value()]);
        if (!result) {
            return unexpected("bindModelNodes failed", result.error());
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
        const auto result = bindModel(meshes, model);
        if (!result) {
            return unexpected("bindModel failed", result.error());
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
