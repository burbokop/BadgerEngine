#include "Camera.h"

#include "Window.h"

namespace BadgerEngine {

void PerspectiveCamera::processInput(const InputProvider& inputProvider, std::chrono::nanoseconds dt, InputParameters parameters)
{
    {
        const auto dtSeconds = static_cast<float>(dt.count()) / 1000 / 1000 / 1000;
        const float delta = (inputProvider.pressed(InputProvider::Key::LeftShift) ? parameters.sprintMovementVelocity : parameters.movementVelocity) * dtSeconds;

        if (inputProvider.pressed(InputProvider::Key::W)) {
            m_position += m_front * delta;
        } else if (inputProvider.pressed(InputProvider::Key::S)) {
            m_position -= m_front * delta;
        }

        if (inputProvider.pressed(InputProvider::Key::A)) {
            m_position -= m_right * delta;
        } else if (inputProvider.pressed(InputProvider::Key::D)) {
            m_position += m_right * delta;
        }

        if (inputProvider.pressed(InputProvider::Key::Space)) {
            m_position -= m_up * delta;
        } else if (inputProvider.pressed(InputProvider::Key::LeftControl)) {
            m_position += m_up * delta;
        }
    }

    {
        const auto mousePosition = inputProvider.mousePosition();

        if (!m_prevMousePosition) {
            m_prevMousePosition = mousePosition;
        }

        const auto delta = (mousePosition - *std::exchange(m_prevMousePosition, mousePosition)) * parameters.rotationVelocity;

        {
            m_yaw += delta.x;
            m_pitch += delta.y;

            if (parameters.pitchLimits) {
                m_pitch = std::clamp(m_pitch, parameters.pitchLimits->bottom, parameters.pitchLimits->top);
            }

            {
                glm::vec3 newFront;
                newFront.x = std::cos(m_yaw) * std::cos(m_pitch);
                newFront.y = std::sin(m_pitch);
                newFront.z = std::sin(m_yaw) * std::cos(m_pitch);
                m_front = glm::normalize(newFront);
                m_right = glm::normalize(glm::cross(m_front, m_worldUp));
                m_up = glm::normalize(glm::cross(m_right, m_front));
            }
        }
    }
}

}
