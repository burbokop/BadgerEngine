#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <numbers>

namespace BadgerEngine {

class Camera {
public:
    virtual glm::mat4 transformation(glm::vec2 extent) const = 0;
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

    glm::mat4 transformation(glm::vec2 extent) const override
    {
        return glm::perspective(m_fovy, extent.x / extent.y, m_near, m_far) * glm::toMat4(m_rotation) * glm::translate(glm::mat4(1.f), m_position);
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

class DirectionBasedOrthographicCamera : public Camera {
public:
    DirectionBasedOrthographicCamera(
        glm::vec3 position = { 0, 0, 0 },
        glm::vec3 direction = { 0, 0, 1.f },
        glm::vec3 up = { 0, 1.f, 0 },
        float near = -10,
        float far = 10)
        : m_position(std::move(position))
        , m_direction(std::move(direction))
        , m_up(std::move(up))
        , m_near(std::move(near))
        , m_far(std::move(far))
    {
        assert(m_near < m_far);
    }

    void setDirection(const glm::vec3& direction)
    {
        m_direction = direction;
    }

    void setPosition(const glm::vec3& position)
    {
        m_position = position;
    }

    /**
     * @brief setOrbit - set position and direction in the way that camera looks at `center` from `direction` and the center is in the middle of its view range
     * @param center
     * @param direction
     */
    void setOrbit(const glm::vec3& center, const glm::vec3& direction)
    {
        // TODO: figure out why `/ 4.f` works while `/ 2.f` does not work
        const auto distanceToCenter = (m_far - m_near) / 4.f;
        m_position = center - glm::normalize(direction) * distanceToCenter;
        m_direction = direction;
    }

    // Camera interface
public:
    virtual glm::mat4 transformation(glm::vec2 extent) const override
    {
        // TODO: remove it
        // extent /= 30;
        extent /= 100;
        return glm::ortho(-extent.x / 2.f, extent.x / 2.f, -extent.y / 2.f, extent.y / 2.f, m_near, m_far) * glm::lookAt(m_position, m_position + m_direction, m_up);
    }

    virtual glm::vec3 position() const override { return m_position; }
    virtual float near() const override { return m_near; }
    virtual float far() const override { return m_far; }

private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;
    float m_near;
    float m_far;
};

}
