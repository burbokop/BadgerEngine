#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace BadgerEngine::Geometry {

struct Vertex {
    glm::vec3 position = { 0, 0, 0 };
    glm::vec3 normal = { 0, 0, 0 };
    glm::vec3 color = { 0, 0, 0 };
    glm::vec2 uv = { 0, 0 };

    static vk::VertexInputBindingDescription bindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions();
};

}
