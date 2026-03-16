#pragma once

#include <glm/glm.hpp>

namespace vk {
struct VertexInputBindingDescription;
struct VertexInputAttributeDescription;
}

namespace BadgerEngine::Geometry {

struct Vertex {
    glm::vec3 position = { 0, 0, 0 };
    glm::vec3 normal = { 0, 0, 0 };
    glm::vec3 tangent = { 0, 0, 0 };
    glm::vec3 bitangent = { 0, 0, 0 };
    glm::vec3 color = { 0, 0, 0 };
    glm::vec2 uv = { 0, 0 };

    static vk::VertexInputBindingDescription bindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 6> attributeDescriptions();
};

}
