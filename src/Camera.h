#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <numbers>

namespace BadgerEngine {

class Camera {
public:
    virtual glm::mat4 transformation(float aspect) const = 0;
    virtual glm::vec3 position() const = 0;
    virtual float near() const = 0;
    virtual float far() const = 0;
};

class PerspectiveCamera : public Camera {
public:
    static constexpr auto DefaultFovy = std::numbers::pi_v<float> / 4.f;

    PerspectiveCamera(
        glm::vec3 position = { 0, 0, 0 },
        glm::quat rotation = glm::quat(1.f, 0, 0, 0),
        float fovy = DefaultFovy,
        float near = 0.1f,
        float far = 100.f)
        : m_position(std::move(position))
        , m_rotation(std::move(rotation))
        , m_fovy(fovy)
        , m_near(near)
        , m_far(far)
    {
    }

    glm::mat4 transformation(float aspect) const override
    {
        return glm::perspective(m_fovy, aspect, m_near, m_far) * glm::toMat4(m_rotation) * glm::translate(glm::mat4(1.f), m_position);
    }

    glm::vec3 position() const override
    {
        return m_position;
    }

    void setPosition(const glm::vec3& position)
    {
        m_position = position;
    }

    void addPosition(const glm::vec3& delta)
    {
        m_position += delta;
    }

    glm::quat rotation() const
    {
        return m_rotation;
    }

    void setRotation(const glm::quat& newRotation)
    {
        m_rotation = newRotation;
    }

    float fovy() const { return m_fovy; }
    float near() const override { return m_near; }
    float far() const override { return m_far; }

private:
    glm::vec3 m_position;
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
