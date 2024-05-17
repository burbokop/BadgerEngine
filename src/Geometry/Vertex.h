#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace BadgerEngine::Geometry {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;

    static vk::VertexInputBindingDescription bindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions();
};

}
