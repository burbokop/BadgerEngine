#pragma once

#include <glm/glm.hpp>

namespace BadgerEngine {

struct PointLight {
    glm::vec4 position;
    glm::vec4 color;
    float intensity;
};

}
