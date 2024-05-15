#include "ObjMesh.h"

#include <cstring>

namespace BadgerEngine::Geometry {

Expected<ObjMesh> ObjMesh::load(const std::filesystem::path& path)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvMap;
    std::vector<glm::vec3> normals;
    // std::vector<std::uint32_t> vertexIndices;
    // std::vector<std::uint32_t> uvIndices;
    // std::vector<std::uint32_t> normalIndices;

    std::vector<Index> indices;

    const auto file = std::fopen(path.c_str(), "r");
    if (file == NULL) {
        return unexpected("Failed to open file " + path.string() + ": " + ::strerror(errno));
    }

    while (true) {
        std::string line;
        if (std::fscanf(file, "%s", line.data()) == EOF) {
            break;
        } else if (std::strcmp(line.c_str(), "v") == 0) {
            float x = 0;
            float y = 0;
            float z = 0;
            std::fscanf(file, "%f %f %f\n", &x, &y, &z);
            vertices.push_back({ x, y, z });
        } else if (std::strcmp(line.c_str(), "vt") == 0) {
            float x = 0;
            float y = 0;
            std::fscanf(file, "%f %f\n", &x, &y);
            uvMap.push_back({ x, y });
        } else if (std::strcmp(line.c_str(), "vn") == 0) {
            float x = 0;
            float y = 0;
            float z = 0;
            std::fscanf(file, "%f %f %f\n", &x, &y, &z);
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
            std::fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &iv1x, &iv1y, &iv1z, &iv2x, &iv2y, &iv2z, &iv3x, &iv3y, &iv3z);

            indices.push_back(Index {
                .vertex = iv1x - 1,
                .uv = iv1y - 1,
                .normal = iv1z - 1 });

            indices.push_back(Index {
                .vertex = iv2x - 1,
                .uv = iv2y - 1,
                .normal = iv2z - 1 });

            indices.push_back(Index {
                .vertex = iv3x - 1,
                .uv = iv3y - 1,
                .normal = iv3z - 1 });
        }
    }

    std::fclose(file);
    return ObjMesh(
        vertices,
        uvMap,
        normals,
        indices);
}

}
