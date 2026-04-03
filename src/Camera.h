#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <numbers>

namespace BadgerEngine {

class InputProvider;

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
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -std::numbers::pi_v<float> / 2,
        float pitch = 0,
        float near = 0.1f,
        float far = 100,
        float fovy = DefaultFovy)
        : m_position(std::move(position))
        , m_up(std::move(up))
        , m_worldUp(std::move(up))
        , m_yaw(std::move(yaw))
        , m_pitch(std::move(pitch))
        , m_near(std::move(near))
        , m_far(std::move(far))
        , m_fovy(std::move(fovy))
    {
    }

    void setPosition(glm::vec3 v)
    {
        m_position = v;
    }

    glm::vec3 front() const { return m_front; }
    float fovy() const { return m_fovy; }
    glm::vec3 right() const { return m_right; }
    glm::vec3 up() const { return m_up; }

    glm::mat4 view() const
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    glm::mat4 projection(float aspectRatio) const
    {
        return glm::perspective(m_fovy, aspectRatio, m_near, m_far);
    }

    struct Limits {
        float top;
        float bottom;
    };

    struct InputParameters {
        /// World coordinate units per second
        float movementVelocity;
        /// World coordinate units per second
        float sprintMovementVelocity;
        /// radians per mouse delta pixels
        float rotationVelocity;
        std::optional<Limits> pitchLimits;
    };

    constexpr static InputParameters DefaultInputParameters = {
        .movementVelocity = 4.f,
        .sprintMovementVelocity = 16.f,
        .rotationVelocity = std::numbers::pi_v<float> / 1800,
        .pitchLimits = Limits {
            .top = 89.0f / 180 * std::numbers::pi_v<float>,
            .bottom = -89.0f / 180 * std::numbers::pi_v<float>,
        },
    };

    void processInput(const InputProvider& inputProvider, std::chrono::nanoseconds dt, InputParameters parameters = DefaultInputParameters);

    // Camera interface
public:
    glm::mat4 transformation(glm::vec2 extent) const override
    {
        return projection(extent.x / extent.y) * view();
    }

    glm::vec3 position() const override
    {
        return m_position;
    }

    float near() const override
    {
        return m_near;
    }

    float far() const override
    {
        return m_far;
    }

private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    float m_yaw;
    float m_pitch;
    float m_near;
    float m_far;
    float m_fovy;
    std::optional<glm::vec2> m_prevMousePosition;
};

class DirectionBasedOrthographicCamera : public Camera {
public:
    DirectionBasedOrthographicCamera(
        glm::vec3 position = { 0, 0, 0 },
        glm::vec3 direction = { 0, 0, 1.f },
        glm::vec3 up = { 0, 1.f, 0 },
        float near = -1,
        float far = 1,
        float scale = 1)
        : m_position(std::move(position))
        , m_direction(std::move(direction))
        , m_up(std::move(up))
        , m_near(std::move(near))
        , m_far(std::move(far))
        , m_scale(std::move(scale))
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

    void setNear(float v) { m_near = v; }
    void setFar(float v) { m_far = v; }
    void setScale(float v) { m_scale = v; }

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
        extent /= m_scale;
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
    float m_scale;
};

}
