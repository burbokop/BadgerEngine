#include "ObjMesh.h"

#include <cstdio>
#include <cstring>

namespace BadgerEngine::Geometry {

namespace {

Expected<ObjMesh> parseFromStream(std::FILE* file)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvMap;
    std::vector<glm::vec3> normals;
    std::vector<ObjMesh::Index> indices;

    while (true) {
        std::string line;
        if (std::fscanf(file, "%s", line.data()) == EOF) {
            break;
        } else if (std::strcmp(line.c_str(), "v") == 0) {
            float x = 0;
            float y = 0;
            float z = 0;
            if (std::fscanf(file, "%f %f %f\n", &x, &y, &z) == EOF) {
                return unexpected("EOF");
            }
            vertices.push_back({ x, y, z });
        } else if (std::strcmp(line.c_str(), "vt") == 0) {
            float x = 0;
            float y = 0;
            if (std::fscanf(file, "%f %f\n", &x, &y) == EOF) {
                return unexpected("EOF");
            }
            uvMap.push_back({ x, y });
        } else if (std::strcmp(line.c_str(), "vn") == 0) {
            float x = 0;
            float y = 0;
            float z = 0;
            if (std::fscanf(file, "%f %f %f\n", &x, &y, &z) == EOF) {
                return unexpected("EOF");
            }
            normals.push_back({ x, y, z });
        } else if (std::strcmp(line.c_str(), "f") == 0) {
            std::uint32_t iv1x = 0;
            std::uint32_t iv1y = 0;
            std::uint32_t iv1z = 0;
            std::uint32_t iv2x = 0;
            std::uint32_t iv2y = 0;
            std::uint32_t iv2z = 0;
            std::uint32_t iv3x = 0;
            std::uint32_t iv3y = 0;
            std::uint32_t iv3z = 0;
            if (std::fscanf(file, "%u/%u/%u %u/%u/%u %u/%u/%u\n", &iv1x, &iv1y, &iv1z, &iv2x, &iv2y, &iv2z, &iv3x, &iv3y, &iv3z) == EOF) {
                return unexpected("EOF");
            }

            indices.push_back(ObjMesh::Index {
                .vertex = iv1x - 1,
                .uv = iv1y - 1,
                .normal = iv1z - 1 });

            indices.push_back(ObjMesh::Index {
                .vertex = iv2x - 1,
                .uv = iv2y - 1,
                .normal = iv2z - 1 });

            indices.push_back(ObjMesh::Index {
                .vertex = iv3x - 1,
                .uv = iv3y - 1,
                .normal = iv3z - 1 });
        }
    }

    return ObjMesh(
        vertices,
        uvMap,
        normals,
        indices);
}

}

Expected<ObjMesh> ObjMesh::load(const std::filesystem::path& path)
{
    const auto file = std::fopen(path.string().c_str(), "r");
    if (file == NULL) {
        return unexpected("Failed to open file " + path.string() + ": " + ::strerror(errno));
    }

    auto result = parseFromStream(file);
    std::fclose(file);
    return result;
}

Expected<ObjMesh> ObjMesh::parse(std::span<const std::uint8_t> bytes)
{
#ifdef _MSC_VER
    return unexpected("Not implemented");
#else
    const auto file = ::fmemopen(const_cast<std::uint8_t*>(bytes.data()), bytes.size(), "r");
    if (file == NULL) {
        return unexpected(std::string("Failed to open file from memory: ") + ::strerror(errno));
    }

    auto result = parseFromStream(file);
    std::fclose(file);
    return result;
#endif
}

}
