#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <numbers>

namespace BadgerEngine {

class PerspectiveCamera {
public:
    static constexpr auto DefaultFovy = std::numbers::pi_v<float> / 4.f;

    PerspectiveCamera(
        glm::vec3 translation = { 0, 0, 0 },
        glm::quat rotation = glm::quat(1.f, 0, 0, 0),
        float fovy = DefaultFovy,
        float near = 0.1f,
        float far = 100.f)
        : m_translation(std::move(translation))
        , m_rotation(std::move(rotation))
        , m_fovy(fovy)
        , m_near(near)
        , m_far(far)
    {
    }

    glm::mat4 transformation(float aspect) const
    {
        return glm::perspective(m_fovy, aspect, m_near, m_far) * glm::toMat4(m_rotation) * glm::translate(glm::mat4(1.f), m_translation);
    }

    glm::vec3 translation() const
    {
        return m_translation;
    }

    void setTranslation(const glm::vec3& newTranslation)
    {
        m_translation = newTranslation;
    }

    void addTranslation(const glm::vec3& delta)
    {
        m_translation += delta;
    }

    glm::quat rotation() const
    {
        return m_rotation;
    }

    void setRotation(const glm::quat& newRotation)
    {
        m_rotation = newRotation;
    }

private:
    glm::vec3 m_translation;
    glm::quat m_rotation;
    float m_fovy;
    float m_near;
    float m_far;
};

// TODO
class OrthogonalCamera {
public:
    OrthogonalCamera();

private:
    glm::vec3 m_translation;
    glm::quat m_rotation;
};

}
